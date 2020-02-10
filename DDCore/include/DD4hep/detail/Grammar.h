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
#ifndef DD4HEP_DDCORE_DETAIL_GRAMMAR_H
#define DD4HEP_DDCORE_DETAIL_GRAMMAR_H

// Framework include files
#include "DD4hep/BasicGrammar.h"
#include "Parsers/spirit/Parsers.h"
#include "Parsers/spirit/ToStream.h"

#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <deque>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Concrete type dependent grammar definition
  /**
   *   \author  M.Frank
   *   \date    13.08.2013
   *   \ingroup DD4HEP
   */
  template <typename TYPE> struct CommonGrammar : BasicGrammar {
    CommonGrammar() : BasicGrammar(typeName(typeid(TYPE))) {}
    /// PropertyGrammar overload: Access to the type information
    virtual const std::type_info& type() const  override { return typeid(TYPE); }
    /// Access to the type information
    virtual bool equals(const std::type_info& other_type) const  override { return other_type == typeid(TYPE); }
    /// Access the object size (sizeof operator)
    virtual size_t sizeOf() const  override { return sizeof(TYPE); }
    /// PropertyGrammar overload: Serialize a property to a string
    virtual std::string str(const void* ptr) const  override;
    /// PropertyGrammar overload: Retrieve value from string
    virtual bool fromString(void* ptr, const std::string& value) const  override;
    /// Opaque object destructor
    virtual void destruct(void* pointer) const  override;
    /// Opaque object copy construction. Memory must be allocated externally
    virtual void copy(void* to, const void* from)  const  override;
    /// Bind opaque address to object
    virtual void bind(void* pointer)  const override { new(pointer) TYPE(); }
    /// Evaluate string value if possible before calling boost::spirit
    virtual int evaluate(void* ptr, const std::string& value) const = 0;
  };
}

/// PropertyGrammar overload: Retrieve value from string
template <typename TYPE> bool dd4hep::CommonGrammar<TYPE>::fromString(void* ptr, const std::string& string_val) const {
  int sc = 0;
  TYPE temp;
  sc = dd4hep::Parsers::parse(temp,string_val);
  if ( !sc ) sc = evaluate(&temp,string_val);
  if ( sc )   {
    *(TYPE*)ptr = temp;
    return true;
  }
  BasicGrammar::invalidConversion(string_val, typeid(TYPE));
  return false;
}

/// Serialize a property to a string
template <typename TYPE> std::string dd4hep::CommonGrammar<TYPE>::str(const void* ptr) const {
  std::stringstream string_rep;
  Utils::toStream(*(TYPE*)ptr,string_rep);
  return string_rep.str();
}

/// Opaque object destructor
template <typename TYPE> void dd4hep::CommonGrammar<TYPE>::destruct(void* pointer) const   {
  TYPE* obj = (TYPE*)pointer;
  obj->~TYPE();
}

/// Opaque object destructor
template <typename TYPE> void dd4hep::CommonGrammar<TYPE>::copy(void* to, const void* from) const   {
  const TYPE* from_obj = (const TYPE*)from;
  new (to) TYPE(*from_obj);
}

namespace dd4hep {

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

  template <typename TYPE> struct Grammar : CommonGrammar<TYPE> {
    int evaluate(void*, const std::string&) const override { return 0; };
  };

  /// specialization of Grammar for containers
  template <typename T> struct ContainerGrammar : CommonGrammar<T> {
    int evaluate(void* ptr, const std::string& val) const override {
      return eval_container((T*)ptr,val);
    }
  };

  /// specialization of Grammar for STL containers
  template <typename T> struct Grammar<std::vector<T>> : ContainerGrammar<std::vector<T>> {};
  template <typename T> struct Grammar<std::list<T>> : ContainerGrammar<std::list<T>> {};
  template <typename T> struct Grammar<std::set<T>> : ContainerGrammar<std::set<T>> {};
  template <typename T> struct Grammar<std::deque<T>> : ContainerGrammar<std::deque<T>> {};
  template <typename T, typename V> struct Grammar<std::map<T,V>> : ContainerGrammar<std::map<T,V>> {};

  /// specialization of Grammar for STL pairs
  template <typename T1, typename T2> struct Grammar<std::pair<T1,T2>> : CommonGrammar<std::pair<T1,T2>> {
    int evaluate(void* ptr, const std::string& val) const override {
      const BasicGrammar& grammar = BasicGrammar::instance<std::pair<T1,T2> >();
      if ( !grammar.fromString((std::pair<T1,T2>*)ptr, val) )  return 0;
      return 1;
    }
  };

} // namespace dd4hep

/// default empty parsing function for types not having a specialization
namespace dd4hep {
  namespace Parsers {
    template<typename TYPE>
    int parse(TYPE&, const std::string&) {
      return 1;
    }
  }
}

/// default intanciation of BasicGrammar, using Grammar concrete class
template<typename TYPE> const dd4hep::BasicGrammar& dd4hep::BasicGrammar::instance() {
  static dd4hep::Grammar<TYPE> gr;
  return gr;
}
#endif  /* DD4HEP_DDCORE_DETAIL_GRAMMAR_H */
