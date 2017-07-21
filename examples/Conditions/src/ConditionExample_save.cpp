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

   geoPluginRun -volmgr -destroy -plugin DD4hep_ConditionExample_save \
   -input file:${DD4hep_DIR}/examples/AlignDet/compact/Telescope.xml \
   -conditions Conditions.root

   Save the conditions store by hand for a set of IOVs.
   Then compute the corresponding alignment entries....

*/
// Framework include files
#include "ConditionExampleObjects.h"
#include "DDCond/ConditionsManager.h"
#include "DDCond/ConditionsIOVPool.h"
#include "DDCond/ConditionsRootPersistency.h"
#include "DD4hep/Factories.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::ConditionExamples;

/// Plugin function: Condition program example
/**
 *  Factory: DD4hep_ConditionExample_save
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/12/2016
 */
static int condition_example (Detector& description, int argc, char** argv)  {

  string input, conditions;
  int    num_iov = 10;
  bool   arg_error = false;
  for(int i=0; i<argc && argv[i]; ++i)  {
    if ( 0 == ::strncmp("-input",argv[i],4) )
      input = argv[++i];
    else if ( 0 == ::strncmp("-conditions",argv[i],4) )
      conditions = argv[++i];
    else if ( 0 == ::strncmp("-iovs",argv[i],4) )
      num_iov = ::atol(argv[++i]);
    else
      arg_error = true;
  }
  if ( arg_error || input.empty() || conditions.empty() )   {
    /// Help printout describing the basic command line interface
    cout <<
      "Usage: -plugin <name> -arg [-arg]                                             \n"
      "     name:   factory name     DD4hep_ConditionExample_save                    \n"
      "     -input       <string>    Geometry file                                   \n"
      "     -conditions  <string>    Conditions output file                          \n"
      "     -iovs        <number>    Number of parallel IOV slots for processing.    \n"
      "\tArguments given: " << arguments(argc,argv) << endl << flush;
    ::exit(EINVAL);
  }

  // First we load the geometry
  description.fromXML(input);

  /******************** Initialize the conditions manager *****************/
  ConditionsManager manager = installManager(description);
  const IOVType*    iov_typ = manager.registerIOVType(0,"run").second;
  if ( 0 == iov_typ )
    except("ConditionsPrepare","++ Unknown IOV type supplied.");

  /******************** Now as usual: create the slice ********************/
  shared_ptr<ConditionsContent> content(new ConditionsContent());
  shared_ptr<ConditionsSlice>   slice(new ConditionsSlice(manager,content));
  Scanner(ConditionsKeys(*content,INFO),description.world());
  Scanner(ConditionsDependencyCreator(*content,DEBUG),description.world());

  /******************** Save the conditions store *********************/
  // Have 10 run-slices [11,20] .... [91,100]
  for(int i=0; i<num_iov; ++i)  {
    IOV iov(iov_typ, IOV::Key(1+i*10,(i+1)*10));
    ConditionsPool*   iov_pool = manager.registerIOV(*iov.iovType, iov.key());
    // Create conditions with all deltas. Use a generic creator
    Scanner(ConditionsCreator(*slice, *iov_pool, INFO),description.world(),0,true);
  }

  char text[132];
  bool output_iovpool  = true;
  bool output_userpool = true;
  bool output_condpool = true;
  size_t count = 0, total_count = 0;
  auto* persist = new cond::ConditionsRootPersistency("DD4hep Conditions");
  // ++++++++++++++++++++++++ Now compute the conditions for each of these IOVs
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
    if ( output_userpool )  {
      /// Add the conditions UserPool to the persistent file
      ::snprintf(text,sizeof(text),"User pool %s:[%ld]",iov_typ->name.c_str(),req_iov.key().first);
      count = persist->add(text,*slice->pool);
      total_count += count;
      printout(ALWAYS,"Example","+++ Added %ld conditions to persistent user pool.",count);
    }
  }
  if ( output_condpool )  {
    cond::ConditionsIOVPool* iov_pool = manager.iovPool(*iov_typ);
    for( const auto& p : iov_pool->elements )  {
      ::snprintf(text,sizeof(text),"Conditions pool %s:[%ld,%ld]",
                 iov_typ->name.c_str(),p.second->iov->key().first,p.second->iov->key().second);
      count = persist->add(text,*p.second);
      total_count += count;
      printout(ALWAYS,"Example","+++ Added %ld conditions to persistent conditions pool.",count);
    }
  }
  if ( output_iovpool )  {
    count = persist->add("ConditionsIOVPool No 1",*manager.iovPool(*iov_typ));
    total_count += count;
    printout(ALWAYS,"Example","+++ Added %ld conditions to persistent IOV pool.",count);
    //count = persist->add("ConditionsIOVPool No 2",*manager.iovPool(*iov_typ));
    //total_count += count;
    printout(ALWAYS,"Example","+++ Added %ld conditions to persistent IOV pool.",count);
  }
  int nBytes = persist->save(conditions.c_str());
  printout(ALWAYS,"Example",
           "+++ Wrote %d Bytes (%ld conditions) of data to '%s'  [%8.3f seconds].",
           nBytes, total_count, conditions.c_str(), persist->duration);
  if ( nBytes > 0 )  {
    printout(ALWAYS,"Example",
             "+++ Successfully saved %ld condition to file.",total_count);
  }
  delete persist;
  
  printout(ALWAYS,"Statistics","+=========================================================================");
  printout(ALWAYS,"Statistics","+  Accessed a total of %ld conditions (S:%6ld,L:%6ld,C:%6ld,M:%ld)",
           total.total(), total.selected, total.loaded, total.computed, total.missing);
  printout(ALWAYS,"Statistics","+=========================================================================");

  // All done.
  return 1;
}

// first argument is the type from the xml file
DECLARE_APPLY(DD4hep_ConditionExample_save,condition_example)
