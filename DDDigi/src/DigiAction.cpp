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
#include "DD4hep/InstanceCount.h"
#include "DDDigi/DigiAction.h"

// C/C++ include files
#include <algorithm>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::digi;

TypeName TypeName::split(const string& type_name, const string& delim) {
  size_t idx = type_name.find(delim);
  string typ = type_name, nam = type_name;
  if (idx != string::npos) {
    typ = type_name.substr(0, idx);
    nam = type_name.substr(idx + 1);
  }
  return TypeName(typ, nam);
}

TypeName TypeName::split(const string& type_name) {
  return split(type_name,"/");
}

/// Standard constructor
DigiAction::DigiAction(const DigiKernel& krnl, const string& nam)
  : m_kernel(krnl), m_name(nam), m_outputLevel(INFO)
{
  InstanceCount::increment(this);
  declareProperty("Name", m_name);
  declareProperty("name", m_name);
  declareProperty("parallel", m_parallel);
  declareProperty("OutputLevel", m_outputLevel);
}

/// Default destructor
DigiAction::~DigiAction() {
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
    printM1("DigiAction: Deleting object %s of type %s Pointer:%p",
            m_name.c_str(),typeName(typeid(*this)).c_str(),(void*)this);
    delete this;
  }
  return count;
}

/// Set the output level; returns previous value
PrintLevel DigiAction::setOutputLevel(PrintLevel new_level)  {
  int old = m_outputLevel;
  m_outputLevel = new_level;
  return (PrintLevel)old;
}

/// Set the parallization flag; returns previous value
bool DigiAction::setExecuteParallel(bool new_value)    {
  bool old = m_parallel;
  m_parallel = new_value;
  return old;
}

/// Check property for existence
bool DigiAction::hasProperty(const string& nam) const    {
  return m_properties.exists(nam);
}

/// Access single property
Property& DigiAction::property(const string& nam)   {
  return properties()[nam];
}

/// Support for messages with variable output level using output level
void DigiAction::print(const char* fmt, ...) const   {
  int level = max(int(outputLevel()),(int)VERBOSE);
  if ( level >= printLevel() )  {
    va_list args;
    va_start(args, fmt);
    dd4hep::printout((PrintLevel)level, m_name.c_str(), fmt, args);
    va_end(args);
  }
}

/// Support for messages with variable output level using output level-1
void DigiAction::printM1(const char* fmt, ...) const   {
  int level = max(outputLevel()-1,(int)VERBOSE);
  if ( level >= printLevel() )  {
    va_list args;
    va_start(args, fmt);
    dd4hep::printout((PrintLevel)level, m_name.c_str(), fmt, args);
    va_end(args);
  }
}

/// Support for messages with variable output level using output level-2
void DigiAction::printM2(const char* fmt, ...) const   {
  int level = max(outputLevel()-2,(int)VERBOSE);
  if ( level >= printLevel() )  {
    va_list args;
    va_start(args, fmt);
    dd4hep::printout((PrintLevel)level, m_name.c_str(), fmt, args);
    va_end(args);
  }
}

/// Support for messages with variable output level using output level-1
void DigiAction::printP1(const char* fmt, ...) const   {
  int level = min(outputLevel()+1,(int)FATAL);
  if ( level >= printLevel() )  {
    va_list args;
    va_start(args, fmt);
    dd4hep::printout((PrintLevel)level, m_name.c_str(), fmt, args);
    va_end(args);
  }
}

/// Support for messages with variable output level using output level-2
void DigiAction::printP2(const char* fmt, ...) const   {
  int level = min(outputLevel()+2,(int)FATAL);
  if ( level >= printLevel() )  {
    va_list args;
    va_start(args, fmt);
    dd4hep::printout((PrintLevel)level, m_name.c_str(), fmt, args);
    va_end(args);
  }
}

/// Support of debug messages.
void DigiAction::debug(const char* fmt, ...) const {
  va_list args;
  va_start(args, fmt);
  dd4hep::printout(dd4hep::DEBUG, m_name, fmt, args);
  va_end(args);
}

/// Support of info messages.
void DigiAction::info(const char* fmt, ...) const {
  va_list args;
  va_start(args, fmt);
  dd4hep::printout(dd4hep::INFO, m_name, fmt, args);
  va_end(args);
}

/// Support of warning messages.
void DigiAction::warning(const char* fmt, ...) const {
  va_list args;
  va_start(args, fmt);
  dd4hep::printout(dd4hep::WARNING, m_name, fmt, args);
  va_end(args);
}

/// Action to support error messages.
void DigiAction::error(const char* fmt, ...) const {
  va_list args;
  va_start(args, fmt);
  dd4hep::printout(dd4hep::ERROR, m_name, fmt, args);
  va_end(args);
}

/// Action to support error messages.
bool DigiAction::return_error(bool return_value, const char* fmt, ...) const {
  va_list args;
  va_start(args, fmt);
  dd4hep::printout(dd4hep::ERROR, m_name, fmt, args);
  va_end(args);
  return return_value;
}

/// Support of fatal messages. Throws exception if required.
void DigiAction::fatal(const char* fmt, ...) const {
  va_list args;
  va_start(args, fmt);
  dd4hep::printout(dd4hep::FATAL, m_name, fmt, args);
  va_end(args);
}

/// Support of exceptions: Print fatal message and throw runtime_error.
void DigiAction::except(const char* fmt, ...) const {
  va_list args;
  va_start(args, fmt);
  string err = dd4hep::format(m_name, fmt, args);
  dd4hep::printout(dd4hep::FATAL, m_name, err.c_str());
  va_end(args);
  throw runtime_error(err);
}
