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
#include "DDDigi/DigiDDG4Input.h"

// C/C++ include files
#include <stdexcept>
#include <unistd.h>

using namespace std;
using namespace dd4hep::digi;

/// Standard constructor
DigiDDG4Input::DigiDDG4Input(const DigiKernel& kernel, const string& nam)
  : DigiInputAction(kernel, nam)
{
  InstanceCount::increment(this);
}

/// Default destructor
DigiDDG4Input::~DigiDDG4Input()   {
  InstanceCount::decrement(this);
}

/// Pre-track action callback
void DigiDDG4Input::execute(DigiContext& /* context */)  const   {
  ::sleep(1);
  info("+++ Virtual method execute()");
  return;
  //except("DigiDDG4Input","+++ Virtual method execute() MUST be overloaded!");
}
