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
#include <DD4hep/Printout.h>
#include <DD4hep/InstanceCount.h>

#include <DDG4/Geant4Data.h>
#include <DDG4/Geant4StepHandler.h>
#include <DDG4/Geant4FastSimHandler.h>

// Geant4 include files
#include <G4Step.hh>
#include <G4Allocator.hh>
#include <G4OpticalPhoton.hh>

using namespace dd4hep::sim;

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
Geant4HitData::Geant4HitData()   {
  InstanceCount::increment(this);
}

/// Default destructor
Geant4HitData::~Geant4HitData() {
  InstanceCount::decrement(this);
}

/// Extract the MC contribution for a given hit from the step information
Geant4HitData::Contribution Geant4HitData::extractContribution(const G4Step* step) {
  Geant4StepHandler h(step);
  double deposit =
    (h.trackDef() == G4OpticalPhoton::OpticalPhotonDefinition()) ? h.trkEnergy() : h.totalEnergy();
  const G4ThreeVector& pre   = h.prePosG4();
  const G4ThreeVector& post  = h.postPosG4();
  G4ThreeVector        mom   = h.track->GetMomentum();
  double               len   = (post-pre).mag() ;
  double               position[] = { (pre.x()+post.x())/2.0,(pre.y()+post.y())/2.0,(pre.z()+post.z())/2.0 };
  double               momentum[] = { mom.x(), mom.y(), mom.z() };
  return Contribution(h.trkID(), h.trkPdgID(), deposit, h.trkTime(), len, position, momentum);
}

/// Extract the MC contribution for a given hit from the step information with BirksLaw effect option
Geant4HitData::Contribution Geant4HitData::extractContribution(const G4Step* step, bool ApplyBirksLaw) {
  Geant4StepHandler h(step);
  if ( ApplyBirksLaw == true ) h.doApplyBirksLaw();
  double deposit =
    (h.trackDef() == G4OpticalPhoton::OpticalPhotonDefinition()) ? h.trkEnergy() : h.totalEnergy();
  const G4ThreeVector& pre  = h.prePosG4();
  const G4ThreeVector& post = h.postPosG4();
  G4ThreeVector        mom  = h.track->GetMomentum();
  double length = (post-pre).mag() ;
  double momentum[] = { mom.x(), mom.y(), mom.z() };
  double position[] = { (pre.x()+post.x())/2.0,(pre.y()+post.y())/2.0,(pre.z()+post.z())/2.0 };
  return Contribution(h.trkID(), h.trkPdgID(), deposit, h.trkTime(), length, position, momentum);
}

/// Extract the MC contribution for a given hit from the fast simulation spot information
Geant4HitData::Contribution Geant4HitData::extractContribution(const Geant4FastSimSpot* spot) {
  Geant4FastSimHandler h(spot);
  const G4Track*       t = spot->primary;
  G4ThreeVector        mom = t->GetMomentum();
  G4ThreeVector        pos = h.avgPositionG4();
  double               position[] = { pos.x(), pos.y(), pos.z() };
  double               momentum[] = { mom.x(), mom.y(), mom.z() };
  return Contribution( h.trkID(), h.trkPdgID(), h.energy(), h. trkTime(), 0e0, position, momentum);
}

/// Default constructor
Geant4Tracker::Hit::Hit()
: Geant4HitData(), position(), momentum(), length(0.0), energyDeposit(0.0), truth()
{
  InstanceCount::increment(this);
}

/// Standard initializing constructor
Geant4Tracker::Hit::Hit(int track_id, int pdg_id, double deposit, double time_stamp,
			double len, const Position& pos, const Direction& mom)
  : Geant4HitData(), position(pos), momentum(mom), length(len), energyDeposit(deposit),
    truth(track_id, pdg_id, deposit, time_stamp, len, pos, mom)
{
  g4ID = track_id;
  InstanceCount::increment(this);
}

/// Optimized constructor for sensitive detectors
Geant4Tracker::Hit::Hit(const Geant4HitData::Contribution& contrib, const Direction& mom, double depo)
  : Geant4HitData(), position(contrib.x, contrib.y, contrib.z),
    momentum(mom), length(contrib.length), energyDeposit(depo), truth(contrib)
{
  g4ID = truth.trackID;
  InstanceCount::increment(this);
}

/// Default destructor
Geant4Tracker::Hit::~Hit() {
  InstanceCount::decrement(this);
}

/// Explicit assignment operation
void Geant4Tracker::Hit::copyFrom(const Hit& c) {
  if ( &c != this )  {
    energyDeposit = c.energyDeposit;
    position = c.position;
    momentum = c.momentum;
    length = c.length;
    truth = c.truth;
  }
}

/// Clear hit content
Geant4Tracker::Hit& Geant4Tracker::Hit::clear() {
  position.SetXYZ(0, 0, 0);
  momentum.SetXYZ(0, 0, 0);
  energyDeposit = 0.0;
  length = 0.0;
  truth.clear();
  return *this;
}

/// Store Geant4 point and step information into tracker hit structure.
Geant4Tracker::Hit& Geant4Tracker::Hit::storePoint(const G4Step* step, const G4StepPoint* pnt) {
  G4Track*      trk = step->GetTrack();
  G4ThreeVector trm = trk->GetMomentum();
  G4ThreeVector pos = pnt->GetPosition();
  G4ThreeVector mom = pnt->GetMomentum();
  double        dep = step->GetTotalEnergyDeposit();
  
  truth.deposit = dep;
  truth.trackID = trk->GetTrackID();
  truth.pdgID   = trk->GetDefinition()->GetPDGEncoding();
  truth.time    = trk->GetGlobalTime();
  truth.setPosition(pos.x(), pos.y(), pos.z()); 
  truth.setMomentum(trm.x(), trm.y(), trm.z()); 

  energyDeposit = dep;
  position.SetXYZ(pos.x(), pos.y(), pos.z());
  momentum.SetXYZ(mom.x(), mom.y(), mom.z());
  length = 0;
  return *this;
}

/// Store Geant4 spot information into tracker hit structure.
Geant4Tracker::Hit& Geant4Tracker::Hit::storePoint(const Geant4FastSimSpot* spot)   {
  const G4Track* trk  = spot->primary;
  double         dep  = spot->hit->GetEnergy();
  G4ThreeVector  trm  = trk->GetMomentum();
  G4ThreeVector  pos  = spot->hitPosition();
  G4ThreeVector  mom  = trk->GetMomentum().unit() * dep;

  this->truth.deposit = dep;
  this->truth.trackID = trk->GetTrackID();
  this->truth.time    = trk->GetGlobalTime();
  this->truth.pdgID   = trk->GetDefinition()->GetPDGEncoding();
  this->truth.setPosition(pos.x(), pos.y(), pos.z()); 
  this->truth.setMomentum(trm.x(), trm.y(), trm.z()); 

  this->energyDeposit = dep;
  this->position.SetXYZ(pos.x(), pos.y(), pos.z());
  this->momentum.SetXYZ(mom.x(), mom.y(), mom.z());
  this->length = 0;
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
