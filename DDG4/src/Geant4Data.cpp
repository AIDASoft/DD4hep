// $Id: Geant4Hits.cpp 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4Data.h"

// Geant4 include files
#include "G4Allocator.hh"
#include "G4OpticalPhoton.hh"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;

/// Default constructor
SimpleRun::SimpleRun()
    : runID(-1), numEvents(0) {
  InstanceCount::increment(this);
}

/// Default destructor
SimpleRun::~SimpleRun() {
  InstanceCount::decrement(this);
}

/// Default constructor
SimpleEvent::SimpleEvent()
    : runID(-1), eventID(-1) {
  InstanceCount::increment(this);
}

/// Default destructor
SimpleEvent::~SimpleEvent() {
  InstanceCount::decrement(this);
}

/// Default constructor
SimpleHit::SimpleHit()
    : cellID(0) {
  InstanceCount::increment(this);
}

/// Default destructor
SimpleHit::~SimpleHit() {
  InstanceCount::decrement(this);
}

/// Extract the MC contribution for a given hit from the step information
SimpleHit::Contribution SimpleHit::extractContribution(G4Step* step) {
  G4Track* trk = step->GetTrack();
  double energy_deposit =
      (trk->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()) ?
          trk->GetTotalEnergy() : step->GetTotalEnergyDeposit();
  Contribution contrib(trk->GetTrackID(), trk->GetDefinition()->GetPDGEncoding(), energy_deposit, trk->GetGlobalTime());
  return contrib;
}

/// Default constructor
SimpleTracker::Hit::Hit()
  : SimpleHit(), position(), momentum(), length(0.0), truth(), energyDeposit(0.0) {
  InstanceCount::increment(this);
}

/// Standard initializing constructor
SimpleTracker::Hit::Hit(int track_id, int pdg_id, double deposit, double time_stamp)
  : SimpleHit(), position(), momentum(), length(0.0), truth(track_id, pdg_id, deposit, time_stamp), energyDeposit(deposit) {
  InstanceCount::increment(this);
}

/// Default destructor
SimpleTracker::Hit::~Hit() {
  InstanceCount::decrement(this);
}

/// Assignment operator
SimpleTracker::Hit& SimpleTracker::Hit::operator=(const Hit& c) {
  if ( &c != this )  {
    position = c.position;
    momentum = c.momentum;
    length = c.length;
    truth = c.truth;
  }
  return *this;
}

/// Clear hit content
SimpleTracker::Hit& SimpleTracker::Hit::clear() {
  position.SetXYZ(0, 0, 0);
  momentum.SetXYZ(0, 0, 0);
  length = 0.0;
  truth.clear();
  return *this;
}

/// Store Geant4 point and step information into tracker hit structure.
SimpleTracker::Hit& SimpleTracker::Hit::storePoint(G4Step* step, G4StepPoint* pnt) {
  G4Track* trk = step->GetTrack();
  G4ThreeVector pos = pnt->GetPosition();
  G4ThreeVector mom = pnt->GetMomentum();

  truth.trackID = trk->GetTrackID();
  truth.pdgID = trk->GetDefinition()->GetPDGEncoding();
  truth.deposit = step->GetTotalEnergyDeposit();
  truth.time = trk->GetGlobalTime();
  position.SetXYZ(pos.x(), pos.y(), pos.z());
  momentum.SetXYZ(mom.x(), mom.y(), mom.z());
  length = 0;
  return *this;
}

/// Default constructor (for ROOT)
SimpleCalorimeter::Hit::Hit()
    : SimpleHit(), position(), truth(), energyDeposit(0) {
  InstanceCount::increment(this);
}

/// Standard constructor
SimpleCalorimeter::Hit::Hit(const Position& pos)
    : SimpleHit(), position(pos), truth(), energyDeposit(0) {
  InstanceCount::increment(this);
}

/// Default destructor
SimpleCalorimeter::Hit::~Hit() {
  InstanceCount::decrement(this);
}
