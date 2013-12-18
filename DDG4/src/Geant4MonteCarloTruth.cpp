// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4MonteCarloTruth.h"

using namespace DD4hep::Simulation;

/// Standard action constructor
Geant4MonteCarloTruth::Geant4MonteCarloTruth()   {
  InstanceCount::increment(this);
}

/// Default destructor
Geant4MonteCarloTruth::~Geant4MonteCarloTruth() {
  InstanceCount::decrement(this);
}
