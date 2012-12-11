// $Id:$
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_XML_LCDDCONVERTER_H
#define DD4HEP_XML_LCDDCONVERTER_H

// Framework include files
#include "DD4hep/GeoHandler.h"
#include "DD4hep/LCDD.h"
#include "XML/XMLElements.h"

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

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   XML namespace declaration
   */
  namespace XML   {


    /** @class LCDDConverter LCDDConverter.h XML/LCDDConverter.h
     * 
     * Geometry converter from DD4hep to Geant 4.
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct LCDDConverter : public Geometry::GeoHandler  {
      typedef std::map<const TGeoElement*,XmlElement*> ElementMap;
      typedef std::map<const TGeoMedium*, XmlElement*> MaterialMap;
      typedef std::map<const TNamed*,     XmlElement*> LimitMap;
      typedef std::map<const TGeoNode*,   XmlElement*> PlacementMap;
      typedef std::map<const TNamed*,     XmlElement*> RegionMap;
      typedef std::map<const TNamed*,     XmlElement*> SensDetMap;
      typedef std::map<const TGeoVolume*, XmlElement*> VolumeMap;
      typedef std::map<const TGeoShape*,  XmlElement*> SolidMap;
      typedef std::map<const TNamed*,     XmlElement*> VisMap;
      typedef std::map<const TNamed*,     XmlElement*> IdSpecMap;
      typedef std::map<const TGeoMatrix*, XmlElement*> TrafoMap;
      struct GeometryInfo : public Geometry::GeoHandler::GeometryInfo {
	ElementMap         xmlElements;
	MaterialMap        xmlMaterials;
	SolidMap           xmlSolids;
	VolumeMap          xmlVolumes;
	PlacementMap       xmlPlacements;
	RegionMap          xmlRegions;
	VisMap             xmlVis;
	LimitMap           xmlLimits;
	IdSpecMap          xmlIdSpecs;
	SensDetMap         xmlSensDets;
	TrafoMap           xmlPositions;
	TrafoMap           xmlRotations;

	ObjectSet          sensitives;
        ObjectSet          regions;
	ObjectSet          limits;
	Document           doc;
	Element            doc_root, doc_header, doc_idDict, doc_detectors, doc_limits, doc_regions,
	  doc_display, doc_gdml, doc_fields, doc_define, doc_materials, doc_solids, doc_structure, doc_setup;
	GeometryInfo();
      };

      LCDD&           m_lcdd;
      bool            m_checkOverlaps;

      typedef std::set<std::string> NameSet;
      mutable NameSet m_checkNames;

      GeometryInfo* m_dataPtr;
      GeometryInfo& data() const { return *m_dataPtr; }


      void checkVolumes(const std::string& name, const TGeoVolume* volume) const;

      
      /// Initializing Constructor
      LCDDConverter( LCDD& lcdd );

      /// Standard destructor
      virtual ~LCDDConverter() {}

      /// Create geometry conversion
      void create(DetElement top);

      /// Add header information in LCDD format
      virtual void handleHeader() const;

      /// Convert the geometry type material into the corresponding Xml object(s).
      virtual Handle_t handleMaterial(const std::string& name, const TGeoMedium* medium) const;

      /// Convert the geometry type element into the corresponding Xml object(s).
      virtual Handle_t handleElement(const std::string& name, const TGeoElement* element) const;

      /// Convert the geometry type solid into the corresponding Xml object(s).
      virtual Handle_t handleSolid(const std::string& name, const TGeoShape* volume) const;

      /// Convert the geometry type logical volume into the corresponding Xml object(s).
      virtual Handle_t handleVolume(const std::string& name, const TGeoVolume* volume) const;
      virtual void collectVolume(const std::string& name, const TGeoVolume* volume) const;

      /// Convert the geometry type volume placement into the corresponding Xml object(s).
      virtual Handle_t handlePlacement(const std::string& name, const TGeoNode* node) const;

      /// Convert the geometry type field into the corresponding Xml object(s).
      ///virtual Handle_t handleField(const std::string& name, Ref_t field) const;

      /// Convert the geometry type region into the corresponding Xml object(s).
      virtual Handle_t handleRegion(const std::string& name, const TNamed* region)  const;

      /// Convert the geometry visualisation attributes to the corresponding Xml object(s).
      virtual Handle_t handleVis(const std::string& name, const TNamed* vis) const;

      /// Convert the geometry id dictionary entry to the corresponding Xml object(s).
      virtual Handle_t handleIdSpec(const std::string& name, const TNamed* vis) const;

      /// Convert the geometry type LimitSet into the corresponding Xml object(s).
      virtual Handle_t handleLimitSet(const std::string& name, const TNamed* limitset)  const;

      /// Convert the geometry type SensitiveDetector into the corresponding Xml object(s).
      virtual Handle_t handleSensitive(const std::string& name, const TNamed* sens_det)  const;

      /// Convert the Position into the corresponding Xml object(s).
      virtual Handle_t handlePosition(const std::string& name, const TGeoMatrix* trafo)   const;

      /// Convert the Rotation into the corresponding Xml object(s).
      virtual Handle_t handleRotation(const std::string& name, const TGeoMatrix* trafo)   const;

      /// Handle the geant 4 specific properties
      void handleProperties(LCDD::Properties& prp) const;
    };
  }    // End namespace XML
}      // End namespace DD4hep

#endif // DD4HEP_XML_LCDDCONVERTER_H
