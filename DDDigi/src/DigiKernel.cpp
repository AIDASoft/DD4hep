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

#include <DDDigi/DigiKernel.h>
#include <DDDigi/DigiContext.h>
#include <DDDigi/DigiActionSequence.h>

#ifdef DD4HEP_USE_TBB
#include <tbb/tbb.h>
#endif

// C/C++ include files
#include <stdexcept>
#include <algorithm>
#include <memory>
#include <chrono>

using namespace dd4hep;
using namespace dd4hep::digi;
namespace  {
  static std::mutex kernel_mutex;
}

/// DigiKernel herlp class: Container of instance variabled
/*
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \ingroup DD4HEP_DIGITIZATION
 */
class DigiKernel::Internals   {
public:
  /// Property pool
  PropertyManager       properties;
  /// Property: Client output levels
  ClientOutputLevels    clientLevels;
  /// Atomic counter: Number of events still to be processed in this run
  std::atomic_int       eventsToDo;
  /// Atomic counter: Number of events still to be processed in this run
  std::atomic_int       eventsFinished;
  /// Atomic counter: Number of events still to be processed in this run
  std::size_t           eventsSubmitted;

  /// Lock to ensure counter safety
  std::mutex            counter_lock        { };

  /// Lock to ensure initialization safetyp
  std::mutex            initializer_lock    { };

  /// Lock for global I/O dependencies
  std::mutex            global_io_lock      { };

  /// Lock for global output logging
  std::mutex            global_output_lock  { };

  /// The main data input action sequence
  DigiActionSequence*   inputAction = 0;
  /// The main event action sequence
  DigiActionSequence*   eventAction = 0;
  /// The main data output action sequence
  DigiActionSequence*   outputAction = 0;
  /// TBB initializer (If TBB is used)
  void*                 tbbInit = 0;
  /// Property: Output level
  int                   outputLevel;
  /// Property: maximum number of events to be processed (if < 0: infinite)
  int                   numEvents;
  /// Property: maximum number of events to be processed in parallel (if TBB)
  int                   maxEventsParallel;
  /// Property: maximum number of threads to be used (if TBB)
  int                   numThreads;
  /// Property: Allow to stop execution from interactive prompt
  bool                  stop = false;
  Internals() = default;
  ~Internals() = default;
};

/// DigiKernel herlp class: TBB wrapper to execute DigiAction instances
/*
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \ingroup DD4HEP_DIGITIZATION
 */
class DigiKernel::Wrapper : public CallWrapper {
public:
  DigiContext& context;
  DigiEventAction*  action = 0;
  Wrapper(DigiContext& c, DigiEventAction* a)
    : context(c), action(a) {}
  Wrapper(Wrapper&& copy) = default;
  Wrapper(const Wrapper& copy) = default;
  Wrapper& operator=(Wrapper&& copy) = delete;
  Wrapper& operator=(const Wrapper& copy) = delete;
  void operator()() const {
    action->execute(context);
  }
};

/// DigiKernel herlp class: TBB wrapper to execute a full event
/*
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \ingroup DD4HEP_DIGITIZATION
 */
class DigiKernel::Processor { 
  DigiKernel& kernel;
public: 
  Processor(DigiKernel& k) : kernel(k) {}
  Processor(Processor&& l) = default;
  Processor(const Processor& l) = default;
  void operator()()  const {
    int todo = 1;
    while( todo >= 0 )   {
      todo = -1;
      {
        std::lock_guard<std::mutex> lock(kernel.internals->counter_lock);
        if( !kernel.internals->stop && kernel.internals->eventsToDo > 0)
          todo = --kernel.internals->eventsToDo;
      }
      if ( todo >= 0 )   {
        int ev_num = kernel.internals->numEvents - todo;
	std::unique_ptr<DigiContext> context = 
	  std::make_unique<DigiContext>(this->kernel,std::make_unique<DigiEvent>(ev_num));
        kernel.executeEvent(std::move(context));
      }
    }
  }
};


/// Standard constructor
DigiKernel::DigiKernel(Detector& description_ref)
  : m_detDesc(&description_ref)
{
  internals = new Internals();
#ifdef DD4HEP_USE_TBB
  internals->numThreads = tbb::task_scheduler_init::default_num_threads();
#else
  internals->numThreads = -1;
#endif
  declareProperty("maxEventsParallel",internals->maxEventsParallel = 1);
  declareProperty("numThreads",       internals->numThreads);
  declareProperty("numEvents",        internals->numEvents = 10);
  declareProperty("stop",             internals->stop = false);
  declareProperty("OutputLevel",      internals->outputLevel = DEBUG);
  declareProperty("OutputLevels",     internals->clientLevels);
  internals->inputAction  = new DigiActionSequence(*this, "InputAction");
  internals->eventAction  = new DigiActionSequence(*this, "EventAction");
  internals->outputAction = new DigiActionSequence(*this, "OutputAction");
  internals->inputAction->setExecuteParallel(false);
  internals->eventAction->setExecuteParallel(false);
  internals->outputAction->setExecuteParallel(false);
  InstanceCount::increment(this);
}

/// Default destructor
DigiKernel::~DigiKernel() {
  std::lock_guard<std::mutex> lock(kernel_mutex);
#ifdef DD4HEP_USE_TBB
  tbb::task_scheduler_init* init = (tbb::task_scheduler_init*)internals->tbbInit;
  if ( init ) delete init;
#endif
  detail::releasePtr(internals->outputAction);
  detail::releasePtr(internals->eventAction);
  detail::releasePtr(internals->inputAction);
  detail::deletePtr(internals);
  InstanceCount::decrement(this);
}

/// Instance accessor
DigiKernel& DigiKernel::instance(Detector& description) {
  static dd4hep::dd4hep_ptr<DigiKernel> s_main_instance(0);
  if ( 0 == s_main_instance.get() )   {
    std::lock_guard<std::mutex> lock(kernel_mutex);
    if ( 0 == s_main_instance.get() )   { // Need to check again!
      s_main_instance.adopt(new DigiKernel(description));
    }
  }
  return *(s_main_instance.get());
}

/// Have a shared initializer lock
std::mutex& DigiKernel::initializer_lock()   const  {
  return internals->initializer_lock;
}

/// Have a global output log lock
std::mutex& DigiKernel::global_output_lock()   const   {
  return internals->global_output_lock;
}

/// Have a global I/O lock (e.g. for ROOT)
std::mutex& DigiKernel::global_io_lock()   const  {
  return internals->global_io_lock;
}

/// Access to the properties of the object
PropertyManager& DigiKernel::properties()   {
  return internals->properties;
}

/// Print the property values
void DigiKernel::printProperties()  const  {
  printout(ALWAYS, "DigiKernel", "OutputLevel:  %d", internals->outputLevel);
  for( const auto& cl : internals->clientLevels )  {
    printout(ALWAYS, "DigiKernel", "OutputLevel[%s]:  %d", cl.first.c_str(), cl.second);
  }
}

/// Check property for existence
bool DigiKernel::hasProperty(const std::string& name) const    {
  return internals->properties.exists(name);
}

/// Access single property
dd4hep::Property& DigiKernel::property(const std::string& name)   {
  return internals->properties[name];
}

/// Access the output level
PrintLevel DigiKernel::outputLevel() const  {
  return (PrintLevel)internals->outputLevel;
}

/// Fill cache with the global output level of a named object. Must be set before instantiation
void DigiKernel::setOutputLevel(const std::string object, PrintLevel new_level)   {
  internals->clientLevels[object] = new_level;
}

/// Retrieve the global output level of a named object.
dd4hep::PrintLevel DigiKernel::getOutputLevel(const std::string object) const   {
  ClientOutputLevels::const_iterator i=internals->clientLevels.find(object);
  if ( i != internals->clientLevels.end() ) return (PrintLevel)(*i).second;
  return dd4hep::PrintLevel(dd4hep::printLevel()-1);
}

/// Set the output level; returns previous value
dd4hep::PrintLevel DigiKernel::setOutputLevel(PrintLevel new_level)  {
  int old = internals->outputLevel;
  internals->outputLevel = new_level;
  return (PrintLevel)old;
}

/// Access current number of events still to process
std::size_t DigiKernel::events_todo()  const   {
  std::lock_guard<std::mutex> lock(this->internals->counter_lock);
  std::size_t evts = this->internals->eventsToDo;
  return evts;
}

/// Access current number of events already processed
std::size_t DigiKernel::events_done()  const   {
  std::lock_guard<std::mutex> lock(this->internals->counter_lock);
  std::size_t evts = this->internals->numEvents - this->internals->eventsToDo;
  return evts;
}

/// Access current number of events processing (events in flight)
std::size_t DigiKernel::events_processing()  const   {
  std::lock_guard<std::mutex> lock(this->internals->counter_lock);
  std::size_t evts = this->internals->eventsSubmitted - this->internals->eventsFinished;
  return evts;
}

/// Construct detector geometry using description plugin
void DigiKernel::loadGeometry(const std::string& compact_file) {
  char* arg = (char*) compact_file.c_str();
  m_detDesc->apply("DD4hep_XMLLoader", 1, &arg);
}

// Utility function to load XML files
void DigiKernel::loadXML(const char* fname) {
  const char* args[] = { fname, 0 };
  m_detDesc->apply("DD4hep_XMLLoader", 1, (char**) args);
}

int DigiKernel::configure()   {
  return 1;//DigiExec::configure(*this);
}

int DigiKernel::initialize()   {
  return 1;//DigiExec::initialize(*this);
}

/// Access to the main input action sequence from the kernel object
DigiActionSequence& DigiKernel::inputAction() const    {
  return *internals->inputAction;
}

/// Access to the main event action sequence from the kernel object
DigiActionSequence& DigiKernel::eventAction() const    {
  return *internals->eventAction;
}

/// Access to the main output action sequence from the kernel object
DigiActionSequence& DigiKernel::outputAction() const    {
  return *internals->outputAction;
}

/// Submit a bunch of actions to be executed in parallel
void DigiKernel::submit (const std::vector<CallWrapper*>& actions)  const    {
#ifdef DD4HEP_USE_TBB
  bool parallel = 0 != internals->tbbInit && internals->numThreads>0;
  if ( parallel )   {
    tbb::task_group que;
    for ( auto* algo : actions )
      que.run( *algo );
    que.wait();
    return;
  }
#endif
  for ( auto* algo : actions )
    (*algo)();
}

/// Submit a bunch of actions to be executed serially
void DigiKernel::execute(const std::vector<CallWrapper*>& actions)  const    {
  for ( auto* algo : actions )
    (*algo)();
}

void DigiKernel::submit(const DigiAction::Actors<DigiEventAction>& actions, DigiContext& context)   const  {
  std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
  bool parallel = 0 != internals->tbbInit && internals->numThreads>0;
#ifdef DD4HEP_USE_TBB
  if ( parallel )   {
    tbb::task_group que;
    for ( auto* i : actions )
      que.run( Wrapper(context, i) );
    que.wait();
    goto print_stamp;
  }
#endif
  actions(&DigiEventAction::execute,context);
  goto print_stamp;

 print_stamp:
  std::chrono::duration<double> secs = std::chrono::system_clock::now() - start;
  printout(DEBUG, "DigiKernel", "+++ Event: %8d Executed %s task group with %3ld members [%8.3g sec]",
           context.event->eventNumber, parallel ? "parallel" : "serial", actions.size(),
           secs.count());
}

void DigiKernel::execute(const DigiAction::Actors<DigiEventAction>& actions, DigiContext& context)   const  {
  actions(&DigiEventAction::execute,context);
}

void DigiKernel::wait(DigiContext& context)   const  {
  if ( context.event ) {}
}

/// Execute one single event
void DigiKernel::executeEvent(std::unique_ptr<DigiContext>&& context)    {
  DigiContext& refContext = *context;
  try {
    inputAction().execute(refContext);
    eventAction().execute(refContext);
    outputAction().execute(refContext);
    notify(std::move(context));
  }
  catch(const std::exception& e)   {
    notify(std::move(context), e);
  }
}

void DigiKernel::notify(std::unique_ptr<DigiContext>&& context)   {
  if ( context )   {
    context->event.reset();
  }
  context.reset();
  ++internals->eventsFinished;
}

void DigiKernel::notify(std::unique_ptr<DigiContext>&& context, const std::exception& e)   {
  internals->stop = true;
  printout(ERROR,"DigiKernel","+++ Exception during event processing [Shall stop the event loop]");
  printout(ERROR,"DigiKernel"," -> %s", e.what());
  notify(std::move(context));
}

int DigiKernel::run()   {
  std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
  internals->stop = false;
  internals->eventsFinished = 0;
  internals->eventsSubmitted = 0;
  internals->eventsToDo = internals->numEvents;
  printout(INFO,
           "DigiKernel","+++ Total number of events:    %d",internals->numEvents);
#ifdef DD4HEP_USE_TBB
  if ( 0 == internals->tbbInit && internals->numThreads>=0 )   {
    if ( 0 == internals->numThreads )
      internals->numThreads = tbb::task_scheduler_init::default_num_threads();
    printout(INFO, "DigiKernel", "+++ Number of TBB threads to:  %d",internals->numThreads);
    printout(INFO, "DigiKernel", "+++ Number of parallel events: %d",internals->maxEventsParallel);
    internals->tbbInit = new tbb::task_scheduler_init(internals->numThreads);
    if ( internals->maxEventsParallel > 1 )   {
      int todo_evt = internals->eventsToDo;
      int num_proc = std::min(todo_evt,internals->maxEventsParallel);
      tbb::task_group main_group;
      for(int i=0; i < num_proc; ++i)
        main_group.run(Processor(*this));
      main_group.wait();
      printout(DEBUG, "DigiKernel", "+++ All event processing threads Synchronized --- Done!");
    }
  }
#endif
  while ( internals->eventsToDo > 0 && !internals->stop )   {
    Processor proc(*this);
    proc();
    ++internals->eventsSubmitted;
  }
  std::chrono::duration<double> duration = std::chrono::system_clock::now() - start;
  double sec = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
  printout(DEBUG, "DigiKernel", "+++ %d Events out of %d processed. "
           "Total: %7.1f seconds %7.3f seconds/event",
           internals->numEvents-int(internals->eventsToDo), internals->numEvents,
           sec, sec/double(std::max(1,internals->numEvents)));
  return 1;
}

int DigiKernel::terminate() {
  printout(INFO, "DigiKernel", "++ Terminate Digi and delete associated actions.");
  m_detDesc->destroyInstance();
  m_detDesc = 0;
  return 1;
}

