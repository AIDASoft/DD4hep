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
#ifndef DD4HEP_DD4HEPROOTPERSISTENCY_H
#define DD4HEP_DD4HEPROOTPERSISTENCY_H

// Framework include files
#include "DD4hep/DetectorData.h"

/// Helper class to support ROOT persistency of Detector objects
class DD4hepRootPersistency : public TNamed, public dd4hep::DetectorData  {
public:
  typedef std::map<std::string, dd4hep::Handle<dd4hep::NamedObject> >  HandleMap;
  typedef std::map<std::string, std::string>          PropertyValues;
  typedef std::map<std::string, PropertyValues>       Properties;
  
  /// Default constructor
  DD4hepRootPersistency() : TNamed() {}
  /// Default destructor
  virtual ~DD4hepRootPersistency() {}

  static int save(dd4hep::Detector& description, const char* fname, const char* instance = "Geometry");
  static int load(dd4hep::Detector& description, const char* fname, const char* instance = "Geometry");
  /// Access the geometry manager of this instance
  TGeoManager& manager() const {
    return *m_manager;
  }
  /// Access to properties
  Properties& properties() const {
    return *(Properties*)&m_properties;
  }
  /// Return handle to material describing air
  dd4hep::Material air() const {
    return m_materialAir;
  }
  /// Return handle to material describing vacuum
  dd4hep::Material vacuum() const {
    return m_materialVacuum;
  }
  /// Return handle to "invisible" visualization attributes
  dd4hep::VisAttr invisible() const {
    return m_invisibleVis;
  }
  /// Return reference to the top-most (world) detector element
  dd4hep::DetElement world() const {
    return m_world;
  }
  /// Return reference to detector element with all tracker devices.
  dd4hep::DetElement trackers() const {
    return m_trackers;
  }
  /// Return handle to the world volume containing everything
  dd4hep::Volume worldVolume() const {
    return m_worldVol;
  }
  /// Return handle to the world volume containing the volume with the tracking devices
  dd4hep::Volume trackingVolume() const {
    return m_trackingVol;
  }
  /// Return handle to the VolumeManager
  dd4hep::VolumeManager volumeManager() const {
    return m_volManager;
  }
  /// Return handle to the combined electromagentic field description.
  dd4hep::OverlayedField field() const {
    return m_field;
  }
  /// Accessor to the header entry
  dd4hep::Header header() const {
    return m_header;
  }
  /// Accessor to the map of constants
  const HandleMap& constants() const {
    return m_define;
  }
  /// Accessor to the map of visualisation attributes
  const HandleMap& visAttributes() const {
    return m_display;
  }
  /// Accessor to the map of limit settings
  const HandleMap& limitsets() const {
    return m_limits;
  }
  /// Accessor to the map of region settings
  const HandleMap& regions() const {
    return m_regions;
  }
  /// Accessor to the map of readout structures
  const HandleMap& readouts() const {
    return m_readouts;
  }
  /// Accessor to the map of sub-detectors
  const HandleMap& detectors() const {
    return m_detectors;
  }
  /// Retrieve a sensitive detector by it's name from the detector description
  const HandleMap& sensitiveDetectors() const {
    return m_sensitive;
  }
  /// Accessor to the map of field entries, which together form the global field
  const HandleMap& fields() const {
    return m_fields;
  }
  /// Accessor to the map of ID specifications
  const HandleMap& idSpecifications() const {
    return m_idDict;
  }

  /// ROOT implementation macro
  ClassDef(DD4hepRootPersistency,1);
};

class DD4hepRootCheck  {
public:
  const DD4hepRootPersistency* object = 0;
  DD4hepRootCheck() = default;
  DD4hepRootCheck(const DD4hepRootCheck& copy) = default;
  DD4hepRootCheck(const DD4hepRootPersistency* o) : object(o) {}
  ~DD4hepRootCheck() = default;
  DD4hepRootCheck& operator=(const DD4hepRootCheck& copy) = default;
  const DD4hepRootCheck& checkDetectors()  const;
  const DD4hepRootCheck& checkMaterials()  const;
  const DD4hepRootCheck& checkReadouts()   const;
  const DD4hepRootCheck& checkFields()   const;
  const DD4hepRootCheck& checkRegions()   const;
  const DD4hepRootCheck& checkIdSpecs()   const;
  const DD4hepRootCheck& checkSensitives()   const;
  const DD4hepRootCheck& checkLimitSets()   const;
  const DD4hepRootCheck& checkVolManager()   const;
  const DD4hepRootCheck& checkDefines()   const;
  const DD4hepRootCheck& checkProperties()   const;
  const DD4hepRootCheck& checkAll()   const;
};


#endif    /* DD4HEP_DD4HEPROOTPERSISTENCY_H         */
