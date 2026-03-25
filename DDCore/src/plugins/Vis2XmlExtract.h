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

#ifndef DDCORE_SRC_PLUGINS_VIS2XMLEXTRACT_H
#define DDCORE_SRC_PLUGINS_VIS2XMLEXTRACT_H

// Framework include files
#include <DD4hep/Detector.h>
#include <DD4hep/GeoHandler.h>
#include <DD4hep/DetFactoryHelper.h>

// C/C++ include files
#include <map>

// Forward declarations
class TGeoVolume;

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
    class Vis2XmlExtract: public GeoHandler {
    public:

      /// Data structure of the geometry converter from dd4hep to Geant 4 in Detector format.
      /**
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CORE
       */
      class GeometryInfo: public GeoHandler::GeometryInfo {
      public:
        std::map<Volume,  xml::XmlElement*> xmlVolumes;
        std::map<VisAttr, xml::XmlElement*> xmlVis;
        xml_doc_t doc;
        xml_elt_t doc_root, doc_display, doc_structure;
        GeometryInfo();
      };

      /// Reference to detector description
      Detector&       m_detDesc;
      GeometryInfo*   m_dataPtr;

      /// Initializing Constructor
      Vis2XmlExtract(Detector& description);

      /// Standard destructor
      virtual ~Vis2XmlExtract();

      /// Create geometry conversion in Vis format
      xml_doc_t createVis(DetElement top);

      virtual xml_h handleVolume(const std::string& name, Volume volume) const;
      virtual xml_h handleVolumeVis(const std::string& name, const TGeoVolume* volume) const;
      /// Convert the geometry visualisation attributes to the corresponding Xml object(s).
      virtual xml_h handleVis(const std::string& name, VisAttr vis) const;
    };
  }    // End namespace xml
}      // End namespace dd4hep
#endif // DDCORE_SRC_PLUGINS_VIS2XMLEXTRACT_H
