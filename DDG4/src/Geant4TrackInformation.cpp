// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
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
