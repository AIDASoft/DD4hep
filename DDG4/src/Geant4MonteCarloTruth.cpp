// $Id: $
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
#include "DDG4/Geant4MonteCarloTruth.h"

using namespace dd4hep::sim;

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

