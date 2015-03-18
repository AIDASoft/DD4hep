// $Id: Conversions.h 1479 2014-12-18 16:00:45Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4hep_XML_XMLUTILITIES_H
#define DD4hep_XML_XMLUTILITIES_H

#include "XML/Conversions.h"
#include "XML/XMLElements.h"
#include "DD4hep/LCDD.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the AIDA detector description toolkit supporting XML utilities
  namespace XML {

    /// Create a solid shape using the plugin mechanism from the attributes of the XML element
    Geometry::Solid createShape(Geometry::LCDD& lcdd, const std::string& shape_type, XML::Element element);
    Geometry::Volume createVolume(Geometry::LCDD& lcdd, const std::string& volume_type, XML::Element element);

  }  /* End namespace XML              */
}    /* End namespace DD4hep           */
#endif    /* DD4hep_XML_XMLUTILITIES_H */
