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
#include "DDG4/Geant4TrackingPreAction.h"
// Geant4 include files
#include "G4TrackingManager.hh"

using namespace DD4hep::Simulation;

/// Standard constructor
Geant4TrackingPreAction::Geant4TrackingPreAction(Geant4Context* ctxt, const std::string& nam)
: Geant4TrackingAction(ctxt, nam) {
  InstanceCount::increment(this);
}

/// Default destructor
Geant4TrackingPreAction::~Geant4TrackingPreAction() {
  InstanceCount::decrement(this);
}

/// Begin-of-tracking callback
void Geant4TrackingPreAction::begin(const G4Track* track) {
  // Is the track valid? Is tracking manager valid?
  // Does trajectory already exist?
  if (0 == track || 0 == trackMgr() || 0 != trackMgr()->GimmeTrajectory())
    return;
  trackMgr()->SetStoreTrajectory(true);
  // create GaussTrajectory and inform Tracking Manager
  G4VTrajectory* tr = context()->createTrajectory(track);
  trackMgr()->SetTrajectory(tr);
}

/// End-of-tracking callback
void Geant4TrackingPreAction::end(const G4Track* /* track */) {
}
