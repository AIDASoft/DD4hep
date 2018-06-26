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

/// Framework include files
#include "MyTrackerHit.h"
#include "G4Track.hh"

using namespace SomeExperiment;

/// Assignment operator
MyTrackerHit& MyTrackerHit::operator=(const MyTrackerHit& c)  {
  if ( &c != this )  {
    position = c.position;
    momentum = c.momentum;
    length = c.length;
    truth = c.truth;
  }
  return *this;
}

/// Clear hit content
MyTrackerHit& MyTrackerHit::clear() {
  position.SetXYZ(0, 0, 0);
  momentum.SetXYZ(0, 0, 0);
  length = 0.0;
  truth.clear();
  return *this;
}

/// Store Geant4 point and step information into tracker hit structure.
MyTrackerHit& MyTrackerHit::storePoint(const G4Step* step, const G4StepPoint* pnt) {
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
