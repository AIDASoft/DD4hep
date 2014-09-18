// $Id: Geant4Hits.cpp 513 2013-04-05 14:31:53Z gaede $
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
#include "DDG4/Geant4Data.h"
#include "DDG4/Geant4StepHandler.h"

// Geant4 include files
#include "G4Step.hh"
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

/// Default destructor
DataExtension::~DataExtension() {
}

/// Default constructor
Geant4HitData::Geant4HitData()
    : cellID(0), extension() {
  InstanceCount::increment(this);
}

/// Default destructor
Geant4HitData::~Geant4HitData() {
  InstanceCount::decrement(this);
}

/// Extract the MC contribution for a given hit from the step information
Geant4HitData::Contribution Geant4HitData::extractContribution(G4Step* step) {
  Geant4StepHandler h(step);
  G4Track* trk = step->GetTrack();
  double deposit =
    (h.trackDef() == G4OpticalPhoton::OpticalPhotonDefinition()) ? h.trkEnergy() : h.totalEnergy();
  const G4ThreeVector& pre  = h.prePosG4();
  const G4ThreeVector& post = h.postPosG4();
  float pos[] = { (pre.x()+post.x())/2.0, (pre.y()+post.y())/2.0, (pre.z()+post.z())/2.0 };
  Contribution contrib(h.trkID(),h.trkPdgID(),deposit,h.trkTime(),pos);
  return contrib;
}

/// Default constructor
Geant4Tracker::Hit::Hit()
  : Geant4HitData(), position(), momentum(), length(0.0), truth(), energyDeposit(0.0) {
  InstanceCount::increment(this);
}

/// Standard initializing constructor
Geant4Tracker::Hit::Hit(int track_id, int pdg_id, double deposit, double time_stamp)
  : Geant4HitData(), position(), momentum(), length(0.0), truth(track_id, pdg_id, deposit, time_stamp), energyDeposit(deposit) {
  InstanceCount::increment(this);
}

/// Default destructor
Geant4Tracker::Hit::~Hit() {
  InstanceCount::decrement(this);
}

/// Assignment operator
Geant4Tracker::Hit& Geant4Tracker::Hit::operator=(const Hit& c) {
  if ( &c != this )  {
    position = c.position;
    momentum = c.momentum;
    length = c.length;
    truth = c.truth;
  }
  return *this;
}

/// Clear hit content
Geant4Tracker::Hit& Geant4Tracker::Hit::clear() {
  position.SetXYZ(0, 0, 0);
  momentum.SetXYZ(0, 0, 0);
  length = 0.0;
  truth.clear();
  return *this;
}

/// Store Geant4 point and step information into tracker hit structure.
Geant4Tracker::Hit& Geant4Tracker::Hit::storePoint(G4Step* step, G4StepPoint* pnt) {
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
Geant4Calorimeter::Hit::Hit()
    : Geant4HitData(), position(), truth(), energyDeposit(0) {
  InstanceCount::increment(this);
}

/// Standard constructor
Geant4Calorimeter::Hit::Hit(const Position& pos)
    : Geant4HitData(), position(pos), truth(), energyDeposit(0) {
  InstanceCount::increment(this);
}

/// Default destructor
Geant4Calorimeter::Hit::~Hit() {
  InstanceCount::decrement(this);
}
