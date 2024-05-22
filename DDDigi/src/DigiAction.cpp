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
#include <DD4hep/InstanceCount.h>
#include <DDDigi/DigiAction.h>

// C/C++ include files
#include <algorithm>

using namespace dd4hep::digi;

TypeName TypeName::split(const std::string& type_name, const std::string& delim) {
  std::size_t idx = type_name.find(delim);
  std::string typ = type_name, nam = type_name;
  if (idx != std::string::npos) {
    typ = type_name.substr(0, idx);
    nam = type_name.substr(idx + 1);
  }
  return TypeName(typ, nam);
}

TypeName TypeName::split(const std::string& type_name) {
  return split(type_name,"/");
}

/// Standard constructor
DigiAction::DigiAction(const DigiKernel& krnl, const std::string& nam)
  : m_kernel(krnl), m_name(nam), m_outputLevel(INFO)
{
  InstanceCount::increment(this);
  declareProperty("name", m_name);
  declareProperty("OutputLevel", m_outputLevel=printLevel());
}

/// Default destructor
DigiAction::~DigiAction() {
  for( auto& ptr : m_extensions )
    ptr.second->destruct();
  m_extensions.clear();
  InstanceCount::decrement(this);
}

/// Implicit destruction
long DigiAction::addRef() {
  return ++m_refCount;
}

/// Decrease reference count. Implicit destruction
long DigiAction::release() {
  long count = --m_refCount;
  if (m_refCount <= 0) {
    debug("Deleting object %s of type %s Pointer:%p",
	  m_name.c_str(),typeName(typeid(*this)).c_str(),(void*)this);
    delete this;
  }
  return count;
}

/// Set DigiAction property
void DigiAction::printProperties()   const {
  const auto& props = properties().properties();
  for( const auto& p : props )   {
    std::string pn = name()+"."+p.first;
    always("+++ %-32s = %-42s  [%s]", pn.c_str(), p.second.str().c_str(), p.second.type().c_str());
  }
}

/// Adopt named property of another action for data processing
void DigiAction::adopt_property(DigiAction* action, const std::string& foreign_name, const std::string& local_name)   {
  if ( action )    {
    PropertyManager& mgr = action->properties();
    Property& prop = mgr.property(foreign_name);
    properties().add(local_name, prop);
    return;
  }
  except("+++ adoptProperty: Invalid source action to access property %s", foreign_name.c_str());
}

/// Adopt named tool to delegate actions
void DigiAction::adopt_tool(DigiAction* /* action */, const std::string& typ)    {
  except("+++ adoptTool: Invalid call: A tool type %s is not useful for action %s",
	 typ.c_str(), c_name());
}

/// Set the output level; returns previous value
dd4hep::PrintLevel DigiAction::setOutputLevel(PrintLevel new_level)  {
  int old = m_outputLevel;
  m_outputLevel = new_level;
  return (PrintLevel)old;
}

/// Check property for existence
bool DigiAction::hasProperty(const std::string& nam) const    {
  return m_properties.exists(nam);
}

/// Access single property
dd4hep::Property& DigiAction::property(const std::string& nam)   {
  return properties()[nam];
}

/// Access single property
const dd4hep::Property& DigiAction::property(const std::string& nam)   const  {
  return properties()[nam];
}

/// Add an extension object to the detector element
uint64_t DigiAction::addExtension(uint64_t key, std::unique_ptr<ExtensionEntry>&& e)  {
  if ( m_extensions.emplace(key, std::move(e)).second )
    return key;
  return 0UL;
}

/// Access an existing extension object from the detector element
void* DigiAction::extension(uint64_t key)  {
  auto iter = m_extensions.find(key);
  if ( iter != m_extensions.end() )
    return iter->second.get();
  return nullptr;
}

/// Support of debug messages.
std::string DigiAction::format(const char* fmt, ...) const {
  va_list args;
  va_start(args, fmt);
  std::string str = dd4hep::format(nullptr, fmt, args);
  va_end(args);
  return str;
}

/// Support for messages with variable output level using output level
void DigiAction::print(const char* fmt, ...) const   {
  int level = std::max(int(outputLevel()),(int)VERBOSE);
  if ( level >= printLevel() )  {
    va_list args;
    va_start(args, fmt);
    dd4hep::printout((PrintLevel)level, m_name.c_str(), fmt, args);
    va_end(args);
  }
}

/// Support of debug messages.
void DigiAction::always(const char* fmt, ...) const {
  va_list args;
  va_start(args, fmt);
  dd4hep::printout(dd4hep::FORCE_ALWAYS, m_name, fmt, args);
  va_end(args);
}

/// Support of debug messages.
void DigiAction::debug(const char* fmt, ...) const {
  int level = std::max(int(outputLevel()),(int)VERBOSE);
  if ( level <= DEBUG )  {
    va_list args;
    va_start(args, fmt);
    dd4hep::printout(dd4hep::FORCE_DEBUG, m_name, fmt, args);
    va_end(args);
  }
}

/// Support of info messages.
void DigiAction::info(const char* fmt, ...) const {
  int level = std::max(int(outputLevel()),(int)VERBOSE);
  if ( level <= INFO )  {
    va_list args;
    va_start(args, fmt);
    dd4hep::printout(dd4hep::FORCE_INFO, m_name, fmt, args);
    va_end(args);
  }
}

/// Support of warning messages.
void DigiAction::warning(const char* fmt, ...) const {
  int level = std::max(int(outputLevel()),(int)VERBOSE);
  if ( level <= WARNING )  {
    va_list args;
    va_start(args, fmt);
    dd4hep::printout(dd4hep::FORCE_WARNING, m_name, fmt, args);
    va_end(args);
  }
}

/// Action to support error messages.
void DigiAction::error(const char* fmt, ...) const {
  int level = std::max(int(outputLevel()),(int)VERBOSE);
  if ( level <= ERROR )  {
    va_list args;
    va_start(args, fmt);
    dd4hep::printout(dd4hep::FORCE_ERROR, m_name, fmt, args);
    va_end(args);
  }
}

/// Action to support error messages.
bool DigiAction::return_error(bool return_value, const char* fmt, ...) const {
  int level = std::max(int(outputLevel()),(int)VERBOSE);
  if ( level <= ERROR )  {
    va_list args;
    va_start(args, fmt);
    dd4hep::printout(dd4hep::FORCE_ERROR, m_name, fmt, args);
    va_end(args);
  }
  return return_value;
}

/// Support of fatal messages. Throws exception if required.
void DigiAction::fatal(const char* fmt, ...) const {
  va_list args;
  va_start(args, fmt);
  dd4hep::printout(dd4hep::FORCE_FATAL, m_name, fmt, args);
  va_end(args);
}

/// Support of exceptions: Print fatal message and throw runtime_error.
void DigiAction::except(const char* fmt, ...) const {
  va_list args;
  va_start(args, fmt);
  std::string err = dd4hep::format(m_name, fmt, args);
  dd4hep::printout(dd4hep::FORCE_FATAL, m_name, err.c_str());
  va_end(args);
  throw std::runtime_error(err);
}

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {
    template <typename VAL>
    int add_action_property(DigiAction* action, const std::string& name, VAL value)   {
      action->addProperty(name, value);
      printout(INFO, "addProperty", "+++ Added property %s of type %s",
               name.c_str(), typeName(typeid(VAL)).c_str());
      return 1;
    }

#define ADD_SINGLE_PROPERTY(X)                                          \
    template int add_action_property<X>(DigiAction* action, const std::string& name, X value);

#define ADD_MAPPED_PROPERTY(K,X)                                        \
    template int add_action_property<std::map<std::string,X> >(DigiAction* action, const std::string& name, std::map<K,X> value);

#define ADD_PROPERTY(X)                         \
    ADD_SINGLE_PROPERTY(X)                      \
    ADD_SINGLE_PROPERTY(std::set<X>)            \
    ADD_SINGLE_PROPERTY(std::list<X>)           \
    ADD_SINGLE_PROPERTY(std::vector<X>)					\
    ADD_MAPPED_PROPERTY(std::string,X)

    ADD_PROPERTY(int)
    ADD_PROPERTY(float)
    ADD_PROPERTY(double)
    ADD_PROPERTY(std::string)
    ADD_PROPERTY(dd4hep::Position)

    ADD_SINGLE_PROPERTY(size_t)
  }
}

#include "DD4hep/GrammarUnparsed.h"
namespace dd4hep   {

  static bool _from_string(const BasicGrammar&, void* ptr, const std::string& value)    {
    static constexpr const char MATCH[] = " object at 0x";
    size_t idx = value.find(MATCH);
    if ( idx != std::string::npos )    {
      void* p = 0;
      const char* cptr = value.c_str()+idx+strlen(MATCH)-2;
      if ( 1 == ::sscanf(cptr,"%p", &p) )   {
	*(void**)ptr = p;
	return true;
      }
    }
    dd4hep::except("FAIL","FAIL");
    return false;
  }

  template <> const GrammarRegistry& GrammarRegistry::pre_note<DigiAction*>(int)   {
    BasicGrammar::specialization_t spec;
    spec.bind = nullptr;
    spec.copy = nullptr;
    spec.eval = nullptr;
    spec.str  = nullptr;
    spec.fromString = _from_string;
    return pre_note_specs<DigiAction*>(spec);
  }
}

static auto s_registry = dd4hep::GrammarRegistry::pre_note<DigiAction*>(1);
