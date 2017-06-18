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
/* 
   Plugin invocation:
   ==================
   This plugin behaves like a main program.
   Invoke the plugin with something like this:

   geoPluginRun -volmgr -destroy -plugin DD4hep_ConditionExample_MT \
   -input file:${CMAKE_INSTALL_PREFIX}/share/DD4hep/examples/AlignDet/compact/Telescope.xml

   Populate the conditions store by hand for a set of IOVs.
   Then compute the corresponding alignment entries....

*/
// Framework include files
#include "ConditionExampleObjects.h"
#include "DDCond/ConditionsManagerObject.h"
#include "DD4hep/Factories.h"
#include "TStatistic.h"
#include "TTimeStamp.h"
#include "TRandom3.h"

#include <mutex>
#include <thread>
#include <unistd.h>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::ConditionExamples;

namespace {
  mutex             printout_lock;

  /// Helper class to simuilate evetn queue
  class EventQueue  {
    vector<std::pair<long,long> > events;
    mutex                         guard;
  public:
    EventQueue() = default;
    ~EventQueue() = default;
    void push(const std::pair<long,long>& e)  {
      lock_guard<mutex> lock(guard);
      events.push_back(e);
    }
    long get()  {
      if ( !events.empty() )  {
        lock_guard<mutex> lock(guard);
        if ( !events.empty() )  {
          pair<long,long>& last = events.back();
          if ( --last.second > 0 )
            return last.first;
          long iov = last.first;
          events.pop_back();
          return iov;
        }
      }
      return -1;
    }
  };
  /// Helper to collect statistics
  class Statistics {
  public:
    /// Protection mutex for thread safe updates.
    mutex             total_guard;
    /// Total number of accesses
    long              total_accesses = 0;
    /// Total number of conditions created
    long              total_created = 0;
    ConditionsManager::Result totals;
    TStatistic create, prepare, access;
    Statistics() : create("Creation"), prepare("Prepare"), access("Access")    {    }
    void print() const  {
      printout(INFO,"Statistics","+  %-12s:  %11.5g +- %11.4g  RMS = %11.5g  N = %lld",
               create.GetName(), create.GetMean(), create.GetMeanErr(),
               create.GetRMS(), create.GetN());
      printout(INFO,"Statistics","+  %-12s:  %11.5g +- %11.4g  RMS = %11.5g  N = %lld",
               prepare.GetName(), prepare.GetMean(), prepare.GetMeanErr(),
               prepare.GetRMS(), prepare.GetN());
      printout(INFO,"Statistics","+  %-12s:  %11.5g +- %11.4g  RMS = %11.5g  N = %lld",
               access.GetName(), access.GetMean(), access.GetMeanErr(),
               access.GetRMS(), access.GetN());
      printout(INFO,"Statistics","+  Accessed a total of %ld conditions (S:%6ld,L:%6ld,C:%6ld,M:%ld) during the test. Created:%ld",
               total_accesses, totals.selected, totals.loaded, totals.computed, totals.missing, total_created);
      printout(INFO,"Statistics","+=========================================================================");
    }
  };

  class Executor {
  public:
    ConditionsManager manager;
    EventQueue&       events;
    ConditionsSlice*  slice;
    const IOVType*    iovTyp;
    Statistics&       stats;
    int               identifier;
    
    Executor(ConditionsManager m, const IOVType* typ, int id, EventQueue& q, Statistics& s)
      : manager(m), events(q), slice(0), iovTyp(typ), stats(s), identifier(id)
    {
    }
    virtual ~Executor()  {
      delete slice;
    }
    int accessConditions(const IOV& iov)  {
      TTimeStamp start;
      int count = Scanner().scan(ConditionsDataAccess(iov,*slice),manager->lcdd().world());
      TTimeStamp stop;
      stats.access.Fill(stop.AsDouble()-start.AsDouble());
      lock_guard<mutex> lock(stats.total_guard);
      stats.total_accesses += count;
      return count;
    }
    void run()  {
      int num_reuse = 0;
      int num_access = 0;
      ConditionsManager::Result res;
      for(long iov_val=events.get(), last_iov=-1; iov_val>0; iov_val=events.get()) {
        IOV iov(iovTyp, iov_val);
        if ( iov_val != last_iov )  {
          TTimeStamp start;
          IOV pool_iov(slice->pool.get() ? slice->pool->validity() : iov);
          res = manager.prepare(iov,*slice);
          TTimeStamp stop;
          stats.prepare.Fill(stop.AsDouble()-start.AsDouble());
          last_iov = iov_val;
          // Now compute the tranformation matrices
          {
            lock_guard<mutex> lock(printout_lock);
            printout(INFO,"Re-use","Thread:%3d Conditions reused: %d times. "
                     "Number of accesses:%d       IOV:%s",
                     identifier, num_reuse, num_access, pool_iov.str().c_str());
            printout(INFO,"Prepare","Thread:%3d Total %ld conditions (S:%6ld,L:%6ld,C:%6ld,M:%ld) of type %s [%8.3f sec]",
                     identifier, res.total(), res.selected, res.loaded, res.computed, res.missing,
                     iov.str().c_str(), stop.AsDouble()-start.AsDouble());
          }
          num_access = accessConditions(iov);
          num_reuse = 0;
          stats.total_guard.lock();
          stats.totals += res;
          stats.total_guard.unlock();
          continue;
        }
        ++num_reuse;
        ::usleep(10000);
        num_access += accessConditions(iov);
        stats.total_guard.lock();
        stats.totals += res;
        stats.total_guard.unlock();
      }
    }
  };
}

/// Plugin function: Condition program example
/**
 *  Factory: DD4hep_ConditionExample_stress
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/12/2016
 */
static int condition_example (Geometry::LCDD& lcdd, int argc, char** argv)  {
  string input;
  int    num_iov = 10, num_threads = 1, num_run = 30;
  bool   arg_error = false;
  for(int i=0; i<argc && argv[i]; ++i)  {
    if ( 0 == ::strncmp("-input",argv[i],4) )
      input = argv[++i];
    else if ( 0 == ::strncmp("-iovs",argv[i],4) )
      num_iov = ::atol(argv[++i]);
    else if ( 0 == ::strncmp("-runs",argv[i],4) )
      num_run = ::atol(argv[++i]);
    else if ( 0 == ::strncmp("-threads",argv[i],4) )
      num_threads = ::atol(argv[++i]);
    else
      arg_error = true;
  }
  if ( arg_error || input.empty() )   {
    /// Help printout describing the basic command line interface
    cout <<
      "Usage: -plugin <name> -arg [-arg]                                             \n"
      "     name:   factory name     DD4hep_ConditionExample_MT                      \n"
      "     -input   <string>        Geometry file                                   \n"
      "     -iovs    <number>        Number of parallel IOV slots for processing.    \n"
      "     -runs    <number>        Number of collision loads to be performed.      \n"
      "     -threads <number>        Number of execution threads.                    \n"
      "\tArguments given: " << arguments(argc,argv) << endl << flush;
    ::exit(EINVAL);
  }

  // First we load the geometry
  lcdd.fromXML(input);

  /******************** Initialize the conditions manager *****************/
  ConditionsManager manager = installManager(lcdd);
  const IOVType*    iov_typ = manager.registerIOVType(0,"run").second;
  if ( 0 == iov_typ )
    except("ConditionsPrepare","++ Unknown IOV type supplied.");

  /******************** Now as usual: create the slice ********************/
  shared_ptr<ConditionsContent> content(new ConditionsContent());
  shared_ptr<ConditionsSlice>   slice(new ConditionsSlice(manager,content));
  Scanner(ConditionsKeys(*content,INFO),lcdd.world());
  Scanner(ConditionsDependencyCreator(*content,DEBUG),lcdd.world());

  Statistics stats;
  EventQueue events;
  /******************** Populate the conditions store *********************/
  // Have e.g. 10 run-slices [1,10], [11,20] .... [91,100]
  for(int i=0; i<num_iov; ++i)  {
    TTimeStamp start;
    IOV iov(iov_typ, IOV::Key(1+i*10,(i+1)*10));
    ConditionsPool*   pool = manager.registerIOV(*iov.iovType, iov.key());
    // Create conditions with all deltas using a generic conditions creator
    int count = Scanner().scan(ConditionsCreator(*slice, *pool, DEBUG),lcdd.world());
    TTimeStamp stop;
    stats.create.Fill(stop.AsDouble()-start.AsDouble());
    printout(INFO,"Example", "Setup %ld conditions for IOV:%s [%8.3f sec]",
             count, iov.str().c_str(),stop.AsDouble()-start.AsDouble());
    stats.total_created += count;
    // Fill the event queue with 10 evt per run
    for(int j=0; j<6; ++j)   {
      events.push(make_pair((i*10)+j,num_run));
    }
  }

  // ++++++++++++++++++++++++ Now compute the conditions for each of these IOVs
  vector<thread*> threads;
  for(int i=0; i<num_threads; ++i)  {
    Executor* exec = new Executor(manager, iov_typ, i, events, stats);
    exec->slice = new ConditionsSlice(*slice);
    thread* t = new thread( [exec]{ exec->run(); delete exec; });
    threads.push_back(t);
  }
  for(thread* t : threads)  {
    t->join();
    delete t;
  }
  printout(INFO,"Statistics",
           "+======= Summary: # of IOV: %3d  # of Threads: %3d ========================",
           num_iov, num_threads);
  stats.print();
  // All done.
  return 1;
}

// first argument is the type from the xml file
DECLARE_APPLY(DD4hep_ConditionExample_MT,condition_example)
