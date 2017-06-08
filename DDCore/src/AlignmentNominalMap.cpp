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

// Framework include files
#include "DD4hep/Printout.h"
#include "DD4hep/DetectorProcessor.h"
#include "DD4hep/AlignmentsNominalMap.h"
#include "DD4hep/detail/AlignmentsInterna.h"
#include "DD4hep/detail/ConditionsInterna.h"

using namespace DD4hep;
using namespace Conditions;
using Alignments::Keys;

/// Standard constructor
AlignmentsNominalMap::AlignmentsNominalMap(DetElement wrld) : world(wrld) {
}

/// Insert a new entry to the map
bool AlignmentsNominalMap::insert(DetElement   /* detector  */,
                                  unsigned int /* key       */,
                                  Condition    /* condition */)   {
  return false;
}

/// Interface to access conditions by hash value
Condition AlignmentsNominalMap::get(DetElement detector, unsigned int key) const   {
  if ( key == Keys::alignmentKey )  {
    return Condition(detector.nominal().ptr());
  }
  return Condition();
}

/// Interface to scan data content of the conditions mapping
void AlignmentsNominalMap::scan(const Processor& processor) const  {
  /// Heklper to implement partial scans.
  struct Scanner  {
    const Processor& proc;
    /// Constructor
    Scanner(const Processor& p) : proc(p){}
    /// Conditions callback for object processing
    int operator()(DetElement de, int /* level */)  const  {
      Condition c = de.nominal();
      return proc(c);
    }
  } scanner(processor);
  // We emulate here a full detector scan, access the nominal alignments and process them by the processor.
  if ( world.isValid() )  {
    Geometry::DetectorScanner().scan(scanner,world,0,true);
    return;
  }
  DD4hep::except("AlignmentsNominalMap",
                 "Cannot scan conditions map for conditions of an invalid top level detector element!");
}

/// Interface to partially scan data content of the conditions mapping
void AlignmentsNominalMap::scan(DetElement   detector,
                                itemkey_type lower,
                                itemkey_type upper,
                                const Processor&   processor) const   {
  if ( detector.isValid() )   {
    if ( lower <= Keys::alignmentKey && upper >= Keys::alignmentKey )  {
      Condition c(detector.nominal().ptr());
      processor(c);
    }
    return;
  }
  DD4hep::except("AlignmentsNominalMap",
                 "Cannot scan conditions map for conditions of an invalid detector element!");
}
