// $Id: $
//==========================================================================
//  AIDA Detector description implementation for LCD
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
#include "DD4hep/LCDD.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Primitives.h"
#include "DD4hep/InstanceCount.h"

#include "DDG4/Geant4Kernel.h"
#include "DDG4/Geant4Context.h"
#include "DDG4/Geant4ActionPhase.h"

#include "DDG4/Geant4RunAction.h"
#include "DDG4/Geant4PhysicsList.h"
#include "DDG4/Geant4EventAction.h"
#include "DDG4/Geant4SteppingAction.h"
#include "DDG4/Geant4TrackingAction.h"
#include "DDG4/Geant4StackingAction.h"
#include "DDG4/Geant4GeneratorAction.h"
#include "DDG4/Geant4DetectorConstruction.h"
#include "DDG4/Geant4UserInitialization.h"
#include "DDG4/Geant4SensDetAction.h"

// Geant4 include files
#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif
#include "G4UIdirectory.hh"

// C/C++ include files
#include <stdexcept>
#include <algorithm>

using namespace std;
using namespace DD4hep::Simulation;

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
Geant4Kernel::Geant4Kernel(LCDD& lcdd_ref)
  : m_runManager(0), m_generatorAction(0), m_runAction(0), m_eventAction(0),
    m_trackingAction(0), m_steppingAction(0), m_stackingAction(0), m_constructionAction(0),
    m_sensDetActions(0), m_physicsList(0), m_userInit(0), m_lcdd(lcdd_ref), 
    m_numThreads(0), m_id(pthread_self()), m_master(this), phase(this)  
{
  m_sensDetActions = new Geant4SensDetSequences();
  m_lcdd.addExtension < Geant4Kernel > (this);
  m_ident = -1;
  declareProperty("UI",m_uiName);
  declareProperty("OutputLevel",    m_outputLevel = DEBUG);
  declareProperty("NumEvents",      m_numEvent = 10);
  declareProperty("OutputLevels",   m_clientLevels);
  declareProperty("NumberOfThreads",m_numThreads);
  //declareProperty("MultiThreaded",  m_multiThreaded=false);
  m_controlName = "/ddg4/";
  m_control = new G4UIdirectory(m_controlName.c_str());
  m_control->SetGuidance("Control for named Geant4 actions");
  m_threadContext = new Geant4Context(this);
  InstanceCount::increment(this);
}

/// Standard constructor
Geant4Kernel::Geant4Kernel(LCDD& lcdd_ref, Geant4Kernel* m, unsigned long ident)
  : m_runManager(0), m_generatorAction(0), m_runAction(0), m_eventAction(0),
    m_trackingAction(0), m_steppingAction(0), m_stackingAction(0), m_constructionAction(0),
    m_sensDetActions(0), m_physicsList(0), m_userInit(0), m_lcdd(lcdd_ref), m_id(ident), 
    m_master(m), phase(this)
{
  char text[64];
  m_numThreads     = 1;
  //m_multiThreaded  = m_master->m_multiThreaded;
  m_ident          = m_master->m_workers.size();
  m_sensDetActions = new Geant4SensDetSequences();
  declareProperty("UI",m_uiName = m_master->m_uiName);
  declareProperty("OutputLevel", m_outputLevel = m_master->m_outputLevel);
  declareProperty("OutputLevels",m_clientLevels = m_master->m_clientLevels);
  ::snprintf(text,sizeof(text),"/ddg4.%d/",(int)(m_master->m_workers.size()));
  m_controlName = text;
  m_control = new G4UIdirectory(m_controlName.c_str());
  m_control->SetGuidance("Control for thread specific Geant4 actions");
  m_threadContext = new Geant4Context(this);
  InstanceCount::increment(this);
}

/// Default destructor
Geant4Kernel::~Geant4Kernel() {
  destroyObjects(m_workers)();
  if ( isMaster() )  {
    releaseObjects(m_globalFilters)();
    releaseObjects(m_globalActions)();
  }
  destroyPhases();
  deletePtr  (m_runManager);
  releasePtr (m_physicsList);
  releasePtr (m_constructionAction);
  releasePtr (m_stackingAction);
  releasePtr (m_steppingAction);
  releasePtr (m_trackingAction);
  releasePtr (m_eventAction);
  releasePtr (m_generatorAction);
  releasePtr (m_runAction);
  releasePtr (m_userInit);
  deletePtr  (m_sensDetActions);
  deletePtr  (m_threadContext);
  if ( isMaster() )  {
    m_lcdd.removeExtension < Geant4Kernel > (false);
    m_lcdd.destroyInstance();
  }
  InstanceCount::decrement(this);
}

/// Instance accessor
Geant4Kernel& Geant4Kernel::instance(LCDD& lcdd) {
  static Geant4Kernel obj(lcdd);
  return obj;
}

/// Accessof the Geant4Kernel object from the LCDD reference extension (if present and registered)
Geant4Kernel& Geant4Kernel::access(LCDD& lcdd) {
  Geant4Kernel* kernel = lcdd.extension<Geant4Kernel>();
  if (!kernel) {
    throw runtime_error(format("Geant4Kernel", "DDG4: The LCDD object has no registered "
                               "extension of type Geant4Kernel [No-Extension]"));
  }
  return *kernel;
}

Geant4Context* Geant4Kernel::workerContext()   {
  if ( m_threadContext ) return m_threadContext;
  throw runtime_error(format("Geant4Kernel", "DDG4: Master kernel object has no thread context! [Invalid Handle]"));
}

/// Create identified worker instance
Geant4Kernel& Geant4Kernel::createWorker()   {
  if ( isMaster() )   {
    unsigned long identifier = ::pthread_self();
    Geant4Kernel* w = new Geant4Kernel(m_lcdd, this, identifier);
    m_workers[identifier] = w;
    printout(INFO,"Geant4Kernel","+++ Created worker instance id=%ul",identifier);
    return *w;
  }
  throw runtime_error(format("Geant4Kernel", "DDG4: Only the master instance may create workers."));
}

/// Access worker instance by it's identifier
Geant4Kernel& Geant4Kernel::worker(unsigned long identifier)    {
  Workers::iterator i = m_workers.find(identifier);
  if ( i != m_workers.end() )   {
    return *((*i).second);
  }
  else if ( !isMultiThreaded() )  {
    unsigned long self = ::pthread_self();
    if ( identifier == self )  {
      return *this;
    }
  }
  throw runtime_error(format("Geant4Kernel", "DDG4: The Kernel object 0x%p does not exists!",(void*)identifier));
}

/// Access number of workers
int Geant4Kernel::numWorkers() const   {
  return m_workers.size();
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
DD4hep::Property& Geant4Kernel::property(const std::string& name)   {
  return properties()[name];
}

/// Fill cache with the global output level of a named object. Must be set before instantiation
void Geant4Kernel::setOutputLevel(const std::string object, PrintLevel new_level)   {
  m_clientLevels[object] = new_level;
}

/// Retrieve the global output level of a named object.
DD4hep::PrintLevel Geant4Kernel::getOutputLevel(const std::string object) const   {
  ClientOutputLevels::const_iterator i=m_clientLevels.find(object);
  if ( i != m_clientLevels.end() ) return (PrintLevel)(*i).second;
  return DD4hep::PrintLevel(DD4hep::printLevel()-1);
}

/// Set the output level; returns previous value
DD4hep::PrintLevel Geant4Kernel::setOutputLevel(PrintLevel new_level)  {
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
#ifdef G4MULTITHREADED
    if ( m_numThreads > 0 )   {
      printout(WARNING,"Geant4Kernel","+++ Multi-threaded mode requested with %d worker threads.",m_numThreads);
      G4MTRunManager* run_mgr = new G4MTRunManager;
      run_mgr->SetNumberOfThreads(m_numThreads);
      m_runManager = run_mgr;
      return *m_runManager;
    }
#endif
    if ( m_numThreads > 0 )   {
      printout(WARNING,"Geant4Kernel","+++ Multi-threaded mode requested, "
               "but not supported by this compilation of Geant4.");
      printout(WARNING,"Geant4Kernel","+++ Falling back to single threaded mode.");
      m_numThreads = 0;
    }
    return *(m_runManager = new G4RunManager);
  }
  throw runtime_error(format("Geant4Kernel", 
                             "DDG4: Only the master thread may instantiate "
                             "a G4RunManager object!"));
}

/// Construct detector geometry using lcdd plugin
void Geant4Kernel::loadGeometry(const std::string& compact_file) {
  char* arg = (char*) compact_file.c_str();
  m_lcdd.apply("DD4hepXMLLoader", 1, &arg);
  //return *this;
}

// Utility function to load XML files
void Geant4Kernel::loadXML(const char* fname) {
  const char* args[] = { fname, 0 };
  m_lcdd.apply("DD4hepXMLLoader", 1, (char**) args);
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
  Geant4Exec::terminate(*this);
  destroyPhases();
  for_each(m_globalFilters.begin(), m_globalFilters.end(), releaseObjects(m_globalFilters));
  for_each(m_globalActions.begin(), m_globalActions.end(), releaseObjects(m_globalActions));
  deletePtr  (m_runManager);
  releasePtr (m_physicsList);
  releasePtr (m_constructionAction);
  releasePtr (m_stackingAction);
  releasePtr (m_steppingAction);
  releasePtr (m_trackingAction);
  releasePtr (m_eventAction);
  releasePtr (m_generatorAction);
  releasePtr (m_runAction);
  deletePtr  (m_sensDetActions);
  //return *this;
  return 1;
}

template <class C> bool Geant4Kernel::registerSequence(C*& seq, const std::string& name) {
  if (!name.empty()) {
    seq = new C(workerContext(), name);
    seq->installMessengers();
    return true;
  }
  throw runtime_error(format("Geant4Kernel", "DDG4: The action '%s' not found. [Action-NotFound]", name.c_str()));
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
    throw runtime_error(format("Geant4Kernel", "DDG4: The action '%s' is already globally "
                               "registered. [Action-Already-Registered]", nam.c_str()));
  }
  throw runtime_error(format("Geant4Kernel", "DDG4: Attempt to globally register an invalid "
                             "action. [Action-Invalid]"));
}

/// Retrieve action from repository
Geant4Action* Geant4Kernel::globalAction(const std::string& action_name, bool throw_if_not_present) {
  GlobalActions::iterator i = m_globalActions.find(action_name);
  if (i == m_globalActions.end()) {
    if (throw_if_not_present) {
      throw runtime_error(format("Geant4Kernel", "DDG4: The action '%s' is not globally "
                                 "registered. [Action-Missing]", action_name.c_str()));
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
    throw runtime_error(format("Geant4Kernel", "DDG4: The filter '%s' is already globally "
                               "registered. [Filter-Already-Registered]", nam.c_str()));
  }
  throw runtime_error(format("Geant4Kernel", "DDG4: Attempt to globally register an invalid "
                             "filter. [Filter-Invalid]"));
}

/// Retrieve filter from repository
Geant4Action* Geant4Kernel::globalFilter(const std::string& filter_name, bool throw_if_not_present) {
  GlobalActions::iterator i = m_globalFilters.find(filter_name);
  if (i == m_globalFilters.end()) {
    if (throw_if_not_present) {
      throw runtime_error(format("Geant4Kernel", "DDG4: The filter '%s' is not already globally "
                                 "registered. [Filter-Missing]", filter_name.c_str()));
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
  throw runtime_error(format("Geant4Kernel", "DDG4: The Geant4 action phase '%s' "
                             "does not exist. [No-Entry]", nam.c_str()));
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
    m_phases.insert(make_pair(nam, p));
    return p;
  }
  else if (throw_on_exist) {
    throw runtime_error(format("Geant4Kernel", "DDG4: The Geant4 action phase %s "
                               "already exists. [Already-Exists]", nam.c_str()));
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
  for_each(m_phases.begin(), m_phases.end(), destroyObjects(m_phases));
}

/// Access generator action sequence
Geant4GeneratorActionSequence* Geant4Kernel::generatorAction(bool create) {
  if (!m_generatorAction && create)
    registerSequence(m_generatorAction, "GeneratorAction");
  return m_generatorAction;
}

/// Access run action sequence
Geant4RunActionSequence* Geant4Kernel::runAction(bool create) {
  if (!m_runAction && create)
    registerSequence(m_runAction, "RunAction");
  return m_runAction;
}

/// Access event action sequence
Geant4EventActionSequence* Geant4Kernel::eventAction(bool create) {
  if (!m_eventAction && create)
    registerSequence(m_eventAction, "EventAction");
  return m_eventAction;
}

/// Access stepping action sequence
Geant4SteppingActionSequence* Geant4Kernel::steppingAction(bool create) {
  if (!m_steppingAction && create)
    registerSequence(m_steppingAction, "SteppingAction");
  return m_steppingAction;
}

/// Access tracking action sequence
Geant4TrackingActionSequence* Geant4Kernel::trackingAction(bool create) {
  if (!m_trackingAction && create)
    registerSequence(m_trackingAction, "TrackingAction");
  return m_trackingAction;
}

/// Access stacking action sequence
Geant4StackingActionSequence* Geant4Kernel::stackingAction(bool create) {
  if (!m_stackingAction && create)
    registerSequence(m_stackingAction, "StackingAction");
  return m_stackingAction;
}

/// Access detector construcion action sequence (geometry+sensitives+field)
Geant4DetectorConstructionSequence* Geant4Kernel::detectorConstruction(bool create)  {
  if (!m_constructionAction && create)
    registerSequence(m_constructionAction, "StackingAction");
  return m_constructionAction;
}

/// Access to the sensitive detector sequences from the kernel object
Geant4SensDetSequences& Geant4Kernel::sensitiveActions() const {
  return *m_sensDetActions;
}

/// Access to the sensitive detector action from the kernel object
Geant4SensDetActionSequence* Geant4Kernel::sensitiveAction(const string& nam) {
  Geant4SensDetActionSequence* ptr = m_sensDetActions->find(nam);
  if (ptr)   {
    return ptr;
  }
  ptr = new Geant4SensDetActionSequence(workerContext(), nam);
  m_sensDetActions->insert(nam, ptr);
  return ptr;
}

/// Access to the physics list
Geant4PhysicsListActionSequence* Geant4Kernel::physicsList(bool create) {
  if (!m_physicsList && create)
    registerSequence(m_physicsList, "PhysicsList");
  return m_physicsList;
}

/// Access to the physics list
Geant4UserInitializationSequence* Geant4Kernel::userInitialization(bool create) {
  if ( !m_userInit && create )   {
    if ( isMaster() )
      registerSequence(m_userInit, "UserInitialization");
    else 
      throw runtime_error(format("Geant4Kernel", 
                                 "DDG4: Only the master thread may initialize "
                                 "a user initialization object!"));
  }
  return m_userInit;
}
