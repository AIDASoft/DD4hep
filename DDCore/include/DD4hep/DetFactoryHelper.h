// $Id$
//====================================================================
//  AIDA Detector description
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4hep_DETECTOR_DETFACTORYHELPER_H
#define DD4hep_DETECTOR_DETFACTORYHELPER_H

#include "XML/XMLDetector.h"
#include "DD4hep/LCDD.h"
#include "DD4hep/Factories.h"

// Shortcuts to elements of the XML namespace
typedef DD4hep::XML::Tag_t xml_tag_t;
typedef DD4hep::XML::Attribute xml_attr_t;
typedef DD4hep::XML::Collection_t xml_coll_t;
typedef DD4hep::XML::Handle_t xml_h;
typedef DD4hep::XML::Element xml_elt_t;
typedef DD4hep::XML::RefElement xml_ref_t;
typedef DD4hep::XML::DetElement xml_det_t;
typedef DD4hep::XML::Component xml_comp_t;
typedef DD4hep::XML::Dimension xml_dim_t;
typedef DD4hep::XML::Document xml_doc_t;
typedef DD4hep::XML::Strng_t Unicode;
typedef DD4hep::Geometry::LCDD lcdd_t;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Geometry sub-namespace declaration
   */
  namespace Geometry {
    static inline std::string _toString(const DD4hep::XML::XmlChar* value) {
      return XML::_toString(value);
    }

    template <typename T> inline std::string _toString(T value, const char* fmt) {
      return XML::_toString(value, fmt);
    }
  }
}

#endif // DD4hep_DETECTOR_DETFACTORYHELPER_H
