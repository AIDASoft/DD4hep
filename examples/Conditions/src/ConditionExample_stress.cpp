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
/* 
   Plugin invocation:
   ==================
   This plugin behaves like a main program.
   Invoke the plugin with something like this:

   geoPluginRun -volmgr -destroy -plugin DD4hep_AlignmentExample_stress \
   -input file:${CMAKE_INSTALL_PREFIX}/share/DD4hep/examples/AlignDet/compact/Telescope.xml

   Populate the conditions store by hand for a set of IOVs.
   Then compute the corresponding alignment entries....

*/
// Framework include files
#include "ConditionExampleObjects.h"
#include "DD4hep/Factories.h"
#include "TStatistic.h"
#include "TTimeStamp.h"
#include "TRandom3.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::ConditionExamples;

/// Plugin function: Condition program example
/**
 *  Factory: DD4hep_ConditionExample_stress
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/12/2016
 */
static int condition_example (Detector& description, int argc, char** argv)  {
  string input;
  int    num_iov = 10, num_runs = 10;
  bool   arg_error = false;
  for(int i=0; i<argc && argv[i]; ++i)  {
    if ( 0 == ::strncmp("-input",argv[i],4) )
      input = argv[++i];
    else if ( 0 == ::strncmp("-iovs",argv[i],4) )
      num_iov = ::atol(argv[++i]);
    else if ( 0 == ::strncmp("-runs",argv[i],4) )
      num_runs = ::atol(argv[++i]);
    else
      arg_error = true;
  }
  if ( arg_error || input.empty() )   {
    /// Help printout describing the basic command line interface
    cout <<
      "Usage: -plugin <name> -arg [-arg]                                             \n"
      "     name:   factory name     DD4hep_AlignmentExample1                        \n"
      "     -input   <string>        Geometry file                                   \n"
      "     -iovs    <number>        Number of parallel IOV slots for processing.    \n"
      "     -runs    <number>        Number of collision loads to be performed.      \n"
      "\tArguments given: " << arguments(argc,argv) << endl << flush;
    ::exit(EINVAL);
  }

  // First we load the geometry
  description.fromXML(input);

  /******************** Initialize the conditions manager *****************/
  ConditionsManager manager = installManager(description);
  const IOVType*    iov_typ = manager.registerIOVType(0,"run").second;
  if ( 0 == iov_typ )  {
    except("ConditionsPrepare","++ Unknown IOV type supplied.");
  }

  /******************** Now as usual: create the slice ********************/
  shared_ptr<ConditionsContent> content(new ConditionsContent());
  shared_ptr<ConditionsSlice>   slice(new ConditionsSlice(manager,content));
  Scanner(ConditionsKeys(*content,INFO),description.world());
  Scanner(ConditionsDependencyCreator(*content,DEBUG),description.world());

  TStatistic cr_stat("Creation"), acc_stat("Access");
  /******************** Populate the conditions store *********************/
  // Have 10 run-slices [11,20] .... [91,100]
  size_t total_created = 0;
  for(int i=0; i<num_iov; ++i)  {
    TTimeStamp start;
    IOV iov(iov_typ, IOV::Key(1+i*10,(i+1)*10));
    ConditionsPool*   iov_pool = manager.registerIOV(*iov.iovType, iov.key());
    // Create conditions with all deltas.  Use a generic creator
    int count = Scanner().scan(ConditionsCreator(*slice, *iov_pool, DEBUG), description.world());
    TTimeStamp stop;
    cr_stat.Fill(stop.AsDouble()-start.AsDouble());
    printout(INFO,"Example", "Setup %ld conditions for IOV:%s [%8.3f sec]",
             count, iov.str().c_str(), stop.AsDouble()-start.AsDouble());
    total_created += count;
  }

  // ++++++++++++++++++++++++ Now compute the conditions for each of these IOVs
  TRandom3 random;
  ConditionsManager::Result total;
  for(int i=0; i<num_runs; ++i)  {
    TTimeStamp start;
    unsigned int rndm = 1+random.Integer(num_iov*10);
    IOV req_iov(iov_typ,rndm);
    // Attach the proper set of conditions to the user pool
    ConditionsManager::Result res = manager.prepare(req_iov,*slice);
    TTimeStamp stop;
    total += res;
    acc_stat.Fill(stop.AsDouble()-start.AsDouble());
    // Now compute the tranformation matrices
    printout(INFO,"Prepare","Total %ld conditions (S:%6ld,L:%6ld,C:%6ld,M:%ld) of type %s [%8.3f sec]",
             res.total(), res.selected, res.loaded, res.computed, res.missing,
             req_iov.str().c_str(), stop.AsDouble()-start.AsDouble());
  }
  printout(INFO,"Statistics","+======= Summary: # of IOV: %3d  # of Runs: %3d ===========================", num_iov, num_runs);
  printout(INFO,"Statistics","+  %-12s:  %11.5g +- %11.4g  RMS = %11.5g  N = %lld",
           cr_stat.GetName(), cr_stat.GetMean(), cr_stat.GetMeanErr(), cr_stat.GetRMS(), cr_stat.GetN());
  printout(INFO,"Statistics","+  %-12s:  %11.5g +- %11.4g  RMS = %11.5g  N = %lld",
           acc_stat.GetName(), acc_stat.GetMean(), acc_stat.GetMeanErr(), acc_stat.GetRMS(), acc_stat.GetN());
  printout(INFO,"Statistics","+  Accessed Total %ld conditions (S:%6ld,L:%6ld,C:%6ld,M:%ld). Created:%ld",
           total.total(), total.selected, total.loaded, total.computed, total.missing, total_created);
  printout(INFO,"Statistics","+=========================================================================");
  // All done.
  return 1;
}

// first argument is the type from the xml file
DECLARE_APPLY(DD4hep_ConditionExample_stress,condition_example)
