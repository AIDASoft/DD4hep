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
#include "Evaluator/Evaluator.h"

// ROOT include files
#include "TDataType.h"
#include "TROOT.h"

// C/C++ include files
#include <algorithm>
#include <stdexcept>
#include <mutex>
#include <map>

#if defined(DD4HEP_PARSER_HEADER)

#define DD4HEP_NEED_EVALUATOR
// This is the case, if the parsers are externalized
// and the dd4hep namespace is renamed!
#include DD4HEP_PARSER_HEADER

#endif

namespace dd4hep {
  const dd4hep::tools::Evaluator& evaluator();
  const dd4hep::tools::Evaluator& g4Evaluator();
}
namespace {
  static const dd4hep::tools::Evaluator* s__eval(&dd4hep::g4Evaluator());
}


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

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Alignments internal namespace declaration
  namespace detail  {
    /// Helper function to parse data type
    std::string grammar_pre_parse_map(const std::string& in);
    /// Helper function to parse data type
    std::string grammar_pre_parse_obj(const std::string& in);
    /// Helper function to parse data type
    std::string grammar_pre_parse_cont(const std::string& in);
    /// Helper to parse single item
    std::pair<int,double> grammar_evaluate_item(std::string val);
  }

  /// Set grammar evaluator
  void setGrammarEvaluator(const std::string& type)    {
    if ( type == "TGeo" )
      s__eval = &evaluator();
    else if ( type == "Geant4" || type == "G4" )
      s__eval = &g4Evaluator();
    else if ( type == "CGS" )
      s__eval = &g4Evaluator();
    else
      except("Grammar","++ Undefined evaluator type: "+type);
  }
}


/// Equality operator
bool dd4hep::BasicGrammar::specialization_t::operator==(const specialization_t& cp)  const  {
  return this->bind  == cp.bind &&
    this->copy       == cp.copy && this->str == cp.str &&
    this->fromString == cp.fromString && this->eval == cp.eval;
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

/// Prenote loadable grammar
void dd4hep::BasicGrammar::pre_note(const std::type_info& info,
				    const BasicGrammar& (*fcn)(),
				    specialization_t specs)   {
  key_type hash = dd4hep::detail::hash64(typeName(info));
  if ( !prenote_registry().emplace(hash, std::make_pair(fcn,specs)).second )  {
    auto j = prenote_registry().find(hash);
    const auto& entry = (*j).second;
    if ( !(entry.first == fcn && entry.second == specs) )   {
      // Error: Already existing grammar.
      dd4hep::except("BasicGrammar","FAILED to add existent registry: %s [%016llX]",
                     typeName(info).c_str(), hash);
    }
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
  std::string to_name   = typeName(to);
  std::string from_name = typeName(from);
  throw unrelated_type_error(from, to,
                             "Data conversion from '" + from_name + 
                             "' to '" + to_name + "' is not implemented.");
}

/// Set cast structure
void dd4hep::BasicGrammar::setCast(const Cast* cast)  const   {
  BasicGrammar* g = const_cast<BasicGrammar*>(this);
  g->specialization.cast = cast;
}

/// Access ABI object cast
const dd4hep::Cast& dd4hep::BasicGrammar::cast() const   {
  if ( specialization.cast )
    return *specialization.cast;
  except("Grammar","Cannot serialize object with incomplete grammar: %s",type_name().c_str());
  return *specialization.cast;
}

/// Serialize an opaque value to a string
std::string dd4hep::BasicGrammar::str(const void* ptr) const    {
  if ( specialization.str )
    return specialization.str(*this, ptr);
  except("Grammar", "Cannot serialize object with incomplete grammar: %s", type_name().c_str());
  return "";
}

/// Set value from serialized string. On successful data conversion TRUE is returned.
bool dd4hep::BasicGrammar::fromString(void* ptr, const std::string& value) const    {
  if ( specialization.fromString )
    return specialization.fromString(*this,ptr, value);
  except("Grammar", "Cannot deserialize object with incomplete grammar: %s", type_name().c_str());
  return false;
}

/// Evaluate string value if possible before calling boost::spirit
int dd4hep::BasicGrammar::evaluate(void* ptr, const std::string& value) const    {
  if ( specialization.eval )
    return specialization.eval(*this, ptr, value);
  except("Grammar", "Cannot evaluate object with incomplete grammar: %s", type_name().c_str());
  return 0;
}

/// Helper to parse single item
std::pair<int,double> dd4hep::detail::grammar_evaluate_item(std::string val)   {
  size_t idx = val.find("(int)");
  if (idx != std::string::npos)
    val.erase(idx, 5);
  while (val[0] == ' ')
    val.erase(0, 1);
  auto result = s__eval->evaluate(val.c_str());
  return result;
}

/// Helper function to parse data type
std::string dd4hep::detail::grammar_pre_parse_map(const std::string& in)   {
  bool ignore_blanks = true;
  bool str_open = false;
  bool obj_open = false;
  bool start = false;
  std::string res = "";
  res.reserve(1024);
  // std::cout << "Pre-parsed   (in):" << in << std::endl;
  for(const char* c = in.c_str(); *c; ++c)   {
    switch(*c)  {
    case '\'':
      res += *c;
      if ( start ) { start = false; }
      if ( str_open ) { str_open = false; }
      break;
    case ':':
      if ( str_open ) { res += '\''; str_open = false; }
      res += *c;
      res += ' ';
      res += '\'';
      str_open = true;
      ignore_blanks = true;
      break;
    case ',':
      if ( !obj_open && str_open ) { res += '\''; str_open = false; }
      res += ",";
      start = true;
      ignore_blanks = true;
      break;
    case '(':
      res += *c ;
      obj_open = true;
      break;
    case ')':
      res += *c ;
      obj_open = false;
      break;
    case '[':
    case '{':
      res += *c ;
      start = true;
      ignore_blanks = true;
      break;
    case ']':
    case '}':
      if ( str_open ) {
	res += '\'';
	str_open = false;
      }
      res += *c ;
      break;
    case ' ':
      if ( !ignore_blanks ) res += *c;
      break;
    default:
      if ( start ) {
	if ( !obj_open ) res += '\'';
	start = false;
	str_open = true;
      }
      ignore_blanks = false;
      res += *c;
      break;
    }
  }
  // std::cout << "Pre-parsed  (out):" << res << std::endl;
  return res;
}

/// Helper function to parse data type
std::string dd4hep::detail::grammar_pre_parse_cont(const std::string& in)   {
  bool ignore_blanks = true;
  bool str_open = false;
  bool start = false;
  std::string res = "";
  res.reserve(1024);
  // std::cout << "Pre-parsed   (in):" << in << std::endl;
  for(const char* c = in.c_str(); *c; ++c)   {
    switch(*c)  {
    case '\'':
      res += *c;
      if ( start ) { start = false; }
      if ( str_open ) { str_open = false; }
      break;
    case ':':
      if ( str_open ) { res += '\''; str_open = false; }
      res += *c;
      res += ' ';
      res += '\'';
      str_open = true;
      ignore_blanks = true;
      break;
    case ',':
      if ( str_open ) { res += '\''; str_open = false; }
      res += ",";
      start = true;
      ignore_blanks = true;
      break;
    case '(':
    case '[':
    case '{':
      res += *c ;
      start = true;
      ignore_blanks = true;
      break;
    case ')':
    case ']':
    case '}':
      if ( str_open ) { res += '\''; str_open = false; }
      res += *c ;
      break;
    case ' ':
      if ( !ignore_blanks ) res += *c;
      break;
    default:
      if ( start ) {
	res += '\'';
	start = false;
	str_open = true;
      }
      ignore_blanks = false;
      res += *c;
      break;
    }
  }
  // std::cout << "Pre-parsed  (out):" << res << std::endl;
  return res;
}

/// Helper function to parse data type
std::string dd4hep::detail::grammar_pre_parse_obj(const std::string& in)   {
  std::string res = "";
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

/// Registry instance singleton
const dd4hep::GrammarRegistry& dd4hep::GrammarRegistry::instance()   {
  static GrammarRegistry s_reg;
  return s_reg;
}

