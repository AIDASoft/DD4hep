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
#ifndef DD4HEPPROPERTYPARSERS_PARSERSGENERATOR_H
#define DD4HEPPROPERTYPARSERS_PARSERSGENERATOR_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <string>
#include <vector>
#include <map>
// ============================================================================
// Boost:
// ============================================================================
#include <boost/mpl/assert.hpp>
#include <boost/type_traits.hpp>
// ============================================================================
// DD4hep
// ============================================================================
#include "UsedParser.h"
#include "GrammarsV2.h"
// ============================================================================
/// Namespace for the AIDA detector description toolkit
namespace DD4hep {
  /// Namespace for the AIDA detector for utilities using boost::spirit parsers
  namespace Parsers {

    // ========================================================================
    typedef std::string::const_iterator IteratorT;
    //typedef boost::spirit::ascii::space_type Skipper;
    typedef SkipperGrammar<IteratorT> Skipper;
    // ========================================================================
    template<typename ResultT> inline int
    parse_(ResultT& result, const std::string& input){
      Skipper skipper;
      typename Grammar_<IteratorT, ResultT, Skipper>::Grammar g;
      IteratorT iter = input.begin(), end = input.end();
      return qi::phrase_parse( iter, end, g, skipper , result) && (iter==end);
    }
    //=========================================================================
    template<> inline int
    parse_(std::string& result, const std::string& input){
      Skipper skipper;
      Grammar_<IteratorT, std::string, Skipper>::Grammar g;
      IteratorT iter = input.begin(), end = input.end();
      if (!(qi::phrase_parse( iter, end, g, skipper, result) && (iter==end))){
        result = input;
      }
      //@attention always
      return true;
    }
    //=========================================================================

  }                                        //  end of namespace Parsers
}                                          //  end of namespace DD4hep

//=============================================================================

// ============================================================================
#define PARSERS_DEF_FOR_SINGLE(Type)                                  \
  PARSERS_DECL_FOR_SINGLE(Type)                                       \
  int DD4hep::Parsers::parse(Type& result, const std::string& input)  \
  {  return DD4hep::Parsers::parse_(result, input);  }
// ============================================================================

#endif // DD4HEPPROPERTYPARSERS_PARSERSGENERATOR_H

