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

using namespace std;
using namespace dd4hep;
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
Geant4HitData::Geant4HitData()
  : cellID(0), flag(0), g4ID(0), extension() {
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
  const G4ThreeVector& pre  = h.prePosG4();
  const G4ThreeVector& post = h.postPosG4();
  double length = (post-pre).mag() ;
  float pos[] = {float((pre.x()+post.x())/2.0),float((pre.y()+post.y())/2.0),float((pre.z()+post.z())/2.0) };
  Contribution contrib(h.trkID(),h.trkPdgID(),deposit,h.trkTime(),length,pos);
  return contrib;
}

/// Extract the MC contribution for a given hit from the step information with BirksLaw effect option
Geant4HitData::Contribution Geant4HitData::extractContribution(const G4Step* step, bool ApplyBirksLaw) {
  Geant4StepHandler h(step);
  if ( ApplyBirksLaw == true ) h.doApplyBirksLaw();
  double deposit =
    (h.trackDef() == G4OpticalPhoton::OpticalPhotonDefinition()) ? h.trkEnergy() : h.totalEnergy();
  const G4ThreeVector& pre  = h.prePosG4();
  const G4ThreeVector& post = h.postPosG4();
  double length = (post-pre).mag() ;
  float pos[] = {float((pre.x()+post.x())/2.0),float((pre.y()+post.y())/2.0),float((pre.z()+post.z())/2.0) };
  Contribution contrib(h.trkID(),h.trkPdgID(),deposit,h.trkTime(),length,pos);
  return contrib;
}

/// Extract the MC contribution for a given hit from the fast simulation spot information
Geant4HitData::Contribution Geant4HitData::extractContribution(const Geant4FastSimSpot* spot) {
  Geant4FastSimHandler h(spot);
  G4ThreeVector        p = h.avgPositionG4();
  double               position[3] = {p.x(), p.y(), p.z()};
  return Contribution(h.trkID(),h.trkPdgID(),h.energy(),h.trkTime(),0e0,position);
}

/// Default constructor
Geant4Tracker::Hit::Hit()
: Geant4HitData(), position(), momentum(), length(0.0), truth(), energyDeposit(0.0)
{
  InstanceCount::increment(this);
}

/// Standard initializing constructor
Geant4Tracker::Hit::Hit(int track_id, int pdg_id, double deposit, double time_stamp)
  : Geant4HitData(), position(), momentum(), length(0.0), truth(track_id, pdg_id, deposit, time_stamp, 0.), energyDeposit(deposit) {
  InstanceCount::increment(this);
}

/// Default destructor
Geant4Tracker::Hit::~Hit() {
  InstanceCount::decrement(this);
}

/// Explicit assignment operation
void Geant4Tracker::Hit::copyFrom(const Hit& c) {
  if ( &c != this )  {
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
  length = 0.0;
  truth.clear();
  return *this;
}

/// Store Geant4 point and step information into tracker hit structure.
Geant4Tracker::Hit& Geant4Tracker::Hit::storePoint(const G4Step* step, const G4StepPoint* pnt) {
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

/// Store Geant4 spot information into tracker hit structure.
Geant4Tracker::Hit& Geant4Tracker::Hit::storePoint(const Geant4FastSimSpot* spot)   {
  const G4Track* trk  = spot->primary;
  G4ThreeVector  pos  = spot->hitPosition();
  G4ThreeVector  mom  = trk->GetMomentum().unit();
  double         dep  = spot->hit->GetEnergy();
  this->truth.deposit = dep;
  this->truth.trackID = trk->GetTrackID();
  this->truth.time    = trk->GetGlobalTime();
  this->truth.pdgID   = trk->GetDefinition()->GetPDGEncoding();
  this->energyDeposit = dep;
  this->position.SetXYZ(pos.x(), pos.y(), pos.z());
  this->momentum.SetXYZ(mom.x()*dep, mom.y()*dep, mom.z()*dep);
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
