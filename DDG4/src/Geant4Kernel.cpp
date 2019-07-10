//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

// Framework include files
#include "DD4hep/Detector.h"
#include "DD4hep/Memory.h"
#include "DD4hep/Plugins.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Primitives.h"
#include "DD4hep/InstanceCount.h"

#include "DDG4/Geant4Kernel.h"
#include "DDG4/Geant4Context.h"
#include "DDG4/Geant4ActionPhase.h"

// Geant4 include files
#include "G4RunManager.hh"
#include "G4UIdirectory.hh"
#include "G4Threading.hh"
#include "G4AutoLock.hh"

// C/C++ include files
#include <stdexcept>
#include <algorithm>
#include <pthread.h>
#include <memory>

using namespace std;
using namespace dd4hep::sim;

namespace {
  G4Mutex kernel_mutex=G4MUTEX_INITIALIZER;
  dd4hep::dd4hep_ptr<Geant4Kernel> s_main_instance(0);
}

/// Standard constructor
Geant4Kernel::PhaseSelector::PhaseSelector(Geant4Kernel* kernel)
  : m_kernel(kernel) {
}

/// Copy constructor
Geant4Kernel::PhaseSelector::PhaseSelector(const PhaseSelector& c)
  : m_kernel(c.m_kernel) {
}

/// Assignment operator
Geant4Kernel::PhaseSelector& Geant4Kernel::PhaseSelector::operator=(const PhaseSelector& c) {
  if ( this != &c )  {
    m_kernel = c.m_kernel;
  }
  return *this;
}

/// Phase access to the map
Geant4ActionPhase& Geant4Kernel::PhaseSelector::operator[](const std::string& nam) const {
  Geant4ActionPhase* action_phase = m_kernel->getPhase(nam);
  if ( action_phase ) {
    return *action_phase;
  }
  throw runtime_error(format("Geant4Kernel", "Attempt to access the nonexisting phase '%s'", nam.c_str()));
}

/// Standard constructor
Geant4Kernel::Geant4Kernel(Detector& description_ref)
  : Geant4ActionContainer(), m_runManager(0), m_control(0), m_trackMgr(0), m_detDesc(&description_ref), 
    m_numThreads(0), m_id(Geant4Kernel::thread_self()), m_master(this), m_shared(0),
    m_threadContext(0), phase(this)
{
  //m_detDesc->addExtension < Geant4Kernel > (this);
  m_ident = -1;
  declareProperty("UI",m_uiName);
  declareProperty("OutputLevel",      m_outputLevel = DEBUG);
  declareProperty("NumEvents",        m_numEvent = 10);
  declareProperty("OutputLevels",     m_clientLevels);
  declareProperty("NumberOfThreads",  m_numThreads);
  declareProperty("DefaultSensitiveType", m_dfltSensitiveDetectorType = "Geant4SensDet");
  declareProperty("SensitiveTypes",   m_sensitiveDetectorTypes);
  declareProperty("RunManagerType",   m_runManagerType = "G4RunManager");
  m_controlName = "/ddg4/";
  m_control = new G4UIdirectory(m_controlName.c_str());
  m_control->SetGuidance("Control for named Geant4 actions");
  setContext(new Geant4Context(this));
  //m_shared = new Geant4Kernel(description_ref, this, -2);
  InstanceCount::increment(this);
}

/// Standard constructor
Geant4Kernel::Geant4Kernel(Geant4Kernel* m, unsigned long ident)
  : Geant4ActionContainer(), m_runManager(0), m_control(0), m_trackMgr(0), m_detDesc(0),
    m_numThreads(1), m_id(ident), m_master(m), m_shared(0),
    m_threadContext(0), phase(this)
{
  char text[64];
  m_detDesc        = m_master->m_detDesc;
  m_world          = m_master->m_world;
  m_ident          = m_master->m_workers.size();
  m_numEvent       = m_master->m_numEvent;
  m_runManagerType = m_master->m_runManagerType;
  m_sensitiveDetectorTypes      = m_master->m_sensitiveDetectorTypes;
  m_dfltSensitiveDetectorType   = m_master->m_dfltSensitiveDetectorType;
  declareProperty("UI",m_uiName = m_master->m_uiName);
  declareProperty("OutputLevel", m_outputLevel = m_master->m_outputLevel);
  declareProperty("OutputLevels",m_clientLevels = m_master->m_clientLevels);
  ::snprintf(text,sizeof(text),"/ddg4.%d/",(int)(m_master->m_workers.size()));
  m_controlName = text;
  m_control = new G4UIdirectory(m_controlName.c_str());
  m_control->SetGuidance("Control for thread specific Geant4 actions");
  setContext(new Geant4Context(this));
  InstanceCount::increment(this);
}

/// Default destructor
Geant4Kernel::~Geant4Kernel() {
  if ( this == s_main_instance.get() )   {
    s_main_instance.release();
  }
  detail::destroyObjects(m_workers);
  if ( isMaster() )  {
    detail::releaseObjects(m_globalFilters);
    detail::releaseObjects(m_globalActions);
  }
  destroyPhases();
  detail::deletePtr(m_runManager);
  Geant4ActionContainer::terminate();
  if ( m_detDesc && isMaster() )  {
    try  {
      //m_detDesc->removeExtension < Geant4Kernel > (false);
      m_detDesc->destroyInstance();
      m_detDesc = 0;
    }
    catch(...)  {
    }
  }
  InstanceCount::decrement(this);
}

/// Instance accessor
Geant4Kernel& Geant4Kernel::instance(Detector& description) {
  if ( 0 == s_main_instance.get() )   {
    G4AutoLock protection_lock(&kernel_mutex);    {
      if ( 0 == s_main_instance.get() )   { // Need to check again!
        s_main_instance.adopt(new Geant4Kernel(description));
      }
    }
  }
  return *(s_main_instance.get());
}

/// Access thread identifier
unsigned long int Geant4Kernel::thread_self()    {
  unsigned long int thr_id = (unsigned long int)::pthread_self();
  return thr_id;
}

/// Create identified worker instance
Geant4Kernel& Geant4Kernel::createWorker()   {
  if ( isMaster() )   {
    unsigned long identifier = thread_self();
    Geant4Kernel* w = new Geant4Kernel(this, identifier);
    m_workers[identifier] = w;
    printout(INFO,"Geant4Kernel","+++ Created worker instance id=%ul",identifier);
    return *w;
  }
  throw runtime_error(format("Geant4Kernel", "DDG4: Only the master instance may create workers."));
}

/// Access worker instance by it's identifier
Geant4Kernel& Geant4Kernel::worker(unsigned long identifier, bool create_if)    {
  Workers::iterator i = m_workers.find(identifier);
  if ( i != m_workers.end() )   {
    return *((*i).second);
  }
  else if ( identifier == m_id )  {
    return *this;
  }
  else if ( !isMultiThreaded() )  {
    unsigned long self = thread_self();
    if ( identifier == self )  {
      return *this;
    }
  }
  else if ( create_if )  {
    return createWorker();
  }
  throw runtime_error(format("Geant4Kernel", "DDG4: The Kernel object 0x%p does not exists!",(void*)identifier));
}

/// Access number of workers
int Geant4Kernel::numWorkers() const   {
  return m_workers.size();
}

/// Access to geometry world
G4VPhysicalVolume* Geant4Kernel::world()  const   {
  if ( this != m_master ) return m_master->world();
  return m_world;
}

/// Set the geometry world
void Geant4Kernel::setWorld(G4VPhysicalVolume* volume)  {
  if ( this == m_master ) m_world = volume;
  else m_master->setWorld(volume);
}

void Geant4Kernel::printProperties()  const  {
  printout(ALWAYS,"Geant4Kernel","OutputLevel:  %d", m_outputLevel);
  printout(ALWAYS,"Geant4Kernel","UI:           %s", m_uiName.c_str());
  printout(ALWAYS,"Geant4Kernel","NumEvents:    %ld",m_numEvent);
  printout(ALWAYS,"Geant4Kernel","NumThreads:   %d", m_numThreads);
  for(ClientOutputLevels::const_iterator i=m_clientLevels.begin(); i!=m_clientLevels.end();++i)  {
    printout(ALWAYS,"Geant4Kernel","OutputLevel[%s]:  %d",(*i).first.c_str(),(*i).second);
  }
}

/// Check property for existence
bool Geant4Kernel::hasProperty(const std::string& name) const    {
  return m_properties.exists(name);
}

/// Access single property
dd4hep::Property& Geant4Kernel::property(const std::string& name)   {
  return properties()[name];
}

/// Fill cache with the global output level of a named object. Must be set before instantiation
void Geant4Kernel::setOutputLevel(const std::string object, PrintLevel new_level)   {
  m_clientLevels[object] = new_level;
}

/// Retrieve the global output level of a named object.
dd4hep::PrintLevel Geant4Kernel::getOutputLevel(const std::string object) const   {
  ClientOutputLevels::const_iterator i=m_clientLevels.find(object);
  if ( i != m_clientLevels.end() ) return (PrintLevel)(*i).second;
  return dd4hep::PrintLevel(dd4hep::printLevel()-1);
}

/// Set the output level; returns previous value
dd4hep::PrintLevel Geant4Kernel::setOutputLevel(PrintLevel new_level)  {
  int old = m_outputLevel;
  m_outputLevel = new_level;
  return (PrintLevel)old;
}

/// Access to the Geant4 run manager
G4RunManager& Geant4Kernel::runManager() {
  if ( m_runManager )  {
    return *m_runManager;
  }
  else if ( isMaster() )   {
    Geant4Action* mgr =
      PluginService::Create<Geant4Action*>(m_runManagerType,
                                           m_context,
                                           string("Geant4RunManager"));
    if ( !mgr )   {
      except("Geant4Kernel",
             "+++ Invalid Geant4RunManager class: %s. Aborting.",
             m_runManagerType.c_str());
    }
    mgr->property("NumberOfThreads").set(m_numThreads);
    mgr->enableUI();
    m_runManager = dynamic_cast<G4RunManager*>(mgr);
    if ( m_runManager )  {
      return *m_runManager;
    }
    except("Geant4Kernel",
           "+++ Invalid Geant4RunManager action: %s. Invalid inheritance.",
           m_runManagerType.c_str());
  }
  except("Geant4Kernel", 
         "+++ Only the master thread may instantiate a G4RunManager object!");
  throw runtime_error("Is never called -- just to satisfy compiler!");
}

/// Construct detector geometry using description plugin
void Geant4Kernel::loadGeometry(const std::string& compact_file) {
  char* arg = (char*) compact_file.c_str();
  m_detDesc->apply("DD4hep_XMLLoader", 1, &arg);
  //return *this;
}

// Utility function to load XML files
void Geant4Kernel::loadXML(const char* fname) {
  const char* args[] = { fname, 0 };
  m_detDesc->apply("DD4hep_XMLLoader", 1, (char**) args);
}

int Geant4Kernel::configure() {
  return Geant4Exec::configure(*this);
}

int Geant4Kernel::initialize() {
  return Geant4Exec::initialize(*this);
}

int Geant4Kernel::run() {
  try  {
    return Geant4Exec::run(*this);
  }
  catch(const exception& e)   {
    printout(FATAL,"Geant4Kernel","+++ Exception while simulating:%s",e.what());
  }
  catch(...)   {
    printout(FATAL,"Geant4Kernel","+++ UNKNOWN exception while simulating.");
  }
  return 0;
}

int Geant4Kernel::runEvents(int num_events) {
  m_numEvent = num_events;
  return Geant4Exec::run(*this);
}

int Geant4Kernel::terminate() {
  const Geant4Kernel* ptr = s_main_instance.get();
  printout(INFO,"Geant4Kernel","++ Terminate Geant4 and delete associated actions.");
  if ( ptr == this )  {
    Geant4Exec::terminate(*this);
  }
  destroyPhases();
  detail::releaseObjects(m_globalFilters);
  detail::releaseObjects(m_globalActions);
  if ( ptr == this )  {
    detail::deletePtr(m_runManager);
  }
  Geant4ActionContainer::terminate();
  if ( ptr == this && m_detDesc )  {
    //m_detDesc->removeExtension < Geant4Kernel > (false);
    m_detDesc->destroyInstance();
    m_detDesc = 0;
  }
  return 1;
}

/// Register action by name to be retrieved when setting up and connecting action objects
/** Note: registered actions MUST be unique.
 *  However, not all actions need to registered....
 *  Only register those, you later need to retrieve by name.
 */
Geant4Kernel& Geant4Kernel::registerGlobalAction(Geant4Action* action) {
  if (action) {
    string nam = action->name();
    GlobalActions::const_iterator i = m_globalActions.find(nam);
    if (i == m_globalActions.end()) {
      action->addRef();
      m_globalActions[nam] = action;
      printout(INFO,"Geant4Kernel","++ Registered global action %s of type %s",
               nam.c_str(),typeName(typeid(*action)).c_str());
      return *this;
    }
    except("Geant4Kernel", "DDG4: The action '%s' is already globally "
           "registered. [Action-Already-Registered]", nam.c_str());
  }
  except("Geant4Kernel",
         "DDG4: Attempt to globally register an invalid action. [Action-Invalid]");
  return *this;
}

/// Retrieve action from repository
Geant4Action* Geant4Kernel::globalAction(const std::string& action_name, bool throw_if_not_present) {
  GlobalActions::iterator i = m_globalActions.find(action_name);
  if (i == m_globalActions.end()) {
    if (throw_if_not_present) {
       except("Geant4Kernel", "DDG4: The action '%s' is not globally "
              "registered. [Action-Missing]", action_name.c_str());
    }
    return 0;
  }
  return (*i).second;
}

/// Register filter by name to be retrieved when setting up and connecting filter objects
/** Note: registered filters MUST be unique.
 *  However, not all filters need to registered....
 *  Only register those, you later need to retrieve by name.
 */
Geant4Kernel& Geant4Kernel::registerGlobalFilter(Geant4Action* filter) {
  if (filter) {
    string nam = filter->name();
    GlobalActions::const_iterator i = m_globalFilters.find(nam);
    if (i == m_globalFilters.end()) {
      filter->addRef();
      m_globalFilters[nam] = filter;
      return *this;
    }
    except("Geant4Kernel", "DDG4: The filter '%s' is already globally "
           "registered. [Filter-Already-Registered]", nam.c_str());
  }
  except("Geant4Kernel",
         "DDG4: Attempt to globally register an invalid filter. [Filter-Invalid]");
  return *this;
}

/// Retrieve filter from repository
Geant4Action* Geant4Kernel::globalFilter(const std::string& filter_name, bool throw_if_not_present) {
  GlobalActions::iterator i = m_globalFilters.find(filter_name);
  if (i == m_globalFilters.end()) {
    if (throw_if_not_present) {
      except("Geant4Kernel", "DDG4: The filter '%s' is not already globally "
             "registered. [Filter-Missing]", filter_name.c_str());
    }
    return 0;
  }
  return (*i).second;
}

/// Execute phase action if it exists
bool Geant4Kernel::executePhase(const std::string& nam, const void** arguments)  const   {
  Phases::const_iterator i = m_phases.find(nam);
  if (i != m_phases.end())   {
    (*i).second->execute(arguments);
    return true;
  }
  return false;
}

/// Access phase by name
Geant4ActionPhase* Geant4Kernel::getPhase(const std::string& nam) {
  Phases::const_iterator i = m_phases.find(nam);
  if (i != m_phases.end()) {
    return (*i).second;
  }
  except("Geant4Kernel", "DDG4: The Geant4 action phase '%s' does not exist. [No-Entry]", nam.c_str());
  return 0;
}

/// Add a new phase to the phase
Geant4ActionPhase* Geant4Kernel::addSimplePhase(const std::string& name, bool throw_on_exist)   {
  return addPhase(name,typeid(void),typeid(void),typeid(void),throw_on_exist);
}

/// Add a new phase
Geant4ActionPhase* Geant4Kernel::addPhase(const std::string& nam, const type_info& arg0, const type_info& arg1,
                                          const type_info& arg2, bool throw_on_exist) {
  Phases::const_iterator i = m_phases.find(nam);
  if (i == m_phases.end()) {
    Geant4ActionPhase* p = new Geant4ActionPhase(workerContext(), nam, arg0, arg1, arg2);
    m_phases.emplace(nam, p);
    return p;
  }
  else if (throw_on_exist) {
    except("Geant4Kernel", "DDG4: The Geant4 action phase %s already exists. [Already-Exists]", nam.c_str());
  }
  return (*i).second;
}

/// Remove an existing phase from the phase. If not existing returns false
bool Geant4Kernel::removePhase(const std::string& nam) {
  Phases::iterator i = m_phases.find(nam);
  if (i != m_phases.end()) {
    delete (*i).second;
    m_phases.erase(i);
    return true;
  }
  return false;
}

/// Destroy all phases. To be called only at shutdown
void Geant4Kernel::destroyPhases() {
  detail::destroyObjects(m_phases);
}
