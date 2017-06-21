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

 geoPluginRun -volmgr -destroy -plugin DD4hep_AlignmentExample_read_xml \
              -input file:${CMAKE_INSTALL_PREFIX}/share/DD4hep/examples/AlignDet/compact/Telescope.xml \
              -delta file:${CMAKE_INSTALL_PREFIX}/share/DD4hep/examples/Conditions/data/repository.xml 

*/
// Framework include files
#include "AlignmentExampleObjects.h"
#include "DD4hep/Factories.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::AlignmentExamples;

/// Plugin function: Alignment program example
/**
 *  Factory: DD4hep_AlignmentExample_read_xml
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/12/2016
 */
static int alignment_example (Detector& description, int argc, char** argv)  {

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
      "     name:   factory name     DD4hep_AlignmentExample_read_xml                \n"
      "     -input   <string>        Geometry file                                   \n"
      "     -deltas  <string>        Alignment deltas (Conditions                    \n"
      "\tArguments given: " << arguments(argc,argv) << endl << flush;
    ::exit(EINVAL);
  }

  // First we load the geometry
  description.fromXML(input);

  ConditionsManager manager  = installManager(description);
  const void*  delta_args[] = {delta.c_str(), 0}; // Better zero-terminate

  description.apply("DD4hep_ConditionsXMLRepositoryParser",1,(char**)delta_args);
  // Now the deltas are stored in the conditions manager in the proper IOV pools
  const IOVType* iov_typ = manager.iovType("run");
  if ( 0 == iov_typ )
    except("ConditionsPrepare","++ Unknown IOV type supplied.");

  IOV req_iov(iov_typ,1500);      // IOV goes from run 1000 ... 2000
  shared_ptr<ConditionsContent> content(new ConditionsContent());
  shared_ptr<ConditionsSlice>   slice(new ConditionsSlice(manager,content));
  cond::fill_content(manager,*content,*iov_typ);

  ConditionsManager::Result cres = manager.prepare(req_iov,*slice);
  // ++++++++++++++++++++++++ Compute the tranformation matrices
  AlignmentsCalculator          calc;
  ConditionsHashMap alignments;

  map<DetElement, Delta> deltas;
  Scanner(deltaCollector(*slice, deltas),description.world());
  printout(INFO,"Prepare","Got a total of %ld Deltas",deltas.size());

  slice->pool->flags |= cond::UserPool::PRINT_INSERT;
  AlignmentsCalculator::Result  ares = calc.compute(deltas, alignments);
  ConditionsSlice::Inserter(*slice,alignments.data);

  // What else ? let's access the data
  size_t total_accessed = Scanner().scan(AlignmentDataAccess(*slice),description.world());

  // What else ? let's print the current selection
  Scanner(AlignedVolumePrinter(slice.get(),"Example"),description.world());

  printout(INFO,"Example",
           "%ld conditions in slice. (T:%ld,S:%ld,L:%ld,C:%ld,M:%ld) "
           "Alignments accessed: %ld (A:%ld,M:%ld) for IOV:%-12s",
           slice->conditions().size(),
           cres.total(), cres.selected, cres.loaded, cres.computed, cres.missing, 
           total_accessed, ares.computed, ares.missing, iov_typ->str().c_str());

  // ++++++++++++++++++++++++ All done.
  return 1;
}

// first argument is the type from the xml file
DECLARE_APPLY(DD4hep_AlignmentExample_read_xml,alignment_example)
