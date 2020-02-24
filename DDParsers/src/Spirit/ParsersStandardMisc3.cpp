//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//==========================================================================
#include "Parsers/spirit/ParsersStandardMiscCommon.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {
  /// Namespace for the AIDA detector for utilities using boost::spirit parsers
  namespace Parsers {

    int parse(std::map<std::string, std::vector<std::string> >& result, const std::string& input) {
      return parse_(result, input);
    }
  }
}

