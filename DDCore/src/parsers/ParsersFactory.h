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
namespace DD4hep  {
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
  }/* Parsers */
} /*  DD4hep */
//=============================================================================
#endif // DD4HEPPROPERTYPARSERS_PARSERSGENERATOR_H

