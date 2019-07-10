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

  /// Standarsd constructor
  template <typename TYPE> Grammar<TYPE>::Grammar() : BasicGrammar(typeName(typeid(TYPE)))  {
  }

  /// Default destructor
  template <typename TYPE> Grammar<TYPE>::~Grammar() {
  }

  /// PropertyGrammar overload: Access to the type information
  template <typename TYPE> const std::type_info& Grammar<TYPE>::type() const {
    return typeid(TYPE);
  }

  /// Access to the type information
  template <typename TYPE> bool Grammar<TYPE>::equals(const std::type_info& other_type) const  {
    return other_type == typeid(TYPE);
  }
  
  /// Access the object size (sizeof operator)
  template <typename TYPE> size_t Grammar<TYPE>::sizeOf() const   {
    return sizeof(TYPE);
  }

  /// Bind opaque address to object
  template <typename TYPE> void Grammar<TYPE>::bind(void* pointer)  const  {
    new(pointer) TYPE();
  }

  /// Evaluate string value if possible before calling boost::spirit
  template <typename TYPE> int Grammar<TYPE>::evaluate(void*, const std::string&) const {
    return 0;
  }

  /// PropertyGrammar overload: Retrieve value from string
  template <typename TYPE> bool Grammar<TYPE>::fromString(void* ptr, const std::string& string_val) const {
    int sc = 0;
    TYPE temp;
    sc = ::dd4hep::Parsers::parse(temp,string_val);
    if ( !sc ) sc = evaluate(&temp,string_val);
#if 0
    std::cout << "Sc=" << sc << "  Converting value: " << string_val 
              << " to type " << typeid(TYPE).name() 
              << std::endl;
#endif
    if ( sc )   {
      *(TYPE*)ptr = temp;
      return true;
    }
    BasicGrammar::invalidConversion(string_val, typeid(TYPE));
    return false;
  }

  /// Serialize a property to a string
  template <typename TYPE> std::string Grammar<TYPE>::str(const void* ptr) const {
    std::stringstream string_rep;
    Utils::toStream(*(TYPE*)ptr,string_rep);
    return string_rep.str();
  }

  /// Opaque object destructor
  template <typename TYPE> void Grammar<TYPE>::destruct(void* pointer) const   {
    TYPE* obj = (TYPE*)pointer;
    obj->~TYPE();
  }

  /// Opaque object destructor
  template <typename TYPE> void Grammar<TYPE>::copy(void* to, const void* from) const   {
    const TYPE* from_obj = (const TYPE*)from;
    new (to) TYPE(*from_obj);
  }

  /// Helper function to parse data type
  static inline std::string pre_parse_obj(const std::string& in)   {
    std::string res = "";
    res.reserve(1024);
    for(const char* c = in.c_str(); *c; ++c)   {
      switch(*c)  {
      case '\'':
        return "Bad object representation";
      case ',':
        res += "','";
        break;
      case '(':
      case '[':
        res += "['";
        break;
      case ')':
      case ']':
        res += "']";
        break;
      default:
        res += *c;
        break;
      }
    }
    //cout << "Pre-parsed:" << res << endl;
    return res;
  }

  /// Insertion function for std vectors
  template <typename TYPE> static int fill_data(std::vector<TYPE>* p,const std::vector<std::string>& temp)  {
    TYPE val;
    const BasicGrammar& grammar = BasicGrammar::instance<TYPE>();
    for(auto i=std::begin(temp); i != std::end(temp); ++i)  {
      if ( !grammar.fromString(&val,*i) )
        return 0;
      p->emplace_back(val);
    }
    return 1;
  }

  /// Insertion function for std lists
  template <typename TYPE> static int fill_data(std::list<TYPE>* p,const std::vector<std::string>& temp)  {
    TYPE val;
    const BasicGrammar& grammar = BasicGrammar::instance<TYPE>();
    for(auto i=std::begin(temp); i != std::end(temp); ++i)  {
      if ( !grammar.fromString(&val,*i) )
        return 0;
      p->emplace_back(val);
    }
    return 1;
  }

  /// Insertion function for std sets
  template <typename TYPE> static int fill_data(std::set<TYPE>* p,const std::vector<std::string>& temp)  {
    TYPE val;
    const BasicGrammar& grammar = BasicGrammar::instance<TYPE>();
    for(auto i=std::begin(temp); i != std::end(temp); ++i)  {
      if ( !grammar.fromString(&val,*i) )
        return 0;
      p->emplace(val);
    }
    return 1;
  }

  /// Insertion function for std sets
  template <typename TYPE> static int fill_data(std::deque<TYPE>* p,const std::vector<std::string>& temp)  {
    TYPE val;
    const BasicGrammar& grammar = BasicGrammar::instance<TYPE>();
    for(auto i=std::begin(temp); i != std::end(temp); ++i)  {
      if ( !grammar.fromString(&val,*i) )
        return 0;
      p->emplace_back(val);
    }
    return 1;
  }

  /// Insertion function for std sets
  template <typename KEY, typename TYPE> static int fill_data(std::map<KEY,TYPE>* p,const std::vector<std::string>& temp)  {
    std::pair<KEY,TYPE> val;
    const BasicGrammar& grammar = BasicGrammar::instance<std::pair<KEY,TYPE> >();
    for(auto i=std::begin(temp); i != std::end(temp); ++i)  {
      if ( !grammar.fromString(&val,*i) )
        return 0;
      p->emplace(val);
    }
    return 1;
  }

  /// Container evaluator
  template <typename TYPE> static int eval_container(TYPE* p, const std::string& str)  {
    std::vector<std::string> buff;
    int sc = Parsers::parse(buff,str);
    if ( sc )  {
      return fill_data(p,buff);
    }
    else   {
      TYPE temp;
      std::string temp_str = pre_parse_obj(str);
      sc = ::dd4hep::Parsers::parse(temp,temp_str);
      if ( sc )   {
        *p = temp;
        return 1;
      }
      buff.clear();
      sc = Parsers::parse(buff,temp_str);
      if ( sc )  {
        return fill_data(p,buff);
      }
    }
    return 0;
  }

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
#define DD4HEP_PARSER_GRAMMAR_CNAME(serial,name)  namespace_dd4hep__grammar_##serial##_##name

#define DD4HEP_DEFINE_PARSER_GRAMMAR_TYPE(x)                            \
  namespace dd4hep {                                                    \
    template<> const BasicGrammar& BasicGrammar::instance<x>()  { static Grammar<x> gr; return gr;}}

#define DD4HEP_DEFINE_PARSER_GRAMMAR_EVAL(x,func)                       \
  namespace dd4hep {                                                    \
    template<> int Grammar<x >::evaluate(void* ptr, const std::string& val) const { return func ((x*)ptr,val); }}

#define DD4HEP_DEFINE_PARSER_GRAMMAR_INSTANCE(serial,x) namespace DD4HEP_PARSER_GRAMMAR_CNAME(serial,0) { \
    static const ::dd4hep::BasicGrammar& DD4HEP_PARSER_GRAMMAR_CNAME(serial,100) = ::dd4hep::BasicGrammar::instance<x>();  }

#define DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,ctxt,x,func)         \
  DD4HEP_DEFINE_PARSER_GRAMMAR_TYPE(x)                                  \
  DD4HEP_DEFINE_PARSER_GRAMMAR_EVAL(x,func)                             \
  DD4HEP_DEFINE_PARSER_GRAMMAR_INSTANCE(DD4HEP_PARSER_GRAMMAR_CNAME(serial,ctxt),x)

#define DD4HEP_DEFINE_PARSER_GRAMMAR_DUMMY_SERIAL(serial,x,func)  \
  PARSERS_DECL_FOR_SINGLE(x)                                      \
  namespace dd4hep   {   namespace Parsers   {                    \
      int parse(x&, const std::string&)     {  return 1;  }    }} \
  DD4HEP_DEFINE_PARSER_GRAMMAR_TYPE(x)                            \
  DD4HEP_DEFINE_PARSER_GRAMMAR_EVAL(x,func)                       \
  DD4HEP_DEFINE_PARSER_GRAMMAR_INSTANCE(serial,x)

#if defined(DD4HEP_HAVE_ALL_PARSERS)
#define DD4HEP_DEFINE_PARSER_GRAMMAR_CONT_SERIAL(serial,x,eval_func)    \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,1,x,eval_func)             \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,2,std::vector<x>, eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,3,std::list<x>,   eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,4,std::set<x>,    eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,5,std::deque<x>,  eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,6,dd4hep::detail::Primitive<x>::int_map_t,     eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,7,dd4hep::detail::Primitive<x>::ulong_map_t,   eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,8,dd4hep::detail::Primitive<x>::string_map_t,  eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,9,dd4hep::detail::Primitive<x>::int_pair_t,    eval_pair) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,10,dd4hep::detail::Primitive<x>::ulong_pair_t,  eval_pair) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,11,dd4hep::detail::Primitive<x>::string_pair_t, eval_pair) 

#define DD4HEP_DEFINE_PARSER_GRAMMAR_CONT_VL_SERIAL(serial,x,eval_func) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,12,x,eval_func)            \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,13,std::vector<x>,eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,14,std::list<x>,eval_container)   

#define DD4HEP_DEFINE_PARSER_GRAMMAR_U_CONT_SERIAL(serial,x)            \
  DD4HEP_DEFINE_PARSER_GRAMMAR_CONT_SERIAL(serial,x,eval_item)          \
  DD4HEP_DEFINE_PARSER_GRAMMAR_CONT_SERIAL(serial,unsigned x,eval_item)

#else

#define DD4HEP_DEFINE_PARSER_GRAMMAR_CONT_SERIAL(serial,x,eval_func)    \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,1,x,eval_func)             \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,2,std::vector<x>, eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,3,std::list<x>,   eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,4,std::set<x>,    eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,5,dd4hep::detail::Primitive<x>::int_map_t,     eval_container) \
    DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,6,dd4hep::detail::Primitive<x>::string_map_t,  eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,7,dd4hep::detail::Primitive<x>::int_pair_t,    eval_pair) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,8,dd4hep::detail::Primitive<x>::string_pair_t, eval_pair) 

#define DD4HEP_DEFINE_PARSER_GRAMMAR_CONT_VL_SERIAL(serial,x,eval_func) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,9,x,eval_func)             \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,10,std::vector<x>,eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,11,std::list<x>,eval_container)   

#define DD4HEP_DEFINE_PARSER_GRAMMAR_U_CONT_SERIAL(serial,x)    \
  DD4HEP_DEFINE_PARSER_GRAMMAR_CONT_SERIAL(serial,x,eval_item)

#endif

#define DD4HEP_DEFINE_PARSER_GRAMMAR(x,func)              DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(__LINE__,__LINE__,x,func)
#define DD4HEP_DEFINE_PARSER_GRAMMAR_CONT(x,eval_func)    DD4HEP_DEFINE_PARSER_GRAMMAR_CONT_SERIAL(__LINE__,x,eval_func)
#define DD4HEP_DEFINE_PARSER_GRAMMAR_U_CONT(x)            DD4HEP_DEFINE_PARSER_GRAMMAR_U_CONT_SERIAL(__LINE__,x)
#define DD4HEP_DEFINE_PARSER_GRAMMAR_CONT_VL(x,eval_func) DD4HEP_DEFINE_PARSER_GRAMMAR_CONT_VL_SERIAL(__LINE__x,eval_func)
#define DD4HEP_DEFINE_PARSER_GRAMMAR_DUMMY(x,func)        DD4HEP_DEFINE_PARSER_GRAMMAR_DUMMY_SERIAL(__LINE__,x,func)

#endif  /* DD4HEP_DDCORE_BASICGRAMMAR_INL_H */
