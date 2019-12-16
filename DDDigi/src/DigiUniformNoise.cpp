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
#include "DDDigi/DigiRandomGenerator.h"
#include "DDDigi/DigiUniformNoise.h"

using namespace dd4hep::digi;

/// Standard constructor
DigiUniformNoise::DigiUniformNoise(const DigiKernel& krnl, const std::string& nam)
  : DigiSignalProcessor(krnl, nam)
{
  declareProperty("minimum", m_min);
  declareProperty("maximum", m_max);
  InstanceCount::increment(this);
}

/// Default destructor
DigiUniformNoise::~DigiUniformNoise() {
  InstanceCount::decrement(this);
}

/// Callback to read event uniformnoise
double DigiUniformNoise::operator()(DigiCellContext& context)  const  {
  return context.context.randomGenerator().uniform(m_min,m_max);
}
