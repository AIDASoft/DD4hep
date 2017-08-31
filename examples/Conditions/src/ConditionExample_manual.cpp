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

   geoPluginRun -volmgr -destroy -plugin DD4hep_ConditionExample_manual_load \
   -input file:${DD4hep_DIR}/examples/AlignDet/compact/Telescope.xml \
   -conditions Conditions.root -runs 10

   Save the conditions store by hand for a set of IOVs.
   Then compute the corresponding alignment entries....

*/
// Framework include files
#include "ConditionExampleObjects.h"
#include "DDCond/ConditionsIOVPool.h"
#include "DDCond/ConditionsManager.h"
#include "DDCond/ConditionsDataLoader.h"
#include "DDCond/ConditionsRootPersistency.h"
#include "DD4hep/Factories.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::ConditionExamples;

static void help(int argc, char** argv)  {
  /// Help printout describing the basic command line interface
  cout <<
    "Usage: -plugin <name> -arg [-arg]                                             \n"
    "     name:   factory name     DD4hep_ConditionExample_manual_load             \n"
    "     -input       <string>    Geometry file                                   \n"
    "     -conditions  <string>    Conditions input file                           \n"
    "     -runs        <number>    Number of parallel IOV slots for processing.    \n"
    "\tArguments given: " << arguments(argc,argv) << endl << flush;
  ::exit(EINVAL);
}

/// Plugin function: Condition program example
/**
 *  Factory: DD4hep_ConditionExample_manual_load
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/12/2016
 */
static int condition_example (Detector& description, int argc, char** argv)  {
  string input, conditions;
  int    num_runs = 10;
  bool   arg_error = false;
  for(int i=0; i<argc && argv[i]; ++i)  {
    if ( 0 == ::strncmp("-input",argv[i],4) )
      input = argv[++i];
    else if ( 0 == ::strncmp("-conditions",argv[i],4) )
      conditions = argv[++i];
    else if ( 0 == ::strncmp("-runs",argv[i],4) )
      num_runs = ::atol(argv[++i]);
    else
      arg_error = true;
  }
  if ( arg_error || input.empty() || conditions.empty() ) help(argc,argv);

  // First we load the geometry
  description.fromXML(input);

  // Now we instantiate and initialize the conditions manager
  description.apply("DD4hep_ConditionsManagerInstaller",0,(char**)0);

  ConditionsManager manager = ConditionsManager::from(description);
  manager["PoolType"]       = "DD4hep_ConditionsLinearPool";
  manager["UserPoolType"]   = "DD4hep_ConditionsMapUserPool";
  manager["UpdatePoolType"] = "DD4hep_ConditionsLinearUpdatePool";
  manager["LoaderType"]     = "root";
  manager.initialize();

  printout(ALWAYS,"Example","Load conditions data from file:%s",conditions.c_str());
  manager.loader()->addSource(conditions);

  /// Create the container with the desired conditions content and an empty conditions slice
  shared_ptr<ConditionsContent> content(new ConditionsContent());
  shared_ptr<ConditionsSlice>   slice(new ConditionsSlice(manager,content));
  
  // Populate the content of required conditions:
  // We scan the DetElement hierarchy and add a couple of conditions
  // for each DetElement
  Scanner(ConditionsKeys(*content,INFO),description.world());

  // Add for each DetElement 3 derived conditions,
  // which all depend on the persistent condition "derived_data"
  // (In the real world this would be very specific derived actions)
  Scanner(ConditionsDependencyCreator(*content,DEBUG),description.world());

  // Now emulate a pseudo event loop: Our conditions are of type "run"
  const IOVType* iov_typ = manager.iovType("run");
  ConditionsManager::Result total;
  for ( int irun=0; irun < num_runs; ++irun )  {
    IOV req_iov(iov_typ,irun*10+5);
    // Select the proper set of conditions from the store (or load them if needed)
    // Attach the selected conditions to the user pool
    ConditionsManager::Result r = manager.prepare(req_iov,*slice);
    total += r;
    if ( 0 == irun )  { // First one we print...
      // We can access the conditions directly from the slice, since the slice
      // implements the ConditionsMap interface.
      Scanner(ConditionsPrinter(slice.get(),"Example"),description.world());
    }
    // Print summary
    printout(ALWAYS,"Prepare","Total %ld conditions (S:%ld,L:%ld,C:%ld,M:%ld) of IOV %s",
             r.total(), r.selected, r.loaded, r.computed, r.missing, req_iov.str().c_str());
  }  
  printout(ALWAYS,"Statistics","+=========================================================================");
  printout(ALWAYS,"Statistics","+  Accessed a total of %ld conditions (S:%6ld,L:%6ld,C:%6ld,M:%ld)",
           total.total(), total.selected, total.loaded, total.computed, total.missing);
  printout(ALWAYS,"Statistics","+=========================================================================");
  // All done.
  return 1;
}

// first argument is the type from the xml file
DECLARE_APPLY(DD4hep_ConditionExample_manual_load,condition_example)
