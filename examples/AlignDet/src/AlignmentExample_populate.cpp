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

   geoPluginRun -volmgr -destroy -plugin DD4hep_AlignmentExample_populate \
   -input file:${DD4hep_DIR}/examples/AlignDet/compact/Telescope.xml

   Populate the conditions store by hand for a set of IOVs.
   Then compute the corresponding alignment entries....

*/
// Framework include files
#include "AlignmentExampleObjects.h"
#include "DD4hep/Factories.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::AlignmentExamples;

/// Plugin function: Alignment program example
/**
 *  Factory: DD4hep_AlignmentExample_populate
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/12/2016
 */
static int alignment_example (Geometry::LCDD& lcdd, int argc, char** argv)  {

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
      "     name:   factory name     DD4hep_AlignmentExample1                        \n"
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

  /******************** Populate the conditions store *********************/
  // Have 10 run-slices [1001,2000] .... [9001,10000]
  size_t total_created = 0;
  for(int i=0; i<num_iov; ++i)  {
    IOV iov(iov_typ, IOV::Key(1+i*10,(i+1)*10));
    ConditionsPool* iov_pool = manager.registerIOV(*iov.iovType, iov.key());
    // Create conditions with all deltas. Use a generic creator
    total_created += Scanner().scan(AlignmentCreator(manager, *iov_pool),lcdd.world());
  }

  /******************** Now as usual: create the slice ********************/
  shared_ptr<ConditionsContent> content(new ConditionsContent());
  shared_ptr<ConditionsSlice>   slice(new ConditionsSlice(manager,content));
  Conditions::fill_content(manager,*content,*iov_typ);

  /******************** Register alignments *******************************/
  // Note: We have to load one set of conditions in order to auto-populate
  //       because we need to see if a detector element actually has alignment
  //       conditions. For this we must access the conditions data.
  //       Unfortunate, but unavoidable.
  //
  IOV iov(iov_typ,10+5);
  manager.prepare(iov,*slice);
  slice->pool->flags |= Conditions::UserPool::PRINT_INSERT;

  // Collect all the delta conditions and make proper alignment conditions out of them
  AlignmentsCalculator::Deltas deltas;
  Scanner(deltaCollector(*slice,deltas),lcdd.world());
  printout(INFO,"Prepare","Got a total of %ld Deltas",deltas.size());

  // ++++++++++++++++++++++++ Now compute the alignments for each of these IOVs
  ConditionsManager::Result cond_total;
  AlignmentsCalculator::Result align_total;
  for(int i=0; i<num_iov; ++i)  {
    IOV req_iov(iov_typ,i*10+5);
    shared_ptr<ConditionsSlice> sl(new ConditionsSlice(manager,content));
    // Attach the proper set of conditions to the user pool
    ConditionsManager::Result cres = manager.prepare(req_iov,*sl);
    sl->pool->flags |= Conditions::UserPool::PRINT_INSERT;
    cond_total += cres;
    // Now compute the tranformation matrices
    AlignmentsCalculator calculator;
    AlignmentsCalculator::Result ares = calculator.compute(deltas,*sl);
    printout(INFO,"Prepare","Total %ld/%ld conditions (S:%ld,L:%ld,C:%ld,M:%ld) of type %s. Alignments:(C:%ld,M:%ld)",
             slice->conditions().size(), cres.total(), cres.selected, cres.loaded,
             cres.computed, cres.missing, iov_typ->str().c_str(), ares.computed, ares.missing);
    align_total += ares;
    if ( ares.missing > 0 ) {
      printout(ERROR,"Compute","Failed tro compute %ld alignments of type %s.",
               ares.missing, iov_typ->str().c_str());
    }
  }
  // What else ? let's access/print the current selection
  Scanner(AlignedVolumePrinter(slice.get(),"Example"),lcdd.world());
  printout(INFO,"Summary","Processed a total %ld conditions (S:%ld,L:%ld,C:%ld,M:%ld) and (C:%ld,M:%ld) alignments. Created:%ld.",
           cond_total.total(), cond_total.selected, cond_total.loaded, cond_total.computed, cond_total.missing,
           align_total.computed, align_total.missing, total_created);
   
  // All done.
  return 1;
}

// first argument is the type from the xml file
DECLARE_APPLY(DD4hep_AlignmentExample_populate,alignment_example)
