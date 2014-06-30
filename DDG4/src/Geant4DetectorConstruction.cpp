// $Id$
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#include "DDG4/Geant4DetectorConstruction.h"
#include "DDG4/Geant4HierarchyDump.h"
#include "DDG4/Geant4Converter.h"
#include "DDG4/Geant4Kernel.h"
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
  : m_outputLevel(PrintLevel(printLevel()-1)), m_lcdd(lcdd), m_world(0) {
}

DD4hep::Simulation::Geant4DetectorConstruction::Geant4DetectorConstruction(DD4hep::Simulation::Geant4Kernel& kernel)
  : m_lcdd(kernel.lcdd()), m_world(0) {
  m_outputLevel = kernel.getOutputLevel("Geant4Converter");
}

/// Default destructor
DD4hep::Simulation::Geant4DetectorConstruction::~Geant4DetectorConstruction() {
}

G4VPhysicalVolume* DD4hep::Simulation::Geant4DetectorConstruction::Construct() {
  typedef Simulation::Geant4Converter Geant4Converter;
  Geant4Mapping& g4map = Geant4Mapping::instance();
  DetElement world = m_lcdd.world();
  Geant4Converter conv(m_lcdd, m_outputLevel);
  Geant4GeometryInfo* info = conv.create(world).detach();
  g4map.attach(info);
  m_world = info->world();
  m_lcdd.apply("DD4hepVolumeManager", 0, 0);
  // Create Geant4 volume manager
  g4map.volumeManager();
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
