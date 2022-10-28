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
#include <DD4hep/InstanceCount.h>
#include <DDDigi/DigiOutputAction.h>

// C/C++ include files
#include <stdexcept>
#include <unistd.h>

using namespace std;
using namespace dd4hep::digi;

/// Standard constructor
DigiOutputAction::DigiOutputAction(const DigiKernel& kernel, const string& nam)
  : DigiEventAction(kernel, nam)
{
  declareProperty("containers", m_containers);
  declareProperty("output",     m_output);
  declareProperty("mask",       m_mask);
  InstanceCount::increment(this);
}

/// Default destructor
DigiOutputAction::~DigiOutputAction()   {
  InstanceCount::decrement(this);
}

/// Pre-track action callback
void DigiOutputAction::execute(DigiContext& /* context */)  const   {
  info("+++ Virtual method execute() --- Should not be called");
  ::sleep(1);
}
