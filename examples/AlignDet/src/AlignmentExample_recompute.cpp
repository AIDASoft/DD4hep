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

 geoPluginRun -volmgr -destroy -plugin DD4hep_AlignmentExample_recompute \
              -input file:${DD4hep_DIR}/examples/AlignDet/compact/Telescope.xml \
              -delta file:${DD4hep_DIR}/examples/Conditions/data/repository.xml 

*/
// Framework include files
#include "AlignmentExampleObjects.h"
#include "DD4hep/Factories.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::AlignmentExamples;
#if 0
/// Plugin function: Alignment program example
/**
 *  Factory: DD4hep_AlignmentExample_recompute
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/12/2016
 */
static int alignment_example (Geometry::LCDD& lcdd, int argc, char** argv)  {

  string input, delta;
  bool arg_error = false;
  for(int i=0; i<argc && argv[i]; ++i)  {
    if ( 0 == ::strncmp("-input",argv[i],4) )
      input = argv[++i];
    else if ( 0 == ::strncmp("-deltas",argv[i],5) )
      delta = argv[++i];
    else
      arg_error = true;
  }
  if ( arg_error || input.empty() || delta.empty() )   {
    /// Help printout describing the basic command line interface
    cout <<
      "Usage: -plugin <name> -arg [-arg]                                             \n"
      "     name:   factory name     DD4hep_AlignmentExample_recompute               \n"
      "     -input   <string>        Geometry file                                   \n"
      "     -deltas  <string>        Alignment deltas (Conditions                    \n"
      "\tArguments given: " << arguments(argc,argv) << endl << flush;
    ::exit(EINVAL);
  }

  // First we load the geometry
  lcdd.fromXML(input);
  installManagers(lcdd);

  ConditionsManager condMgr  = ConditionsManager::from(lcdd);
  const void* delta_args[] = {delta.c_str(), 0}; // Better zero-terminate

  lcdd.apply("DD4hep_ConditionsXMLRepositoryParser",1,(char**)delta_args);
  // Now the deltas are stored in the conditions manager in the proper IOV pools
  const IOVType* iov_typ = condMgr.iovType("run");
  if ( 0 == iov_typ )  {
    except("ConditionsPrepare","++ Unknown IOV type supplied.");
  }
  IOV req_iov(iov_typ,1500);      // IOV goes from run 1000 ... 2000
  shared_ptr<ConditionsContent> content(new ConditionsContent());
  shared_ptr<ConditionsSlice>   slice(new ConditionsSlice(condMgr,content));
  Conditions::fill_content(condMgr,*content,*iov_typ);
  ConditionsManager::Result   cres_align = condMgr.prepare(req_iov,*slice_align);


  // ++++++++++++++++++++++++ Compute the tranformation matrices
  AlignmentsCalculator          calc;
  AlignmentsCalculator::Deltas  deltas;
  Conditions::ConditionsHashMap alignments;
  AlignmentsCalculator::Result  ares = calc.compute(deltas, alignments);
  ConditionsSlice::Inserter(*slice).process(alignments.data);

  // What else ? let's access the data
  Scanner().scan(AlignmentDataAccess(*slice_align),lcdd.world());

  // What else ? let's print the current selection
  Alignments::AlignedVolumePrinter print_align(*slice_align,"Example");
  Scanner().scan(print_align,lcdd.world());

  // What else ? let's access the data
  //Scanner().scan(AlignmentReset(*slice_align->pool,INFO),lcdd.world());
  // Print again the changed values

  // ++++++++++++++++++++++++ Compute the tranformation matrices
  shared_ptr<ConditionsSlice> slice_reset(new ConditionsSlice(condMgr,content));

  ConditionsManager::Result   cres_reset = condMgr.prepare(req_iov,*slice_reset);


  AlignmentsManager::Result   ares_reset = alignMgr.compute(*slice_reset);
  Alignments::AlignedVolumePrinter print_reset(slice_reset->pool.get(),"Example");
  Scanner().scan(print_reset,lcdd.world());
  
  printout(INFO,"Example",
           "Setup %ld/%ld conditions (S:%ld,L:%ld,C:%ld,M:%ld) (A:%ld,M:%ld) for IOV:%-12s",
           slice_align->conditions().size(),
           cres_align.total(), cres_align.selected, cres_align.loaded, cres_align.computed, cres_align.missing, 
           ares_align.computed, ares_align.missing, iov_typ->str().c_str());
  printout(INFO,"Example",
           "Setup %ld/%ld conditions (S:%ld,L:%ld,C:%ld,M:%ld) (A:%ld,M:%ld) for IOV:%-12s",
           slice_reset->conditions().size(),
           cres_reset.total(), cres_reset.selected, cres_reset.loaded, cres_reset.computed, cres_reset.missing, 
           ares_reset.computed, ares_reset.missing, iov_typ->str().c_str());

  // ++++++++++++++++++++++++ All done.
  return 1;
}

// first argument is the type from the xml file
DECLARE_APPLY(DD4hep_AlignmentExample_recompute,alignment_example)
#endif
