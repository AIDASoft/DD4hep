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

// Framework include files
#include "DD4hep/Printout.h"
#include "DD4hep/Primitives.h"
#include "Parsers/Parsers.h"
#include "DD4hep/ComponentProperties.h"

// C/C++ include files
#include <stdexcept>
#include <cstring>

using namespace std;
using namespace dd4hep;

/// Default destructor
PropertyConfigurator::~PropertyConfigurator()   {
}

/// Default constructor
PropertyGrammar::PropertyGrammar(const BasicGrammar& g) : m_grammar(g) {
}

/// Default destructor
PropertyGrammar::~PropertyGrammar() {
}

/// Error callback on invalid conversion
void PropertyGrammar::invalidConversion(const std::type_info& from,
                                        const std::type_info& to)
{
  BasicGrammar::invalidConversion(from,to);
}

/// Error callback on invalid conversion
void PropertyGrammar::invalidConversion(const std::string& value,
                                        const std::type_info& to)
{
  BasicGrammar::invalidConversion(value,to);
}

/// Access to the type information
const std::type_info& PropertyGrammar::type() const  {
  return m_grammar.type();
}

/// Serialize an opaque value to a string
std::string PropertyGrammar::str(const void* ptr) const  {
  return m_grammar.str(ptr);
}

/// Set value from serialized string. On successful data conversion TRUE is returned.
bool PropertyGrammar::fromString(void* ptr, const std::string& value) const  {
  return m_grammar.fromString(ptr,value);
}

/// Property type name
string Property::type(const Property& property) {
  return type(property.grammar().type());
}

/// Property type name
string Property::type(const type_info& typ) {
  return typeName(typ);
}

/// Property type name
string Property::type() const {
  return Property::type(grammar().type());
}

const PropertyGrammar& Property::grammar() const {
  if ( m_hdl )
    return *m_hdl;
  throw runtime_error("Attempt to access property grammar from invalid object.");
}

/// Conversion to string value
string Property::str() const {
  if ( m_hdl && m_par ) {
    return m_hdl->str(m_par);
  }
  throw runtime_error("Attempt to access property grammar from invalid object.");
}

/// Conversion from string value
const Property& Property::str(const std::string& input)   const {
  if ( m_hdl && m_par )   {
    m_hdl->fromString(m_par,input);
    return *this;
  }
  throw runtime_error("Attempt to access property grammar from invalid object.");
}

/// Conversion from string value
Property& Property::str(const std::string& input)    {
  if ( m_hdl && m_par )   {
    m_hdl->fromString(m_par,input);
    return *this;
  }
  throw runtime_error("Attempt to access property grammar from invalid object.");
}

/// Assignment operator / set new balue
Property& Property::operator=(const char* val) {
  if ( val ) {
    this->set < string > (val);
    return *this;
  }
  throw runtime_error("Attempt to set invalid string to property!");
}

/// Default constructor
PropertyManager::PropertyManager() {
}

/// Default destructor
PropertyManager::~PropertyManager() {
  m_properties.clear();
}

/// Access total number of properties
size_t PropertyManager::size()  const   {
  return m_properties.size();
}

/// Export properties of another instance
void PropertyManager::adopt(const PropertyManager& copy)   {
  m_properties = copy.m_properties;
}

/// Check for existence
bool PropertyManager::exists(const std::string& name) const   {
  Properties::const_iterator i = m_properties.find(name);
  return i != m_properties.end();
}

/// Verify that this property does not exist (throw exception if the name was found)
void PropertyManager::verifyNonExistence(const string& name) const {
  Properties::const_iterator i = m_properties.find(name);
  if (i == m_properties.end())
    return;
  throw runtime_error("The property:" + name + " already exists for this component.");
}

/// Verify that this property exists (throw exception if the name was not found)
PropertyManager::Properties::const_iterator
PropertyManager::verifyExistence(const string& name) const {
  Properties::const_iterator i = m_properties.find(name);
  if (i != m_properties.end())
    return i;
  throw runtime_error("PropertyManager: Unknown property:" + name);
}

/// Verify that this property exists (throw exception if the name was not found)
PropertyManager::Properties::iterator
PropertyManager::verifyExistence(const string& name) {
  Properties::iterator i = m_properties.find(name);
  if (i != m_properties.end())
    return i;
  throw runtime_error("PropertyManager: Unknown property:" + name);
}

/// Access property by name (CONST)
Property& PropertyManager::property(const string& name) {
  return (*verifyExistence(name)).second;
}

/// Access property by name
const Property& PropertyManager::property(const string& name) const {
  return (*verifyExistence(name)).second;
}

/// Access property by name
Property& PropertyManager::operator[](const string& name) {
  return (*verifyExistence(name)).second;
}

/// Access property by name
const Property& PropertyManager::operator[](const string& name) const {
  return (*verifyExistence(name)).second;
}

/// Add a new property
void PropertyManager::add(const string& name, const Property& prop) {
  verifyNonExistence(name);
  m_properties.emplace(name, prop);
}

/// Bulk set of all properties
void PropertyManager::set(const string& component_name, PropertyConfigurator& cfg) {
  for (auto& i : m_properties )
    cfg.set(i.second.grammar(), component_name, i.first, i.second.ptr());
}

/// Dump string values
void PropertyManager::dump() const {
  for (const auto& i : m_properties )
    printout(ALWAYS, "PropertyManager", "Property %s = %s",
             i.first.c_str(), i.second.str().c_str());
}

/// Standard PropertyConfigurable constructor
PropertyConfigurable::PropertyConfigurable()  {
}

/// Default PropertyConfigurable destructor
PropertyConfigurable::~PropertyConfigurable()   {
}

/// Check property for existence
bool PropertyConfigurable::hasProperty(const string& nam) const    {
  return m_properties.exists(nam);
}

/// Access single property
Property& PropertyConfigurable::property(const string& nam)   {
  return properties()[nam];
}

#include "DD4hep/detail/Grammar_parsed.h"
namespace dd4hep { 
  namespace Parsers {
    template <> int parse(Property& result, const std::string& input) {
      result.str(input); 
      return 1;
    }
    template <> std::ostream& toStream(const Property& result, std::ostream& os) {
      return os << result.str();
    }
  }
  template<> int Grammar<Property>::evaluate(void* _p, const std::string& _v) const {
    return eval_obj ((Property*)_p,_v);
  }
  template class Grammar<Property>;
}

