// $Id$
//==========================================================================
//  AIDA Detector description implementation for LCD
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
#ifndef DD4hep_DETECTOR_DETFACTORYHELPER_H
#define DD4hep_DETECTOR_DETFACTORYHELPER_H

#include "XML/XMLDetector.h"
#include "XML/XMLChildValue.h"
#include "DD4hep/LCDD.h"
#include "DD4hep/Factories.h"
#include "DD4hep/DD4hepUnits.h"

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
typedef DD4hep::XML::ChildValue xml_val_t;
typedef DD4hep::XML::Document xml_doc_t;
typedef DD4hep::XML::Strng_t Unicode;
typedef DD4hep::Geometry::LCDD lcdd_t;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {

    /// std::string conversion of XML strings (e.g. Unicode for Xerces-C)
    static inline std::string _toString(const DD4hep::XML::XmlChar* value) {
      return XML::_toString(value);
    }

    /// std::string conversion of arbitrary entities including user defined formatting.
    template <typename T> inline std::string _toString(T value, const char* fmt) {
      return XML::_toString(value, fmt);
    }
  }
}

#endif // DD4hep_DETECTOR_DETFACTORYHELPER_H
