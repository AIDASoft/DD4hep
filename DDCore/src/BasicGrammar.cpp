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
#include "DD4hep/Exceptions.h"
#include "DD4hep/BasicGrammar.h"
#include "DD4hep/detail/Grammar.h"

// ROOT include files
#include "TDataType.h"
#include "TROOT.h"

// C/C++ include files
#include <stdexcept>
#include <mutex>
#include <map>

namespace {

  std::mutex s_mutex;
  
  // This static object needs to be in function to trick out static constructors populating this registry....
  static std::map<dd4hep::BasicGrammar::key_type, dd4hep::BasicGrammar*>& registry()  {
    static std::map<dd4hep::BasicGrammar::key_type, dd4hep::BasicGrammar*> s_registry;
    return s_registry;
  }
}

/// Default constructor
dd4hep::BasicGrammar::BasicGrammar(const std::string& typ)
  : name(typ), hash_value(dd4hep::detail::hash64(typ))
{
  if ( !registry().emplace(hash_value,this).second )  {
    // Error: Already existing grammar.
    dd4hep::except("BasicGrammar","FAILED to add existent registry: %s [%016llX]",name.c_str(), hash_value);    
  }
}

/// Default destructor
dd4hep::BasicGrammar::~BasicGrammar()   {
}

/// Second step initialization after the virtual table is fixed
void dd4hep::BasicGrammar::initialize()  const  {
  std::lock_guard<std::mutex> lock(s_mutex);
  if ( !inited )   {
    TClass* cl = gROOT->GetClass(type());
    if ( cl )  {
      root_class = cl;
      inited = true;
      return;
    }
    root_data_type = TDataType::GetType(type());
    if ( root_data_type == kOther_t )  {
      except("BasicGrammar",
             "+++ ERROR +++ Cannot initialize gammar object: %s. "
             "No TClass and no data type information present!",name.c_str());
    }
    inited = true;
  }
}

/// Access ROOT data type for fundamentals
int dd4hep::BasicGrammar::initialized_data_type()  const   {
  this->initialize();
  return root_data_type;
}

/// Access the ROOT class for complex objects
TClass* dd4hep::BasicGrammar::initialized_clazz()  const   {
  this->initialize();
  return root_class;
}

/// Lookup existing grammar using hash code (reading objects)
const dd4hep::BasicGrammar& dd4hep::BasicGrammar::get(key_type hash_code)   {
  auto i = registry().find(hash_code);
  if ( i != registry().end() ) return *((*i).second);
  dd4hep::except("BasicGrammar","FAILED to look up non existent registry: %016llX",hash_code);
  throw "Error";  // Not reachable anyhow. Simply to please the compiler!
}

/// Error callback on invalid conversion
void dd4hep::BasicGrammar::invalidConversion(const std::string& value, const std::type_info& to) {
  std::string to_name = typeName(to);
  throw unrelated_value_error(to,
                              "Data conversion of " + value + " to type '" + 
                              to_name + "' is not defined.");
}

/// Error callback on invalid conversion
void dd4hep::BasicGrammar::invalidConversion(const std::type_info& from, const std::type_info& to) {
  std::string to_name = typeName(to);
  std::string from_name = typeName(from);
  throw unrelated_type_error(from, to,
                             "Data conversion from '" + from_name + 
                             "' to '" + to_name + "' is not implemented.");
}

/// Standard constructor
dd4hep::SimpleGrammar::SimpleGrammar(size_t siz, const std::type_info& typ)
  : BasicGrammar(dd4hep::typeName(typ)), object_size(siz), object_type(typ)
{
}

/// Default destructor
dd4hep::SimpleGrammar::~SimpleGrammar()   {
}

/// PropertyGrammar overload: Serialize a property to a string
std::string dd4hep::SimpleGrammar::str(const void* /* ptr */) const    {
  dd4hep::except("SimpleGrammar","+++ The string representation str() is not implemented for grammar type %s",
                 this->type_name().c_str());
  return "";
}

/// PropertyGrammar overload: Retrieve value from string
bool dd4hep::SimpleGrammar::fromString(void* /* ptr */, const std::string& /* value */) const    {
  dd4hep::except("SimpleGrammar","+++ The data parsing from string is not implemented for grammar type %s",
                 this->type_name().c_str());
  return false;
}

/// Opaque object destructor
void dd4hep::SimpleGrammar::destruct(void* /* pointer */) const    {
  dd4hep::except("SimpleGrammar","+++ Object destruction is not implemented for grammar type %s",
                 this->type_name().c_str());
}

/// Opaque object copy construction. Memory must be allocated externally
void dd4hep::SimpleGrammar::copy(void* /* to */, const void* /* from */)  const    {
  dd4hep::except("SimpleGrammar","+++ Object copy is not implemented for grammar type %s",
                 this->type_name().c_str());
}

/// Bind opaque address to object
void dd4hep::SimpleGrammar::bind(void* /* pointer */)  const   {
  dd4hep::except("SimpleGrammar","+++ Object destruction is not implemented for grammar type %s",
                 this->type_name().c_str());
}
