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
#include "DD4hep/Grammar.h"

// ROOT include files
#include "TDataType.h"
#include "TROOT.h"

// C/C++ include files
#include <algorithm>
#include <stdexcept>
#include <mutex>
#include <map>


namespace {
  std::mutex s_mutex;

  typedef const dd4hep::BasicGrammar& (*grammar_create_t)();
  typedef std::pair<grammar_create_t, dd4hep::BasicGrammar::specialization_t> grammar_args_t;

  // This static object needs to be in function to trick out static constructors populating this registry....
  static std::map<dd4hep::BasicGrammar::key_type, dd4hep::BasicGrammar*>& active_registry()  {
    static std::map<dd4hep::BasicGrammar::key_type, dd4hep::BasicGrammar*> s_registry;
    return s_registry;
  }
  static std::map<dd4hep::BasicGrammar::key_type, grammar_args_t>& prenote_registry()  {
    static std::map<dd4hep::BasicGrammar::key_type, grammar_args_t> s_registry;
    return s_registry;
  }
}

/// Default constructor
dd4hep::BasicGrammar::BasicGrammar(const std::string& typ)
  : name(typ), hash_value(dd4hep::detail::hash64(typ))
{
  auto j = prenote_registry().find(hash_value);
  if ( j != prenote_registry().end() )   {
    specialization = j->second.second;
  }
  if ( !active_registry().emplace(hash_value,this).second )   {
  }
}

/// Default destructor
dd4hep::BasicGrammar::~BasicGrammar()   {
}

void dd4hep::BasicGrammar::pre_note(const std::type_info& info,
				    const BasicGrammar& (*fcn)(),
				    specialization_t specs)   {
  key_type hash = dd4hep::detail::hash64(typeName(info));
  if ( !prenote_registry().emplace(hash,std::make_pair(fcn,specs)).second )  {
    // Error: Already existing grammar.
    dd4hep::except("BasicGrammar","FAILED to add existent registry: %s [%016llX]",
                   typeName(info).c_str(), hash);    
  }
}

/// Lookup existing grammar using hash code (reading objects)
const dd4hep::BasicGrammar& dd4hep::BasicGrammar::get(key_type hash)   {
  auto i = active_registry().find(hash);
  if ( i != active_registry().end() )
    return *(i->second);
  auto j = prenote_registry().find(hash);
  if ( j != prenote_registry().end() )
    return (j->second.first)();
  dd4hep::except("BasicGrammar","FAILED to look up non existent registry: %016llX",hash);
  throw "Error";  // Not reachable anyhow. Simply to please the compiler!
}

/// Lookup existing grammar using hash code (reading objects)
const dd4hep::BasicGrammar& dd4hep::BasicGrammar::get(const std::type_info& info)    {
  key_type hash = dd4hep::detail::hash64(typeName(info));
  auto i = active_registry().find(hash);
  if ( i != active_registry().end() )
    return *(i->second);
  auto j = prenote_registry().find(hash);
  if ( j != prenote_registry().end() )
    return (j->second.first)();
  dd4hep::except("BasicGrammar","FAILED to look up non existent registry: %016llX [%s]",
                 hash, typeName(info).c_str());
  throw "Error";  // Not reachable anyhow. Simply to please the compiler!  
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

/// Serialize an opaque value to a string
std::string dd4hep::BasicGrammar::str(const void* ptr) const    {
  if ( specialization.str )
    return specialization.str(*this,ptr);
  except("Grammar","Cannot serialize object with incomplete grammar: %s",type_name().c_str());
  return "";
}

/// Set value from serialized string. On successful data conversion TRUE is returned.
bool dd4hep::BasicGrammar::fromString(void* ptr, const std::string& value) const    {
  if ( specialization.fromString )
    return specialization.fromString(*this,ptr, value);
  except("Grammar","Cannot deserialize object with incomplete grammar: %s",type_name().c_str());
  return false;
}

/// Evaluate string value if possible before calling boost::spirit
int dd4hep::BasicGrammar::evaluate(void* ptr, const std::string& value) const    {
  if ( specialization.eval )
    return specialization.eval(*this,ptr, value);
  except("Grammar","Cannot evaluate object with incomplete grammar: %s",type_name().c_str());
  return 0;
}

/// Registry instance singleton
const dd4hep::GrammarRegistry& dd4hep::GrammarRegistry::instance()   {
  static GrammarRegistry s_reg;
  return s_reg;
}

