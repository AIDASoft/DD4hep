// $Id: $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//==========================================================================
#include "ParsersStandardMiscCommon.h"

int DD4hep::Parsers::parse(std::map<unsigned int, std::string>& result, const std::string& input) {
  return DD4hep::Parsers::parse_(result, input);
}

int DD4hep::Parsers::parse(std::string& name, std::string& value ,
                           const std::string& input ) {
  Skipper skipper;
  KeyValueGrammar<IteratorT, Skipper> g;
  KeyValueGrammar<IteratorT, Skipper>::ResultT result;
  std::string::const_iterator iter = input.begin();
  bool parse_result = qi::phrase_parse(iter, input.end(), g, skipper,
                                       result) && (iter==input.end());
  if (parse_result) {
    name = result.first;
    value = result.second;
  }
  return parse_result;
}

int DD4hep::Parsers::parse(std::map<std::string, std::pair<double, double> >& result, const std::string& input) {
  return DD4hep::Parsers::parse_(result, input);
}
