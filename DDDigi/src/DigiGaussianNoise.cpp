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
#include "DDDigi/DigiGaussianNoise.h"

using namespace dd4hep::digi;

/// Standard constructor
DigiGaussianNoise::DigiGaussianNoise(const DigiKernel& krnl, const std::string& nam)
  : DigiSignalProcessor(krnl, nam)
{
  InstanceCount::increment(this);
}

/// Default destructor
DigiGaussianNoise::~DigiGaussianNoise() {
  InstanceCount::decrement(this);
}

/// Initialize the noise source
void DigiGaussianNoise::initialize()   {

  DigiSignalProcessor::initialize();
}

/// Callback to read event gaussiannoise
double DigiGaussianNoise::operator()(const DigiCellData& /* data */)  const    {
  return 0.0;
}
