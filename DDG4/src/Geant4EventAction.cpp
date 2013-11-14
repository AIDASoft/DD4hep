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
#include "DDG4/Geant4EventAction.h"
// C/C++ include files
#include <stdexcept>

using namespace DD4hep::Simulation;

/// Standard constructor
Geant4EventAction::Geant4EventAction(Geant4Context* context, const std::string& nam)
    : Geant4Action(context, nam) {
  InstanceCount::increment(this);
}

/// Default destructor
Geant4EventAction::~Geant4EventAction() {
  InstanceCount::decrement(this);
}

/// begin-of-event callback
void Geant4EventAction::begin(const G4Event*) {
}

/// End-of-event callback
void Geant4EventAction::end(const G4Event*) {
}

/// Standard constructor
Geant4EventActionSequence::Geant4EventActionSequence(Geant4Context* context, const std::string& name)
    : Geant4Action(context, name) {
  m_needsControl = true;
  InstanceCount::increment(this);
}

/// Default destructor
Geant4EventActionSequence::~Geant4EventActionSequence() {
  m_actors(&Geant4Action::release);
  m_actors.clear();
  m_begin.clear();
  m_end.clear();
  InstanceCount::decrement(this);
}

/// Add an actor responding to all callbacks. Sequence takes ownership.
void Geant4EventActionSequence::adopt(Geant4EventAction* action) {
  if (action) {
    action->addRef();
    m_actors.add(action);
    return;
  }
  throw std::runtime_error("Geant4EventActionSequence: Attempt to add invalid actor!");
}

/// Pre-track action callback
void Geant4EventActionSequence::begin(const G4Event* event) {
  m_actors(&Geant4EventAction::begin, event);
  m_begin(event);
}

/// Post-track action callback
void Geant4EventActionSequence::end(const G4Event* event) {
  m_end(event);
  m_actors(&Geant4EventAction::end, event);
}
