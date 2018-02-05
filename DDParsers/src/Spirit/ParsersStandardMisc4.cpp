// $Id: $
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

int dd4hep::Parsers::parse(std::map<std::string, std::vector<int> >& result, const std::string& input) {
  return dd4hep::Parsers::parse_(result, input);
}

int dd4hep::Parsers::parse(std::map<std::string, std::vector<double> >& result, const std::string& input) {
  return dd4hep::Parsers::parse_(result, input);
}
