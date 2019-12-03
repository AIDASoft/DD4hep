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
#include "DDDigi/DigiSegmentation.h"
#include "DDDigi/DigiRandomGenerator.h"
#include "DDDigi/DigiExponentialNoise.h"

using namespace dd4hep::digi;

/// Standard constructor
DigiExponentialNoise::DigiExponentialNoise(const DigiKernel& krnl, const std::string& nam)
  : DigiSignalProcessor(krnl, nam)
{
  declareProperty("tau",     m_tau);
  InstanceCount::increment(this);
}

/// Default destructor
DigiExponentialNoise::~DigiExponentialNoise() {
  InstanceCount::decrement(this);
}

/// Callback to read event exponentialnoise
double DigiExponentialNoise::operator()(DigiContext& context, const DigiCellData&)  const  {
  return context.randomGenerator().exponential(m_tau);
}
