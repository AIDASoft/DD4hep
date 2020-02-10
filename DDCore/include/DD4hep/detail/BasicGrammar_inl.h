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
#include "Evaluator/Evaluator.h"
#include "Parsers/spirit/Parsers.h"
#include "Parsers/spirit/ToStream.h"

#if defined(DD4HEP_PARSER_HEADER)

#define DD4HEP_NEED_EVALUATOR
// This is the case, if the parsers are externalized
// and the dd4hep namespace is renamed!
#include DD4HEP_PARSER_HEADER

#endif

namespace dd4hep { dd4hep::tools::Evaluator& g4Evaluator();  }
namespace {  static dd4hep::tools::Evaluator& s__eval(dd4hep::g4Evaluator());  }

// C/C++ include files
#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <deque>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Item evaluator
  template <typename T> inline int eval_item(T* ptr, std::string val)  {
    size_t idx = val.find("(int)");
    if (idx != std::string::npos)
      val.erase(idx, 5);
    while (val[0] == ' ')
      val.erase(0, 1);
    double result = s__eval.evaluate(val.c_str());
    if (s__eval.status() != tools::Evaluator::OK) {
      return 0;
    }
    *ptr = (T)result;
    return 1;
  }

  /// String evaluator
  template <> inline int eval_item<std::string>(std::string* ptr, std::string val)  {
    *ptr = val;
    return 1;
  }

  /// Item evaluator
  template <typename T,typename Q> inline int eval_pair(std::pair<T,Q>* ptr, std::string str)  {
    const BasicGrammar& grammar = BasicGrammar::instance<std::pair<T,Q> >();
    if ( !grammar.fromString(ptr,str) )  return 0;
    return 1;
  }

  /// Object evaluator
  template<typename T> inline int eval_obj(T* ptr, const std::string& str)  {
    return BasicGrammar::instance<T>().fromString(ptr,pre_parse_obj(str));
  }

  /// User object evaluator
  /// Do NOTHING version! Function present to formally satisfy code. User implementation required
  template<typename T> inline int eval_none(T*, const std::string&)  {
    return 1;
  }
  template <typename T> inline int parse_none(T&, const std::string&) {
    return 1;
  }

  // Containers of objects are not handled!
  
}      // End namespace dd4hep

#define DD4HEP_DEFINE_PARSER_GRAMMAR_EVAL(x,func)                       \
  namespace dd4hep {                                                    \
    template<> struct Grammar<x> : CommonGrammar<x> {                   \
      int evaluate(void* ptr, const std::string& val) const {           \
        return func ((x*)ptr,val);                                      \
      }                                                                 \
    };                                                                  \
  }

#if defined(DD4HEP_HAVE_ALL_PARSERS)
#define DD4HEP_DEFINE_PARSER_GRAMMAR_CONT_SERIAL(serial,x,eval_func)    \
  DD4HEP_DEFINE_PARSER_GRAMMAR_EVAL(x,eval_func)             \
  DD4HEP_DEFINE_PARSER_GRAMMAR_EVAL(std::list<x>,   eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_EVAL(std::set<x>,    eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_EVAL(std::deque<x>,  eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_EVAL(dd4hep::detail::Primitive<x>::int_map_t,     eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_EVAL(dd4hep::detail::Primitive<x>::ulong_map_t,   eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_EVAL(dd4hep::detail::Primitive<x>::string_map_t,  eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_EVAL(dd4hep::detail::Primitive<x>::int_pair_t,    eval_pair) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_EVAL(dd4hep::detail::Primitive<x>::ulong_pair_t,  eval_pair) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_EVAL(dd4hep::detail::Primitive<x>::string_pair_t, eval_pair) 

#define DD4HEP_DEFINE_PARSER_GRAMMAR_CONT_VL_SERIAL(serial,x,eval_func) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_EVAL(x,eval_func)            \
  DD4HEP_DEFINE_PARSER_GRAMMAR_EVAL(std::list<x>,eval_container)   

#define DD4HEP_DEFINE_PARSER_GRAMMAR_U_CONT_SERIAL(serial,x)            \
  DD4HEP_DEFINE_PARSER_GRAMMAR_CONT_SERIAL(serial,x,eval_item)          \
  DD4HEP_DEFINE_PARSER_GRAMMAR_CONT_SERIAL(serial,unsigned x,eval_item)

#else

#define DD4HEP_DEFINE_PARSER_GRAMMAR_CONT_SERIAL(serial,x,eval_func)    \
  DD4HEP_DEFINE_PARSER_GRAMMAR_EVAL(x,eval_func)             \
  DD4HEP_DEFINE_PARSER_GRAMMAR_EVAL(std::list<x>,   eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_EVAL(std::set<x>,    eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_EVAL(dd4hep::detail::Primitive<x>::int_map_t,     eval_container) \
    DD4HEP_DEFINE_PARSER_GRAMMAR_EVAL(dd4hep::detail::Primitive<x>::string_map_t,  eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_EVAL(dd4hep::detail::Primitive<x>::int_pair_t,    eval_pair) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_EVAL(dd4hep::detail::Primitive<x>::string_pair_t, eval_pair) 

#define DD4HEP_DEFINE_PARSER_GRAMMAR_CONT_VL_SERIAL(serial,x,eval_func) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_EVAL(x,eval_func)             \
  DD4HEP_DEFINE_PARSER_GRAMMAR_EVAL(std::list<x>,eval_container)   

#define DD4HEP_DEFINE_PARSER_GRAMMAR_U_CONT_SERIAL(serial,x)    \
  DD4HEP_DEFINE_PARSER_GRAMMAR_CONT_SERIAL(serial,x,eval_item)

#endif

#define DD4HEP_DEFINE_PARSER_GRAMMAR(x,func)              DD4HEP_DEFINE_PARSER_GRAMMAR_EVAL(x,func)
#define DD4HEP_DEFINE_PARSER_GRAMMAR_CONT(x,eval_func)    DD4HEP_DEFINE_PARSER_GRAMMAR_CONT_SERIAL(__LINE__,x,eval_func)
#define DD4HEP_DEFINE_PARSER_GRAMMAR_U_CONT(x)            DD4HEP_DEFINE_PARSER_GRAMMAR_U_CONT_SERIAL(__LINE__,x)
#define DD4HEP_DEFINE_PARSER_GRAMMAR_CONT_VL(x,eval_func) DD4HEP_DEFINE_PARSER_GRAMMAR_CONT_VL_SERIAL(__LINE__x,eval_func)

#endif  /* DD4HEP_DDCORE_BASICGRAMMAR_INL_H */
