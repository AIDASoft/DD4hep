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

  /** Helper function to get attribute or return the default.
   *  If the hasAttr does not return true then the supplied default value is returned.
   *  This is useful when building a detector and you want to supply a default value.
   */
  template <typename T>
    T getAttrOrDefault(const dd4hep::xml::Element& e, const dd4hep::xml::XmlChar* attr_name, T default_value)
    {
      return (e.hasAttr(attr_name)) ? e.attr<T>(attr_name) : default_value;
    }

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
