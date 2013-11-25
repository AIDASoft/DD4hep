// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_COMPONENTPROPERTIES_INL_H
#define DD4HEP_DDG4_COMPONENTPROPERTIES_INL_H

// Framework include files
#include "DDG4/ComponentProperties.h"

#ifdef DD4HEP_USE_BOOST
#include "DDG4/Parsers.h"
#include "DDG4/ToStream.h"
#endif

// C/C++ include files
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <stdexcept>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /** @class PropertyGrammarCollection
   *
   *   @author  M.Frank
   *   @date    13.08.2013
   */
  template <typename TYPE> struct Grammar : public PropertyGrammar {
    /// Standarsd constructor
    Grammar();
    /// Default destructor
    virtual ~Grammar();
    /// Singleton instantiator
    static const Grammar& instance();
    /// PropertyGrammar overload: Access to the type information
    virtual const std::type_info& type() const;
    /// PropertyGrammar overload: Serialize a property to a string
    virtual std::string str(const void* ptr) const;
    /// PropertyGrammar overload: Retrieve value from string
    virtual bool fromString(void* ptr, const std::string& value) const;
  };

  /// Standarsd constructor
  template <typename TYPE> Grammar<TYPE>::Grammar() {
  }

  /// Default destructor
  template <typename TYPE> Grammar<TYPE>::~Grammar() {
  }

  /// Access concrete Grammar object
  template <class TYPE> const Grammar<TYPE>& Grammar<TYPE>::instance() {
    static Grammar<TYPE> s;
    return s;
  }

  /// PropertyGrammar overload: Access to the type information
  template <typename TYPE> const std::type_info& Grammar<TYPE>::type() const {
    return typeid(TYPE);
  }
  /// PropertyGrammar overload: Retrieve value from string
  template <typename TYPE> bool Grammar<TYPE>::fromString(void* ptr, const std::string& str) const {
#ifdef DD4HEP_USE_BOOST
    TYPE temp;
    int sc = Parsers::parse(temp,str);
    //std::cout << "Converting value: " << str << " to type " << typeid(TYPE).name() << std::endl;
    if ( sc ) {
      *(TYPE*)ptr = temp;
      return true;
    }
    PropertyGrammar::invalidConversion(str, typeid(TYPE));
    return false;
#else
    if (!ptr || str.length() == 0) {
    }
    throw std::runtime_error("This version of DD4HEP is not compiled to use boost::spirit.\n"
        "To enable elaborated property handling set DD4HEP_USE_BOOST=ON\n"
        "and BOOST_INCLUDE_DIR=<boost include path>");
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

  /// Setup property
  template <typename TYPE> void Property::make(TYPE& val) {
    m_hdl = &Grammar < TYPE > ::instance();
    m_par = &val;
  }

  /// Set value of this property
  template <typename TYPE> void Property::set(const TYPE& value) {
    const PropertyGrammar& g = grammar();
    if (g.type() == typeid(TYPE))
      *(TYPE*) m_par = value;
    else if (!g.fromString(m_par, Grammar < TYPE > ::instance().str(&value)))
      PropertyGrammar::invalidConversion(typeid(TYPE), g.type());
  }

  /// Assignment operator / set new balue
  template <typename TYPE> Property& Property::operator=(const TYPE& val) {
    this->set(val);
    return *this;
  }

  /// Retrieve value from stack (large values e.g. vectors etc.)
  template <typename TYPE> void Property::value(TYPE& value) const {
    const PropertyGrammar& g = grammar();
    if (g.type() == typeid(TYPE))
      value = *(TYPE*) m_par;
    else if (!Grammar < TYPE > ::instance().fromString(&value, this->str()))
      PropertyGrammar::invalidConversion(g.type(), typeid(TYPE));
  }

  /// Retrieve value
  template <typename TYPE> TYPE Property::value() const {
    TYPE temp;
    this->value(temp);
    return temp;
  }

#define DD4HEP_INSTANTIATE_PROPERTY_TYPE(x)				    \
  template x Property::value() const;                                       \
  template void Property::value(x& value) const;                            \
  template void Property::set(const x& value);                              \
  template Property& Property::operator=(const x& value);                   \
  template void Property::make(x& value)

}      // End namespace DD4hep

#endif // DD4HEP_DDG4_COMPONENTPROPERTIES_INL_H
