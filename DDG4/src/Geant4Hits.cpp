// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DDG4/Geant4Hits.h"

// Geant4 include files
#include "G4Allocator.hh"
#include "G4ParticleDefinition.hh"
#include "G4ChargedGeantino.hh"
#include "G4OpticalPhoton.hh"
#include "G4Geantino.hh"

// C/C++ include files
#include <iostream>

using namespace std;
using namespace DD4hep::Simulation;

/// Check if the Geant4 track is a Geantino
bool Geant4Hit::isGeantino(G4Track* track)  {
  if ( track ) {
    G4ParticleDefinition* def = track->GetDefinition();
    if ( def == G4ChargedGeantino::Definition() )
      return true;
    if ( def != G4Geantino::Definition() ) {
      return true;
    }
  }
  return false;
}

Geant4Hit::Contribution Geant4Hit::extractContribution(G4Step* step) {
  G4Track* trk     = step->GetTrack();
  double energy_deposit = (trk->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
    ? trk->GetTotalEnergy() : step->GetTotalEnergyDeposit();
  Contribution  contrib(trk->GetTrackID(),
			trk->GetDefinition()->GetPDGEncoding(),
			energy_deposit,
			trk->GetGlobalTime());
  return contrib;
}

static G4Allocator<Geant4TrackerHit> TrackerHitAllocator;

/// Default constructor
Geant4TrackerHit::Geant4TrackerHit() : Geant4Hit(), position(), momentum(), length(0.0), truth()
{
}

/// Standard initializing constructor
Geant4TrackerHit::Geant4TrackerHit(int track_id, int pdg_id, double deposit, double time_stamp)
  : Geant4Hit(), position(), momentum(), length(0.0), truth(track_id, pdg_id, deposit, time_stamp)
{
}

/// Assignment operator
Geant4TrackerHit& Geant4TrackerHit::operator=(const Geant4TrackerHit& c)   {
  position = c.position;
  momentum = c.momentum;
  length   = c.length;
  truth    = c.truth;
  return *this;
}

/// Clear hit content
Geant4TrackerHit& Geant4TrackerHit::clear()  {
  position.set(0,0,0);
  momentum.set(0,0,0);
  length = 0.0;
  truth.clear();
  return *this;
}

/// Store Geant4 point and step information into tracker hit structure.
Geant4TrackerHit& Geant4TrackerHit::storePoint(G4Step* step, G4StepPoint* pnt)  {
  G4Track*      trk = step->GetTrack();
  G4ThreeVector pos = pnt->GetPosition();
  G4ThreeVector mom = pnt->GetMomentum();

  truth.trackID = trk->GetTrackID();
  truth.pdgID   = trk->GetDefinition()->GetPDGEncoding();
  truth.deposit = step->GetTotalEnergyDeposit();
  truth.time    = trk->GetGlobalTime();
  position.set(pos.x(),pos.y(),pos.z());
  momentum.set(mom.x(),mom.y(),mom.z());
  length = 0;
  return *this;
}

/// Geant4 required object allocator
void* Geant4TrackerHit::operator new(size_t)   { 
  return TrackerHitAllocator.MallocSingle();    
}

/// Geat4 required object destroyer
void Geant4TrackerHit::operator delete(void *p) { 
  TrackerHitAllocator.FreeSingle((Geant4TrackerHit*)p); 
}


static G4Allocator<Geant4CalorimeterHit> CalorimeterHitAllocator;

/// Standard constructor
Geant4CalorimeterHit::Geant4CalorimeterHit(const Position& pos)
  : Geant4Hit(), position(pos), truth(), energyDeposit(0)
{
}

/// Geant4 required object allocator
void* Geant4CalorimeterHit::operator new(size_t)   { 
  return CalorimeterHitAllocator.MallocSingle();    
}

/// Geat4 required object destroyer
void Geant4CalorimeterHit::operator delete(void *p) { 
  CalorimeterHitAllocator.FreeSingle((Geant4CalorimeterHit*)p); 
}

