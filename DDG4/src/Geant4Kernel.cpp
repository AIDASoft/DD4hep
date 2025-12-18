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
#include <DD4hep/Detector.h>
#include <DD4hep/Memory.h>
#include <DD4hep/Plugins.h>
#include <DD4hep/Printout.h>
#include <DD4hep/Primitives.h>
#include <DD4hep/InstanceCount.h>

#include <DDG4/Geant4Kernel.h>
#include <DDG4/Geant4Context.h>
#include <DDG4/Geant4Interrupts.h>
#include <DDG4/Geant4ActionPhase.h>

// Geant4 include files
#include <G4RunManager.hh>
#include <G4ScoringManager.hh>
#include <G4UIdirectory.hh>
#include <G4Threading.hh>
#include <G4AutoLock.hh>

// C/C++ include files
#include <algorithm>
#include <pthread.h>
#include <csignal>
#include <memory>

using namespace dd4hep::sim;

namespace {

  G4Mutex kernel_mutex = G4MUTEX_INITIALIZER;
  Geant4Kernel* s_main_instance = nullptr;
  void description_unexpected()    {
    try  {
      throw;
    }
    catch( std::exception& e )  {
      std::cout << "\n"
           << "**************************************************** \n"
           << "*  A runtime error has occured :                     \n"
           << "*    " << e.what()   <<  std::endl
           << "*  the program will have to be terminated - sorry.   \n"
           << "**************************************************** \n"
           <<  std::endl;
      // this provokes ROOT seg fault and stack trace (comment out to avoid it)
      ::exit(1) ;
    }
  }
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
  if( Geant4ActionPhase* action_phase = m_kernel->getPhase(nam) )
    return *action_phase;
  throw except("Geant4Kernel", "Attempt to access the nonexisting phase '%s'", nam.c_str());
}

/// Standard constructor
Geant4Kernel::Geant4Kernel(Detector& description_ref)
  : Geant4ActionContainer(), m_detDesc(&description_ref), 
    m_id(Geant4Kernel::thread_self()), m_master(this), phase(this)
{
  m_ident = -1;
  declareProperty("UI",                   m_uiName);
  declareProperty("OutputLevel",          m_outputLevel = DEBUG);
  declareProperty("NumEvents",            m_numEvent = 10);
  declareProperty("OutputLevels",         m_clientLevels);
  declareProperty("NumberOfThreads",      m_numThreads = 0);
  declareProperty("HaveScoringManager",   m_haveScoringMgr = false);
  declareProperty("SensitiveTypes",       m_sensitiveDetectorTypes);
  declareProperty("RunManagerType",       m_runManagerType = "G4RunManager");
  declareProperty("DefaultSensitiveType", m_dfltSensitiveDetectorType = "Geant4SensDet");
  m_interrupts = new Geant4Interrupts(*this);
  m_controlName = "/ddg4/";
  m_control = new G4UIdirectory(m_controlName.c_str());
  m_control->SetGuidance("Control for named Geant4 actions");
  setContext(new Geant4Context(this));
  InstanceCount::increment(this);
}

/// Standard constructor
Geant4Kernel::Geant4Kernel(Geant4Kernel* krnl, unsigned long ident)
  : Geant4ActionContainer(), m_id(ident), m_master(krnl), phase(this)
{
  char text[64];
  m_numThreads     = 1; // Slave instance for one single thread
  m_detDesc        = m_master->m_detDesc;
  m_world          = m_master->m_world;
  m_ident          = m_master->m_workers.size();
  m_numEvent       = m_master->m_numEvent;
  declareProperty("RunManagerType", m_runManagerType = m_master->m_runManagerType);
  m_sensitiveDetectorTypes      = m_master->m_sensitiveDetectorTypes;
  m_dfltSensitiveDetectorType   = m_master->m_dfltSensitiveDetectorType;
  declareProperty("UI",m_uiName = m_master->m_uiName);
  declareProperty("OutputLevel",  m_outputLevel  = m_master->m_outputLevel);
  declareProperty("OutputLevels", m_clientLevels = m_master->m_clientLevels);
  ::snprintf(text, sizeof(text), "/ddg4.%d/", (int)(m_master->m_workers.size()));
  m_controlName = text;
  m_control = new G4UIdirectory(m_controlName.c_str());
  m_control->SetGuidance("Control for thread specific Geant4 actions");
  setContext(new Geant4Context(this));
  InstanceCount::increment(this);
}

/// Default destructor
Geant4Kernel::~Geant4Kernel() {
  if ( this == s_main_instance )   {
    s_main_instance = nullptr;
  }
  detail::destroyObjects(m_workers);
  if ( isMaster() )  {
    detail::releaseObjects(m_globalFilters);
    detail::releaseObjects(m_globalActions);
    detail::deletePtr(m_interrupts);
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
  if ( nullptr == s_main_instance )   {
    G4AutoLock protection_lock(&kernel_mutex);    {
      if ( nullptr == s_main_instance )   { // Need to check again!
        /// Install here the termination handler
        std::set_terminate(description_unexpected);
        s_main_instance = new Geant4Kernel(description);
      }
    }
  }
  return *s_main_instance;
}

/// Access interrupt handler. Will be created on the first call
Geant4Interrupts& Geant4Kernel::interruptHandler()  const  {
  if ( isMaster() )
    return *this->m_interrupts;	
  return this->m_master->interruptHandler();
}

/// Trigger smooth end-of-event-loop with finishing currently processing event
void Geant4Kernel::triggerStop()  {
  printout(INFO, "Geant4Kernel",
	   "+++ Stop signal seen. Will finish after current event(s) have been processed.");
  printout(INFO, "Geant4Kernel",
	   "+++ Depending on the complexity of the simulation, this may take some time ...");
  this->m_master->m_processEvents = EVENTLOOP_HALT;
}

/// Access flag if event loop is enabled
bool Geant4Kernel::processEvents()  const  {
  return this->m_master->m_processEvents == EVENTLOOP_RUNNING;
}

/// Install DDG4 default handler for a given signal. If no handler: return false
bool Geant4Kernel::registerInterruptHandler(int sig_num)   {
  if ( sig_num == SIGINT )  {
    return interruptHandler().registerHandler_SIGINT();
  }
  return false;
}

/// (Re-)apply registered interrupt handlers to override potentially later registrations by other libraries
void Geant4Kernel::applyInterruptHandlers()  {
  interruptHandler().applyHandlers();
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
    printout(INFO, "Geant4Kernel", "+++ Created worker instance id=%ul",identifier);
    return *w;
  }
  except("Geant4Kernel", "DDG4: Only the master instance may create workers.");
  throw std::runtime_error("Geant4Kernel::createWorker");
}

/// Access worker instance by its identifier
Geant4Kernel& Geant4Kernel::worker(unsigned long identifier, bool create_if)    {
  if ( Workers::iterator i=m_workers.find(identifier); i != m_workers.end() )   {
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
  else if( create_if )  {
    return createWorker();
  }
  except("Geant4Kernel", "DDG4: The Kernel object 0x%p does not exists!",(void*)identifier);
  throw std::runtime_error("Geant4Kernel::worker");
}

/// Access number of workers
int Geant4Kernel::numWorkers() const   {
  return m_workers.size();
}

/// Access to geometry world
G4VPhysicalVolume* Geant4Kernel::world()  const   {
  if( this != m_master ) return m_master->world();
  return m_world;
}

/// Set the geometry world
void Geant4Kernel::setWorld(G4VPhysicalVolume* volume)  {
  if( this == m_master ) m_world = volume;
  else m_master->setWorld(volume);
}

/// Add new sensitive type to factory list
void Geant4Kernel::defineSensitiveDetectorType(const std::string& type, const std::string& factory)  {
  auto iter = m_sensitiveDetectorTypes.find(type);
  if( iter == m_sensitiveDetectorTypes.end() )  {
    printout(INFO,"Geant4Kernel","+++ Define sensitive type: %s -> %s", type.c_str(), factory.c_str());
    m_sensitiveDetectorTypes.emplace(type, factory);
    return;
  }
  else if( iter->first == type && iter->second == factory )  {
    return;
  }
  except("Geant4Kernel",
         "+++ The sensitive type %s is already defined and used %s. Cannot overwrite with %s",
         type.c_str(), iter->second.c_str(), factory.c_str());
}

void Geant4Kernel::printProperties()  const  {
  printout(ALWAYS,"Geant4Kernel","OutputLevel:  %d",  m_outputLevel);
  printout(ALWAYS,"Geant4Kernel","UI:           %s",  m_uiName.c_str());
  printout(ALWAYS,"Geant4Kernel","NumEvents:    %ld", m_numEvent);
  printout(ALWAYS,"Geant4Kernel","NumThreads:   %d",  m_numThreads);
  for( const auto& [name, level] : m_clientLevels )
    printout(ALWAYS,"Geant4Kernel","OutputLevel[%s]:  %d", name.c_str(), level);
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
  if( auto i=m_clientLevels.find(object); i != m_clientLevels.end() )
    return (PrintLevel)(*i).second;
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
                                           std::string("Geant4RunManager"));
    if ( !mgr )   {
      except("Geant4Kernel",
             "+++ Invalid Geant4RunManager class: %s. Aborting.",
             m_runManagerType.c_str());
    }
    mgr->property("NumberOfThreads").set(m_numThreads);
    mgr->enableUI();
    if ( this->m_haveScoringMgr )  {
      if ( nullptr == G4ScoringManager::GetScoringManager() )  {
        except("Geant4Kernel", "+++ FAILED to create the G4ScoringManager instance.");
      }
    }
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
  throw std::runtime_error("Is never called -- just to satisfy compiler!");
}

/// Construct detector geometry using description plugin
void Geant4Kernel::loadGeometry(const std::string& compact_file)  {
  char* arg = (char*) compact_file.c_str();
  m_detDesc->apply("DD4hep_XMLLoader", 1, &arg);
  //return *this;
}

// Utility function to load XML files
void Geant4Kernel::loadXML(const char* fname)  {
  const char* args[] = { fname, 0 };
  m_detDesc->apply("DD4hep_XMLLoader", 1, (char**) args);
}

/// Run dd4hep plugin
long Geant4Kernel::runPlugin( const std::string& plugin, const std::vector<std::string>& args )  {
  std::vector<const char*> arguments;
  arguments.reserve( args.size()+1 );
  for( const auto& a : args )
    arguments.push_back( a.c_str() );
  arguments.push_back( nullptr );
  return m_detDesc->apply( plugin.c_str(), args.size(), (char**) &arguments.at(0) );
}

/// Register configure callback
void Geant4Kernel::register_configure(const std::function<void()>& callback)  {
  m_actionConfigure.push_back(callback);
}

/// Register initialize callback
void Geant4Kernel::register_initialize(const std::function<void()>& callback)  {
  m_actionInitialize.push_back(callback);
}

/// Register terminate callback
void Geant4Kernel::register_terminate(const std::function<void()>& callback)  {
  m_actionTerminate.push_back(callback);
}

/// Configure Geant4 kernel object
int Geant4Kernel::configure() {
  int status = Geant4Exec::configure(*this);
  if ( status )   {
    for(auto& call : m_actionConfigure) call();
    return status;
  }
  except("Geant4Kernel","++ FAILED to configure DDG4 executive");
  return status;
}

/// Initialize Geant4 kernel object
int Geant4Kernel::initialize() {
  int status = Geant4Exec::initialize(*this);
  if ( status )   {
    for(auto& call : m_actionInitialize) call();
    return status;
  }
  except("Geant4Kernel","++ FAILED to initialize DDG4 executive");
  return status;
}

/// Run Geant4
int Geant4Kernel::run() {
  try  {
    auto result = Geant4Exec::run(*this);
    // flush the geant4 stream buffer
    G4cout << G4endl;
    return result;
  }
  catch(const std::exception& e)   {
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
  const Geant4Kernel* ptr = s_main_instance;
  printout(INFO,"Geant4Kernel","++ Terminate Geant4 and delete associated actions.");
  if ( ptr == this )  {
    auto calls = std::move(m_actionTerminate);
    for(auto& call : calls) call();
    Geant4Exec::terminate(*this);
    m_actionTerminate = std::move(calls);
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
  if( action ) {
    const std::string& nam = action->name();
    if( auto i=m_globalActions.find(nam); i == m_globalActions.end() ) {
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
Geant4Action* Geant4Kernel::globalAction(const std::string& nam, bool throw_if_not_present) {
  if( auto i=m_globalActions.find(nam); i != m_globalActions.end() )
    return (*i).second;
  if( throw_if_not_present )   {
    except("Geant4Kernel", "DDG4: The action '%s' is not globally "
           "registered. [Action-Missing]", nam.c_str());
  }
  return nullptr;
}

/// Register filter by name to be retrieved when setting up and connecting filter objects
/** Note: registered filters MUST be unique.
 *  However, not all filters need to registered....
 *  Only register those, you later need to retrieve by name.
 */
Geant4Kernel& Geant4Kernel::registerGlobalFilter(Geant4Action* filter) {
  if( filter )   {
    const std::string& nam = filter->name();
    if( auto i=m_globalFilters.find(nam); i == m_globalFilters.end()) {
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
  if( auto i=m_globalFilters.find(filter_name); i != m_globalFilters.end())
    return (*i).second;
  if (throw_if_not_present) {
    except("Geant4Kernel", "DDG4: The filter '%s' is not already globally "
           "registered. [Filter-Missing]", filter_name.c_str());
  }
  return nullptr;
}

/// Execute phase action if it exists
bool Geant4Kernel::executePhase(const std::string& nam, const void** arguments)  const   {
  if( auto i=m_phases.find(nam); i != m_phases.end() )   {
    (*i).second->execute(arguments);
    return true;
  }
  return false;
}

/// Access phase by name
Geant4ActionPhase* Geant4Kernel::getPhase(const std::string& nam)   {
  if( auto i=m_phases.find(nam); i != m_phases.end() )
    return (*i).second;
  except("Geant4Kernel", "DDG4: The Geant4 action phase '%s' does not exist. [No-Entry]", nam.c_str());
  return nullptr;
}

/// Add a new phase to the phase
Geant4ActionPhase* Geant4Kernel::addSimplePhase(const std::string& name, bool throw_on_exist)   {
  return addPhase(name,typeid(void),typeid(void),typeid(void),throw_on_exist);
}

/// Add a new phase
Geant4ActionPhase* Geant4Kernel::addPhase(const std::string& nam,
                                          const std::type_info& arg0,
                                          const std::type_info& arg1,
                                          const std::type_info& arg2,
                                          bool throw_on_exist)
{
  if( auto i=m_phases.find(nam); i == m_phases.end() )   {
    Geant4ActionPhase* p = new Geant4ActionPhase(workerContext(), nam, arg0, arg1, arg2);
    m_phases.emplace(nam, p);
    return p;
  }
  else if (throw_on_exist) {
    except("Geant4Kernel", "DDG4: The Geant4 action phase %s already exists. [Already-Exists]", nam.c_str());
  }
  return nullptr;
}

/// Remove an existing phase from the phase. If not existing returns false
bool Geant4Kernel::removePhase(const std::string& nam) {
  if( auto i=m_phases.find(nam); i != m_phases.end() )   {
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
