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
#include "DDDigi/DigiGaussianNoise.h"

using namespace dd4hep::digi;

/// Standard constructor
DigiGaussianNoise::DigiGaussianNoise(const DigiKernel& krnl, const std::string& nam)
  : DigiSignalProcessor(krnl, nam)
{
  declareProperty("meam",    m_mean);
  declareProperty("sigma",   m_sigma);
  declareProperty("cutoff",  m_cutoff);
  InstanceCount::increment(this);
}

/// Default destructor
DigiGaussianNoise::~DigiGaussianNoise() {
  InstanceCount::decrement(this);
}

/// Callback to read event gaussiannoise
double DigiGaussianNoise::operator()(DigiContext& context, const DigiCellData& data)  const  {
  if ( data.signal < m_cutoff )
    return 0;
  return context.randomGenerator().gaussian(m_mean,m_sigma);
}
