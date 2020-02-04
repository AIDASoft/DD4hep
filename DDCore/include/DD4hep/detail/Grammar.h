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

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Concrete type dependent grammar definition
  /**
   *   \author  M.Frank
   *   \date    13.08.2013
   *   \ingroup DD4HEP
   */
  template <typename TYPE> class Grammar : public BasicGrammar {
  private:
    
    friend class BasicGrammar;
    /// Default destructor
    virtual ~Grammar() {};
    /// Standard constructor
    Grammar() : BasicGrammar(typeName(typeid(TYPE))) {}


  public:

    /** Base class overrides   */
    /// PropertyGrammar overload: Access to the type information
    virtual const std::type_info& type() const  override;
    /// Access to the type information
    virtual bool equals(const std::type_info& other_type) const  override;
    /// Access the object size (sizeof operator)
    virtual size_t sizeOf() const  override;
    /// PropertyGrammar overload: Serialize a property to a string
    virtual std::string str(const void* ptr) const  override;
    /// PropertyGrammar overload: Retrieve value from string
    virtual bool fromString(void* ptr, const std::string& value) const  override;
    /// Opaque object destructor
    virtual void destruct(void* pointer) const  override;
    /// Opaque object copy construction. Memory must be allocated externally
    virtual void copy(void* to, const void* from)  const  override;
    /// Bind opaque address to object
    virtual void bind(void* pointer)  const override;

    /** Class member function   */
    /// Evaluate string value if possible before calling boost::spirit
    virtual int evaluate(void* ptr, const std::string& value) const;
  };
}

/// PropertyGrammar overload: Access to the type information
template <typename TYPE> const std::type_info& dd4hep::Grammar<TYPE>::type() const {
  return typeid(TYPE);
}

/// Access to the type information
template <typename TYPE> bool dd4hep::Grammar<TYPE>::equals(const std::type_info& other_type) const  {
  return other_type == typeid(TYPE);
}

/// Access the object size (sizeof operator)
template <typename TYPE> size_t dd4hep::Grammar<TYPE>::sizeOf() const   {
  return sizeof(TYPE);
}

/// Bind opaque address to object
template <typename TYPE> void dd4hep::Grammar<TYPE>::bind(void* pointer)  const  {
  new(pointer) TYPE();
}

/// Evaluate string value if possible before calling boost::spirit
template <typename TYPE> int dd4hep::Grammar<TYPE>::evaluate(void*, const std::string&) const {
  return 0;
}

/// default empty parsing function for types not having a specialization
namespace dd4hep {
  namespace Parsers {
    template<typename TYPE>
    int parse(TYPE&, const std::string&) {
      return 1;
    }
  }
}

/// PropertyGrammar overload: Retrieve value from string
template <typename TYPE> bool dd4hep::Grammar<TYPE>::fromString(void* ptr, const std::string& string_val) const {
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
template <typename TYPE> std::string dd4hep::Grammar<TYPE>::str(const void* ptr) const {
  std::stringstream string_rep;
  Utils::toStream(*(TYPE*)ptr,string_rep);
  return string_rep.str();
}

/// Opaque object destructor
template <typename TYPE> void dd4hep::Grammar<TYPE>::destruct(void* pointer) const   {
  TYPE* obj = (TYPE*)pointer;
  obj->~TYPE();
}

/// Opaque object destructor
template <typename TYPE> void dd4hep::Grammar<TYPE>::copy(void* to, const void* from) const   {
  const TYPE* from_obj = (const TYPE*)from;
  new (to) TYPE(*from_obj);
}

/// default intanciation of BasicGrammar, using Grammar concrete class
template<typename TYPE> const dd4hep::BasicGrammar& dd4hep::BasicGrammar::instance() {
  static dd4hep::Grammar<TYPE> gr;
  return gr;
}
#endif  /* DD4HEP_DDCORE_DETAIL_GRAMMAR_H */
