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
#include <sstream>

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
      using hash_t       = uint64_t;
      struct entry_t   {
	hash_t   hash;
	std::string  data;
      };
      using ElementMap       = std::map<Atom,              entry_t>;
      using MaterialMap      = std::map<Material,          entry_t>;
      using LimitMap         = std::map<LimitSet,          entry_t>;
      using PlacementMap     = std::map<PlacedVolume,      entry_t>;
      using RegionMap        = std::map<Region,            entry_t>;
      using SensDetMap       = std::map<SensitiveDetector, entry_t>;
      using VolumeMap        = std::map<Volume,            entry_t>;
      using IdSpecMap        = std::map<IDDescriptor,      entry_t>;
      using SegmentationMap  = std::map<Segmentation,      entry_t>;
      using VisMap           = std::map<VisAttr,           entry_t>;
      using AlignmentMap     = std::map<Alignment,         entry_t>;
      using SolidMap         = std::map<Solid,             entry_t>;
      using FieldMap         = std::map<OverlayedField,    entry_t>;
      using TrafoMap         = std::map<const TGeoMatrix*, entry_t>;
      using MapOfDetElements = std::map<DetElement,        entry_t>;

      /// Data structure of the geometry converter from dd4hep to Geant 4 in Detector format.
      /**
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CORE
       */
      class GeometryInfo: public GeoHandler::GeometryInfo {
      public:
        ElementMap       mapOfElements;
        MaterialMap      mapOfMaterials;
        SolidMap         mapOfSolids;
        VolumeMap        mapOfVolumes;
        PlacementMap     mapOfPlacements;
        RegionMap        mapOfRegions;
        VisMap           mapOfVis;
        LimitMap         mapOfLimits;
        IdSpecMap        mapOfIdSpecs;
	SegmentationMap  mapOfSegmentations;
        SensDetMap       mapOfSensDets;
        TrafoMap         mapOfPositions;
        TrafoMap         mapOfRotations;
        FieldMap         mapOfFields;
	AlignmentMap     mapOfAlignments;
	MapOfDetElements mapOfDetElements;
	entry_t  header;

        GeometryInfo() = default;
      };

      /// Reference to detector description
      Detector& m_detDesc;
      GeometryInfo* m_dataPtr;
      mutable std::stringstream debug_hash;

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

      entry_t      empty_entry { 0UL, "" };
      std::string  newline = "";

      /// Property: precision of hashed printouts
      mutable int precision     { 6 };
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
      void hash_debug(const std::string& prefix, const entry_t& str, int flag=0)  const;
      entry_t make_entry(std::stringstream& log)  const;
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
      typedef std::vector<hash_t> hashes_t;
      void checksumPlacement(PlacedVolume pv, hashes_t& hashes, bool recursive)  const;
      void checksumDetElement(int level, DetElement det, hashes_t& hashes, bool recursive)  const;

      /// Add header information in Detector format
      virtual const entry_t& handleHeader() const;

      /// Convert the geometry type material into the corresponding gdml string
      virtual const entry_t& handleMaterial(Material medium) const;

      /// Convert the geometry type element into the corresponding gdml string
      virtual const entry_t& handleElement(Atom element) const;

      /// Convert the geometry type solid into the corresponding gdml string
      virtual const entry_t& handleSolid(Solid solid) const;

      /// Convert the geometry type logical volume into the corresponding gdml string
      virtual const entry_t& handleVolume(Volume volume) const;
      virtual void collectVolume(Volume volume) const;

      /// Convert the geometry type volume placement into the corresponding gdml string
      virtual const entry_t& handlePlacement(PlacedVolume node) const;

      /// Convert alignment entry into the corresponding gdml string
      const entry_t& handleAlignment(Alignment alignment)  const;

      /// Convert the geometry type field into the corresponding gdml string
      ///virtual const entry_t& handleField(Ref_t field) const;

      /// Convert the geometry type region into the corresponding gdml string
      virtual const entry_t& handleRegion(Region region) const;

      /// Convert the geometry visualisation attributes to the corresponding gdml string
      virtual const entry_t& handleVis(VisAttr vis) const;

      /// Convert the geometry id dictionary entry to the corresponding gdml string
      virtual const entry_t& handleIdSpec(IDDescriptor idspec) const;

      /// Convert the geometry type LimitSet into the corresponding gdml string
      virtual const entry_t& handleLimitSet(LimitSet limitset) const;

      virtual const entry_t& handleDetElement(DetElement det)  const;

      /// Convert the geometry type SensitiveDetector into the corresponding gdml string
      virtual const entry_t& handleSensitive(SensitiveDetector sens_det) const;

      /// Convert the segmentation of a SensitiveDetector into the corresponding Detector object
      virtual const entry_t& handleSegmentation(Segmentation seg) const;

      /// Convert the Position into the corresponding gdml string
      virtual const entry_t& handlePosition(const TGeoMatrix* trafo) const;

      /// Convert the Rotation into the corresponding gdml string
      virtual const entry_t& handleRotation(const TGeoMatrix* trafo) const;

      /// Convert the electric or magnetic fields into the corresponding gdml string
      virtual const entry_t& handleField(OverlayedField field) const;

      /// Handle the geant 4 specific properties
      const entry_t& handleProperties(Detector::Properties& prp) const;

      /// Dump elements used in this apparatus
      void dump_elements()   const;
      /// Dump materials used in this apparatus
      void dump_materials()   const;
      /// Dump solids used in this apparatus
      void dump_solids()   const;
      /// Dump volumes used in this apparatus
      void dump_volumes()   const;
      /// Dump placements used in this apparatus
      void dump_placements()   const;
      /// Dump detelements used in this apparatus
      void dump_detelements()   const;
      /// Dump iddescriptors used in this apparatus
      void dump_iddescriptors()   const;
      /// Dump segmentations used in this apparatus
      void dump_segmentations()   const;
      /// Dump sensitives used in this apparatus
      void dump_sensitives()   const;

    };
  }    // End namespace xml
}      // End namespace dd4hep
#endif // DDCORE_SRC_PLUGINS_DETECTORCHECKSUM_H
