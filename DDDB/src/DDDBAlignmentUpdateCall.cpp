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
//
// DDDB is a detector description convention developed by the LHCb experiment.
// For further information concerning the DTD, please see:
// http://lhcb-comp.web.cern.ch/lhcb-comp/Frameworks/DetDesc/Documents/lhcbDtd.pdf
//
//==========================================================================

// Framework includes
#include "DDDB/DDDBAlignmentUpdateCall.h"
#include "DDDB/DDDBConditionPrinter.h"
#include "DD4hep/ConditionsData.h"
#include "DD4hep/Printout.h"

using namespace DD4hep;
using namespace DD4hep::Conditions;

/// Interface to client Callback in order to update the condition
DDDB::DDDBAlignmentUpdateCall::Condition
DDDB::DDDBAlignmentUpdateCall::operator()(const ConditionKey& key, const UpdateContext& context)
{
  Condition        cond = context.condition(0);
  AbstractMap&     src  = cond.get<AbstractMap>();
  OpaqueDataBlock& par  = src.firstParam().second;
  DetElement       det  = context.dependency.detector;
  printout(DEBUG,"AlignmentUpdate","++ Building dependent condition: %s Detector [%d]: %s ",
           key.name.c_str(), det.level(), det.path().c_str());
  if ( par.typeInfo() == typeid(Data::Delta) )  {
    const Data::Delta& delta = src.first<Data::Delta>();
    return AlignmentsUpdateCall::handle(key, context, delta);
  }
  // Somehow the condition is not of type Data::Delta. This is an ERROR.
  // Here only print and return an empty alignment condition.
  // Otherwise the return is not accepted!
  // TODO: To be decided how to handle this error
  Alignments::AlignmentCondition target(key.name);
  Data& data = target.data();
  data.detector = det;
  printout(INFO,"AlignmentUpdate","++ Failed to access alignment-Delta from %s",
           cond->value.c_str());
  ConditionPrinter("AlignmentUpdate")(cond);
  return target;
}
