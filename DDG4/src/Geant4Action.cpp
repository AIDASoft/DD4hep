// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#include <algorithm>
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4Context.h"
#include "DDG4/Geant4Action.h"
#include "DDG4/Geant4Kernel.h"
#include "DDG4/Geant4UIMessenger.h"

// Geant4 include files
#include "G4UIdirectory.hh"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;

TypeName TypeName::split(const string& type_name, const std::string& delim) {
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
Geant4Action::Geant4Action(Geant4Context* context, const string& nam)
    : m_context(0), 
      m_control(0), m_outputLevel(INFO), m_needsControl(false), m_name(nam), m_refCount(1) {
  InstanceCount::increment(this);
  if ( context ) m_context = *context;
  m_outputLevel = context ? context->kernel().getOutputLevel(nam) : (printLevel()-1);
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
    cout << "Geant4Action: Deleting object " << name() 
	 << " of type " << typeName(typeid(*this)) 
	 << " Ptr:" << (void*)this
	 << endl;
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
bool Geant4Action::hasProperty(const std::string& name) const    {
  return m_properties.exists(name);
}

/// Access single property
Property& Geant4Action::property(const std::string& name)   {
  return properties()[name];
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
void Geant4Action::print(const std::string& fmt, ...) const   {
  if ( outputLevel() >= printLevel() )  {
    va_list args;
    va_start(args, fmt);
    DD4hep::printout((PrintLevel)outputLevel(),m_name, fmt, args);
    va_end(args);
  }
}

/// Support for messages with variable output level using output level
void Geant4Action::print(const std::string& tag, const std::string& fmt, ...) const  {
  if ( outputLevel() >= printLevel() )  {
    va_list args;
    va_start(args, fmt);
    DD4hep::printout((PrintLevel)outputLevel(),tag, fmt, args);
    va_end(args);
  }
}

/// Support of debug messages.
void Geant4Action::debug(const string& fmt, ...) const {
  va_list args;
  va_start(args, fmt);
  DD4hep::printout(DD4hep::DEBUG, "Geant4Action", fmt, args);
  va_end(args);
}

/// Support for messages with variable output level using output level
void Geant4Action::debug(const std::string& tag, const std::string& fmt, ...) const  {
  va_list args;
  va_start(args, fmt);
  DD4hep::printout(DD4hep::DEBUG, tag, fmt, args);
  va_end(args);
}

/// Support of info messages.
void Geant4Action::info(const string& fmt, ...) const {
  va_list args;
  va_start(args, fmt);
  DD4hep::printout(DD4hep::INFO, "Geant4Action", fmt, args);
  va_end(args);
}

/// Support for messages with variable output level using output level
void Geant4Action::info(const std::string& tag, const std::string& fmt, ...) const  {
  va_list args;
  va_start(args, fmt);
  DD4hep::printout(DD4hep::INFO, tag, fmt, args);
  va_end(args);
}

/// Support of warning messages.
void Geant4Action::warning(const string& fmt, ...) const {
  va_list args;
  va_start(args, fmt);
  DD4hep::printout(DD4hep::WARNING, "Geant4Action", fmt, args);
  va_end(args);
}

/// Support for messages with variable output level using output level
void Geant4Action::warning(const std::string& tag, const std::string& fmt, ...) const  {
  va_list args;
  va_start(args, fmt);
  DD4hep::printout(DD4hep::WARNING, tag, fmt, args);
  va_end(args);
}

/// Action to support error messages.
void Geant4Action::error(const string& fmt, ...) const {
  va_list args;
  va_start(args, fmt);
  DD4hep::printout(DD4hep::ERROR, "Geant4Action", fmt, args);
  va_end(args);
}

/// Support for messages with variable output level using output level
void Geant4Action::error(const std::string& tag, const std::string& fmt, ...) const  {
  va_list args;
  va_start(args, fmt);
  DD4hep::printout(DD4hep::ERROR, tag, fmt, args);
  va_end(args);
}

/// Action to support error messages.
bool Geant4Action::error(bool return_value, const string& fmt, ...) const {
  va_list args;
  va_start(args, fmt);
  DD4hep::printout(DD4hep::ERROR, "Geant4Action", fmt, args);
  va_end(args);
  return return_value;
}

/// Support of fatal messages. Throws exception if required.
void Geant4Action::fatal(const string& fmt, ...) const {
  string err;
  va_list args;
  va_start(args, fmt);
  DD4hep::printout(DD4hep::FATAL, "Geant4Action", fmt, args);
  va_end(args);
}

/// Support for messages with variable output level using output level
void Geant4Action::fatal(const std::string& tag, const std::string& fmt, ...) const  {
  va_list args;
  va_start(args, fmt);
  DD4hep::printout(DD4hep::FATAL, tag, fmt, args);
  va_end(args);
}

/// Support of exceptions: Print fatal message and throw runtime_error.
void Geant4Action::except(const string& fmt, ...) const {
  string err;
  va_list args;
  va_start(args, fmt);
  DD4hep::printout(DD4hep::FATAL, "Geant4Action", fmt, args);
  err = DD4hep::format("Geant4Action", fmt, args);
  va_end(args);
  throw runtime_error(err);
}

/// Support for messages with variable output level using output level
void Geant4Action::except(const std::string& tag, const std::string& fmt, ...) const  {
  va_list args;
  va_start(args, fmt);
  DD4hep::printout(DD4hep::FATAL, tag, fmt, args);
  va_end(args);
}

/// Abort Geant4 Run by throwing a G4Exception with type RunMustBeAborted
void Geant4Action::abortRun(const string& exception, const string& fmt, ...) const {
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
