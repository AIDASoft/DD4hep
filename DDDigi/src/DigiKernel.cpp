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

#include <TRandom.h>

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

  /// Configure callbacks
  CallbackSequence      configurators       { };
  /// Initialize callbacks
  CallbackSequence      initializers        { };
  //// Termination callback
  CallbackSequence      terminators         { };
      /// Register start event callback
  CallbackSequence      start_event         { };
  /// Register end event callback
  CallbackSequence      end_event           { };

  /// The main data input action sequence
  DigiActionSequence*   inputAction         { nullptr };
  /// The main event action sequence
  DigiActionSequence*   eventAction         { nullptr };
  /// The main data output action sequence
  DigiActionSequence*   outputAction        { nullptr };

  /// Random generator
  TRandom* root_random;
  std::shared_ptr<DigiRandomGenerator> random  {};
  /// TBB initializer (If TBB is used)
  void*                 tbbInit             { nullptr };
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
template<typename ACTION, typename ARG> class DigiKernel::Wrapper  {
public:
  ACTION*  action = 0;
  ARG   context;
  Wrapper(ACTION* a, ARG c) : action(a), context(c) {}
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
	context->set_random_generator(this->kernel.internals->random);
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
  internals->root_random = new TRandom();
  internals->random = std::make_shared<DigiRandomGenerator>();
  internals->random->engine = [this] {  return this->internals->root_random->Uniform(1.0);  };
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

/// Configure the digitization: call all registered configurators
int DigiKernel::configure()   {
  internals->configurators();
  return 1;
}

/// Initialize the digitization: call all registered initializers
int DigiKernel::initialize()   {
  internals->initializers();
  return 1;
}

/// Register configure callback
void DigiKernel::register_configure(const Callback& callback)   const  {
  std::lock_guard<std::mutex> lock(initializer_lock());
  internals->configurators.add(callback);
}

/// Register initialize callback
void DigiKernel::register_initialize(const Callback& callback)   const  {
  std::lock_guard<std::mutex> lock(initializer_lock());
  internals->initializers.add(callback);
}

/// Register terminate callback
void DigiKernel::register_terminate(const Callback& callback)   const  {
  std::lock_guard<std::mutex> lock(initializer_lock());
  internals->terminators.add(callback);
}

/// Register start event callback
void DigiKernel::register_start_event(const Callback& callback)   const  {
  internals->start_event.add(callback);
}

/// Register end event callback
void DigiKernel::register_end_event(const Callback& callback)   const  {
  internals->end_event.add(callback);
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
void DigiKernel::submit (ParallelCall*const algorithms[], std::size_t count, void* context, bool parallel)  const    {
#ifdef DD4HEP_USE_TBB
  bool para = parallel && (0 != internals->tbbInit && internals->numThreads>0);
  if ( para )   {
    tbb::task_group que;
    printout(INFO,"DigiKernel","+++ Executing chunk of %ld execution entries in parallel", count);
    for( std::size_t i=0; i<count; ++i)
      que.run( Wrapper<ParallelCall,void*>(algorithms[i], context) );
    que.wait();
    return;
  }
#endif
  printout(INFO,"DigiKernel","+++ Executing chunk of %ld execution entries sequentially", count);
  for( std::size_t i=0; i<count; ++i)
    algorithms[i]->execute(context);
}

/// Submit a bunch of actions to be executed in parallel
void DigiKernel::submit (const std::vector<ParallelCall*>& algorithms, void* data, bool parallel)  const  {
  this->submit(&algorithms[0], algorithms.size(), data, parallel);
}

void DigiKernel::wait(DigiContext& context)   const  {
  if ( context.event ) {}
}

/// Execute one single event
void DigiKernel::executeEvent(std::unique_ptr<DigiContext>&& context)    {
  DigiContext& refContext = *context;
  try {
    internals->start_event(&refContext);
    inputAction().execute(refContext);
    eventAction().execute(refContext);
    outputAction().execute(refContext);
    internals->end_event(&refContext);
    notify(std::move(context));
  }
  catch(const std::exception& e)   {
    notify(std::move(context), e);
  }
}

/// Notify kernel that the execution of one single event finished
void DigiKernel::notify(std::unique_ptr<DigiContext>&& context)   {
  if ( context )   {
    context->event.reset();
  }
  context.reset();
  ++internals->eventsFinished;
}

/// Notify kernel that the execution of one single event finished
void DigiKernel::notify(std::unique_ptr<DigiContext>&& context, const std::exception& e)   {
  internals->stop = true;
  printout(ERROR,"DigiKernel","+++ Exception during event processing [Shall stop the event loop]");
  printout(ERROR,"DigiKernel"," -> %s", e.what());
  notify(std::move(context));
}

/// Run the digitization sequence over the requested number of events
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

/// Terminate the digitization: call all registered terminators and release the allocated resources
int DigiKernel::terminate() {
  printout(INFO, "DigiKernel", "++ Terminate Digi and delete associated actions.");
  internals->terminators();
  m_detDesc->destroyInstance();
  m_detDesc = 0;
  return 1;
}

