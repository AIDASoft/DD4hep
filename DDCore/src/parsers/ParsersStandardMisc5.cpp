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
#include "ParsersStandardMiscCommon.h"

#if defined(DD4HEP_HAVE_ALL_PARSERS)
int dd4hep::Parsers::parse(std::map<unsigned int, std::string>& result, const std::string& input) {
  return dd4hep::Parsers::parse_(result, input);
}
#endif

int dd4hep::Parsers::parse(std::string& name, std::string& value, const std::string& input ) {
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

int dd4hep::Parsers::parse(std::map<std::string, std::pair<double, double> >& result, const std::string& input) {
  return dd4hep::Parsers::parse_(result, input);
}
