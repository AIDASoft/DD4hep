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

#include "DDDigi/DigiKernel.h"
#include "DDDigi/DigiContext.h"
#include "DDDigi/DigiActionSequence.h"

#include "tbb/tbb.h"

// C/C++ include files
#include <stdexcept>
#include <algorithm>
#include <memory>
#include <chrono>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::digi;

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
class DigiKernel::Wrapper  {
public:
  DigiContext& context;
  DigiAction*  action = 0;
  Wrapper(DigiContext& c, DigiAction* a)
    : context(c), action(a) {}
  Wrapper(Wrapper&& copy) = default;
  Wrapper(const Wrapper& copy) = default;
  Wrapper& operator=(Wrapper&& copy) = default;
  Wrapper& operator=(const Wrapper& copy) = default;
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
    int todo;
    while( !kernel.internals->stop &&
           (todo = --kernel.internals->eventsToDo) >= 0 )    {
      int ev_num = kernel.internals->numEvents - todo;
      unique_ptr<DigiContext> c = make_unique<DigiContext>(&kernel);
      unique_ptr<DigiEvent>   e = make_unique<DigiEvent>(ev_num);
      c->setEvent(e.release());
      kernel.executeEvent(c.release());
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
    static std::mutex kernel_mutex;
    std::lock_guard<std::mutex> lock(kernel_mutex);
    if ( 0 == s_main_instance.get() )   { // Need to check again!
      s_main_instance.adopt(new DigiKernel(description));
    }
  }
  return *(s_main_instance.get());
}

/// Access to the properties of the object
PropertyManager& DigiKernel::properties()   {
  return internals->properties;
}

/// Print the property values
void DigiKernel::printProperties()  const  {
  printout(ALWAYS,"DigiKernel","OutputLevel:  %d", internals->outputLevel);
  for(ClientOutputLevels::const_iterator i=internals->clientLevels.begin(); i!=internals->clientLevels.end();++i)  {
    printout(ALWAYS,"DigiKernel","OutputLevel[%s]:  %d",(*i).first.c_str(),(*i).second);
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

void DigiKernel::submit(const DigiAction::Actors<DigiAction>& actions, DigiContext& context)   const  {
  chrono::system_clock::time_point start = chrono::system_clock::now();
  bool parallel = 0 != internals->tbbInit && internals->numThreads>0;
#ifdef DD4HEP_USE_TBB
  if ( parallel )   {
    tbb::task_group que;
    for(auto i=actions.begin(); i!=actions.end(); ++i)
      que.run(Wrapper(context, *i));
    que.wait();
    goto print_stamp;
  }
#endif
  actions(&DigiAction::execute,context);

 print_stamp:
  chrono::duration<double> secs = chrono::system_clock::now() - start;
  printout(DEBUG,"DigiKernel","+++ Event: %8d Executed %s task group with %3ld members [%8.3g sec]",
           context.event().eventNumber, parallel ? "parallel" : "serial", actions.size(),
           secs.count());
}

void DigiKernel::execute(const DigiAction::Actors<DigiAction>& actions, DigiContext& context)   const  {
  actions(&DigiAction::execute,context);
}

void DigiKernel::wait(DigiContext& context)   const  {
  if ( context.eventPtr() ) {}
}

/// Execute one single event
void DigiKernel::executeEvent(DigiContext* context)    {
  DigiContext& refContext = *context;
  try {
    inputAction().execute(refContext);
    eventAction().execute(refContext);
    outputAction().execute(refContext);
    notify(context);
  }
  catch(const exception& e)   {
    notify(context, e);
  }
}

void DigiKernel::notify(DigiContext* context)   {
  if ( context )   {
    DigiEvent* event = context->eventPtr();
    if ( event )    {
      context->setEvent(0);
      detail::deletePtr(event);
    }
    delete context;
  }
}

void DigiKernel::notify(DigiContext* context, const std::exception& e)   {
  internals->stop = true;
  printout(ERROR,"DigiKernel","+++ Exception during event processing: %s. [%s]",
           e.what(), "Shall stop the event loop!");
  notify(context);
}

int DigiKernel::run()   {
  chrono::system_clock::time_point start = chrono::system_clock::now();
  internals->stop = false;
  internals->eventsToDo = internals->numEvents;
  printout(INFO,
           "DigiKernel","+++ Total number of events:    %d",internals->numEvents);
#ifdef DD4HEP_USE_TBB
  if ( 0 == internals->tbbInit && internals->numThreads>=0 )   {
    if ( 0 == internals->numThreads )
      internals->numThreads = tbb::task_scheduler_init::default_num_threads();
    printout(INFO,
             "DigiKernel","+++ Number of TBB threads to:  %d",internals->numThreads);
    printout(INFO,
             "DigiKernel","+++ Number of parallel events: %d",internals->maxEventsParallel);
    internals->tbbInit = new tbb::task_scheduler_init(internals->numThreads);
    if ( internals->maxEventsParallel > 1 )   {
      int todo_evt = internals->eventsToDo;
      int num_proc = std::min(todo_evt,internals->maxEventsParallel);
      tbb::task_group main_group;
      for(int i=0; i < num_proc; ++i)
        main_group.run(Processor(*this));
      main_group.wait();
      printout(DEBUG,"DigiKernel","+++ All event processing threads Synchronized --- Done!");
    }
  }
#endif
  if ( internals->eventsToDo > 0 )   {
    for(int i=0; i<internals->numEvents; ++i)   {
      unique_ptr<DigiContext> context = make_unique<DigiContext>(this);
      ++internals->eventsToDo;
      executeEvent(context.release());
    }
  }
  chrono::duration<double> duration = chrono::system_clock::now() - start;
  double sec = chrono::duration_cast<chrono::seconds>(duration).count();
  printout(DEBUG,"DigiKernel","+++ Event processing finished. Total: %7.3f seconds %7.3f seconds/event",
           sec, sec/double(std::max(1,internals->numEvents)));
  return 1;
}

int DigiKernel::terminate() {
  printout(INFO,"DigiKernel","++ Terminate Digi and delete associated actions.");
  m_detDesc->destroyInstance();
  m_detDesc = 0;
  return 1;
}

