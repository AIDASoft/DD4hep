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
#include "DDDigi/DigiRandomNoise.h"

using namespace dd4hep::digi;

/// Standard constructor
DigiRandomNoise::DigiRandomNoise(const DigiKernel& krnl, const std::string& nam)
  : DigiSignalProcessor(krnl, nam)
{
  InstanceCount::increment(this);
}

/// Default destructor
DigiRandomNoise::~DigiRandomNoise() {
  InstanceCount::decrement(this);
}

/// Initialize the noise source
void DigiRandomNoise::initialize()   {
  std::default_random_engine generator;
  m_noise.init(m_poles, m_alpha, m_variance);
  m_noise.normalize(generator, 5000);
  DigiSignalProcessor::initialize();
}

/// Callback to read event randomnoise
double DigiRandomNoise::operator()(const DigiCellData& /* data */)  const    {
  return 0.0;
}
