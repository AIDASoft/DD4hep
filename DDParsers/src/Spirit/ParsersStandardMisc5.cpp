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

#if defined(DD4HEP_HAVE_ALL_PARSERS)
/// Namespace for the AIDA detector description toolkit
namespace dd4hep {
  /// Namespace for the AIDA detector for utilities using boost::spirit parsers
  namespace Parsers {

    template <> int parse(std::map<unsigned int, std::string>& result, const std::string& input) {
      return parse_(result, input);
    }
  }
}
#endif
#if 0
/// Namespace for the AIDA detector description toolkit
namespace dd4hep {
  /// Namespace for the AIDA detector for utilities using boost::spirit parsers
  namespace Parsers {

    template <> int parse(std::string& name, std::string& value, const std::string& input ) {
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

    template <> int parse(std::map<std::string, std::pair<double, double> >& result, const std::string& input) {
      return parse_(result, input);
    }
  }
}
#endif
