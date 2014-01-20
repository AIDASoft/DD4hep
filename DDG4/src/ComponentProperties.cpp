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
#include "DDG4/ComponentProperties_inl.h"
#include "DDG4/ToStream.h"

// C/C++ include files
#include <stdexcept>
#include <cstring>
#include <set>

using namespace std;
using namespace DD4hep;

/// Default constructor
PropertyGrammar::PropertyGrammar() {
}

/// Default destructor
PropertyGrammar::~PropertyGrammar() {
}

/// Error callback on invalid conversion
void PropertyGrammar::invalidConversion(const string& value, const type_info& to) {
  string to_name = typeinfoName(to);
  throw unrelated_value_error(to,
      "The Property data conversion of '" + value + "' to type " + to_name + " is not defined.");
}

/// Error callback on invalid conversion
void PropertyGrammar::invalidConversion(const type_info& from, const type_info& to) {
  string to_name = typeinfoName(to);
  string from_name = typeinfoName(from);
  throw unrelated_type_error(from, to,
      "The Property data conversion from type " + from_name + " to " + to_name + " is not implemented.");
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
  return typeinfoName(typ);
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

#include "XML/Evaluator.h"
namespace DD4hep { XmlTools::Evaluator& g4Evaluator();  }
namespace {  static XmlTools::Evaluator& s__eval(DD4hep::g4Evaluator());  }

static string pre_parse_obj(const string& in)   {
  string res = "";
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

template <typename TYPE> static int fill(std::vector<TYPE>* p,const std::vector<string>& temp)  {
  const Grammar<TYPE>& g = Grammar<TYPE>::instance();
  TYPE val;
  for(std::vector<string>::const_iterator i=temp.begin(); i != temp.end(); ++i)  {
    if ( !g.fromString(&val,*i) )
      return 0;
    p->push_back(val);
  }
  return 1;
}
template <typename TYPE> static int fill(std::list<TYPE>* p,const std::vector<string>& temp)  {
  const Grammar<TYPE>& g = Grammar<TYPE>::instance();
  TYPE val;
  for(std::vector<string>::const_iterator i=temp.begin(); i != temp.end(); ++i)  {
    if ( !g.fromString(&val,*i) )
      return 0;
    p->push_back(val);
  }
  return 1;
}
template <typename TYPE> static int fill(std::set<TYPE>* p,const std::vector<string>& temp)  {
  const Grammar<TYPE>& g = Grammar<TYPE>::instance();
  TYPE val;
  for(std::vector<string>::const_iterator i=temp.begin(); i != temp.end(); ++i)  {
    if ( !g.fromString(&val,*i) )
      return 0;
    p->insert(val);
  }
  return 1;
}

template <typename TYPE> static int eval(TYPE* p, const string& str)  {
#ifdef DD4HEP_USE_BOOST
  std::vector<string> buff;
  int sc = Parsers::parse(buff,str);
  if ( sc )  {
    return fill(p,buff);
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
      return fill(p,buff);
    }
  }
#else
  if ( p && str.empty() ) return 0;
#endif
  return 0;
}

template <typename T> static int _eval(T* p, string s)  {
  size_t idx = s.find("(int)");
  if (idx != string::npos)
    s.erase(idx, 5);
  while (s[0] == ' ')
    s.erase(0, 1);
  double result = s__eval.evaluate(s.c_str());
  if (s__eval.status() != XmlTools::Evaluator::OK) {
    return 0;
  }
  *p = (T)result;
  return 1;
}

static int _eval(ROOT::Math::XYZPoint* p, const string& str)
{  return Grammar<ROOT::Math::XYZPoint>::instance().fromString(p,pre_parse_obj(str));  }
static int _eval(ROOT::Math::XYZVector* p, const string& str)
{  return Grammar<ROOT::Math::XYZVector>::instance().fromString(p,pre_parse_obj(str));  }
static int _eval(ROOT::Math::PxPyPzEVector* p, const string& str)
{  return Grammar<ROOT::Math::PxPyPzEVector>::instance().fromString(p,pre_parse_obj(str));  }

#define DD4HEP_INSTANTIATE_PROPERTY_EVALUATE(x) \
  template<> int Grammar<x >::evaluate(void* p, const string& v) const { return _eval((x*)p,v); }

#define DD4HEP_INSTANTIATE_PROPERTY_EVALUATE1(x) DD4HEP_INSTANTIATE_PROPERTY_EVALUATE(x)\
  template<> int Grammar<vector<x> >::evaluate(void* p,const string& v)const{return eval((std::vector<x>*)p,v);} \
  template<> int Grammar<list<x>   >::evaluate(void* p,const string& v)const{return eval((std::list<x>*)p,v);} \
  template<> int Grammar<set<x>    >::evaluate(void* p,const string& v)const{return eval((std::set<x>*)p,v);}

#define DD4HEP_INSTANTIATE_PROPERTY_TYPE1(x)            \
  DD4HEP_INSTANTIATE_PROPERTY_TYPE(x);			\
  DD4HEP_INSTANTIATE_PROPERTY_TYPE(std::vector<x>);	\
  DD4HEP_INSTANTIATE_PROPERTY_TYPE(std::list<x>);	\
  DD4HEP_INSTANTIATE_PROPERTY_TYPE(std::set<x>)

#define DD4HEP_INSTANTIATE_PROPERTY_TYPE2(x)	\
  DD4HEP_INSTANTIATE_PROPERTY_TYPE1(x);		\
  DD4HEP_INSTANTIATE_PROPERTY_TYPE1(unsigned x)

// Macros for evaluated properties:

#define DD4HEP_INSTANTIATE_PROPERTY_TYPE0E(x)   \
  DD4HEP_INSTANTIATE_PROPERTY_EVALUATE(x)       \
  DD4HEP_INSTANTIATE_PROPERTY_TYPE(x)

#define DD4HEP_INSTANTIATE_PROPERTY_TYPE1E(x)   \
  DD4HEP_INSTANTIATE_PROPERTY_EVALUATE1(x)      \
  DD4HEP_INSTANTIATE_PROPERTY_TYPE1(x)

#define DD4HEP_INSTANTIATE_PROPERTY_TYPE2E(x)	\
  DD4HEP_INSTANTIATE_PROPERTY_TYPE1E(x);	\
  DD4HEP_INSTANTIATE_PROPERTY_TYPE1E(unsigned x)

namespace DD4hep {

  DD4HEP_INSTANTIATE_PROPERTY_TYPE2E(char);
  DD4HEP_INSTANTIATE_PROPERTY_TYPE2E(short);
  DD4HEP_INSTANTIATE_PROPERTY_TYPE2E(int);
  DD4HEP_INSTANTIATE_PROPERTY_TYPE2E(long);
  DD4HEP_INSTANTIATE_PROPERTY_TYPE2E(long long);

  DD4HEP_INSTANTIATE_PROPERTY_TYPE1E(bool);
  DD4HEP_INSTANTIATE_PROPERTY_TYPE1E(float);
  DD4HEP_INSTANTIATE_PROPERTY_TYPE1E(double);

  // STL objects
  DD4HEP_INSTANTIATE_PROPERTY_TYPE1(string);

  typedef map<string, int> map_string_int;
  DD4HEP_INSTANTIATE_PROPERTY_TYPE(map_string_int);

  // ROOT::Math Object instances
  DD4HEP_INSTANTIATE_PROPERTY_TYPE0E(ROOT::Math::XYZPoint);
  DD4HEP_INSTANTIATE_PROPERTY_TYPE0E(ROOT::Math::XYZVector);
  DD4HEP_INSTANTIATE_PROPERTY_TYPE0E(ROOT::Math::PxPyPzEVector);
}
