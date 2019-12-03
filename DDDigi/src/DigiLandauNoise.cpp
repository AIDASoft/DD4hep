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
#include "DDDigi/DigiLandauNoise.h"
#include "DDDigi/DigiSegmentation.h"
#include "DDDigi/DigiRandomGenerator.h"

using namespace dd4hep::digi;

/// Standard constructor
DigiLandauNoise::DigiLandauNoise(const DigiKernel& krnl, const std::string& nam)
  : DigiSignalProcessor(krnl, nam)
{
  declareProperty("meam",    m_mean);
  declareProperty("sigma",   m_sigma);
  declareProperty("cutoff",  m_cutoff);
  InstanceCount::increment(this);
}

/// Default destructor
DigiLandauNoise::~DigiLandauNoise() {
  InstanceCount::decrement(this);
}

/// Callback to read event landaunoise
double DigiLandauNoise::operator()(DigiContext& context, const DigiCellData& data)  const  {
  if ( data.signal < m_cutoff )
    return 0;
  return context.randomGenerator().landau(m_mean,m_sigma);
}
