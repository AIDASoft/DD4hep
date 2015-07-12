// $Id$
//==========================================================================
//  AIDA Detector description implementation for LCD
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
#include "DDG4/Geant4DetectorConstruction.h"
#include "DDG4/Geant4HierarchyDump.h"
#include "DDG4/Geant4Converter.h"
#include "DDG4/Geant4Kernel.h"
#include "DD4hep/LCDD.h"

#include "TGeoManager.h"
#include "G4PVPlacement.hh"

// C/C++ include files
#include <iostream>

#ifdef GEANT4_HAS_GDML
#include "G4GDMLParser.hh"
#endif

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;

namespace {
  static Geant4DetectorConstruction* s_instance = 0;
}

/// Instance accessor
Geant4DetectorConstruction* Geant4DetectorConstruction::instance(Geant4Kernel& kernel)   {
  if ( 0 == s_instance ) s_instance = new Geant4DetectorConstruction(kernel);
  return s_instance;
}

/// Initializing constructor for other clients
Geant4DetectorConstruction::Geant4DetectorConstruction(Geometry::LCDD& lcdd)
  : Geant4Action(0,"DetectorConstruction"), m_lcdd(lcdd), m_world(0)
{
  m_outputLevel = PrintLevel(printLevel()-1);
  s_instance = this;
}

/// Initializing constructor for DDG4
Geant4DetectorConstruction::Geant4DetectorConstruction(Geant4Kernel& kernel)
  : Geant4Action(0,"DetectorConstruction"), m_lcdd(kernel.lcdd()), m_world(0)
{
  m_outputLevel = kernel.getOutputLevel("Geant4Converter");
  s_instance = this;
}

/// Default destructor
Geant4DetectorConstruction::~Geant4DetectorConstruction() {
  s_instance = 0;
}

G4VPhysicalVolume* Geant4DetectorConstruction::Construct() {
  Geant4Mapping& g4map = Geant4Mapping::instance();
  Geometry::DetElement world = m_lcdd.world();
  Geant4Converter conv(m_lcdd, m_outputLevel);
  Geant4GeometryInfo* geo_info = conv.create(world).detach();
  g4map.attach(geo_info);
  m_world = geo_info->world();
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

/// Access to the converted regions
const Geant4GeometryMaps::RegionMap& Geant4DetectorConstruction::regions() const   {
  Geant4GeometryInfo* p = Geant4Mapping::instance().ptr();
  if ( p ) return p->g4Regions;
  throw runtime_error("+++ Geant4DetectorConstruction::regions: Access not possible. Geometry is not yet converted!");
}

/// Access to the converted sensitive detectors
const Geant4GeometryMaps::SensDetMap& Geant4DetectorConstruction::sensitives() const   {
  Geant4GeometryInfo* p = Geant4Mapping::instance().ptr();
  if ( p ) return p->g4SensDets;
  throw runtime_error("+++ Geant4DetectorConstruction::sensitives: Access not possible. Geometry is not yet converted!");
}

/// Access to the converted volumes
const Geant4GeometryMaps::VolumeMap& Geant4DetectorConstruction::volumes() const   {
  Geant4GeometryInfo* p = Geant4Mapping::instance().ptr();
  if ( p ) return p->g4Volumes;
  throw runtime_error("+++ Geant4DetectorConstruction::volumes: Access not possible. Geometry is not yet converted!");
}

/// Access to the converted shapes
const Geant4GeometryMaps::SolidMap& Geant4DetectorConstruction::shapes() const   {
  Geant4GeometryInfo* p = Geant4Mapping::instance().ptr();
  if ( p ) return p->g4Solids;
  throw runtime_error("+++ Geant4DetectorConstruction::shapes: Access not possible. Geometry is not yet converted!");
}

/// Access to the converted limit sets
const Geant4GeometryMaps::LimitMap& Geant4DetectorConstruction::limits() const   {
  Geant4GeometryInfo* p = Geant4Mapping::instance().ptr();
  if ( p ) return p->g4Limits;
  throw runtime_error("+++ Geant4DetectorConstruction::limits: Access not possible. Geometry is not yet converted!");
}

/// Access to the converted assemblies
const Geant4GeometryMaps::AssemblyMap& Geant4DetectorConstruction::assemblies() const   {
  Geant4GeometryInfo* p = Geant4Mapping::instance().ptr();
  if ( p ) return p->g4AssemblyVolumes;
  throw runtime_error("+++ Geant4DetectorConstruction::assemblies: Access not possible. Geometry is not yet converted!");
}

/// Access to the converted placements
const Geant4GeometryMaps::PlacementMap& Geant4DetectorConstruction::placements() const   {
  Geant4GeometryInfo* p = Geant4Mapping::instance().ptr();
  if ( p ) return p->g4Placements;
  throw runtime_error("+++ Geant4DetectorConstruction::placements: Access not possible. Geometry is not yet converted!");
}

/// Access to the converted materials
const Geant4GeometryMaps::MaterialMap& Geant4DetectorConstruction::materials() const   {
  Geant4GeometryInfo* p = Geant4Mapping::instance().ptr();
  if ( p ) return p->g4Materials;
  throw runtime_error("+++ Geant4DetectorConstruction::materials: Access not possible. Geometry is not yet converted!");
}

/// Access to the converted elements
const Geant4GeometryMaps::ElementMap& Geant4DetectorConstruction::elements() const   {
  Geant4GeometryInfo* p = Geant4Mapping::instance().ptr();
  if ( p ) return p->g4Elements;
  throw runtime_error("+++ Geant4DetectorConstruction::elements: Access not possible. Geometry is not yet converted!");
}
