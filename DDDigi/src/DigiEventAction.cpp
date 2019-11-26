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

// Framework include files
#include "DD4hep/InstanceCount.h"
#include "DDDigi/DigiEventAction.h"

/// Standard constructor
dd4hep::digi::DigiEventAction::DigiEventAction(const DigiKernel& krnl, const std::string& nam)
  : DigiAction(krnl, nam)
{
  InstanceCount::increment(this);
  declareProperty("parallel", m_parallel);
}

/// Default destructor
dd4hep::digi::DigiEventAction::~DigiEventAction() {
  InstanceCount::decrement(this);
}

/// Set the parallization flag; returns previous value
bool dd4hep::digi::DigiEventAction::setExecuteParallel(bool new_value)    {
  bool old = m_parallel;
  m_parallel = new_value;
  return old;
}

