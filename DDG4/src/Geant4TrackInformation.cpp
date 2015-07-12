// $Id: Handle.h 570 2013-05-17 07:47:11Z markus.frank $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation européenne pour la Recherche nucléaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

// Framework include files
#include "DDG4/Geant4TrackInformation.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;

/// Default constructor
Geant4TrackInformation::Geant4TrackInformation()
  : G4VUserTrackInformation(), m_flags(0) {
}

/// Standard destructor
Geant4TrackInformation::~Geant4TrackInformation() {
}

/// Access flag if track should be stored
Geant4TrackInformation& Geant4TrackInformation::storeTrack(bool value) {
  value ? m_flags |= STORE : m_flags &= ~STORE;
  return *this;
}
