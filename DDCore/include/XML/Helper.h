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
#ifndef DD4HEP_XML_HELPER_H
#define DD4HEP_XML_HELPER_H

// Framework include files
#include "XML/XML.h"
#include "DD4hep/Detector.h"
#include "DD4hep/Factories.h"
#include "DD4hep/DD4hepUnits.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace detail {

    /// std::string conversion of XML strings (e.g. Unicode for Xerces-C)
    static inline std::string _toString(const dd4hep::xml::XmlChar* value) {
      return xml::_toString(value);
    }

    /// std::string conversion of arbitrary entities including user defined formatting.
    template <typename T> inline std::string _toString(T value, const char* fmt) {
      return xml::_toString(value, fmt);
    }
  }
}

#endif // DD4HEP_XML_HELPER_H
