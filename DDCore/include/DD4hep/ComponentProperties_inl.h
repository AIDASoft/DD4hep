// $Id$
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
#ifndef DD4HEP_DDG4_COMPONENTPROPERTIES_INL_H
#define DD4HEP_DDG4_COMPONENTPROPERTIES_INL_H

// Framework include files
#include "DD4hep/BasicGrammar.h"
#include "DD4hep/ComponentProperties.h"

// C/C++ include files
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <stdexcept>

/// DD4hep namespace declaration
namespace DD4hep {

  /// Setup property
  template <typename TYPE> void Property::make(TYPE& val) {
    static PropertyGrammar g(BasicGrammar::instance<TYPE>());
    m_hdl = &g;
    m_par = &val;
  }

  /// Set value of this property
  template <typename TYPE> void Property::set(const TYPE& val) {
    const PropertyGrammar& g = grammar();
    if (g.type() == typeid(TYPE))
      *(TYPE*) m_par = val;
    else if (!g.fromString(m_par, BasicGrammar::instance< TYPE >().str(&val)))
      PropertyGrammar::invalidConversion(typeid(TYPE), g.type());
  }

  /// Assignment operator / set new balue
  template <typename TYPE> Property& Property::operator=(const TYPE& val) {
    this->set(val);
    return *this;
  }

  /// Retrieve value from stack (large values e.g. vectors etc.)
  template <typename TYPE> void Property::value(TYPE& val) const {
    const PropertyGrammar& g = grammar();
    if (g.type() == typeid(TYPE))
      val = *(TYPE*) m_par;
    else if (!BasicGrammar::instance< TYPE >().fromString(&val, this->str()))
      PropertyGrammar::invalidConversion(g.type(), typeid(TYPE));
  }

  /// Retrieve value
  template <typename TYPE> TYPE Property::value() const {
    TYPE temp;
    this->value(temp);
    return temp;
  }

}      // End namespace DD4hep

// Instantiate single property
#define DD4HEP_DEFINE_PROPERTY_TYPE(x)                    \
  template x Property::value() const;                     \
  template void Property::value(x& value) const;          \
  template void Property::set(const x& value);            \
  template Property& Property::operator=(const x& value); \
  template void Property::make(x& value)

// Instantiate single property with support for STL containers
#define DD4HEP_DEFINE_PROPERTY_CONT(x)          \
  DD4HEP_DEFINE_PROPERTY_TYPE(x);               \
  DD4HEP_DEFINE_PROPERTY_TYPE(std::vector<x>);  \
  DD4HEP_DEFINE_PROPERTY_TYPE(std::list<x>);    \
  DD4HEP_DEFINE_PROPERTY_TYPE(std::set<x>)

#if defined(DD4HEP_HAVE_ALL_PARSERS)
////
//// Parser dictionaries get too large. Do not support unsigned stuff!
////
// Instantiate single property with support for STL containers + same for unsigned
#define DD4HEP_DEFINE_PROPERTY_U_CONT(x)        \
  DD4HEP_DEFINE_PROPERTY_CONT(x);               \
  DD4HEP_DEFINE_PROPERTY_CONT(unsigned x)

#else

#define DD4HEP_DEFINE_PROPERTY_U_CONT(x) DD4HEP_DEFINE_PROPERTY_CONT(x)

#endif // DD4HEP_HAVE_ALL_PARSERS
#endif // DD4HEP_DDG4_COMPONENTPROPERTIES_INL_H
