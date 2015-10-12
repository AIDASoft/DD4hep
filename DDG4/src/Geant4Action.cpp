// $Id: $
//==========================================================================
//  AIDA Detector description implementation for LCD
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
#include "DDG4/Geant4Context.h"
#include "DDG4/Geant4Action.h"
#include "DDG4/Geant4Kernel.h"
#include "DDG4/Geant4UIMessenger.h"

// Geant4 include files
#include "G4UIdirectory.hh"

// C/C++ include files
#include <algorithm>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;

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

void Geant4Action::ContextUpdate::operator()(Geant4Action* action) const  {
  if ( context )  {
    action->m_context.setRun(context->runPtr());
    action->m_context.setEvent(context->eventPtr());
  }
#if 0
  else  {
    action->m_context.setRun(0);
    action->m_context.setEvent(0);
  }
#endif
}

/// Standard constructor
Geant4Action::Geant4Action(Geant4Context* ctxt, const string& nam)
  : m_context(0),
    m_control(0), m_outputLevel(INFO), m_needsControl(false), m_name(nam), m_refCount(1) {
  InstanceCount::increment(this);
  if ( ctxt ) m_context = *ctxt;
  m_outputLevel = ctxt ? ctxt->kernel().getOutputLevel(nam) : (printLevel()-1);
  declareProperty("Name", m_name);
  declareProperty("name", m_name);
  declareProperty("OutputLevel", m_outputLevel);
  declareProperty("Control", m_needsControl);
}

/// Default destructor
Geant4Action::~Geant4Action() {
  InstanceCount::decrement(this);
}

/// Implicit destruction
long Geant4Action::addRef() {
  return ++m_refCount;
}

/// Decrease reference count. Implicit destruction
long Geant4Action::release() {
  long count = --m_refCount;
  if (m_refCount <= 0) {
    printM1("Geant4Action: Deleting object %s of type %s Pointer:%p",
            m_name.c_str(),typeName(typeid(*this)).c_str(),(void*)this);
    delete this;
  }
  return count;
}

/// Set the output level; returns previous value
PrintLevel Geant4Action::setOutputLevel(PrintLevel new_level)  {
  int old = m_outputLevel;
  m_outputLevel = new_level;
  return (PrintLevel)old;
}

/// Set object properties
Geant4Action& Geant4Action::setProperties(PropertyConfigurator& setup) {
  m_properties.set(m_name, setup);
  return *this;
}

/// Check property for existence
bool Geant4Action::hasProperty(const string& nam) const    {
  return m_properties.exists(nam);
}

/// Access single property
Property& Geant4Action::property(const string& nam)   {
  return properties()[nam];
}

/// Install all control messenger if wanted
void Geant4Action::installMessengers() {
  //m_needsControl = true;
  if (m_needsControl && !m_control) {
    string path = context()->kernel().directoryName();
    path += name() + "/";
    m_control = new Geant4UIMessenger(name(), path);
    installPropertyMessenger();
    installCommandMessenger();
  }
}

/// Install property control messenger if wanted
void Geant4Action::installPropertyMessenger() {
  m_control->exportProperties(m_properties);
}

/// Install command control messenger if wanted
void Geant4Action::installCommandMessenger()   {
}

/// Access to the UI messenger
Geant4UIMessenger* Geant4Action::control() const   {
  if ( m_control )   {
    return m_control;
  }
  except("No control was installed for this action item.");
  return 0;
}

/// Enable and install UI messenger
void Geant4Action::enableUI()   {
  m_needsControl = true;
  installMessengers();
}

/// Support for messages with variable output level using output level
void Geant4Action::print(const char* fmt, ...) const   {
  int level = outputLevel();
  if ( level >= printLevel() )  {
    va_list args;
    va_start(args, fmt);
    DD4hep::printout((PrintLevel)level, m_name.c_str(), fmt, args);
    va_end(args);
  }
}

/// Support for messages with variable output level using output level-1
void Geant4Action::printM1(const char* fmt, ...) const   {
  int level = max(outputLevel()-1,(int)VERBOSE);
  if ( level >= printLevel() )  {
    va_list args;
    va_start(args, fmt);
    DD4hep::printout((PrintLevel)level, m_name.c_str(), fmt, args);
    va_end(args);
  }
}

/// Support for messages with variable output level using output level-2
void Geant4Action::printM2(const char* fmt, ...) const   {
  int level = max(outputLevel()-2,(int)VERBOSE);
  if ( level >= printLevel() )  {
    va_list args;
    va_start(args, fmt);
    DD4hep::printout((PrintLevel)level, m_name.c_str(), fmt, args);
    va_end(args);
  }
}

/// Support for messages with variable output level using output level-1
void Geant4Action::printP1(const char* fmt, ...) const   {
  int level = min(outputLevel()+1,(int)FATAL);
  if ( level >= printLevel() )  {
    va_list args;
    va_start(args, fmt);
    DD4hep::printout((PrintLevel)level, m_name.c_str(), fmt, args);
    va_end(args);
  }
}

/// Support for messages with variable output level using output level-2
void Geant4Action::printP2(const char* fmt, ...) const   {
  int level = min(outputLevel()+2,(int)FATAL);
  if ( level >= printLevel() )  {
    va_list args;
    va_start(args, fmt);
    DD4hep::printout((PrintLevel)level, m_name.c_str(), fmt, args);
    va_end(args);
  }
}

/// Support of debug messages.
void Geant4Action::debug(const char* fmt, ...) const {
  va_list args;
  va_start(args, fmt);
  DD4hep::printout(DD4hep::DEBUG, m_name, fmt, args);
  va_end(args);
}

/// Support of info messages.
void Geant4Action::info(const char* fmt, ...) const {
  va_list args;
  va_start(args, fmt);
  DD4hep::printout(DD4hep::INFO, m_name, fmt, args);
  va_end(args);
}

/// Support of warning messages.
void Geant4Action::warning(const char* fmt, ...) const {
  va_list args;
  va_start(args, fmt);
  DD4hep::printout(DD4hep::WARNING, m_name, fmt, args);
  va_end(args);
}

/// Action to support error messages.
void Geant4Action::error(const char* fmt, ...) const {
  va_list args;
  va_start(args, fmt);
  DD4hep::printout(DD4hep::ERROR, m_name, fmt, args);
  va_end(args);
}

/// Action to support error messages.
bool Geant4Action::error(bool return_value, const char* fmt, ...) const {
  va_list args;
  va_start(args, fmt);
  DD4hep::printout(DD4hep::ERROR, m_name, fmt, args);
  va_end(args);
  return return_value;
}

/// Support of fatal messages. Throws exception if required.
void Geant4Action::fatal(const char* fmt, ...) const {
  va_list args;
  va_start(args, fmt);
  DD4hep::printout(DD4hep::FATAL, m_name, fmt, args);
  va_end(args);
}

/// Support of exceptions: Print fatal message and throw runtime_error.
void Geant4Action::except(const char* fmt, ...) const {
  va_list args;
  va_start(args, fmt);
  DD4hep::printout(DD4hep::FATAL, m_name, fmt, args);
  string err = DD4hep::format(m_name, fmt, args);
  va_end(args);
  throw runtime_error(err);
}

/// Abort Geant4 Run by throwing a G4Exception with type RunMustBeAborted
void Geant4Action::abortRun(const string& exception, const char* fmt, ...) const {
  string desc, typ = typeName(typeid(*this));
  string issuer = name()+" ["+typ+"]";
  va_list args;
  va_start(args, fmt);
  desc = DD4hep::format("*** Geant4Action:", fmt, args);
  va_end(args);
  G4Exception(issuer.c_str(),exception.c_str(),RunMustBeAborted,desc.c_str());
  //throw runtime_error(issuer+"> "+desc);
}

/// Access to the main run action sequence from the kernel object
Geant4RunActionSequence& Geant4Action::runAction() const {
  return m_context.kernel().runAction();
}

/// Access to the main event action sequence from the kernel object
Geant4EventActionSequence& Geant4Action::eventAction() const {
  return m_context.kernel().eventAction();
}

/// Access to the main stepping action sequence from the kernel object
Geant4SteppingActionSequence& Geant4Action::steppingAction() const {
  return m_context.kernel().steppingAction();
}

/// Access to the main tracking action sequence from the kernel object
Geant4TrackingActionSequence& Geant4Action::trackingAction() const {
  return m_context.kernel().trackingAction();
}

/// Access to the main stacking action sequence from the kernel object
Geant4StackingActionSequence& Geant4Action::stackingAction() const {
  return m_context.kernel().stackingAction();
}

/// Access to the main generator action sequence from the kernel object
Geant4GeneratorActionSequence& Geant4Action::generatorAction() const {
  return m_context.kernel().generatorAction();
}
