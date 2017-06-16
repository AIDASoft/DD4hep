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
#ifndef DD4HEP_JSON_HELPER_H
#define DD4HEP_JSON_HELPER_H

// Framework include files
#include "JSON/Detector.h"
#include "JSON/ChildValue.h"
#include "DD4hep/Detector.h"
#include "DD4hep/Factories.h"
#include "DD4hep/DD4hepUnits.h"

// Shortcuts to elements of the JSON namespace
typedef dd4hep::json::Attribute       json_attr_t;
typedef dd4hep::json::Collection_t    json_coll_t;
typedef dd4hep::json::Handle_t        json_h;
typedef dd4hep::json::Element         json_elt_t;
typedef dd4hep::json::Element         json_ref_t;
typedef dd4hep::json::DetElement      json_det_t;
typedef dd4hep::json::Component       json_comp_t;
typedef dd4hep::json::Dimension       json_dim_t;
typedef dd4hep::json::ChildValue      json_val_t;
typedef dd4hep::json::Document        json_doc_t;
typedef dd4hep::json::DocumentHolder  json_doc_holder_t;
typedef dd4hep::json::DocumentHandler json_handler_t;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace detail {

    /// std::string conversion of JSON strings (e.g. Unicode for Xerces-C)
    static inline std::string _toString(const char* value) {
      return json::_toString(value);
    }

    /// std::string conversion of arbitrary entities including user defined formatting.
    template <typename T> inline std::string _toString(T value, const char* fmt) {
      return json::_toString(value, fmt);
    }
  }
}

#endif // DD4HEP_JSON_HELPER_H
