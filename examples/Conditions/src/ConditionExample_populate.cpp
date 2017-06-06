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

   geoPluginRun -volmgr -destroy -plugin DD4hep_ConditionExample_populate \
   -input file:${DD4hep_DIR}/examples/AlignDet/compact/Telescope.xml

   Populate the conditions store by hand for a set of IOVs.
   Then compute the corresponding alignment entries....

*/
// Framework include files
#include "ConditionExampleObjects.h"
#include "DD4hep/Factories.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::ConditionExamples;

/// Plugin function: Condition program example
/**
 *  Factory: DD4hep_ConditionExample_populate
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/12/2016
 */
static int condition_example (Geometry::LCDD& lcdd, int argc, char** argv)  {

  string input;
  int    num_iov = 10;
  bool   arg_error = false;
  for(int i=0; i<argc && argv[i]; ++i)  {
    if ( 0 == ::strncmp("-input",argv[i],4) )
      input = argv[++i];
    else if ( 0 == ::strncmp("-iovs",argv[i],4) )
      num_iov = ::atol(argv[++i]);
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

  /******************** Populate the conditions store *********************/
  // Have 10 run-slices [11,20] .... [91,100]
  for(int i=0; i<num_iov; ++i)  {
    IOV iov(iov_typ, IOV::Key(1+i*10,(i+1)*10));
    ConditionsPool*   iov_pool = manager.registerIOV(*iov.iovType, iov.key());
    // Create conditions with all deltas. Use a generic creator
    Scanner(ConditionsCreator(*slice, *iov_pool, INFO),lcdd.world(),0,true);
  }
  
  // ++++++++++++++++++++++++ Now compute the conditions for each of these IOVs
  ConditionsManager::Result total;
  for(int i=0; i<num_iov; ++i)  {
    IOV req_iov(iov_typ,i*10+5);
    // Select the proper set of conditions and attach them to the user pool
    ConditionsManager::Result r = manager.prepare(req_iov,*slice);
    total += r;
    if ( 0 == i )  { // First one we print...
      Scanner(ConditionsPrinter(slice.get(),"Example"),lcdd.world());
    }
    // Now compute the tranformation matrices
    printout(INFO,"Prepare","Total %ld conditions (S:%ld,L:%ld,C:%ld,M:%ld) of IOV %s",
             r.total(), r.selected, r.loaded, r.computed, r.missing, req_iov.str().c_str());
  }  
  printout(INFO,"Statistics","+=========================================================================");
  printout(INFO,"Statistics","+  Accessed Total %ld conditions (S:%6ld,L:%6ld,C:%6ld,M:%ld)",
           total.total(), total.selected, total.loaded, total.computed, total.missing);
  printout(INFO,"Statistics","+=========================================================================");
  // All done.
  return 1;
}

// first argument is the type from the xml file
DECLARE_APPLY(DD4hep_ConditionExample_populate,condition_example)
