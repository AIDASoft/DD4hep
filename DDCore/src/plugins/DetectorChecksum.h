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
#ifndef DDCORE_SRC_PLUGINS_DETECTORCHECKSUM_H
#define DDCORE_SRC_PLUGINS_DETECTORCHECKSUM_H

/// Framework include files
#include <DD4hep/Detector.h>
#include <DD4hep/GeoHandler.h>
#include <DD4hep/DD4hepUnits.h>

/// C/C++ include files

/// Forward declarations

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace detail {

    /// Geometry converter from dd4hep to Geant 4 in Detector format.
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CORE
     */
    class DetectorChecksum: public GeoHandler {
    public:
      using checksum_t       = uint64_t;
      using ElementMap       = std::map<Atom,              std::string>;
      using MaterialMap      = std::map<Material,          std::string>;
      using LimitMap         = std::map<LimitSet,          std::string>;
      using PlacementMap     = std::map<PlacedVolume,      std::string>;
      using RegionMap        = std::map<Region,            std::string>;
      using SensDetMap       = std::map<SensitiveDetector, std::string>;
      using VolumeMap        = std::map<Volume,            std::string>;
      using IdSpecMap        = std::map<IDDescriptor,      std::string>;
      using SegmentationMap  = std::map<Segmentation,      std::string>;
      using VisMap           = std::map<VisAttr,           std::string>;
      using AlignmentMap     = std::map<Alignment,         std::string>;
      using SolidMap         = std::map<const TGeoShape*,  std::string>;
      using FieldMap         = std::map<OverlayedField,    std::string>;
      using TrafoMap         = std::map<const TGeoMatrix*, std::string>;
      using MapOfDetElements = std::map<DetElement,        std::string>;

      /// Data structure of the geometry converter from dd4hep to Geant 4 in Detector format.
      /**
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CORE
       */
      class GeometryInfo: public GeoHandler::GeometryInfo {
      public:
        ElementMap      mapOfElements;
        MaterialMap     mapOfMaterials;
        SolidMap        mapOfSolids;
        VolumeMap       mapOfVolumes;
        PlacementMap    mapOfPlacements;
        RegionMap       mapOfRegions;
        VisMap          mapOfVis;
        LimitMap        mapOfLimits;
        IdSpecMap       mapOfIdSpecs;
	SegmentationMap mapOfSegmentations;
        SensDetMap      mapOfSensDets;
        TrafoMap        mapOfPositions;
        TrafoMap        mapOfRotations;
        FieldMap        mapOfFields;
	AlignmentMap    mapOfAlignments;
	MapOfDetElements mapOfDetElements;
	std::string  header;

        GeometryInfo() = default;
      };

      /// Reference to detector description
      Detector& m_detDesc;
      GeometryInfo* m_dataPtr;

      std::string  m_len_unit_nam = "mm";
      std::string  m_ang_unit_nam = "deg";
      std::string  m_ene_unit_nam = "GeV";
      std::string  m_densunit_nam = "g/cm3";
      std::string  m_atomunit_nam = "g/mole";
      double       m_len_unit = dd4hep::mm;
      double       m_ang_unit = dd4hep::deg;
      double       m_ene_unit = dd4hep::GeV;
      double       m_densunit = dd4hep::g/dd4hep::cm3;
      double       m_atomunit = dd4hep::g/dd4hep::mole;

      double       m_len__def = dd4hep::mm;
      double       m_ang__def = dd4hep::deg;
      double       m_ene__def = dd4hep::GeV;
      double       m_dens_def = dd4hep::g/dd4hep::cm3;
      double       m_atom_def = dd4hep::g/dd4hep::mole;

      std::string  empty_string;
      std::string  newline = "";

      /// Property: precision of hashed printouts
      int precision     { 6 };
      /// Property: Include readout property in detector hash
      int hash_readout  { 0 };
      /// Property: maximum depth level for printouts
      int max_level     { 1 };
      /// Property: debug level
      int debug         { 4 };

      GeometryInfo& data() const {
        return *m_dataPtr;
      }
      void configure();
      std::stringstream logger()   const;

      /// Initializing Constructor
      DetectorChecksum(Detector& description);

      /// Standard destructor
      virtual ~DetectorChecksum();

      template <typename T> std::string refName(T handle)  const;
      template <typename T> std::string attr_name(T  handle)  const;
      void collect_det_elements(DetElement top)  const;

      /// Create geometry conversion in Detector format
      void analyzeDetector(DetElement top);
      checksum_t checksumPlacement(PlacedVolume pv, unsigned long long int hash, bool recursive)  const;
      checksum_t checksumDetElement(int level, DetElement det, unsigned long long int hash, bool recursive)  const;

      /// Add header information in Detector format
      virtual const std::string& handleHeader() const;

      /// Convert the geometry type material into the corresponding gdml string
      virtual const std::string& handleMaterial(Material medium) const;

      /// Convert the geometry type element into the corresponding gdml string
      virtual const std::string& handleElement(Atom element) const;

      /// Convert the geometry type solid into the corresponding gdml string
      virtual const std::string& handleSolid(Solid solid) const;

      /// Convert the geometry type logical volume into the corresponding gdml string
      virtual const std::string& handleVolume(Volume volume) const;
      virtual void collectVolume(Volume volume) const;

      /// Convert the geometry type volume placement into the corresponding gdml string
      virtual const std::string& handlePlacement(PlacedVolume node) const;

      /// Convert alignment entry into the corresponding gdml string
      const std::string& handleAlignment(Alignment alignment)  const;

      /// Convert the geometry type field into the corresponding gdml string
      ///virtual const std::string& handleField(Ref_t field) const;

      /// Convert the geometry type region into the corresponding gdml string
      virtual const std::string& handleRegion(Region region) const;

      /// Convert the geometry visualisation attributes to the corresponding gdml string
      virtual const std::string& handleVis(VisAttr vis) const;

      /// Convert the geometry id dictionary entry to the corresponding gdml string
      virtual const std::string& handleIdSpec(IDDescriptor idspec) const;

      /// Convert the geometry type LimitSet into the corresponding gdml string
      virtual const std::string& handleLimitSet(LimitSet limitset) const;

      virtual const std::string& handleDetElement(DetElement det)  const;

      /// Convert the geometry type SensitiveDetector into the corresponding gdml string
      virtual const std::string& handleSensitive(SensitiveDetector sens_det) const;

      /// Convert the segmentation of a SensitiveDetector into the corresponding Detector object
      virtual const std::string& handleSegmentation(Segmentation seg) const;

      /// Convert the Position into the corresponding gdml string
      virtual const std::string& handlePosition(const TGeoMatrix* trafo) const;

      /// Convert the Rotation into the corresponding gdml string
      virtual const std::string& handleRotation(const TGeoMatrix* trafo) const;

      /// Convert the electric or magnetic fields into the corresponding gdml string
      virtual const std::string& handleField(OverlayedField field) const;

      /// Handle the geant 4 specific properties
      const std::string& handleProperties(Detector::Properties& prp) const;
    };
  }    // End namespace xml
}      // End namespace dd4hep
#endif // DDCORE_SRC_PLUGINS_DETECTORCHECKSUM_H
