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
#include "DDG4/ComponentUtils_inl.h"

// C/C++ include files
#include <stdexcept>
#include <cstring>
#include <map>

#ifndef _WIN32
#include <cxxabi.h>
#endif

using namespace std;
using namespace DD4hep;

namespace  {

  const std::string __typename( const char* class_name) {
    std::string result;
#ifdef _WIN32
    long off = 0;
    if ( ::strncmp(class_name, "class ", 6) == 0 )   {
      // The returned name is prefixed with "class "
      off = 6;
    }
    if ( ::strncmp(class_name, "struct ", 7) == 0 )   {
      // The returned name is prefixed with "struct "
      off = 7;
    }
    if ( off > 0 )    {
      std::string tmp = class_name + off;
      long loc = 0;
      while( (loc = tmp.find("class ")) > 0 )  {
	tmp.erase(loc, 6);
      }
      loc = 0;
      while( (loc = tmp.find("struct ")) > 0 )  {
	tmp.erase(loc, 7);
      }
      result = tmp;
    }
    else  {
      result = class_name;
    }
    // Change any " *" to "*"
    while ( (off=result.find(" *")) != std::string::npos ) {
      result.replace(off, 2, "*");
    }
    // Change any " &" to "&"
    while ( (off=result.find(" &")) != std::string::npos ) {
      result.replace(off, 2, "&");
    }

#elif defined(__linux) || defined(__APPLE__)
    if ( ::strlen(class_name) == 1 ) {
      // See http://www.realitydiluted.com/mirrors/reality.sgi.com/dehnert_engr/cxx/abi.pdf
      // for details
      switch(class_name[0]) {
      case 'v':
        result = "void";
        break;
      case 'w':
        result = "wchar_t";
        break;
      case 'b':
        result = "bool";
        break;
      case 'c':
        result = "char";
        break;
      case 'a':
        result = "signed char";
        break;
      case 'h':
        result = "unsigned char";
        break;
      case 's':
        result = "short";
        break;
      case 't':
        result = "unsigned short";
        break;
      case 'i':
        result = "int";
        break;
      case 'j':
        result = "unsigned int";
        break;
      case 'l':
        result = "long";
        break;
      case 'm':
        result = "unsigned long";
        break;
      case 'x':
        result = "long long";
        break;
      case 'y':
        result = "unsigned long long";
        break;
      case 'n':
        result = "__int128";
        break;
      case 'o':
        result = "unsigned __int128";
        break;
      case 'f':
        result = "float";
        break;
      case 'd':
        result = "double";
        break;
      case 'e':
        result = "long double";
        break;
      case 'g':
        result = "__float128";
        break;
      case 'z':
        result = "ellipsis";
        break;
      }
    }
    else  {
      int   status;
      char* realname;
      realname = abi::__cxa_demangle(class_name, 0, 0, &status);
      if (realname == 0) return class_name;
      result = realname;
      free(realname);
      /// substitute ', ' with ','
      std::string::size_type pos = result.find(", ");
      while( std::string::npos != pos ) {
        result.replace( pos , 2 , "," ) ;
        pos = result.find(", ");
      }
    }
#endif
    return result;
  }
}

/// ABI information about type names
std::string DD4hep::typeinfoName(const std::type_info& type)   {
  return __typename(type.name());
}

void DD4hep::typeinfoCheck(const std::type_info& typ1, const std::type_info& typ2, const std::string& text)  {
  if ( typ1 != typ2 )  {
    throw unrelated_type_error(typ1,typ2,text);
  }
}

string unrelated_type_error::msg(const std::type_info& typ1, const std::type_info& typ2,const string& text)   {
  std::string m = "The types "+__typename(typ1.name())+" and "+__typename(typ2.name())+
    " are not related. "+text;
  return m;
}

/// Default constructor
PropertyGrammar::PropertyGrammar()   {
}

/// Default destructor
PropertyGrammar::~PropertyGrammar()  {
}

/// Error callback on invalid conversion
void PropertyGrammar::invalidConversion(const type_info& from, const type_info& to)   {
  string to_name   = __typename(to.name());
  string from_name = __typename(from.name());
  throw unrelated_type_error(from,to,
			     "The Property data conversion from type "+from_name+
			     " to "+to_name+" is not implemented.");
}

/// Default constructor
Property::Property() 
  : m_par(0), m_hdl(0)
{
}

/// Copy constructor  
Property::Property(const Property& property) 
  : m_par(property.m_par), m_hdl(property.m_hdl)
{
}

/// Assignment operator
Property& Property::operator=(const Property& property) {
  m_par  = property.m_par;
  m_hdl  = property.m_hdl;
  return *this;
}

/// Property type name
string Property::type(const Property& property)    {
  return type(property.grammar().type());
}

/// Property type name
string Property::type(const type_info& typ)   {
  return __typename(typ.name());
}

/// Property type name
string Property::type()  const   {
  return Property::type(grammar().type());
}

const PropertyGrammar& Property::grammar()  const   {
  if ( m_hdl ) return *m_hdl;
  throw runtime_error("Attempt to access property grammar from invalid object.");
}

/// Conversion to string value
string Property::str() const    {
  if ( m_hdl && m_par )   {
    return m_hdl->str(m_par);
  }
  throw runtime_error("Attempt to access property grammar from invalid object.");
}

/// Conversion from string value
Property& Property::str(const std::string& input)   {
  if ( m_hdl && m_par )   {
    m_hdl->fromString(m_par,input);
    return *this;
  }
  throw runtime_error("Attempt to access property grammar from invalid object.");
}

/// Assignment operator / set new balue
Property& Property::operator=(const string& val)  {
  this->set<string>(val);
  return *this;
}

/// Assignment operator / set new balue
Property& Property::operator=(const char* val)  {
  if ( val )  {
    this->set<string>(val);
    return *this;
  }
  throw runtime_error("Attempt to set invalid string to property!");
}

/// Default constructor
PropertyManager::PropertyManager()   {
}

/// Default destructor
PropertyManager::~PropertyManager()  {
  m_properties.clear();
}

/// Verify that this property does not exist (throw exception if the name was found)
void PropertyManager::verifyNonExistence(const string& name) const   {
  Properties::const_iterator i = m_properties.find(name);
  if ( i == m_properties.end() ) return;
  throw runtime_error("The property:"+name+" already exists for this component.");
}

/// Verify that this property exists (throw exception if the name was not found)
PropertyManager::Properties::const_iterator
PropertyManager::verifyExistence(const string& name) const  {
  Properties::const_iterator i = m_properties.find(name);
  if ( i != m_properties.end() ) return i;
  throw runtime_error("PropertyManager: Unknown property:"+name);
}

/// Verify that this property exists (throw exception if the name was not found)
PropertyManager::Properties::iterator
PropertyManager::verifyExistence(const string& name)  {
  Properties::iterator i = m_properties.find(name);
  if ( i != m_properties.end() ) return i;
  throw runtime_error("PropertyManager: Unknown property:"+name);
}

/// Access property by name (CONST)
Property& PropertyManager::property(const string& name)  {
  return (*verifyExistence(name)).second;
}

/// Access property by name
const Property& PropertyManager::property(const string& name) const   {
  return (*verifyExistence(name)).second;
}

/// Access property by name
Property& PropertyManager::operator[](const string& name)  {
  return (*verifyExistence(name)).second;
}

/// Access property by name
const Property& PropertyManager::operator[](const string& name)  const {
  return (*verifyExistence(name)).second;
}

/// Add a new property 
void PropertyManager::add(const string& name, const Property& property)  {
  verifyNonExistence(name);
  m_properties.insert(make_pair(name,property));
}

/// Bulk set of all properties
void PropertyManager::set(const string& component_name, PropertyConfigurator& cfg)   {
  for(Properties::iterator i = m_properties.begin(); i!=m_properties.end(); ++i)  {
    Property& p = (*i).second;
    cfg.set(p.grammar(),component_name,(*i).first,p.ptr());
  }
}

/// Dump string values
void PropertyManager::dump()  const   {
  for(Properties::const_iterator i = m_properties.begin(); i!=m_properties.end(); ++i)  {
    const Property& p = (*i).second;
    printout(ALWAYS,"PropertyManager","Property %s = %s",(*i).first.c_str(),p.str().c_str());
  }
}

#define DD4HEP_INSTANTIATE_PROPERTY_TYPE1(x) DD4HEP_INSTANTIATE_PROPERTY_TYPE(x); \
  DD4HEP_INSTANTIATE_PROPERTY_TYPE(std::vector<x>);			\
  DD4HEP_INSTANTIATE_PROPERTY_TYPE(std::list<x>);			\
  DD4HEP_INSTANTIATE_PROPERTY_TYPE(std::set<x>)

#define DD4HEP_INSTANTIATE_PROPERTY_TYPE2(x)	\
  DD4HEP_INSTANTIATE_PROPERTY_TYPE1(x);		\
  DD4HEP_INSTANTIATE_PROPERTY_TYPE1(unsigned x)

namespace DD4hep {
DD4HEP_INSTANTIATE_PROPERTY_TYPE2(char);
DD4HEP_INSTANTIATE_PROPERTY_TYPE2(short);
DD4HEP_INSTANTIATE_PROPERTY_TYPE2(int);
DD4HEP_INSTANTIATE_PROPERTY_TYPE2(long);
DD4HEP_INSTANTIATE_PROPERTY_TYPE2(long long);

DD4HEP_INSTANTIATE_PROPERTY_TYPE1(bool);
DD4HEP_INSTANTIATE_PROPERTY_TYPE1(float);
DD4HEP_INSTANTIATE_PROPERTY_TYPE1(double);
DD4HEP_INSTANTIATE_PROPERTY_TYPE1(string);

typedef map<string,int> map_string_int;
DD4HEP_INSTANTIATE_PROPERTY_TYPE(map_string_int);
}
