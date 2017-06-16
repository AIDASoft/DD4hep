// $Id: $
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
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4TrackingPostAction.h"
#include "DDG4/Geant4TrackHandler.h"

// Geant4 include files
#include "G4TrackingManager.hh"

// C/C++ include files
#include <algorithm>

using namespace std;
using namespace dd4hep::sim;

/// Helper class to manipulate strings
/**
 *  \author  M.Frank
 *  \version 1.0
 *  \ingroup DD4HEP_SIMULATION
 */
struct FindString {
  const std::string& m_name;
  FindString(const std::string& n)
    : m_name(n) {
  }
  bool operator()(const std::string& n) const {
    return m_name == n;
  }
  //template <class T> bool operator==(const std::pair<std::string,T>& cmp)  const
  //{  return cmp.first == m_name;  }
  //template <class T> bool operator==(const std::pair<T,std::string>& cmp)  const
  //{  return cmp.second == m_name;  }
};

/// Standard constructor
Geant4TrackingPostAction::Geant4TrackingPostAction(Geant4Context* ctxt, const std::string& nam)
: Geant4TrackingAction(ctxt, nam) {
  InstanceCount::increment(this);
  declareProperty("IgnoredProcesses",  m_ignoredProcs);
  declareProperty("RequiredProcesses", m_requiredProcs);
  declareProperty("StoreMarkedTracks", m_storeMarkedTracks=true);
}

/// Default destructor
Geant4TrackingPostAction::~Geant4TrackingPostAction() {
  InstanceCount::decrement(this);
}

/// Begin-of-tracking callback
void Geant4TrackingPostAction::begin(const G4Track* track) {

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
void Geant4TrackingPostAction::end(const G4Track* tr) {
  if (0 == tr)
    return;
  else if (0 == trackMgr())
    return;

  Geant4TrackHandler track(tr);
  //Geant4TrackInformation* info = track.info<Geant4TrackInformation>();
  const string& proc = track.creatorProcess()->GetProcessName();
  StringV::const_iterator trIt = find_if(m_ignoredProcs.begin(), m_ignoredProcs.end(), FindString(proc));
  if (trIt != m_ignoredProcs.end()) {
    warning("Particles created by processes of type %s are not kept!", (*trIt).c_str());
    return;
  }
  trIt = find_if(m_requiredProcs.begin(), m_requiredProcs.end(), FindString(proc));
  if (trIt != m_requiredProcs.end()) {
    saveTrack(track);
    return;
  }

  if (m_storeMarkedTracks) {
  }

}

void Geant4TrackingPostAction::saveTrack(const G4Track* track) {
  if (0 != track) {
  }
}

