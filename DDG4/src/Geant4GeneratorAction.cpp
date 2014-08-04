// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4GeneratorAction.h"
// C/C++ include files
#include <stdexcept>

using namespace DD4hep::Simulation;

/// Standard constructor
Geant4GeneratorAction::Geant4GeneratorAction(Geant4Context* context, const std::string& name)
    : Geant4Action(context, name) {
  InstanceCount::increment(this);
}

/// Default destructor
Geant4GeneratorAction::~Geant4GeneratorAction() {
  InstanceCount::decrement(this);
}

/// Standard constructor
Geant4GeneratorActionSequence::Geant4GeneratorActionSequence(Geant4Context* context, const std::string& name)
    : Geant4Action(context, name) {
  m_needsControl = true;
  InstanceCount::increment(this);
}

/// Default destructor
Geant4GeneratorActionSequence::~Geant4GeneratorActionSequence() {
  m_actors(&Geant4GeneratorAction::release);
  m_actors.clear();
  m_calls.clear();
  InstanceCount::decrement(this);
}

/// Add an actor responding to all callbacks. Sequence takes ownership.
void Geant4GeneratorActionSequence::adopt(Geant4GeneratorAction* action) {
  if (action) {
    action->addRef();
    m_actors.add(action);
    return;
  }
  throw std::runtime_error("Geant4GeneratorActionSequence: Attempt to add invalid actor!");
}

/// Generator callback
void Geant4GeneratorActionSequence::operator()(G4Event* event) {
  m_actors(ContextUpdate(context()));
  m_actors(&Geant4GeneratorAction::operator(), event);
  m_calls(event);
  m_actors(ContextUpdate(0));
}
