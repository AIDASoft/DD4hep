#include "G4DetectorConstruction.h"
#include "DD4hep/LCDD.h"

// VGM demo
#include "Geant4GM/volumes/Factory.h"
#include "RootGM/volumes/Factory.h"
#include "TGeoManager.h"


using namespace DD4hep;
using namespace DD4hep::Geometry;

G4DetectorConstruction::G4DetectorConstruction(const std::string& xmlfile) : m_lcdd(LCDD::getInstance()), m_compactfile(xmlfile) {
  // We need to construct the geometry at this level already
  m_lcdd.fromCompact(m_compactfile);
}

G4DetectorConstruction::~G4DetectorConstruction() {}

G4VPhysicalVolume* G4DetectorConstruction::Construct() {
  
  // Import geometry from Root to VGM
  RootGM::Factory rtFactory;
  rtFactory.SetDebug(1);
  rtFactory.Import(gGeoManager->GetTopNode());
  
  // Export VGM geometry to Geant4
  //
  Geant4GM::Factory g4Factory;
  g4Factory.SetDebug(1);
  rtFactory.Export(&g4Factory);
  G4VPhysicalVolume* world = g4Factory.World();
  return world;
}
