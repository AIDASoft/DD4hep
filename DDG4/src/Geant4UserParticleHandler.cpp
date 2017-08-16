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
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4ParticleHandler.h"
#include "DDG4/Geant4UserParticleHandler.h"
#include "CLHEP/Units/SystemOfUnits.h"

using namespace dd4hep::sim;

/// Standard constructor
Geant4UserParticleHandler::Geant4UserParticleHandler(Geant4Context* ctxt, const std::string& nam)
: Geant4Action(ctxt,nam)
{
  InstanceCount::increment(this);
  declareProperty("MinimalKineticEnergy",m_kinEnergyCut = 100e0*CLHEP::MeV);
  m_needsControl = true;
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

/// Callback to be answered if the particle MUST be kept during recombination step
bool Geant4UserParticleHandler::keepParticle(Particle& particle)   {
  return Geant4ParticleHandler::defaultKeepParticle(particle);
}
