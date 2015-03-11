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
#include "DDG4/Geant4StackingAction.h"

// C/C++ include files
#include <stdexcept>

using namespace DD4hep::Simulation;

/// Standard constructor
Geant4StackingAction::Geant4StackingAction(Geant4Context* ctxt, const std::string& nam)
: Geant4Action(ctxt, nam) {
  InstanceCount::increment(this);
}

/// Default destructor
Geant4StackingAction::~Geant4StackingAction() {
  InstanceCount::decrement(this);
}

/// Standard constructor
Geant4StackingActionSequence::Geant4StackingActionSequence(Geant4Context* ctxt, const std::string& nam)
: Geant4Action(ctxt, nam) {
  m_needsControl = true;
  InstanceCount::increment(this);
}

/// Default destructor
Geant4StackingActionSequence::~Geant4StackingActionSequence() {
  m_actors(&Geant4StackingAction::release);
  m_actors.clear();
  m_newStage.clear();
  m_prepare.clear();
  InstanceCount::decrement(this);
}

/// Add an actor responding to all callbacks. Sequence takes ownership.
void Geant4StackingActionSequence::adopt(Geant4StackingAction* action) {
  if (action) {
    action->addRef();
    m_actors.add(action);
    return;
  }
  throw std::runtime_error("Geant4StackingActionSequence: Attempt to add invalid actor!");
}

/// Pre-track action callback
void Geant4StackingActionSequence::newStage() {
  m_actors(ContextUpdate(context()));
  m_actors(&Geant4StackingAction::newStage);
  m_newStage();
}

/// Post-track action callback
void Geant4StackingActionSequence::prepare() {
  m_actors(&Geant4StackingAction::prepare);
  m_prepare();
  m_actors(ContextUpdate());
}
