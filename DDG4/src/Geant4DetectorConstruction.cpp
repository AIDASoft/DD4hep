#include "DDG4/Geant4DetectorConstruction.h"
#include "DDG4/Geant4Converter.h"
#include "DD4hep/LCDD.h"
#include "TGeoManager.h"
#include "G4PVPlacement.hh"

#ifdef GEANT4_HAS_GDML
#include "G4GDMLParser.hh"
#endif

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;


DD4hep::Simulation::Geant4DetectorConstruction::Geant4DetectorConstruction(Geometry::LCDD& lcdd)
: m_lcdd(lcdd), m_world(0)
{
}

G4VPhysicalVolume* DD4hep::Simulation::Geant4DetectorConstruction::Construct() {
  typedef Simulation::Geant4Converter Geant4Converter;
  TGeoNode* top = gGeoManager->GetTopNode();
  Geant4Converter& conv = Geant4Converter::instance();
  DetElement world = m_lcdd.world();
  conv.create(world);
  Geant4Converter::G4GeometryInfo& info = conv.data();
  m_world = info.g4Placements[top];
#ifdef GEANT4_HAS_GDML
  if ( ::getenv("DUMP_GDML") ) {
    G4GDMLParser parser;
    parser.Write("detector.gdml",m_world);
  }
#endif
  return m_world;
}
