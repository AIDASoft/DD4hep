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

/// Copy constructor
Particle::Particle(const Particle& c)
  : id(c.id), g4Parent(c.g4Parent), parent(c.parent), reason(c.reason), 
    steps(c.steps), secondaries(c.secondaries), pdgID(c.pdgID),
    vsx(c.vsx), vsy(c.vsy), vsz(c.vsz), 
    vex(c.vex), vey(c.vey), vez(c.vez), 
    psx(c.psx), psy(c.psy), psz(c.psz), 
    pex(c.pex), pey(c.pey), pez(c.pez), 
    energy(c.energy), time(c.time),
    process(c.process), definition(c.definition),
    daughters(c.daughters)
{
  InstanceCount::increment(this);
}

/// Default constructor
Particle::Particle()
  : id(0), g4Parent(0), parent(0), reason(0), 
    steps(0), secondaries(0), pdgID(0),
    vsx(0.0), vsy(0.0), vsz(0.0), 
    vex(0.0), vey(0.0), vez(0.0), 
    psx(0.0), psy(0.0), psz(0.0), 
    pex(0.0), pey(0.0), pez(0.0), 
    energy(0.0), time(0.0),
    process(0), definition(0),
    daughters()
{
  InstanceCount::increment(this);
}

/// Default destructor
Particle::~Particle()  {
  InstanceCount::decrement(this);
}

/// Remove daughter from set
void Particle::removeDaughter(int id_daughter)  {
  set<int>::iterator j = daughters.find(id_daughter);
  if ( j != daughters.end() ) daughters.erase(j);
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
  truth.pdgID   = trk->GetDefinition()->GetPDGEncoding();
  truth.deposit = step->GetTotalEnergyDeposit();
  truth.time    = trk->GetGlobalTime();
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
