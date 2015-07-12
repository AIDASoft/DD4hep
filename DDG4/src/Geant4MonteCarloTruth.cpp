// $Id: Handle.h 570 2013-05-17 07:47:11Z markus.frank $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation européenne pour la Recherche nucléaire (CERN)
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

/// Standard constructor
Geant4DummyTruthHandler::Geant4DummyTruthHandler(Geant4Context* ctxt,const std::string& nam) 
  : Geant4Action(ctxt,nam), Geant4MonteCarloTruth()
{
}

/// Default destructor
Geant4DummyTruthHandler::~Geant4DummyTruthHandler()
{
}

/// Mark a Geant4 track to be kept for later MC truth analysis
void Geant4DummyTruthHandler::mark(const G4Track*)  {
}

/// Store a track, with a flag
void Geant4DummyTruthHandler::mark(const G4Track*, int ) {
}

/// Mark a Geant4 track of the step to be kept for later MC truth analysis
void Geant4DummyTruthHandler::mark(const G4Step*) {
}

/// Store a track produced in a step to be kept for later MC truth analysis
void Geant4DummyTruthHandler::mark(const G4Step*, int ) {
}

