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
#include "DDG4/Geant4SteppingAction.h"
using namespace DD4hep::Simulation;

/// Standard constructor
Geant4SteppingAction::Geant4SteppingAction(Geant4Context* context, const std::string& name)
: Geant4Action(context, name) {
  InstanceCount::increment(this);
}

/// Default destructor
Geant4SteppingAction::~Geant4SteppingAction() {
  InstanceCount::decrement(this);
}

/// User stepping callback
void Geant4SteppingAction::operator()(const G4Step*, G4SteppingManager*) {
}

/// Standard constructor
Geant4SteppingActionSequence::Geant4SteppingActionSequence(Geant4Context* context, const std::string& name)
: Geant4Action(context, name) {
  m_needsControl = true;
  InstanceCount::increment(this);
}

/// Default destructor
Geant4SteppingActionSequence::~Geant4SteppingActionSequence() {
  m_actors(&Geant4SteppingAction::release);
  m_actors.clear();
  m_calls.clear();
  InstanceCount::decrement(this);
}

/// Pre-track action callback
void Geant4SteppingActionSequence::operator()(const G4Step* step, G4SteppingManager* mgr) {
  m_actors(ContextUpdate(context()));
  m_actors(&Geant4SteppingAction::operator(), step, mgr);
  m_calls(step, mgr);
  m_actors(ContextUpdate());
}

/// Add an actor responding to all callbacks. Sequence takes ownership.
void Geant4SteppingActionSequence::adopt(Geant4SteppingAction* action) {
  if (action) {
    action->addRef();
    m_actors.add(action);
    return;
  }
  throw std::runtime_error("Geant4SteppingActionSequence: Attempt to add invalid actor!");
}
