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
#include <DD4hep/Printout.h>
#include <DD4hep/Primitives.h>
#include <Parsers/Parsers.h>
#include <DD4hep/ComponentProperties.h>

// C/C++ include files
#include <stdexcept>
#include <cstring>

using namespace dd4hep;

/// Property type name
std::string Property::type(const Property& property) {
  return type(property.grammar().type());
}

/// Property type name
std::string Property::type(const std::type_info& typ) {
  return typeName(typ);
}

/// Property type name
std::string Property::type() const {
  return Property::type(grammar().type());
}

const BasicGrammar& Property::grammar() const {
  if ( m_hdl )
    return *m_hdl;
  throw std::runtime_error("Attempt to access property grammar from invalid object.");
}

/// Conversion to string value
std::string Property::str() const {
  if ( m_hdl && m_par ) {
    return m_hdl->str(m_par);
  }
  throw std::runtime_error("Attempt to access property grammar from invalid object.");
}

/// Conversion from string value
const Property& Property::str(const std::string& input)   const {
  if ( m_hdl && m_par )   {
    m_hdl->fromString(m_par,input);
    return *this;
  }
  throw std::runtime_error("Attempt to access property grammar from invalid object.");
}

/// Conversion from string value
Property& Property::str(const std::string& input)    {
  if ( m_hdl && m_par )   {
    m_hdl->fromString(m_par,input);
    return *this;
  }
  throw std::runtime_error("Attempt to access property grammar from invalid object.");
}

/// Assignment operator / set new value
Property& Property::operator=(const char* val) {
  if ( val ) {
    this->set < std::string > (val);
    return *this;
  }
  throw std::runtime_error("Attempt to set invalid string to property!");
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

/// Import properties of another instance
void PropertyManager::adopt(const PropertyManager& copy)   {
  m_properties = copy.m_properties;
}

/// Check for existence
bool PropertyManager::exists(const std::string& name) const   {
  Properties::const_iterator i = m_properties.find(name);
  return i != m_properties.end();
}

/// Verify that this property does not exist (throw exception if the name was found)
void PropertyManager::verifyNonExistence(const std::string& name) const {
  Properties::const_iterator i = m_properties.find(name);
  if (i == m_properties.end())
    return;
  throw std::runtime_error("The property:" + name + " already exists for this component.");
}

/// Verify that this property exists (throw exception if the name was not found)
PropertyManager::Properties::const_iterator
PropertyManager::verifyExistence(const std::string& name) const {
  Properties::const_iterator i = m_properties.find(name);
  if (i != m_properties.end())
    return i;
  throw std::runtime_error("PropertyManager: Unknown property:" + name);
}

/// Verify that this property exists (throw exception if the name was not found)
PropertyManager::Properties::iterator
PropertyManager::verifyExistence(const std::string& name) {
  Properties::iterator i = m_properties.find(name);
  if (i != m_properties.end())
    return i;
  throw std::runtime_error("PropertyManager: Unknown property:" + name);
}

/// Access property by name (CONST)
Property& PropertyManager::property(const std::string& name) {
  return (*verifyExistence(name)).second;
}

/// Access property by name
const Property& PropertyManager::property(const std::string& name) const {
  return (*verifyExistence(name)).second;
}

/// Access property by name
Property& PropertyManager::operator[](const std::string& name) {
  return (*verifyExistence(name)).second;
}

/// Access property by name
const Property& PropertyManager::operator[](const std::string& name) const {
  return (*verifyExistence(name)).second;
}

/// Add a new property
void PropertyManager::add(const std::string& name, const Property& prop) {
  verifyNonExistence(name);
  m_properties.emplace(name, prop);
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
bool PropertyConfigurable::hasProperty(const std::string& nam) const    {
  return m_properties.exists(nam);
}

/// Access single property
Property& PropertyConfigurable::property(const std::string& nam)   {
  return properties()[nam];
}

#include <DD4hep/GrammarParsed.h>
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
  // Ensure the grammars are registered
  template class Grammar<Property>;
  static auto s_registry = GrammarRegistry::pre_note<Property>(1);
}

