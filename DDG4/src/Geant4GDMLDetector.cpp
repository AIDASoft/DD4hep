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
#include "DDG4/Geant4GDMLDetector.h"

// C/C++ include files
#include <iostream>

//#define GEANT4_HAS_GDML

#ifdef GEANT4_HAS_GDML
#include "G4GDMLParser.hh"
#endif

using namespace std;
using namespace DD4hep;

DD4hep::Simulation::Geant4GDMLDetector::Geant4GDMLDetector(const std::string& gdmlFile)
  : m_fileName(gdmlFile), m_world(0) {
}

G4VPhysicalVolume* DD4hep::Simulation::Geant4GDMLDetector::Construct() {
#ifdef GEANT4_HAS_GDML
  G4GDMLParser parser;
  parser.Read( m_fileName );
  m_world = parser.GetWorldVolume();
#endif
  return m_world;
}
