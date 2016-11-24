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
#include "DDAlign/DDAlignForwardCall.h"
#include "DD4hep/ConditionsData.h"
#include "DD4hep/Printout.h"

using namespace DD4hep;
using namespace DD4hep::Conditions;

/// Interface to client Callback in order to update the condition
Condition
Alignments::DDAlignForwardCall::operator()(const ConditionKey& key, const UpdateContext& context)
{
  Data::Delta delta;
  DetElement det  = context.dependency.detector;
  Condition c = AlignmentUpdateCall::handle(key, context, delta);
  printout(INFO,"DDAlignForward","++ Building child alignment condition: %s Detector [%d]: %s [%p]",
           key.name.c_str(), det.level(), det.path().c_str(), c.ptr());
  return c;
}
