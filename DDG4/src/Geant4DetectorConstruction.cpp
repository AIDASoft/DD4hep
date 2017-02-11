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
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4Mapping.h"
#include "DDG4/Geant4GeometryInfo.h"
#include "DDG4/Geant4DetectorConstruction.h"

#include "G4VUserDetectorConstruction.hh"
#include "G4SDManager.hh"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;

/// Helper: Assign sensitive detector to logical volume
void Geant4DetectorConstructionContext::setSensitiveDetector(G4LogicalVolume* vol, G4VSensitiveDetector* sd)   {
  //detector->SetSensitiveDetector(vol,sd);
  G4SDManager::GetSDMpointer()->AddNewDetector(sd);
  vol->SetSensitiveDetector(sd);
}

/// Standard Constructor
Geant4DetectorConstruction::Geant4DetectorConstruction(Geant4Context* ctxt, const std::string& nam)
  : Geant4Action(ctxt,nam)
{
}

/// Default destructor
Geant4DetectorConstruction::~Geant4DetectorConstruction()  {
}

/// Geometry construction callback. Called at "Construct()"
void Geant4DetectorConstruction::constructGeo(Geant4DetectorConstructionContext* )   {
}

/// Electromagnetic field construction callback. Called at "ConstructSDandField()"
void Geant4DetectorConstruction::constructField(Geant4DetectorConstructionContext* )   {
}

/// Sensitive detector construction callback. Called at "ConstructSDandField()"
void Geant4DetectorConstruction::constructSensitives(Geant4DetectorConstructionContext* )   {
}


/// Standard Constructor
Geant4DetectorConstructionSequence::Geant4DetectorConstructionSequence(Geant4Context* ctxt, const std::string& nam)
  : Geant4Action(ctxt,nam)
{
  m_needsControl = true;
  InstanceCount::increment(this);
}

/// Default destructor
Geant4DetectorConstructionSequence::~Geant4DetectorConstructionSequence()   {
  m_actors(&Geant4DetectorConstruction::release);  
  InstanceCount::decrement(this);
}

/// Set or update client context
void Geant4DetectorConstructionSequence::updateContext(Geant4Context* ctxt)   {
  m_context = ctxt;
  m_actors(&Geant4DetectorConstruction::updateContext,ctxt);  
}

/// Add an actor responding to all callbacks. Sequence takes ownership.
void Geant4DetectorConstructionSequence::adopt(Geant4DetectorConstruction* action)   {
  if (action) {
    action->addRef();
    m_actors.add(action);
    return;
  }
  throw runtime_error("Geant4RunActionSequence: Attempt to add invalid actor!");
}

/// Geometry construction callback. Called at "Construct()"
void Geant4DetectorConstructionSequence::constructGeo(Geant4DetectorConstructionContext* ctxt)   {
  m_actors(&Geant4DetectorConstruction::constructGeo, ctxt);  
}

/// Electromagnetic field construction callback. Called at "ConstructSDandField()"
void Geant4DetectorConstructionSequence::constructField(Geant4DetectorConstructionContext* ctxt)   {
  m_actors(&Geant4DetectorConstruction::constructField, ctxt);  
}

/// Sensitive detector construction callback. Called at "ConstructSDandField()"
void Geant4DetectorConstructionSequence::constructSensitives(Geant4DetectorConstructionContext* ctxt)   {
  m_actors(&Geant4DetectorConstruction::constructSensitives, ctxt);  
}

/// Access to the converted regions
const Geant4GeometryMaps::RegionMap& Geant4DetectorConstructionSequence::regions() const   {
  Geant4GeometryInfo* p = Geant4Mapping::instance().ptr();
  if ( p ) return p->g4Regions;
  throw runtime_error("+++ Geant4DetectorConstructionSequence::regions: Access not possible. Geometry is not yet converted!");
}
#if 0
/// Access to the converted sensitive detectors
const Geant4GeometryMaps::SensDetMap& Geant4DetectorConstructionSequence::sensitives() const   {
  Geant4GeometryInfo* p = Geant4Mapping::instance().ptr();
  if ( p ) return p->g4SensDets;
  throw runtime_error("+++ Geant4DetectorConstructionSequence::sensitives: Access not possible. Geometry is not yet converted!");
}
#endif
/// Access to the converted volumes
const Geant4GeometryMaps::VolumeMap& Geant4DetectorConstructionSequence::volumes() const   {
  Geant4GeometryInfo* p = Geant4Mapping::instance().ptr();
  if ( p ) return p->g4Volumes;
  throw runtime_error("+++ Geant4DetectorConstructionSequence::volumes: Access not possible. Geometry is not yet converted!");
}

/// Access to the converted shapes
const Geant4GeometryMaps::SolidMap& Geant4DetectorConstructionSequence::shapes() const   {
  Geant4GeometryInfo* p = Geant4Mapping::instance().ptr();
  if ( p ) return p->g4Solids;
  throw runtime_error("+++ Geant4DetectorConstructionSequence::shapes: Access not possible. Geometry is not yet converted!");
}

/// Access to the converted limit sets
const Geant4GeometryMaps::LimitMap& Geant4DetectorConstructionSequence::limits() const   {
  Geant4GeometryInfo* p = Geant4Mapping::instance().ptr();
  if ( p ) return p->g4Limits;
  throw runtime_error("+++ Geant4DetectorConstructionSequence::limits: Access not possible. Geometry is not yet converted!");
}

/// Access to the converted assemblies
const Geant4GeometryMaps::AssemblyMap& Geant4DetectorConstructionSequence::assemblies() const   {
  Geant4GeometryInfo* p = Geant4Mapping::instance().ptr();
  if ( p ) return p->g4AssemblyVolumes;
  throw runtime_error("+++ Geant4DetectorConstructionSequence::assemblies: Access not possible. Geometry is not yet converted!");
}

/// Access to the converted placements
const Geant4GeometryMaps::PlacementMap& Geant4DetectorConstructionSequence::placements() const   {
  Geant4GeometryInfo* p = Geant4Mapping::instance().ptr();
  if ( p ) return p->g4Placements;
  throw runtime_error("+++ Geant4DetectorConstructionSequence::placements: Access not possible. Geometry is not yet converted!");
}

/// Access to the converted materials
const Geant4GeometryMaps::MaterialMap& Geant4DetectorConstructionSequence::materials() const   {
  Geant4GeometryInfo* p = Geant4Mapping::instance().ptr();
  if ( p ) return p->g4Materials;
  throw runtime_error("+++ Geant4DetectorConstructionSequence::materials: Access not possible. Geometry is not yet converted!");
}

/// Access to the converted elements
const Geant4GeometryMaps::ElementMap& Geant4DetectorConstructionSequence::elements() const   {
  Geant4GeometryInfo* p = Geant4Mapping::instance().ptr();
  if ( p ) return p->g4Elements;
  throw runtime_error("+++ Geant4DetectorConstructionSequence::elements: Access not possible. Geometry is not yet converted!");
}




