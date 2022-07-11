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
#include "ConditionAnyExampleObjects.h"
#include "DD4hep/Factories.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::ConditionExamples;

/// Plugin function: Condition program example
/**
 *  Factory: DD4hep_ConditionExample_populate
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/12/2016
 */
static int condition_example (Detector& description, int argc, char** argv)  {

  string     input;
  PrintLevel print_level = INFO;
  int        num_iov = 10, extend = 0;
  bool       arg_error = false;
  for(int i=0; i<argc && argv[i]; ++i)  {
    if ( 0 == ::strncmp("-input",argv[i],4) )
      input = argv[++i];
    else if ( 0 == ::strncmp("-iovs",argv[i],4) )
      num_iov = ::atol(argv[++i]);
    else if ( 0 == ::strncmp("-extend",argv[i],4) )
      extend = ::atol(argv[++i]);
    else if ( 0 == ::strncmp("-print",argv[i],4) )
      print_level = dd4hep::decodePrintLevel(argv[++i]);
    else
      arg_error = true;
  }
  if ( arg_error || input.empty() )   {
    /// Help printout describing the basic command line interface
    cout <<
      "Usage: -plugin <name> -arg [-arg]                                             \n"
      "     name:   factory name     DD4hep_ConditionExample_populate                \n"
      "     -input   <string>        Geometry file                                   \n"
      "     -iovs    <number>        Number of parallel IOV slots for processing.    \n"
      "     -print   <leve>          Set print level (number or string)              \n"
      "\tArguments given: " << arguments(argc,argv) << endl << flush;
    ::exit(EINVAL);
  }

  // First we load the geometry
  description.fromXML(input);

  detail::have_condition_item_inventory(1);
  
  /******************** Initialize the conditions manager *****************/
  ConditionsManager manager = installManager(description);
  const IOVType*    iov_typ = manager.registerIOVType(0,"run").second;
  if ( 0 == iov_typ )   {
    except("ConditionsPrepare","++ Unknown IOV type supplied.");
  }
  /******************** Now as usual: create the slice ********************/
  shared_ptr<ConditionsContent> content(new ConditionsContent());
  shared_ptr<ConditionsSlice>   slice(new ConditionsSlice(manager,content));
  Scanner(ConditionsAnyKeys(*content,INFO),description.world());
  Scanner(ConditionsAnyDependencyCreator(*content,DEBUG,false,extend),description.world());

  /******************** Populate the conditions store *********************/
  // Have 10 run-slices [11,20] .... [91,100]
  for(int i=0; i<num_iov; ++i)  {
    IOV iov(iov_typ, IOV::Key(1+i*10,(i+1)*10));
    ConditionsPool*   iov_pool = manager.registerIOV(*iov.iovType, iov.key());
    // Create conditions with all deltas. Use a generic creator
    Scanner(ConditionsAnyCreator(*slice, *iov_pool, print_level),description.world(),0,true);
  }
  
  // ++++++++++++++++++++++++ Now compute the conditions for each of these IOVs
  ConditionsManager::Result total;
  for(int i=0; i<num_iov; ++i)  {
    IOV req_iov(iov_typ,i*10+5);
    // Select the proper set of conditions and attach them to the user pool
    ConditionsManager::Result r = manager.prepare(req_iov,*slice);
    total += r;
    if ( 0 == i )  { // First one we print...
      Scanner(ConditionsAnyDataAccess(req_iov, *slice, print_level), description.world());
    }
    // Now compute the tranformation matrices
    printout(ALWAYS,"Prepare","+  Total %ld conditions (S:%ld,L:%ld,C:%ld,M:%ld) of IOV %s",
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
DECLARE_APPLY(DD4hep_ConditionAnyExample_populate,condition_example)
