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

// Framework includes
#include "DDAlign/AlignmentsForward.h"
#include "DDAlign/AlignmentsUpdateCall.h"

#include "DD4hep/Printout.h"
#include "DD4hep/DetAlign.h"
#include "DD4hep/DetConditions.h"
#include "DDCond/ConditionsSlice.h"

using namespace DD4hep;
using namespace DD4hep::Alignments;

/// Initializing constructor
AlignmentsForward::AlignmentsForward(Slice& s, AlignmentsUpdateCall* c)
  : slice(s), updateCall(c), extension("#alignment/Transformations"),
    alias("Alignment"), haveAlias(true), printLevel(DEBUG)
{
}

/// Default destructor
AlignmentsForward::~AlignmentsForward()   {
  releasePtr(updateCall);
}

/// Overloadable: call to construct the alignment conditions name. Specialize for change
std::string AlignmentsForward::construct_name(DetElement de) const   {
  return de.path()+extension;
}

/// Callback to output conditions information
int AlignmentsForward::processElement(DetElement de)  {
  if ( de.isValid() )  {
    DetElement parent = de.parent();
    if ( parent.isValid() && parent.hasAlignments() && !de.hasAlignments() )  {
      std::string alignment_name = construct_name(de);
      if ( !alignment_name.empty() )  {
        DetAlign align(de);
        Conditions::ConditionKey k(alignment_name);
        //
        // The alignment access through the DetElement object is optional!
        // It is slow and deprecated. The access using the UserPool directly
        // is highly favored.
        //
        align.alignments().insertKey(k.name);
        if ( haveAlias && !alias.empty() )  {
          align.alignments().insertKey("Alignment",k.name);
        }
        //
        // Now add the dependency to the alignmant manager
        Conditions::DependencyBuilder b(k, updateCall, de);
        bool result = slice.insert(b.release());
        if ( result )   {
          printout(printLevel,"AlignForward",
                   "++ Added Alignment child dependency Cond:%s Key:%16llX",
                   k.name.c_str(), k.hash);
          return 1;
        }
        printout(ERROR,"AlignForward",
                 "++ FAILED to add Alignment dependency Cond:%s Key:%16llX",
                 k.name.c_str(), k.hash);
      }
    }
  }
  return 1;
}
