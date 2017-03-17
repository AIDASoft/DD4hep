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
#ifndef DD4HEP_JSON_HELPER_H
#define DD4HEP_JSON_HELPER_H

// Framework include files
#include "JSON/Detector.h"
#include "JSON/ChildValue.h"
#include "DD4hep/LCDD.h"
#include "DD4hep/Factories.h"
#include "DD4hep/DD4hepUnits.h"

// Shortcuts to elements of the JSON namespace
typedef DD4hep::JSON::Attribute       json_attr_t;
typedef DD4hep::JSON::Collection_t    json_coll_t;
typedef DD4hep::JSON::Handle_t        json_h;
typedef DD4hep::JSON::Element         json_elt_t;
typedef DD4hep::JSON::Element         json_ref_t;
typedef DD4hep::JSON::DetElement      json_det_t;
typedef DD4hep::JSON::Component       json_comp_t;
typedef DD4hep::JSON::Dimension       json_dim_t;
typedef DD4hep::JSON::ChildValue      json_val_t;
typedef DD4hep::JSON::Document        json_doc_t;
typedef DD4hep::JSON::DocumentHolder  json_doc_holder_t;
typedef DD4hep::JSON::DocumentHandler json_handler_t;
typedef DD4hep::Geometry::LCDD        lcdd_t;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {

    /// std::string conversion of JSON strings (e.g. Unicode for Xerces-C)
    static inline std::string _toString(const char* value) {
      return JSON::_toString(value);
    }

    /// std::string conversion of arbitrary entities including user defined formatting.
    template <typename T> inline std::string _toString(T value, const char* fmt) {
      return JSON::_toString(value, fmt);
    }
  }
}

#endif // DD4HEP_JSON_HELPER_H
