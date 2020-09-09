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
#ifndef PARSERS_SPIRIT_PARSERSFACTORY_H
#define PARSERS_SPIRIT_PARSERSFACTORY_H 1
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
// dd4hep
// ============================================================================
#include "Parsers/config.h"
#include "Parsers/spirit/UsedParser.h"
#include "Parsers/spirit/GrammarsV2.h"
#include "Parsers/spirit/GrammarsV2.h"
#include "Parsers/spirit/ToStream.h"
// ============================================================================
/// Namespace for the AIDA detector description toolkit
namespace dd4hep {
  /// Namespace for the AIDA detector for utilities using boost::spirit parsers
  namespace Parsers {
    template <typename TYPE>
    int parse(TYPE& result, const std::string& input);
    template <typename TYPE>
    std::ostream& toStream(const TYPE& obj, std::ostream& s);

    
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
}                                          //  end of namespace dd4hep

//=============================================================================

// ============================================================================
#define PARSERS_DEF_FOR_SINGLE(Type)                                    \
  namespace dd4hep  {                                                   \
  namespace Parsers  {                                                  \
    template <> int parse(Type& result, const std::string& input)       \
    {  return parse_(result, input);  }                                 \
    template <> std::ostream& toStream(const Type& obj, std::ostream& s) \
    {  return toStream_(obj, s); }}}
// ============================================================================

#endif // PARSERS_SPIRIT_PARSERSFACTORY_H

