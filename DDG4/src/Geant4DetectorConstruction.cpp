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
#include <DD4hep/Plugins.h>
#include <DD4hep/InstanceCount.h>
#include <DDG4/Geant4Mapping.h>
#include <DDG4/Geant4GeometryInfo.h>
#include <DDG4/Geant4DetectorConstruction.h>

#include <G4VUserDetectorConstruction.hh>
#include <G4SDManager.hh>

using namespace dd4hep::sim;

/// Helper: Assign sensitive detector to logical volume
void Geant4DetectorConstructionContext::setSensitiveDetector(G4LogicalVolume* vol, G4VSensitiveDetector* sd)  {
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

/// Create Geant4 sensitive detector object using the factory mechanism
G4VSensitiveDetector*
Geant4DetectorConstruction::createSensitiveDetector(const std::string& typ,
                                                    const std::string& nam)  {
  Detector& dsc = context()->detectorDescription();
  G4VSensitiveDetector* g4sd = PluginService::Create<G4VSensitiveDetector*>(typ, nam, &dsc);
  if( g4sd )  {
    print("+++ Subdetector: %-32s  type: %s.", nam.c_str(), typ.c_str());
  }
  else  {
    PluginDebug dbg;
    g4sd = PluginService::Create<G4VSensitiveDetector*>(typ, nam, &dsc);
    if ( !g4sd )  {
      except("createSensitiveDetector: FATAL Failed to "
             "create Geant4 sensitive detector %s of type %s.",
             nam.c_str(), typ.c_str());
    }
    print("+++ Subdetector: %-32s  type: %s.", nam.c_str(), typ.c_str());
  }
  if ( g4sd )  {
    g4sd->Activate(true);
    G4SDManager::GetSDMpointer()->AddNewDetector(g4sd);
  }
  return g4sd;
}

/// Geometry construction callback. Called at "Construct()"
void Geant4DetectorConstruction::constructGeo(Geant4DetectorConstructionContext* )  {
}

/// Electromagnetic field construction callback. Called at "ConstructSDandField()"
void Geant4DetectorConstruction::constructField(Geant4DetectorConstructionContext* )  {
}

/// Sensitive detector construction callback. Called at "ConstructSDandField()"
void Geant4DetectorConstruction::constructSensitives(Geant4DetectorConstructionContext* )  {
}

/// Standard Constructor
Geant4DetectorConstructionSequence::Geant4DetectorConstructionSequence(Geant4Context* ctxt, const std::string& nam)
  : Geant4Action(ctxt,nam)
{
  m_needsControl = true;
  InstanceCount::increment(this);
}

/// Default destructor
Geant4DetectorConstructionSequence::~Geant4DetectorConstructionSequence()  {
  m_actors(&Geant4DetectorConstruction::release);  
  InstanceCount::decrement(this);
}

/// Set or update client context
void Geant4DetectorConstructionSequence::updateContext(Geant4Context* ctxt)  {
  m_context = ctxt;
  m_actors(&Geant4DetectorConstruction::updateContext,ctxt);  
}

/// Add an actor responding to all callbacks. Sequence takes ownership.
void Geant4DetectorConstructionSequence::adopt(Geant4DetectorConstruction* action)  {
  if (action) {
    action->addRef();
    m_actors.add(action);
    return;
  }
  except("++ Attempt to add an invalid actor!");
}

/// Access an actor by name
Geant4DetectorConstruction* Geant4DetectorConstructionSequence::get(const std::string& nam)  const  {
  for(auto* i : m_actors)  {
    if ( i->name() == nam )  {
      return i;
    }
  }
  except("++ Attempt to access invalid actor %s!",nam.c_str());
  return nullptr;
}

/// Geometry construction callback. Called at "Construct()"
void Geant4DetectorConstructionSequence::constructGeo(Geant4DetectorConstructionContext* ctxt)  {
  m_actors(&Geant4DetectorConstruction::constructGeo, ctxt);  
}

/// Electromagnetic field construction callback. Called at "ConstructSDandField()"
void Geant4DetectorConstructionSequence::constructField(Geant4DetectorConstructionContext* ctxt)  {
  m_actors(&Geant4DetectorConstruction::constructField, ctxt);  
}

/// Sensitive detector construction callback. Called at "ConstructSDandField()"
void Geant4DetectorConstructionSequence::constructSensitives(Geant4DetectorConstructionContext* ctxt)  {
  m_actors(&Geant4DetectorConstruction::constructSensitives, ctxt);  
}

/// Access to the converted regions
const std::map<dd4hep::Region, G4Region*>&
Geant4DetectorConstructionSequence::regions() const  {
  Geant4GeometryInfo* p = Geant4Mapping::instance().ptr();
  if ( p ) return p->g4Regions;
  throw std::runtime_error("+++ Geant4DetectorConstructionSequence::regions: Access not possible. Geometry is not yet converted!");
}

/// Access to the converted volumes
const std::map<dd4hep::Volume, G4LogicalVolume*>&
Geant4DetectorConstructionSequence::volumes() const  {
  Geant4GeometryInfo* p = Geant4Mapping::instance().ptr();
  if ( p ) return p->g4Volumes;
  throw std::runtime_error("+++ Geant4DetectorConstructionSequence::volumes: Access not possible. Geometry is not yet converted!");
}

/// Access to the converted shapes
const std::map<const TGeoShape*, G4VSolid*>& Geant4DetectorConstructionSequence::shapes() const  {
  Geant4GeometryInfo* p = Geant4Mapping::instance().ptr();
  if ( p ) return p->g4Solids;
  throw std::runtime_error("+++ Geant4DetectorConstructionSequence::shapes: Access not possible. Geometry is not yet converted!");
}

/// Access to the converted limit sets
const std::map<dd4hep::LimitSet, G4UserLimits*>&
Geant4DetectorConstructionSequence::limits() const  {
  Geant4GeometryInfo* p = Geant4Mapping::instance().ptr();
  if ( p ) return p->g4Limits;
  throw std::runtime_error("+++ Geant4DetectorConstructionSequence::limits: Access not possible. Geometry is not yet converted!");
}

/// Access to the converted assemblies
const std::map<dd4hep::PlacedVolume, Geant4AssemblyVolume*>&
Geant4DetectorConstructionSequence::assemblies() const  {
  Geant4GeometryInfo* p = Geant4Mapping::instance().ptr();
  if ( p ) return p->g4AssemblyVolumes;
  throw std::runtime_error("+++ Geant4DetectorConstructionSequence::assemblies: Access not possible. Geometry is not yet converted!");
}

/// Access to the converted placements
const std::map<dd4hep::PlacedVolume, G4VPhysicalVolume*>&
Geant4DetectorConstructionSequence::placements() const  {
  Geant4GeometryInfo* p = Geant4Mapping::instance().ptr();
  if ( p ) return p->g4Placements;
  throw std::runtime_error("+++ Geant4DetectorConstructionSequence::placements: Access not possible. Geometry is not yet converted!");
}

/// Access to the converted materials
const Geant4GeometryMaps::MaterialMap& Geant4DetectorConstructionSequence::materials() const  {
  Geant4GeometryInfo* p = Geant4Mapping::instance().ptr();
  if ( p ) return p->g4Materials;
  throw std::runtime_error("+++ Geant4DetectorConstructionSequence::materials: Access not possible. Geometry is not yet converted!");
}

/// Access to the converted elements
const Geant4GeometryMaps::ElementMap& Geant4DetectorConstructionSequence::elements() const  {
  Geant4GeometryInfo* p = Geant4Mapping::instance().ptr();
  if ( p ) return p->g4Elements;
  throw std::runtime_error("+++ Geant4DetectorConstructionSequence::elements: Access not possible. Geometry is not yet converted!");
}
