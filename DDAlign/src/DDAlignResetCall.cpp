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
#include "DDAlign/DDAlignResetCall.h"
#include "DD4hep/ConditionsData.h"
#include "DD4hep/Printout.h"

using namespace DD4hep;
using namespace DD4hep::Conditions;

/// Interface to client Callback in order to reset the condition
Condition
Alignments::DDAlignResetCall::operator()(const ConditionKey& key, const UpdateContext& context)
{
  Condition  cond = context.condition(0);
  DetElement det  = context.dependency.detector;
  if ( cond.typeInfo() == typeid(Data::Delta) )  {
    Data::Delta empty_delta;
    Data::Delta& delta = cond.get<Data::Delta>();
    delta = empty_delta;
    Condition c = AlignmentsUpdateCall::handle(key, context, empty_delta);
    printLevel = INFO;
    printout(printLevel,"DDAlignReset","++ Building dependent condition: %s Detector [%d]: %s [%p]",
             key.name.c_str(), det.level(), det.path().c_str(), c.ptr());
    return c;
  }
  // Somehow the condition is not of type Data::Delta. This is an ERROR.
  return invalidDataType(key, context);
}
