#include "DDG4/Geant4GDMLDetector.h"
#include <iostream>

//#define GEANT4_HAS_GDML 

#ifdef GEANT4_HAS_GDML
#include "G4GDMLParser.hh"
#endif

using namespace std;
using namespace DD4hep;

DD4hep::Simulation::Geant4GDMLDetector::Geant4GDMLDetector(const std::string& gdmlFile ) 
: m_fileName(gdmlFile), m_world(0)
{
}

G4VPhysicalVolume* DD4hep::Simulation::Geant4GDMLDetector::Construct() {
#ifdef GEANT4_HAS_GDML
  G4GDMLParser parser;
  parser.Read( m_fileName );
  m_world = parser.GetWorldVolume();
#endif
  return m_world;
}
