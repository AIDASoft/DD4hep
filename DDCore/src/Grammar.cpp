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
#include <stdexcept>
#include <mutex>
#include <map>

namespace {

  std::mutex s_mutex;
  
  // This static object needs to be in function to trick out static constructors populating this registry....
  static std::map<dd4hep::BasicGrammar::key_type, dd4hep::BasicGrammar*>& active_registry()  {
    static std::map<dd4hep::BasicGrammar::key_type, dd4hep::BasicGrammar*> s_registry;
    return s_registry;
  }
  static std::map<dd4hep::BasicGrammar::key_type, dd4hep::BasicGrammar*>& prenote_registry()  {
    static std::map<dd4hep::BasicGrammar::key_type, dd4hep::BasicGrammar*> s_registry;
    return s_registry;
  }
}

/// Default constructor
dd4hep::BasicGrammar::BasicGrammar(const std::string& typ)
  : name(typ), hash_value(dd4hep::detail::hash64(typ))
{
}

/// Default destructor
dd4hep::BasicGrammar::~BasicGrammar()   {
}

void dd4hep::BasicGrammar::pre_note(BasicGrammar* grammar)    {
  if ( !prenote_registry().emplace(grammar->hash_value,grammar).second )  {
    // Error: Already existing grammar.
    dd4hep::except("BasicGrammar","FAILED to add existent registry: %s [%016llX]",
                   grammar->name.c_str(), grammar->hash_value);    
  }
}

/// Lookup existing grammar using hash code (reading objects)
const dd4hep::BasicGrammar& dd4hep::BasicGrammar::get(key_type hash_code)   {
  auto i = active_registry().find(hash_code);
  if ( i != active_registry().end() )   {
    return *((*i).second);
  }
  i = prenote_registry().find(hash_code);
  if ( i != prenote_registry().end() )  {
    BasicGrammar& g = *((*i).second);
    active_registry().emplace(g.hash_value,&g);
    return g;
  }
  dd4hep::except("BasicGrammar","FAILED to look up non existent registry: %016llX",hash_code);
  throw "Error";  // Not reachable anyhow. Simply to please the compiler!
}

/// Lookup existing grammar using hash code (reading objects)
const dd4hep::BasicGrammar& dd4hep::BasicGrammar::get(const std::type_info& info)    {
  key_type hash = dd4hep::detail::hash64(typeName(info));
  auto i = active_registry().find(hash);
  if ( i != active_registry().end() )
    return *((*i).second);
  i = prenote_registry().find(hash);
  if ( i != prenote_registry().end() )  {
    BasicGrammar& g = *((*i).second);
    active_registry().emplace(g.hash_value,&g);
    return g;
  }
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
