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
#include "DDDigi/DigiInputAction.h"

// C/C++ include files
#include <stdexcept>
#include <unistd.h>

using namespace std;
using namespace dd4hep::digi;

/// Standard constructor
DigiInputAction::DigiInputAction(const DigiKernel& kernel, const string& nam)
  : DigiAction(kernel, nam)
{
  declareProperty("Input", m_input);
  InstanceCount::increment(this);
}

/// Default destructor
DigiInputAction::~DigiInputAction()   {
  InstanceCount::decrement(this);
}

/// Pre-track action callback
void DigiInputAction::execute(DigiContext& /* context */)  const   {
  ::sleep(1);
  info("+++ Virtual method execute()");
  return;
  //except("DigiInputAction","+++ Virtual method execute() MUST be overloaded!");
}
