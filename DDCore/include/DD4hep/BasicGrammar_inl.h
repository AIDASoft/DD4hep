//==========================================================================
//  AIDA Detector description implementation for LCD
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
#include "DD4hep/Primitives.h"
#include "DD4hep/BasicGrammar.h"

#ifdef DD4HEP_USE_BOOST
#if defined(DD4HEP_PARSER_HEADER)

#define DD4HEP_NEED_EVALUATOR
// This is the case, if the parsers are externalized
// and the DD4hep namespace is renamed!
#include DD4HEP_PARSER_HEADER

#else

#include "XML/Evaluator.h"
#include "DD4hep/Parsers.h"
#include "DD4hep/ToStream.h"
namespace DD4hep { XmlTools::Evaluator& g4Evaluator();  }
#endif
#endif
namespace {  static XmlTools::Evaluator& s__eval(DD4hep::g4Evaluator());  }

// C/C++ include files
#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <deque>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Concrete type dependent grammar definition
  /**
   *   \author  M.Frank
   *   \date    13.08.2013
   *   \ingroup DD4HEP
   */
  template <typename TYPE> class Grammar : public BasicGrammar {
    /// Cached type information name
    std::string m_typeName;
  public:
    /// Standarsd constructor
    Grammar();
    /// Default destructor
    virtual ~Grammar();
    /// PropertyGrammar overload: Access to the type information
    virtual const std::type_info& type() const;
    /// Access to the type information name
    virtual const std::string& type_name() const;    
    /// Access the object size (sizeof operator)
    virtual size_t sizeOf() const;
    /// PropertyGrammar overload: Serialize a property to a string
    virtual std::string str(const void* ptr) const;
    /// PropertyGrammar overload: Retrieve value from string
    virtual bool fromString(void* ptr, const std::string& value) const;
    /// Evaluate string value if possible before calling boost::spirit
    virtual int evaluate(void* ptr, const std::string& value) const;
  };

  /// Standarsd constructor
  template <typename TYPE> Grammar<TYPE>::Grammar() {
    m_typeName = typeName(typeid(TYPE));
  }

  /// Default destructor
  template <typename TYPE> Grammar<TYPE>::~Grammar() {
  }

  /// PropertyGrammar overload: Access to the type information
  template <typename TYPE> const std::type_info& Grammar<TYPE>::type() const {
    return typeid(TYPE);
  }

  /// PropertyGrammar overload: Access to the type information
  template <typename TYPE> const std::string& Grammar<TYPE>::type_name() const {
    return m_typeName;
  }

  /// Access the object size (sizeof operator)
  template <typename TYPE> size_t Grammar<TYPE>::sizeOf() const   {
    return sizeof(TYPE);
  }

  /// Evaluate string value if possible before calling boost::spirit
  template <typename TYPE> int Grammar<TYPE>::evaluate(void*, const std::string&) const {
    return 0;
  }

  /// PropertyGrammar overload: Retrieve value from string
  template <typename TYPE> bool Grammar<TYPE>::fromString(void* ptr, const std::string& string_val) const {
    int sc = 0;
    TYPE temp;
#ifdef DD4HEP_USE_BOOST
    sc = Parsers::parse(temp,string_val);
#endif
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
#ifndef DD4HEP_USE_BOOST
    throw std::runtime_error("This version of DD4HEP is not compiled to use boost::spirit.\n"
                             "To enable elaborated property handling set DD4HEP_USE_BOOST=ON\n"
                             "and BOOST_INCLUDE_DIR=<boost include path>");
#else
    BasicGrammar::invalidConversion(string_val, typeid(TYPE));
    return false;
#endif
  }

  /// Serialize a property to a string
  template <typename TYPE> std::string Grammar<TYPE>::str(const void* ptr) const {
#ifdef DD4HEP_USE_BOOST
    std::stringstream string_rep;
    Utils::toStream(*(TYPE*)ptr,string_rep);
    return string_rep.str();
#else
    if (ptr) {
    }
    throw std::runtime_error("This version of DD4HEP is not compiled to use boost::spirit.\n"
                             "To enable elaborated property handling set DD4HEP_USE_BOOST=ON\n"
                             "and BOOST_INCLUDE_DIR=<boost include path>");
#endif
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
    const BasicGrammar& g = BasicGrammar::instance<TYPE>();
    for(auto i=std::begin(temp); i != std::end(temp); ++i)  {
      if ( !g.fromString(&val,*i) )
        return 0;
      p->push_back(val);
    }
    return 1;
  }

  /// Insertion function for std lists
  template <typename TYPE> static int fill_data(std::list<TYPE>* p,const std::vector<std::string>& temp)  {
    TYPE val;
    const BasicGrammar& g = BasicGrammar::instance<TYPE>();
    for(auto i=std::begin(temp); i != std::end(temp); ++i)  {
      if ( !g.fromString(&val,*i) )
        return 0;
      p->push_back(val);
    }
    return 1;
  }

  /// Insertion function for std sets
  template <typename TYPE> static int fill_data(std::set<TYPE>* p,const std::vector<std::string>& temp)  {
    TYPE val;
    const BasicGrammar& g = BasicGrammar::instance<TYPE>();
    for(auto i=std::begin(temp); i != std::end(temp); ++i)  {
      if ( !g.fromString(&val,*i) )
        return 0;
      p->insert(val);
    }
    return 1;
  }

  /// Insertion function for std sets
  template <typename TYPE> static int fill_data(std::deque<TYPE>* p,const std::vector<std::string>& temp)  {
    TYPE val;
    const BasicGrammar& g = BasicGrammar::instance<TYPE>();
    for(auto i=std::begin(temp); i != std::end(temp); ++i)  {
      if ( !g.fromString(&val,*i) )
        return 0;
      p->push_back(val);
    }
    return 1;
  }

  /// Insertion function for std sets
  template <typename KEY, typename TYPE> static int fill_data(std::map<KEY,TYPE>* p,const std::vector<std::string>& temp)  {
    std::pair<KEY,TYPE> val;
    const BasicGrammar& g = BasicGrammar::instance<std::pair<KEY,TYPE> >();
    for(auto i=std::begin(temp); i != std::end(temp); ++i)  {
      if ( !g.fromString(&val,*i) )
        return 0;
      p->insert(val);
    }
    return 1;
  }

  /// Container evaluator
  template <typename TYPE> static int eval_container(TYPE* p, const std::string& str)  {
#ifdef DD4HEP_USE_BOOST
    std::vector<std::string> buff;
    int sc = Parsers::parse(buff,str);
    if ( sc )  {
      return fill_data(p,buff);
    }
    else   {
      TYPE temp;
      std::string temp_str = pre_parse_obj(str);
      sc = Parsers::parse(temp,temp_str);
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
#else
    if ( p && str.empty() ) return 0;
#endif
    return 0;
  }

  /// Item evaluator
  template <typename T> inline int eval_item(T* p, std::string s)  {
    size_t idx = s.find("(int)");
    if (idx != std::string::npos)
      s.erase(idx, 5);
    while (s[0] == ' ')
      s.erase(0, 1);
#ifdef DD4HEP_USE_BOOST
    double result = s__eval.evaluate(s.c_str());
    if (s__eval.status() != XmlTools::Evaluator::OK) {
      return 0;
    }
    *p = (T)result;
    return 1;
#else
    return 0;
#endif
  }

  /// String evaluator
  template <> inline int eval_item<std::string>(std::string* p, std::string s)  {
    *p = s;
    return 1;
  }

  /// Item evaluator
  template <typename T,typename Q> inline int eval_pair(std::pair<T,Q>* p, std::string s)  {
    const BasicGrammar& g = BasicGrammar::instance<std::pair<T,Q> >();
    if ( !g.fromString(p,s) )  return 0;
    return 1;
  }

  /// Object evaluator
  template<typename T> inline int eval_obj(T* p, const std::string& str)  {
    return BasicGrammar::instance<T>().fromString(p,pre_parse_obj(str));
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
  
}      // End namespace DD4hep

#define DD4HEP_DEFINE_PARSER_GRAMMAR_TYPE(x)                            \
  namespace DD4hep {                                                    \
    template<> const BasicGrammar& BasicGrammar::instance<x>()  { static Grammar<x> s; return s;}}

#define DD4HEP_DEFINE_PARSER_GRAMMAR_EVAL(x,func)                       \
  namespace DD4hep {                                                    \
    template<> int Grammar<x >::evaluate(void* p, const std::string& v) const { return func ((x*)p,v); }}

#define DD4HEP_DEFINE_PARSER_GRAMMAR(x,func)    \
  DD4HEP_DEFINE_PARSER_GRAMMAR_TYPE(x)          \
  DD4HEP_DEFINE_PARSER_GRAMMAR_EVAL(x,func)

#if defined(DD4HEP_HAVE_ALL_PARSERS)
#define DD4HEP_DEFINE_PARSER_GRAMMAR_CONT(x,eval_func)                  \
  DD4HEP_DEFINE_PARSER_GRAMMAR(x,eval_func)                             \
  DD4HEP_DEFINE_PARSER_GRAMMAR(std::vector<x>, eval_container)          \
  DD4HEP_DEFINE_PARSER_GRAMMAR(std::list<x>,   eval_container)          \
  DD4HEP_DEFINE_PARSER_GRAMMAR(std::set<x>,    eval_container)          \
  DD4HEP_DEFINE_PARSER_GRAMMAR(std::deque<x>,  eval_container)          \
  DD4HEP_DEFINE_PARSER_GRAMMAR(DD4hep::Primitive<x>::int_map_t,     eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR(DD4hep::Primitive<x>::ulong_map_t,   eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR(DD4hep::Primitive<x>::string_map_t,  eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR(DD4hep::Primitive<x>::int_pair_t,    eval_pair) \
  DD4HEP_DEFINE_PARSER_GRAMMAR(DD4hep::Primitive<x>::ulong_pair_t,  eval_pair) \
  DD4HEP_DEFINE_PARSER_GRAMMAR(DD4hep::Primitive<x>::string_pair_t, eval_pair) 

#define DD4HEP_DEFINE_PARSER_GRAMMAR_CONT_VL(x,eval_func)     \
  DD4HEP_DEFINE_PARSER_GRAMMAR(x,eval_func)                   \
  DD4HEP_DEFINE_PARSER_GRAMMAR(std::vector<x>,eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR(std::list<x>,eval_container)   

#define DD4HEP_DEFINE_PARSER_GRAMMAR_U_CONT(x)            \
  DD4HEP_DEFINE_PARSER_GRAMMAR_CONT(x,eval_item)          \
  DD4HEP_DEFINE_PARSER_GRAMMAR_CONT(unsigned x,eval_item)

#else

#define DD4HEP_DEFINE_PARSER_GRAMMAR_CONT(x,eval_func)                  \
  DD4HEP_DEFINE_PARSER_GRAMMAR(x,eval_func)                             \
  DD4HEP_DEFINE_PARSER_GRAMMAR(std::vector<x>, eval_container)          \
  DD4HEP_DEFINE_PARSER_GRAMMAR(std::list<x>,   eval_container)          \
  DD4HEP_DEFINE_PARSER_GRAMMAR(std::set<x>,    eval_container)          \
  DD4HEP_DEFINE_PARSER_GRAMMAR(DD4hep::Primitive<x>::int_map_t,     eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR(DD4hep::Primitive<x>::string_map_t,  eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR(DD4hep::Primitive<x>::int_pair_t,    eval_pair) \
  DD4HEP_DEFINE_PARSER_GRAMMAR(DD4hep::Primitive<x>::string_pair_t, eval_pair) 

#define DD4HEP_DEFINE_PARSER_GRAMMAR_CONT_VL(x,eval_func)     \
  DD4HEP_DEFINE_PARSER_GRAMMAR(x,eval_func)                   \
  DD4HEP_DEFINE_PARSER_GRAMMAR(std::vector<x>,eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR(std::list<x>,eval_container)   

#define DD4HEP_DEFINE_PARSER_GRAMMAR_U_CONT(x)    \
  DD4HEP_DEFINE_PARSER_GRAMMAR_CONT(x,eval_item)

#endif

#endif  /* DD4HEP_DDCORE_BASICGRAMMAR_INL_H */
