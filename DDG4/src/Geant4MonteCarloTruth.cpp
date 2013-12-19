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

/// Standard constructor
Geant4DummyTruthHandler::Geant4DummyTruthHandler(Geant4Context* ctxt,const std::string& nam) : Geant4Action(ctxt,nam), Geant4MonteCarloTruth()
{
}

/// Default destructor
Geant4DummyTruthHandler::~Geant4DummyTruthHandler()
{
}

/// Mark a Geant4 track to be kept for later MC truth analysis
void Geant4DummyTruthHandler::mark(const G4Track* )  {
}

/// Store a track, with a flag
void Geant4DummyTruthHandler::mark(const G4Track* , bool ) {
}

/// Mark a Geant4 track of the step to be kept for later MC truth analysis
void Geant4DummyTruthHandler::mark(const G4Step* ) {
}

/// Store a track produced in a step to be kept for later MC truth analysis
void Geant4DummyTruthHandler::mark(const G4Step* , bool ) {
}

