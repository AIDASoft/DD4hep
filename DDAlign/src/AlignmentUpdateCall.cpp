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
#include "DDAlign/AlignmentUpdateCall.h"
#include "DDAlign/AlignmentsManager.h"
#include "DD4hep/ConditionsPrinter.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/Printout.h"

using namespace DD4hep::Alignments;

/// Default constructor
AlignmentUpdateCall::AlignmentUpdateCall()
  : DD4hep::Conditions::ConditionUpdateCall(), printLevel(DEBUG)
{
  InstanceCount::increment(this);
}

/// Default destructor
AlignmentUpdateCall::~AlignmentUpdateCall() {
  InstanceCount::decrement(this);
}

AlignmentUpdateCall::Condition
AlignmentUpdateCall::handle(const ConditionKey& key, const UpdateContext& ctxt, const Delta& delta)  {
  AlignmentCondition target(key.name);
  AlignmentData&     data = target.data();
  data.delta     = delta;
  data.flag      = AlignmentData::HAVE_NONE;
  data.detector  = ctxt.dependency.detector;
  //
  // This here is the main difference compared to other derived conditions:
  // ----------------------------------------------------------------------
  //
  // We enforce here that all computations, which require an update of the corresponding
  // alignment matrices are stored in "new_alignments", since the update callback registers
  // all new entries using this user parameter when calling  AlignmentsManager::newEntry.
  // For this reason also ALL specific update calls must base themself in the
  // Alignment update callback.
  //
  // We MUST register the undigested condition to the alignment manager in order to
  // later be able to compute the derived transformations.
  // The derived transfomrations can only be computed 'later' once all delta stubs
  // are present in a second pass. This is necessary, because the parent information
  // may actually be supplied also 'later'.
  //
  AlignmentsManager::newEntry(ctxt, data.detector, &ctxt.dependency, target);
  return target;
}

/// Handler to be called if the Alignment cannot be created due to a bad underlying data type.
AlignmentUpdateCall::Condition
AlignmentUpdateCall::invalidDataType(const ConditionKey& key, const UpdateContext& context)  {
  // Here only print and return an empty alignment condition.
  // Otherwise the return is not accepted!
  // TODO: To be decided how to handle this error
  Condition  cond = context.condition(0);
  DetElement det  = context.dependency.detector;
  Alignments::AlignmentCondition target(key.name);
  Data& data = target.data();
  data.detector = det;
  printout(ERROR,"AlignmentUpdate","++ Failed to access alignment-Delta for %s from %s",
           det.path().c_str(), cond->value.c_str());
  printout(ERROR,"AlignmentUpdate","++ The true data type is: %s",typeName(cond.typeInfo()).c_str());
  Conditions::ConditionsPrinter()(cond);
  return target;
}
