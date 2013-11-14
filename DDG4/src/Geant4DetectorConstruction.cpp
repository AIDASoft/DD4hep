#include "DDG4/Geant4DetectorConstruction.h"
#include "DDG4/Geant4HierarchyDump.h"
#include "DDG4/Geant4Converter.h"
#include "DD4hep/LCDD.h"
#include "TGeoManager.h"
#include "G4PVPlacement.hh"
#include <iostream>

#ifdef GEANT4_HAS_GDML
#include "G4GDMLParser.hh"
#endif

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

DD4hep::Simulation::Geant4DetectorConstruction::Geant4DetectorConstruction(Geometry::LCDD& lcdd)
    : m_lcdd(lcdd), m_world(0) {
}

G4VPhysicalVolume* DD4hep::Simulation::Geant4DetectorConstruction::Construct() {
  typedef Simulation::Geant4Converter Geant4Converter;
  TGeoNode* top = gGeoManager->GetTopNode();
  Geant4Mapping& g4map = Geant4Mapping::instance();
  DetElement world = m_lcdd.world();
  Geant4Converter conv(m_lcdd);
  Geant4GeometryInfo* info = conv.create(world).detach();
  g4map.attach(info);
  Geant4VolumeManager mgr = g4map.volumeManager();
  m_world = mgr.placement(top);
  m_lcdd.apply("DD4hepVolumeManager", 0, 0);
  //Geant4HierarchyDump dmp(m_lcdd);
  //dmp.dump("",m_world);
#ifdef GEANT4_HAS_GDML
  if ( ::getenv("DUMP_GDML") ) {
    G4GDMLParser parser;
    parser.Write("detector.gdml",m_world);
  }
#endif
  return m_world;
}
