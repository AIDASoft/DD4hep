// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/Printout.h"
#include "DD4hep/BasicGrammar.h"
#include "DDG4/ComponentProperties.h"

// C/C++ include files
#include <stdexcept>
#include <cstring>

using namespace std;
using namespace DD4hep;

/// Default constructor
PropertyGrammar::PropertyGrammar(const BasicGrammar& g) : m_grammar(g) {
}

/// Default destructor
PropertyGrammar::~PropertyGrammar() {
}

/// Error callback on invalid conversion
void PropertyGrammar::invalidConversion(const std::type_info& from, const std::type_info& to)  {
  BasicGrammar::invalidConversion(from,to);
}

/// Error callback on invalid conversion
void PropertyGrammar::invalidConversion(const std::string& value, const std::type_info& to)   {
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

/// Default constructor
Property::Property()
  : m_par(0), m_hdl(0) {
}

/// Copy constructor
Property::Property(const Property& property)
    : m_par(property.m_par), m_hdl(property.m_hdl) {
}

/// Assignment operator
Property& Property::operator=(const Property& property) {
  if ( &property != this )  {
    m_par = property.m_par;
    m_hdl = property.m_hdl;
  }
  return *this;
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
  if (m_hdl)
    return *m_hdl;
  throw runtime_error("Attempt to access property grammar from invalid object.");
}

/// Conversion to string value
string Property::str() const {
  if (m_hdl && m_par ) {
    return m_hdl->str(m_par);
  }
  throw runtime_error("Attempt to access property grammar from invalid object.");
}

/// Conversion from string value
Property& Property::str(const std::string& input) {
  if (m_hdl && m_par )   {
    m_hdl->fromString(m_par,input);
    return *this;
  }
  throw runtime_error("Attempt to access property grammar from invalid object.");
}

/// Assignment operator / set new balue
//Property& Property::operator=(const string& val)  {
//  this->set<string>(val);
//  return *this;
//}

/// Assignment operator / set new balue
Property& Property::operator=(const char* val) {
  if (val) {
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
PropertyManager::Properties::const_iterator PropertyManager::verifyExistence(const string& name) const {
  Properties::const_iterator i = m_properties.find(name);
  if (i != m_properties.end())
    return i;
  throw runtime_error("PropertyManager: Unknown property:" + name);
}

/// Verify that this property exists (throw exception if the name was not found)
PropertyManager::Properties::iterator PropertyManager::verifyExistence(const string& name) {
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
void PropertyManager::add(const string& name, const Property& property) {
  verifyNonExistence(name);
  m_properties.insert(make_pair(name, property));
}

/// Bulk set of all properties
void PropertyManager::set(const string& component_name, PropertyConfigurator& cfg) {
  for (Properties::iterator i = m_properties.begin(); i != m_properties.end(); ++i) {
    Property& p = (*i).second;
    cfg.set(p.grammar(), component_name, (*i).first, p.ptr());
  }
}

/// Dump string values
void PropertyManager::dump() const {
  for (Properties::const_iterator i = m_properties.begin(); i != m_properties.end(); ++i) {
    const Property& p = (*i).second;
    printout(ALWAYS, "PropertyManager", "Property %s = %s", (*i).first.c_str(), p.str().c_str());
  }
}

#include "DDG4/ComponentProperties_inl.h"
#include <vector>
#include <list>
#include <set>
#include <map>

#include "Math/Point3D.h"
#include "Math/Vector3D.h"
#include "Math/Vector4D.h"

namespace DD4hep {
  DD4HEP_DEFINE_PROPERTY_U_CONT(char);
  DD4HEP_DEFINE_PROPERTY_U_CONT(short);
  DD4HEP_DEFINE_PROPERTY_U_CONT(int);
  DD4HEP_DEFINE_PROPERTY_U_CONT(long);
  DD4HEP_DEFINE_PROPERTY_U_CONT(long long);

  DD4HEP_DEFINE_PROPERTY_CONT(bool);
  DD4HEP_DEFINE_PROPERTY_CONT(float);
  DD4HEP_DEFINE_PROPERTY_CONT(double);

  // STL objects
  DD4HEP_DEFINE_PROPERTY_CONT(string);

  typedef map<string, int> map_string_int;
  DD4HEP_DEFINE_PROPERTY_TYPE(map_string_int);

  // ROOT::Math Object instances
  DD4HEP_DEFINE_PROPERTY_TYPE(ROOT::Math::XYZPoint);
  DD4HEP_DEFINE_PROPERTY_TYPE(ROOT::Math::XYZVector);
  DD4HEP_DEFINE_PROPERTY_TYPE(ROOT::Math::PxPyPzEVector);
}
