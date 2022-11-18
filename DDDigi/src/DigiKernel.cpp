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
#include <DDDigi/DigiMonitorHandler.h>

#ifdef DD4HEP_USE_TBB
#include <tbb/task_group.h>
#include <tbb/global_control.h>
#else
namespace tbb {  struct global_control { enum { max_allowed_parallelism = -1 }; }; }
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
  /// Property: Client output levels
  ClientOutputLevels    clientLevels;
  /// Atomic counter: Number of events still to be processed in this run
  std::atomic_int       events_todo;
  /// Atomic counter: Number of events still to be processed in this run
  std::atomic_int       events_finished;
  /// Atomic counter: Number of events still to be processed in this run
  std::size_t           events_submitted;

  /// Lock to ensure counter safety
  std::mutex            counter_lock        { };

  /// Lock to ensure initialization safetyp
  std::mutex            initializer_lock    { };

  /// Lock for global I/O dependencies
  std::mutex            global_io_lock      { };

  /// Lock for global output logging
  std::mutex            global_output_lock  { };

  using callbacks_t    = std::vector<std::function<void()> >;
  using ev_callbacks_t = std::vector<std::function<void(DigiContext&)> >;

  /// Configure callbacks
  callbacks_t           configurators       { };
  /// Initialize callbacks
  callbacks_t           initializers        { };
  /// Termination callback
  callbacks_t           terminators         { };
  /// Register start event callback
  ev_callbacks_t        start_event         { };
  /// Register end event callback
  ev_callbacks_t        end_event           { };

  /// The main data input action sequence
  DigiActionSequence*   input_action         { nullptr };
  /// The main event action sequence
  DigiActionSequence*   event_action         { nullptr };
  /// The main data output action sequence
  DigiActionSequence*   output_action        { nullptr };
  /// The histogram handler entity
  DigiMonitorHandler*   monitor_handler    { nullptr };

  /// Random generator
  TRandom* root_random;
  /// Shared random number generator
  std::shared_ptr<DigiRandomGenerator> random  { };
  /// TBB initializer (If TBB is used)
  std::unique_ptr<tbb::global_control> tbb_init { };
  /// Property: Output level
  int                   outputLevel;
  /// Property: maximum number of events to be processed (if < 0: infinite)
  int                   numEvents;
  /// Property: maximum number of events to be processed in parallel (if TBB)
  int                   maxEventsParallel;
  /// Property: maximum number of threads to be used (if TBB)
  int                   num_threads;
  /// Property: Allow to stop execution from interactive prompt
  bool                  stop = false;

public:
  /// Default constructor
  Internals() = default;
  /// Default destructor
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
        if( !kernel.internals->stop && kernel.internals->events_todo > 0)
          todo = --kernel.internals->events_todo;
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
  : DigiAction(*this, "DigiKernel"), m_detDesc(&description_ref)
{
  internals = new Internals();
  internals->num_threads = tbb::global_control::max_allowed_parallelism;
  declareProperty("maxEventsParallel",internals->maxEventsParallel = 1);
  declareProperty("numThreads",       internals->num_threads);
  declareProperty("numEvents",        internals->numEvents = 10);
  declareProperty("stop",             internals->stop = false);
  declareProperty("OutputLevels",     internals->clientLevels);
  auto* h = new DigiMonitorHandler(*this, "MonitorData");
  properties().add("MonitorOutput", h->property("MonitorOutput"));
  internals->monitor_handler = h;

  internals->input_action  = new DigiActionSequence(*this, "InputAction");
  internals->event_action  = new DigiActionSequence(*this, "EventAction");
  internals->output_action = new DigiActionSequence(*this, "OutputAction");
  internals->input_action->setExecuteParallel(false);
  internals->event_action->setExecuteParallel(false);
  internals->output_action->setExecuteParallel(false);
  internals->root_random = new TRandom();
  internals->random = std::make_shared<DigiRandomGenerator>();
  internals->random->engine = [this] {  return internals->root_random->Uniform(1.0);  };
  InstanceCount::increment(this);
}

/// Default destructor
DigiKernel::~DigiKernel() {
  std::lock_guard<std::mutex> lock(kernel_mutex);
  internals->tbb_init.reset();
  detail::releasePtr(internals->monitor_handler);
  detail::releasePtr(internals->output_action);
  detail::releasePtr(internals->event_action);
  detail::releasePtr(internals->input_action);
  detail::deletePtr(internals->root_random);
  internals->random.reset();
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

/// Print the property values
void DigiKernel::printProperties()  const  {
  this->DigiAction::printProperties();
  for( const auto& cl : internals->clientLevels )  {
    always("OutputLevel[%s]:  %d", cl.first.c_str(), cl.second);
  }
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

/// Access current number of events still to process
std::size_t DigiKernel::events_todo()  const   {
  std::lock_guard<std::mutex> lock(internals->counter_lock);
  std::size_t evts = internals->events_todo;
  return evts;
}

/// Access current number of events already processed
std::size_t DigiKernel::events_done()  const   {
  std::lock_guard<std::mutex> lock(internals->counter_lock);
  std::size_t evts = internals->numEvents - internals->events_todo;
  return evts;
}

/// Access current number of events processing (events in flight)
std::size_t DigiKernel::events_processing()  const   {
  std::lock_guard<std::mutex> lock(internals->counter_lock);
  std::size_t evts = internals->events_submitted - internals->events_finished;
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
  for(auto& call : internals->configurators) call();
  return 1;
}

/// Initialize the digitization: call all registered initializers
int DigiKernel::initialize()   {
  for(auto& call : internals->initializers) call();
  return 1;
}

/// Access to the main input action sequence from the kernel object
DigiActionSequence& DigiKernel::inputAction() const    {
  return *internals->input_action;
}

/// Access to the main event action sequence from the kernel object
DigiActionSequence& DigiKernel::eventAction() const    {
  return *internals->event_action;
}

/// Access to the main output action sequence from the kernel object
DigiActionSequence& DigiKernel::outputAction() const    {
  return *internals->output_action;
}

/// Register configure callback
void DigiKernel::register_configure(const std::function<void()>& callback)   const  {
  std::lock_guard<std::mutex> lock(initializer_lock());
  internals->configurators.push_back(callback);
}

/// Register initialize callback
void DigiKernel::register_initialize(const std::function<void()>& callback)  const  {
  std::lock_guard<std::mutex> lock(initializer_lock());
  internals->initializers.push_back(callback);
}

/// Register terminate callback
void DigiKernel::register_terminate(const std::function<void()>& callback)   const  {
  std::lock_guard<std::mutex> lock(initializer_lock());
  internals->terminators.push_back(callback);
}

/// Register start event callback
void DigiKernel::register_start_event(const std::function<void(DigiContext&)>& callback)   const  {
  internals->start_event.push_back(callback);
}

/// Register end event callback
void DigiKernel::register_end_event(const std::function<void(DigiContext&)>& callback)   const  {
  internals->end_event.push_back(callback);
}

/// Registration of monitoring objects eventually saved by the handler
void DigiKernel::register_monitor(DigiAction* action, TNamed* object)  const    {
  if ( action && object )  {
    std::lock_guard<std::mutex> lock(internals->counter_lock);
    internals->monitor_handler->adopt(action, object);
    return;
  }
  except("DigiKernel","+++ Invalid monitor request from action %s with object %s: %s",
	 action ? action->name().c_str() : "[Invalid]",
	 object ? object->GetName() : "[Invalid]",
	 object ? object->GetTitle() : "");
}

/// Submit a bunch of actions to be executed in parallel
void DigiKernel::submit (DigiContext& context, ParallelCall*const algorithms[], std::size_t count, void* data, bool parallel)  const    {
  const char* tag = context.event->id();
#ifdef DD4HEP_USE_TBB
  bool para = parallel && (internals->tbb_init && internals->num_threads > 0);
  if ( para )   {
    tbb::task_group que;
    info("%s+++ Executing chunk of %3ld execution entries in parallel", tag, count);
    for( std::size_t i=0; i<count; ++i)
      que.run( Wrapper<ParallelCall,void*>(algorithms[i], data) );
    que.wait();
    return;
  }
#endif
  info("%s+++ Executing chunk of %3ld execution entries sequentially", tag, count);
  for( std::size_t i=0; i<count; ++i)
    algorithms[i]->execute(data);
}

/// Submit a bunch of actions to be executed in parallel
void DigiKernel::submit (DigiContext& context, const std::vector<ParallelCall*>& algorithms, void* data, bool parallel)  const  {
  submit(context, &algorithms[0], algorithms.size(), data, parallel);
}

void DigiKernel::wait(DigiContext& context)   const  {
  if ( context.event ) {}
}

/// Execute one single event
void DigiKernel::executeEvent(std::unique_ptr<DigiContext>&& context)    {
  DigiContext& refContext = *context;
  try {
    for(auto& call : internals->start_event) call(refContext);
    inputAction().execute(refContext);
    eventAction().execute(refContext);
    outputAction().execute(refContext);
    for(auto& call : internals->end_event) call(refContext);
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
  ++internals->events_finished;
}

/// Notify kernel that the execution of one single event finished
void DigiKernel::notify(std::unique_ptr<DigiContext>&& context, const std::exception& e)   {
  const char* tag = context->event->id();
  internals->stop = true;
  error("%s+++ Exception during event processing [Shall stop the event loop]", tag);
  error("%s -> %s", tag, e.what());
  notify(std::move(context));
}

/// Run the digitization sequence over the requested number of events
int DigiKernel::run()   {
  std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
  internals->stop = false;
  internals->events_finished = 0;
  internals->events_submitted = 0;
  internals->events_todo = internals->numEvents;
  info("+++ Total number of events:    %d",internals->numEvents);
#ifdef DD4HEP_USE_TBB
  if ( !internals->tbb_init && internals->num_threads > 0 )   {
    using ctrl_t = tbb::global_control;
    if ( 0 == internals->num_threads )  {
      internals->num_threads = ctrl_t::max_allowed_parallelism;
    }
    info("+++ Number of TBB threads:     %d",internals->num_threads);
    info("+++ Number of parallel events: %d",internals->maxEventsParallel);
    internals->tbb_init = std::make_unique<ctrl_t>(ctrl_t::max_allowed_parallelism,internals->num_threads+1);
    if ( internals->maxEventsParallel >= 0 )   {
      int todo_evt = internals->events_todo;
      int num_proc = std::min(todo_evt,internals->maxEventsParallel);
      tbb::task_group main_group;
      for(int i=0; i < num_proc; ++i)
        main_group.run(Processor(*this));
      main_group.wait();
    }
    debug("+++ All event processing threads Synchronized --- Done!");
  }
  else
#endif
    {
      while ( internals->events_todo > 0 && !internals->stop )   {
	Processor proc(*this);
	proc();
	++internals->events_submitted;
      }
    }
  std::chrono::duration<double> duration = std::chrono::system_clock::now() - start;
  double sec = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
  info("+++ %d Events out of %d processed. "
       "Total: %7.1f seconds %7.3f seconds/event",
       internals->numEvents-int(internals->events_todo), internals->numEvents,
       sec, sec/double(std::max(1,internals->numEvents)));
  return 1;
}

/// Terminate the digitization: call all registered terminators and release the allocated resources
int DigiKernel::terminate() {
  info("++ Saving monitoring quantities.");
  internals->monitor_handler->save();
  info("++ Terminate Digi and delete associated actions.");
  for(auto& call : internals->terminators) call();
  m_detDesc->destroyInstance();
  m_detDesc = 0;
  return 1;
}

