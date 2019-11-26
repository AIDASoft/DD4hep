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
#include "DDDigi/DigiSignalProcessor.h"

/// Standard constructor
dd4hep::digi::DigiSignalProcessor::DigiSignalProcessor(const DigiKernel& krnl, const std::string& nam)
  : DigiAction(krnl, nam)
{
  InstanceCount::increment(this);
}

/// Default destructor
dd4hep::digi::DigiSignalProcessor::~DigiSignalProcessor() {
  InstanceCount::decrement(this);
}

