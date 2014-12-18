// $Id$
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_GEOMETRY_LCDDCONVERTER_H
#define DD4HEP_GEOMETRY_LCDDCONVERTER_H

// Framework include files
#include "DD4hep/LCDD.h"
#include "DD4hep/GeoHandler.h"
#include "DD4hep/DetFactoryHelper.h"

// C/C++ include files
#include <set>
#include <map>
#include <vector>

// Forward declarations
class TGeoVolume;
class TGeoElement;
class TGeoShape;
class TGeoMedium;
class TGeoNode;
class TGeoMatrix;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {

    // Forward declarations
    class SensitiveDetectorObject;

    /// Geometry converter from DD4hep to Geant 4 in LCDD format.
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
     */
    class LCDDConverter: public GeoHandler {
    public:
      typedef XML::XmlElement XmlElement;
      typedef std::map<Atom,              XmlElement*> ElementMap;
      typedef std::map<Material,          XmlElement*> MaterialMap;
      typedef std::map<LimitSet,          XmlElement*> LimitMap;
      typedef std::map<PlacedVolume,      XmlElement*> PlacementMap;
      typedef std::map<Region,            XmlElement*> RegionMap;
      typedef std::map<SensitiveDetector, XmlElement*> SensDetMap;
      typedef std::map<Volume,            XmlElement*> VolumeMap;
      typedef std::map<IDDescriptor,      XmlElement*> IdSpecMap;
      typedef std::map<VisAttr,           XmlElement*> VisMap;
      typedef std::map<const TGeoShape*,  XmlElement*> SolidMap;
      typedef std::map<OverlayedField,    XmlElement*> FieldMap;
      typedef std::map<const TGeoMatrix*, XmlElement*> TrafoMap;
      /// Data structure of the geometry converter from DD4hep to Geant 4 in LCDD format.
      /**
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_GEOMETRY
       */
      class GeometryInfo: public GeoHandler::GeometryInfo {
      public:
        ElementMap xmlElements;
        MaterialMap xmlMaterials;
        SolidMap xmlSolids;
        VolumeMap xmlVolumes;
        PlacementMap xmlPlacements;
        RegionMap xmlRegions;
        VisMap xmlVis;
        LimitMap xmlLimits;
        IdSpecMap xmlIdSpecs;
        SensDetMap xmlSensDets;
        TrafoMap xmlPositions;
        TrafoMap xmlRotations;
        FieldMap xmlFields;
        SensitiveDetectorSet sensitives;
        RegionSet regions;
        LimitSetSet limits;
        // These we need for redundancy and checking the data integrity
        typedef std::map<std::string, const TNamed*> CheckIter;
        struct _checks {
          std::map<std::string, const TNamed*> positions, rotations, volumes, solids, materials;
        };
        mutable _checks checks;
        void check(const std::string& name, const TNamed* n, std::map<std::string, const TNamed*>& array) const;
        void checkPosition(const std::string& name, const TNamed* n) const {
          check(name, n, checks.positions);
        }
        void checkRotation(const std::string& name, const TNamed* n) const {
          check(name, n, checks.rotations);
        }
        void checkVolume(const std::string& name, const TNamed* n) const {
          check(name, n, checks.volumes);
        }
        void checkShape(const std::string& name, const TNamed* n) const {
          check(name, n, checks.solids);
        }
        void checkMaterial(const std::string& name, Material n) const {
          check(name, n.ptr(), checks.materials);
        }

        xml_doc_t doc;
        xml_h identity_rot, identity_pos;
        xml_elt_t doc_root, doc_header, doc_idDict, doc_detectors, doc_limits, doc_regions, doc_display, doc_gdml, doc_fields,
          doc_define, doc_materials, doc_solids, doc_structure, doc_setup;
        GeometryInfo();
      };
      typedef std::set<std::string> NameSet;

      /// Reference to detector description
      LCDD& m_lcdd;
      mutable NameSet m_checkNames;
      GeometryInfo* m_dataPtr;

      GeometryInfo& data() const {
        return *m_dataPtr;
      }

      /// Data integrity checker
      void checkVolumes(const std::string& name, Volume volume) const;

      /// Initializing Constructor
      LCDDConverter(LCDD& lcdd);

      /// Standard destructor
      virtual ~LCDDConverter();

      /// Create geometry conversion in GDML format
      xml_doc_t createGDML(DetElement top);

      /// Create geometry conversion in LCDD format
      xml_doc_t createLCDD(DetElement top);

      /// Create geometry conversion in Vis format
      xml_doc_t createVis(DetElement top);

      /// Add header information in LCDD format
      virtual void handleHeader() const;

      /// Convert the geometry type material into the corresponding Xml object(s).
      virtual xml_h handleMaterial(const std::string& name, Material medium) const;

      /// Convert the geometry type element into the corresponding Xml object(s).
      virtual xml_h handleElement(const std::string& name, Atom element) const;

      /// Convert the geometry type solid into the corresponding Xml object(s).
      virtual xml_h handleSolid(const std::string& name, const TGeoShape* volume) const;

      /// Convert the geometry type logical volume into the corresponding Xml object(s).
      virtual xml_h handleVolume(const std::string& name, Volume volume) const;
      virtual xml_h handleVolumeVis(const std::string& name, const TGeoVolume* volume) const;
      virtual void collectVolume(const std::string& name, const TGeoVolume* volume) const;

      /// Convert the geometry type volume placement into the corresponding Xml object(s).
      virtual xml_h handlePlacement(const std::string& name, PlacedVolume node) const;

      /// Convert the geometry type field into the corresponding Xml object(s).
      ///virtual xml_h handleField(const std::string& name, Ref_t field) const;

      /// Convert the geometry type region into the corresponding Xml object(s).
      virtual xml_h handleRegion(const std::string& name, Region region) const;

      /// Convert the geometry visualisation attributes to the corresponding Xml object(s).
      virtual xml_h handleVis(const std::string& name, VisAttr vis) const;

      /// Convert the geometry id dictionary entry to the corresponding Xml object(s).
      virtual xml_h handleIdSpec(const std::string& name, IDDescriptor idspec) const;

      /// Convert the geometry type LimitSet into the corresponding Xml object(s).
      virtual xml_h handleLimitSet(const std::string& name, LimitSet limitset) const;

      /// Convert the geometry type SensitiveDetector into the corresponding Xml object(s).
      virtual xml_h handleSensitive(const std::string& name, SensitiveDetector sens_det) const;

      /// Convert the segmentation of a SensitiveDetector into the corresponding LCDD object
      virtual xml_h handleSegmentation(Segmentation seg) const;

      /// Convert the Position into the corresponding Xml object(s).
      virtual xml_h handlePosition(const std::string& name, const TGeoMatrix* trafo) const;

      /// Convert the Rotation into the corresponding Xml object(s).
      virtual xml_h handleRotation(const std::string& name, const TGeoMatrix* trafo) const;

      /// Convert the electric or magnetic fields into the corresponding Xml object(s).
      virtual xml_h handleField(const std::string& name, OverlayedField field) const;

      /// Handle the geant 4 specific properties
      void handleProperties(LCDD::Properties& prp) const;
    };
  }    // End namespace XML
}      // End namespace DD4hep

#endif // DD4HEP_GEOMETRY_LCDDCONVERTER_H
