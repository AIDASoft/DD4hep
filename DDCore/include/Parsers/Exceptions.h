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
#ifndef PARSERS_EXCEPTIONS_H
#define PARSERS_EXCEPTIONS_H

// Framework include files
#include "Parsers/config.h"

// C/C++ include files
#include <string>
#include <typeinfo>
#include <stdexcept>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Exception class to signal operations on unrelated classes
  /**
   *   \author  M.Frank
   *   \date    13.08.2013
   *   \ingroup DD4HEP
   */
  struct unrelated_type_error : public std::runtime_error {
    static std::string msg(const std::type_info& typ1, const std::type_info& typ2, const std::string& text);
    unrelated_type_error(const std::type_info& typ1, const std::type_info& typ2, const std::string& text = "")
      : std::runtime_error(msg(typ1, typ2, text)) {
    }
  };

  /// Exception class to signal operations on unrelated types of values
  /**
   *   \author  M.Frank
   *   \date    13.08.2013
   *   \ingroup DD4HEP
   */
  struct unrelated_value_error : public std::runtime_error {
    static std::string msg(const std::type_info& typ, const std::string& text);
    unrelated_value_error(const std::type_info& typ, const std::string& text = "")
      : std::runtime_error(msg(typ, text)) {
    }
  };

}      // End namespace dd4hep

#endif // PARSERS_EXCEPTIONS_H
