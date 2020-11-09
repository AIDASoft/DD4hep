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
using namespace dd4hep;
using namespace dd4hep::AlignmentExamples;

/// Plugin function: Alignment program example
/**
 *  Factory: DD4hep_AlignmentExample_nominal
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/12/2016
 */
static int alignment_example (Detector& description, int argc, char** argv)  {
  class Collector  {
  public:
    map<DetElement, Delta>& deltas;
    ConditionsMap&          mapping;
    Collector(map<DetElement, Delta>& del, ConditionsMap& cond_map)
      : deltas(del), mapping(cond_map) {}
    // Here we test the ConditionsMap interface of the AlignmentsNominalMap
    int operator()(DetElement de, int )  const    {
      Alignment a = mapping.get(de, align::Keys::alignmentKey);
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
  description.fromXML(input);


  // ++++++++++++++++++++++++ Try scam with the fake AlignmentsNominalMap
  AlignmentsNominalMap nominal(description.world());
  
  // Collect all the delta conditions and make proper alignment conditions out of them
  map<DetElement, Delta> deltas;
  // Show that the access interface works:
  int num_delta = Scanner().scan(Collector(deltas,nominal),description.world());
  /// Show that utilities can work with this one:
  int num_printed = Scanner().scan(AlignmentsPrinter(&nominal),description.world());
  printout(ALWAYS,"Prepare","Got a total of %ld Deltas (Nominals: %d , Printed: %d)",
           deltas.size(), num_delta, num_printed);

  // ++++++++++++++++++++++++ Now compute the alignments for a generic slice
  ConditionsTreeMap slice;
  // Now compute the tranformation matrices
  AlignmentsCalculator calculator;
  AlignmentsCalculator::Result ares = calculator.compute(deltas,slice);  
  printout(ALWAYS,"Compute","Total %ld conditions inserted. Alignments:(C:%ld,M:%ld)",
           slice.data.size(), ares.computed, ares.missing);
  if ( ares.missing > 0 ) {
    printout(ERROR,"Compute","Failed tro compute %ld alignments.",ares.missing);
  }
  printout(ALWAYS,"Summary","Printed %d, scanned %d and computed a total of %ld alignments (C:%ld,M:%ld).",
           num_printed, num_delta, slice.data.size(), ares.computed, ares.missing);
  // All done.
  return 1;
}

// first argument is the type from the xml file
DECLARE_APPLY(DD4hep_AlignmentExample_nominal,alignment_example)
