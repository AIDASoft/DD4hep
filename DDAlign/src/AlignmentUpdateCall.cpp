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

// Framework include files
#include "DDAlign/AlignmentUpdateCall.h"
#include "DDAlign/AlignmentsManager.h"
#include "DD4hep/InstanceCount.h"

using namespace DD4hep::Alignments;

/// Default constructor
AlignmentUpdateCall::AlignmentUpdateCall() : DD4hep::Conditions::ConditionUpdateCall()
{
}

/// Default destructor
AlignmentUpdateCall::~AlignmentUpdateCall() {
}

AlignmentUpdateCall::Condition
AlignmentUpdateCall::handle(const ConditionKey& key, const UpdateContext& ctxt, const Delta& delta)
{
  AlignmentCondition target(key.name);
  AlignmentData&     data = target.data();
  data.delta    = delta;
  data.flag     = AlignmentData::HAVE_NONE;
  data.detector = ctxt.dependency.detector;
  // We MUST register the undigested condition to the alignment manager in order to
  // later be able to compute the derived transformations.
  // The derived transfomrations can only be computed 'later' once all delta stubs
  // are present in a second pass. This is necessary, because the parent information
  // may actually be supplied also 'later'.
  AlignmentsManager::newEntry(ctxt, data.detector, &ctxt.dependency, target);
  return target;
}
