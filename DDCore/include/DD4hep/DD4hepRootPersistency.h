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
/**
 *  \author  M.Frank
 *  \version 1.0
 *  \ingroup DD4HEP_CORE
 */
class DD4hepRootPersistency : public TNamed  {
public:
  typedef std::map<std::string, dd4hep::Handle<dd4hep::NamedObject> >  HandleMap;

  /// The main data block
  dd4hep::DetectorData*     m_data = 0;
  /// Helper since plain segmentations cannot be saved
  std::map<dd4hep::Readout,std::pair<dd4hep::IDDescriptor,dd4hep::DDSegmentation::Segmentation*> > m_segments;
  /// Helper to save alignment conditions from the DetElement nominals
  std::map<dd4hep::DetElement,dd4hep::AlignmentCondition> nominals;

  /// Default constructor
  DD4hepRootPersistency();

  /// Default destructor
  virtual ~DD4hepRootPersistency();

  /// Save an existing detector description in memory to a ROOT file
  static int save(dd4hep::Detector& description, const char* fname, const char* instance = "Geometry");
  /// Load an detector description from a ROOT file to memory
  static int load(dd4hep::Detector& description, const char* fname, const char* instance = "Geometry");
  
  /// Access the geometry manager of this instance
  TGeoManager& manager() const                {    return *m_data->m_manager;         }
  /// Return handle to material describing air
  dd4hep::Material air() const                {    return m_data->m_materialAir;      }
  /// Return handle to material describing vacuum
  dd4hep::Material vacuum() const             {    return m_data->m_materialVacuum;   }
  /// Return handle to "invisible" visualization attributes
  dd4hep::VisAttr invisible() const           {    return m_data->m_invisibleVis;     }
  /// Return reference to the top-most (world) detector element
  dd4hep::DetElement world() const            {    return m_data->m_world;            }
  /// Return reference to detector element with all tracker devices.
  dd4hep::DetElement trackers() const         {    return m_data->m_trackers;         }
  /// Return handle to the world volume containing everything
  dd4hep::Volume worldVolume() const          {    return m_data->m_worldVol;         }
  /// Return handle to the world volume containing the volume with the tracking devices
  dd4hep::Volume trackingVolume() const       {    return m_data->m_trackingVol;      }
  /// Return handle to the VolumeManager
  dd4hep::VolumeManager volumeManager() const {    return m_data->m_volManager;       }
  /// Return handle to the combined electromagentic field description.
  dd4hep::OverlayedField field() const        {    return m_data->m_field;            }
  /// Accessor to the header entry
  dd4hep::Header header() const               {    return m_data->m_header;           }
  /// Accessor to the map of constants
  const HandleMap& constants() const          {    return m_data->m_define;           }
  /// Accessor to the map of visualisation attributes
  const HandleMap& visAttributes() const      {    return m_data->m_display;          }
  /// Accessor to the map of limit settings
  const HandleMap& limitsets() const          {    return m_data->m_limits;           }
  /// Accessor to the map of region settings
  const HandleMap& regions() const            {    return m_data->m_regions;          }
  /// Accessor to the map of readout structures
  const HandleMap& readouts() const           {    return m_data->m_readouts;         }
  /// Accessor to the map of sub-detectors
  const HandleMap& detectors() const          {    return m_data->m_detectors;        }
  /// Retrieve a sensitive detector by it's name from the detector description
  const HandleMap& sensitiveDetectors() const {    return m_data->m_sensitive;        }
  /// Accessor to the map of field entries, which together form the global field
  const HandleMap& fields() const             {    return m_data->m_fields;           }
  /// Accessor to the map of ID specifications
  const HandleMap& idSpecifications() const   {    return m_data->m_idDict;           }

  /// ROOT implementation macro
  ClassDef(DD4hepRootPersistency,1);
};


/// Helper class to check various ingredients of the Detector object after loaded from ROOT
/**
 *  \author  M.Frank
 *  \version 1.0
 *  \ingroup DD4HEP_CORE
 */
class DD4hepRootCheck  {
public:
  /// Reference to the detector model object
  dd4hep::Detector* object=0;
  /// Default constructor
  DD4hepRootCheck() = default;
  /// Copy constructor
  DD4hepRootCheck(const DD4hepRootCheck& copy) = default;
  /// Initializing constructor
  DD4hepRootCheck(dd4hep::Detector* o) : object(o) {}
  /// Default destructor
  virtual ~DD4hepRootCheck() = default;
  /// Assignment operator
  DD4hepRootCheck& operator=(const DD4hepRootCheck& copy) = default;
  /// Call to check a Material object
  size_t checkMaterials()  const;
  /// Check the collection of define statements
  size_t checkConstants()   const;
  /// Check detector description properties (string defines)
  size_t checkProperties()   const;
  /// Call to theck the DD4hep fields
  size_t checkFields()   const;
  /// Call to check a Region object
  size_t checkRegions()   const;
  /// Call to check a Readout object
  size_t checkReadouts()   const;
  /// Call to check an ID specification
  size_t checkIdSpecs()   const;
  /// Call to check a sensitive detector
  size_t checkSensitives()   const;
  /// Call to check a limit-set object
  size_t checkLimitSets()   const;
  /// Call to check the volume manager hierarchy
  size_t checkVolManager()   const;
  /// Call to check the nominal alignments in the DetElement hierarchy
  size_t checkNominals()   const;
  /// Call to check a top level Detector element (subdetector)
  size_t checkDetectors()  const;
  /// Call to check segmentations starting from the top level Detector element
  size_t checkSegmentations()  const;
  /// Check all of the above
  size_t checkAll()   const;
};

#endif    /* DD4HEP_DD4HEPROOTPERSISTENCY_H         */
