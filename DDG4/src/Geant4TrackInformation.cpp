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
#include "DDG4/Geant4TrackInformation.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::sim;

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
