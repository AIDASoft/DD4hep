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

   geoPluginRun -volmgr -destroy -plugin DD4hep_ConditionExample_load \
   -input file:${DD4hep_DIR}/examples/AlignDet/compact/Telescope.xml \
   -conditions Conditions.root

   Save the conditions store by hand for a set of IOVs.
   Then compute the corresponding alignment entries....

*/
// Framework include files
#include "ConditionExampleObjects.h"
#include "DDCond/ConditionsIOVPool.h"
#include "DDCond/ConditionsManager.h"
#include "DDCond/ConditionsRootPersistency.h"
#include "DD4hep/Factories.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::ConditionExamples;

static void help(int argc, char** argv)  {
  /// Help printout describing the basic command line interface
  cout <<
    "Usage: -plugin <name> -arg [-arg]                                             \n"
    "     name:   factory name     DD4hep_ConditionExample_load                    \n"
    "     -input       <string>    Geometry file                                   \n"
    "     -conditions  <string>    Conditions input file                           \n"
    "     -iovs        <number>    Number of parallel IOV slots for processing.    \n"
    "     -restore     <string>    Restore strategy: iovpool, userpool or condpool.\n"
    "\tArguments given: " << arguments(argc,argv) << endl << flush;
  ::exit(EINVAL);
}

/// Plugin function: Condition program example
/**
 *  Factory: DD4hep_ConditionExample_load
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/12/2016
 */
static int condition_example (Detector& description, int argc, char** argv)  {
  string input, conditions, restore="iovpool";
  int    num_iov = 10;
  bool   arg_error = false;
  for(int i=0; i<argc && argv[i]; ++i)  {
    if ( 0 == ::strncmp("-input",argv[i],4) )
      input = argv[++i];
    else if ( 0 == ::strncmp("-conditions",argv[i],4) )
      conditions = argv[++i];
    else if ( 0 == ::strncmp("-restore",argv[i],4) )
      restore = argv[++i];
    else if ( 0 == ::strncmp("-iovs",argv[i],4) )
      num_iov = ::atol(argv[++i]);
    else
      arg_error = true;
  }
  if ( arg_error || input.empty() || conditions.empty() ) help(argc,argv);

  // First we load the geometry
  description.fromXML(input);

  /******************** Initialize the conditions manager *****************/
  ConditionsManager manager = installManager(description);
  shared_ptr<ConditionsContent> content(new ConditionsContent());
  shared_ptr<ConditionsSlice>   slice(new ConditionsSlice(manager,content));
  Scanner(ConditionsKeys(*content,INFO),description.world());
  Scanner(ConditionsDependencyCreator(*content,DEBUG),description.world());

  /******************** Load the conditions from file *********************/
  try  {
    auto pers = cond::ConditionsRootPersistency::load(conditions.c_str(),"DD4hep Conditions");
    printout(ALWAYS,"Statistics","+=========================================================================");
    printout(ALWAYS,"Statistics","+  Loaded conditions object from file %s. Took %8.3f seconds.",
             conditions.c_str(),pers->duration);
    size_t num_cond = 0;
    if      ( restore == "iovpool" )
      num_cond = pers->importIOVPool("ConditionsIOVPool No 1","run",manager);
    else if ( restore == "userpool" )
      num_cond = pers->importUserPool("*","run",manager);
    else if ( restore == "condpool" )
      num_cond = pers->importConditionsPool("*","run",manager);
    else
      help(argc,argv);

    printout(ALWAYS,"Statistics","+  Imported %ld conditions from %s to IOV pool. Took %8.3f seconds.",
             num_cond, restore.c_str(), pers->duration);
    printout(ALWAYS,"Statistics","+=========================================================================");
  }
  catch(const exception& e)    {
    printout(ERROR,"ConditionsExample","Failed to import ROOT object(s): %s",e.what());    
    throw;
  }
  
  // ++++++++++++++++++++++++ Now compute the conditions for each of these IOVs
  const IOVType* iov_typ = manager.iovType("run");
  cond::ConditionsIOVPool* pool = manager.iovPool(*iov_typ);
  for( const auto& p : pool->elements )
    p.second->print("*");

  ConditionsManager::Result total;
  for(int i=0; i<num_iov; ++i)  {
    IOV req_iov(iov_typ,i*10+5);
    // Select the proper set of conditions and attach them to the user pool
    ConditionsManager::Result r = manager.prepare(req_iov,*slice);
    total += r;
    if ( 0 == i )  { // First one we print...
      Scanner(ConditionsPrinter(slice.get(),"Example"),description.world());
    }
    // Now compute the tranformation matrices
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
DECLARE_APPLY(DD4hep_ConditionExample_load,condition_example)
