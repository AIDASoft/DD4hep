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
#include "DD4hep/Printout.h"
#include "DD4hep/DetAlign.h"
#include "DD4hep/DetConditions.h"
#include "DD4hep/objects/AlignmentsInterna.h"

#include "DDAlign/AlignmentsForward.h"
#include "DDAlign/AlignmentUpdateCall.h"

using namespace DD4hep;
using namespace DD4hep::Alignments;

/// Initializing constructor
AlignmentsForward::AlignmentsForward(AlignmentsManager m, AlignmentUpdateCall* c, UserPool* p)
  : alignmentMgr(m), updateCall(c), user_pool(p)
{
}

/// Default destructor
AlignmentsForward::~AlignmentsForward()   {
  releasePtr(updateCall);
}

/// Callback to output conditions information
int AlignmentsForward::processElement(DetElement de)  {
  if ( de.isValid() )  {
    DetElement parent = de.parent();
    if ( parent.isValid() && parent.hasAlignments() && !de.hasAlignments() )  {
      DetAlign align(de);
      Conditions::ConditionKey k(de.path()+"#alignment/Tranformations");
      //
      // The alignment access through the DetElement object is optional!
      // It is slow and deprecated. The access using the UserPool directly
      // is highly favored.
      //
      align.alignments()->addKey(k.name);
      align.alignments()->addKey("Alignment",k.name);
      //
      // Now add the dependency to the alignmant manager
      Conditions::DependencyBuilder b(k, updateCall->addRef(), de);
      bool result = alignmentMgr.adoptDependency(b.release());
      if ( result )   {
        printout(INFO,"AlignForward",
                 "++ Added Alignment child dependency Cond:%s Key:%08X",
                 k.name.c_str(), k.hash);
        return 1;
      }
      printout(ERROR,"AlignForward",
               "++ FAILED to add Alignment dependency Cond:%s Key:%08X",
               k.name.c_str(), k.hash);
    }
  }
  return 1;
}
