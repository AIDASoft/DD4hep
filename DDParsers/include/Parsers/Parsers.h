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
//
// Setup XML parsing for the use of Apache Xerces-C and TiXml
//
//==========================================================================
#ifndef PARSERS_PARSERS_H
#define PARSERS_PARSERS_H

#include "Parsers/config.h"
#include <iostream>
#include <string>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {
  /// Namespace for the spirit parsers of the AIDA detector description toolkit
  namespace Parsers {
    /// Template for the conversion from string to object
    template <typename TYPE> int parse(TYPE& result, const std::string& input);
    /// Template for the conversion object to string
    template <typename TYPE> std::ostream& toStream(const TYPE& obj, std::ostream& s);
  }    // End namespace Parsers
}      // End namespace dd4hep
#endif // PARSERS_PARSERS_H
