// $Id: Handle.h 570 2013-05-17 07:47:11Z markus.frank $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation européenne pour la Recherche nucléaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//==========================================================================
#include "ParsersStandardMiscCommon.h"

int DD4hep::Parsers::parse(std::map<std::string, int>& result, const std::string& input) {
  return DD4hep::Parsers::parse_(result, input);
}

int DD4hep::Parsers::parse(std::map<std::string, unsigned int>& result, const std::string& input) {
  return DD4hep::Parsers::parse_(result, input);
}

int DD4hep::Parsers::parse(std::map<std::string, double>& result, const std::string& input) {
  return DD4hep::Parsers::parse_(result, input);
}

int DD4hep::Parsers::parse(std::map<std::string, std::vector<std::string> >& result, const std::string& input) {
  return DD4hep::Parsers::parse_(result, input);
}
