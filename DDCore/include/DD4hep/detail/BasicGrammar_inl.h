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
// NOTE:
//
// This is an internal include file. It should only be included to 
// instantiate code. Otherwise the BasicGrammar include file should be
// sufficient for all practical purposes.
//
//==========================================================================
#ifndef DD4HEP_DDCORE_BASICGRAMMAR_INL_H
#define DD4HEP_DDCORE_BASICGRAMMAR_INL_H

// Framework include files
#include "DD4hep/config.h"
#include "DD4hep/Primitives.h"
#include "DD4hep/detail/Grammar.h"
#include "Parsers/spirit/Parsers.h"
#include "Parsers/spirit/ToStream.h"

#if defined(DD4HEP_PARSER_HEADER)

#define DD4HEP_NEED_EVALUATOR
// This is the case, if the parsers are externalized
// and the dd4hep namespace is renamed!
#include DD4HEP_PARSER_HEADER

#endif


// C/C++ include files
#include <string>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Object evaluator
  template<typename T> inline int eval_obj(T* ptr, const std::string& str)  {
    return BasicGrammar::instance<T>().fromString(ptr,pre_parse_obj(str));
  }
  
}      // End namespace dd4hep

#define DD4HEP_DEFINE_PARSER_GRAMMAR_EVAL(x,func)                       \
  namespace dd4hep {                                                    \
    template<> struct Grammar<x> : CommonGrammar<x> {                   \
      int evaluate(void* ptr, const std::string& val) const {           \
        return func ((x*)ptr,val);                                      \
      }                                                                 \
    };                                                                  \
  }

#define DD4HEP_DEFINE_PARSER_GRAMMAR(x,func)              DD4HEP_DEFINE_PARSER_GRAMMAR_EVAL(x,func)
#define DD4HEP_DEFINE_PARSER_GRAMMAR_CONT(x,func)         DD4HEP_DEFINE_PARSER_GRAMMAR_EVAL(x,func)

#endif  /* DD4HEP_DDCORE_BASICGRAMMAR_INL_H */
