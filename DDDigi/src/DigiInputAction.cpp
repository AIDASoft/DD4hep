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
#include <DDDigi/DigiInputAction.h>

// C/C++ include files
#include <stdexcept>
#include <unistd.h>

using namespace std;
using namespace dd4hep::digi;

/// Standard constructor
DigiInputAction::DigiInputAction(const DigiKernel& kernel, const string& nam)
  : DigiEventAction(kernel, nam)
{
  declareProperty("input",   m_input_sources);
  declareProperty("segment", m_input_segment);
  declareProperty("mask",    m_input_mask);
  declareProperty("rescan",  m_input_rescan);
  InstanceCount::increment(this);
}

/// Default destructor
DigiInputAction::~DigiInputAction()   {
  InstanceCount::decrement(this);
}

/// Pre-track action callback
void DigiInputAction::execute(DigiContext& /* context */)  const   {
  info("+++ Virtual method execute() --- Should not be called");
  ::sleep(1);
}
