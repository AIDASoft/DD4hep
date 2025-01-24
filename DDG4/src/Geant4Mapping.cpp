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
#include <DDG4/Geant4Mapping.h>
#include <DD4hep/Printout.h>
#include <DD4hep/VolumeManager.h>
#include <G4PVPlacement.hh>

using namespace dd4hep::sim;

/// Initializing Constructor
Geant4Mapping::Geant4Mapping(const Detector& description_ref)
  : m_detDesc(description_ref), m_dataPtr(0) {
}

/// Standard destructor
Geant4Mapping::~Geant4Mapping() {
  if (m_dataPtr)
    delete m_dataPtr;
  m_dataPtr = 0;
}

/// Possibility to define a singleton instance
Geant4Mapping& Geant4Mapping::instance() {
  static Geant4Mapping inst(Detector::getInstance());
  return inst;
}

/// When resolving pointers, we must check for the validity of the data block
void Geant4Mapping::checkValidity() const {
  if (m_dataPtr)
    return;
  except("Geant4Mapping", "Attempt to access an invalid data block!");
}

/// Create new data block. Delete old data block if present.
Geant4GeometryInfo& Geant4Mapping::init() {
  Geant4GeometryInfo* p = detach();
  if (p)
    delete p;
  attach(new Geant4GeometryInfo());
  return data();
}

/// Release data and pass over the ownership
Geant4GeometryInfo* Geant4Mapping::detach() {
  Geant4GeometryInfo* p = m_dataPtr;
  m_dataPtr = 0;
  return p;
}

/// Set a new data block
void Geant4Mapping::attach(Geant4GeometryInfo* data_ptr) {
  m_dataPtr = data_ptr;
}

/// Access the volume manager
Geant4VolumeManager Geant4Mapping::volumeManager() const {
  if ( m_dataPtr ) {
    if ( !m_dataPtr->has_volmgr ) {
      return Geant4VolumeManager(m_detDesc, m_dataPtr);
    }
    return Geant4VolumeManager(Handle < Geant4GeometryInfo > (m_dataPtr));
  }
  except("Geant4Mapping", "Cannot create volume manager without Geant4 geometry info [Invalid-Info]");
  return {};
}

/// Accessor to resolve geometry placements
dd4hep::PlacedVolume Geant4Mapping::placement(const G4VPhysicalVolume* node) const {
  checkValidity();
  const Geant4GeometryMaps::PlacementMap& pm = m_dataPtr->g4Placements;
  for( const auto& entry : pm )  {
    if ( entry.second == node )
      return PlacedVolume(entry.first);
  }
  return PlacedVolume(0);
}
