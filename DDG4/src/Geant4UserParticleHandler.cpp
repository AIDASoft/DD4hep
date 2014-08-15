// $Id: Geant4Field.cpp 888 2013-11-14 15:54:56Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
// Framework include files
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4UserParticleHandler.h"

using namespace DD4hep::Simulation;

/// Standard constructor
Geant4UserParticleHandler::Geant4UserParticleHandler(Geant4Context* context, const std::string& nam)
  : Geant4Action(context,nam)
{
  InstanceCount::increment(this);
}

/// Default destructor
Geant4UserParticleHandler::~Geant4UserParticleHandler()  {
  InstanceCount::decrement(this);
}

/// Event generation action callback
void Geant4UserParticleHandler::generate(G4Event* /* event */, Geant4ParticleHandler* /* handler */)   {
  //printout(INFO,name(),"+++ Generate event");
}

/// Pre-event action callback
void Geant4UserParticleHandler::begin(const G4Event* /* event */)  {
}

/// Post-event action callback
void Geant4UserParticleHandler::end(const G4Event* /* event */)   {
  //printout(INFO,name(),"+++ End event");
}

/// User stepping callback
void Geant4UserParticleHandler::step(const G4Step* /* step */, G4SteppingManager* /* mgr */, Particle& /* particle */)  {
}

/// Pre-track action callback
void Geant4UserParticleHandler::begin(const G4Track* /* track */, Particle& /* particle */)  {
}

/// Post-track action callback
void Geant4UserParticleHandler::end(const G4Track* /* track */, Particle& /* particle */)  {
}

/// Callback when parent should be combined
void Geant4UserParticleHandler::combine(Particle& /* to_be_deleted */, Particle& /* remaining_parent */)  {
}
