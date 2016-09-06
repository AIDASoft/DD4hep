// $Id$
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
#include "DDAlign/AlignmentsManager.h"
#include "DD4hep/ConditionsData.h"
#include "DD4hep/Alignments.h"
#include "DD4hep/Printout.h"

using namespace DD4hep;
using namespace DD4hep::Conditions;

/// Default constructor
DDDB::DDDBAlignmentUpdateCall::DDDBAlignmentUpdateCall()
  : Conditions::ConditionUpdateCall()
{
}

/// Default destructor
DDDB::DDDBAlignmentUpdateCall::~DDDBAlignmentUpdateCall() {
}

/// Interface to client Callback in order to update the condition
DDDB::DDDBAlignmentUpdateCall::Condition
DDDB::DDDBAlignmentUpdateCall::operator()(const ConditionKey& key, const UpdateContext& context)
{
  typedef OpaqueDataBlock Param;
  typedef Alignments::AlignmentData Data;
  Alignments::AlignmentCondition target(key.name);
  Data&          data = target.data();
  Condition      cond = context.condition(0);
  AbstractMap&   src  = cond.get<AbstractMap>();
  const Param&   par  = src.firstParam().second;
  DetElement     det  = context.dependency.detector;
  printout(DEBUG,"AlignmentUpdate","++ Building dependent condition: %s Detector [%d]: %s ",
           key.name.c_str(), det.level(), det.path().c_str());
  if ( par.typeInfo() == typeid(Data::Delta) )  {
    const Data::Delta& delta = src.first<Data::Delta>();
    data.delta = delta;
    data.flag  = Data::HAVE_NONE;
  }
  else  {
    printout(INFO,"AlignmentUpdate","++ Failed to access alignment-Delta from %s",
             cond->value.c_str());
    ConditionPrinter()(cond);
  }
  data.detector = det;
  Alignments::AlignmentsManager::newEntry(context, det, &context.dependency, target);
  // A callback returning no condition is illegal!
  // Need to crosscheck though if the alignment IOV interval of parents may be
  // smaller then the daughter IOV interval.
  // I though expect, that this is a purely academic case.
  return target;
}
