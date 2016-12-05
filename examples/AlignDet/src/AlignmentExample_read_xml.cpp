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

 geoPluginRun -volmgr -destroy -plugin DD4hep_AlignmentExample_read_xml \
              -input file:${DD4hep_DIR}/examples/AlignDet/compact/Telescope.xml \
              -delta file:${DD4hep_DIR}/examples/Conditions/data/repository.xml 

*/
// Framework include files
#include "AlignmentExampleObjects.h"
#include "DD4hep/Factories.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::AlignmentExamples;

/// Plugin function: Alignment program example
/**
 *  Factory: DD4hep_AlignmentExample_read_xml
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
      "     name:   factory name     DD4hep_AlignmentExample2                        \n"
      "     -input   <string>        Geometry file                                   \n"
      "     -deltas  <string>        Alignment deltas (Conditions                    \n"
      "\tArguments given: " << arguments(argc,argv) << endl << flush;
    ::exit(EINVAL);
  }

  // First we load the geometry
  lcdd.fromXML(input);
  installManagers(lcdd);

  ConditionsManager condMgr  = ConditionsManager::from(lcdd);
  AlignmentsManager alignMgr = AlignmentsManager::from(lcdd);
  const void* delta_args[] = {delta.c_str(), 0}; // Better zero-terminate

  lcdd.apply("DD4hep_ConditionsXMLRepositoryParser",1,(char**)delta_args);
  // Now the deltas are stored in the conditions manager in the proper IOV pools
  const IOVType* iov_typ = condMgr.iovType("run");
  if ( 0 == iov_typ )  {
    except("ConditionsPrepare","++ Unknown IOV type supplied.");
  }
  IOV req_iov(iov_typ,1500);      // IOV goes from run 1000 ... 2000
  dd4hep_ptr<ConditionsSlice> slice(createSlice(condMgr,*iov_typ));
  ConditionsManager::Result cres = condMgr.prepare(req_iov,*slice);
    
  // ++++++++++++++++++++++++ We need a valid set of conditions to do this!
  registerAlignmentCallbacks(lcdd,*slice,alignMgr);

  // ++++++++++++++++++++++++ Compute the tranformation matrices
  AlignmentsManager::Result ares = alignMgr.compute(*slice);

  // What else ? let's access the data
  Scanner().scan2(AlignmentDataAccess(slice->pool().get()),lcdd.world());

  // What else ? let's print the current selection
  Alignments::AlignedVolumePrinter printer(slice->pool().get(),"Example");
  Scanner().scan(printer,lcdd.world());

  printout(INFO,"Example",
           "Setup %ld/%ld conditions (S:%ld,L:%ld,C:%ld,M:%ld) (A:%ld,M:%ld) for IOV:%-12s",
           slice->conditions().size(),
           cres.total(), cres.selected, cres.loaded, cres.computed, cres.missing, 
           ares.computed, ares.missing, iov_typ->str().c_str());

  // ++++++++++++++++++++++++ All done.
  return 1;
}

// first argument is the type from the xml file
DECLARE_APPLY(DD4hep_AlignmentExample_read_xml,alignment_example)
