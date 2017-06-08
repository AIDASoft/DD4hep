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

   geoPluginRun -volmgr -destroy -plugin DD4hep_AlignmentExample_nominal \
   -input file:${DD4hep_DIR}/examples/AlignDet/compact/Telescope.xml

   Access the DetElement nominal conditions using the AlignmentNominalMap.
   Any use of DDCond is inhibited.

   1) We use the generic printer, which during the detector element scan accesses the
      conditions map.
   2) We use a delta scanner to extract the nominal deltas from the DetElement's 
      nominal alignments
   3) We use a ConditionsTreeMap to perform the alignments re-computation.
*/
// Framework include files
#include "AlignmentExampleObjects.h"
#include "DD4hep/AlignmentsNominalMap.h"
#include "DD4hep/Factories.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::AlignmentExamples;

/// Plugin function: Alignment program example
/**
 *  Factory: DD4hep_AlignmentExample_nominal
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/12/2016
 */
static int alignment_example (Geometry::LCDD& lcdd, int argc, char** argv)  {
  class Collector  {
  public:
    AlignmentsCalculator::Deltas& deltas;
    ConditionsMap&                mapping;
    Collector(AlignmentsCalculator::Deltas& d, ConditionsMap& m)
      : deltas(d), mapping(m) {}
    // Here we test the ConditionsMap interface of the AlignmentsNominalMap
    int operator()(DetElement de, int )  const    {
      Alignment a = mapping.get(de, Alignments::Keys::alignmentKey);
      deltas.insert(make_pair(de,a.delta()));
      return 1;
    }
  };
  string input;
  bool   arg_error = false;
  for(int i=0; i<argc && argv[i]; ++i)  {
    if ( 0 == ::strncmp("-input",argv[i],4) )
      input = argv[++i];
    else
      arg_error = true;
  }
  if ( arg_error || input.empty() )   {
    /// Help printout describing the basic command line interface
    cout <<
      "Usage: -plugin <name> -arg [-arg]                                             \n"
      "     name:   factory name     DD4hep_AlignmentExample1                        \n"
      "     -input   <string>        Geometry file                                   \n"
      "\tArguments given: " << arguments(argc,argv) << endl << flush;
    ::exit(EINVAL);
  }

  // First we load the geometry
  lcdd.fromXML(input);


  // ++++++++++++++++++++++++ Try scam with the fake AlignmentsNominalMap
  Conditions::AlignmentsNominalMap nominal(lcdd.world());
  
  // Collect all the delta conditions and make proper alignment conditions out of them
  AlignmentsCalculator::Deltas deltas;
  // Show that the access interface works:
  int num_delta = Scanner().scan(Collector(deltas,nominal),lcdd.world());
  /// Show that utilities can work with this one:
  int num_printed = Scanner().scan(AlignmentsPrinter(&nominal),lcdd.world());
  printout(INFO,"Prepare","Got a total of %ld Deltas (Nominals: %d , Printed: %d)",
           deltas.size(), num_delta, num_printed);

  // ++++++++++++++++++++++++ Now compute the alignments for a generic slice
  Conditions::ConditionsTreeMap slice;
  // Now compute the tranformation matrices
  AlignmentsCalculator calculator;
  AlignmentsCalculator::Result ares = calculator.compute(deltas,slice);  
  printout(INFO,"Compute","Total %ld conditions inserted. Alignments:(C:%ld,M:%ld)",
           slice.data.size(), ares.computed, ares.missing);
  if ( ares.missing > 0 ) {
    printout(ERROR,"Compute","Failed tro compute %ld alignments.",ares.missing);
  }
  printout(INFO,"Summary","Printed %d, scanned %d and computed a total of %ld alignments (C:%ld,M:%ld).",
           num_printed, num_delta, slice.data.size(), ares.computed, ares.missing);
  // All done.
  return 1;
}

// first argument is the type from the xml file
DECLARE_APPLY(DD4hep_AlignmentExample_nominal,alignment_example)
