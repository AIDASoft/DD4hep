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
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4SteppingAction.h"

using namespace DD4hep::Simulation;

/// Standard constructor
Geant4SteppingAction::Geant4SteppingAction(Geant4Context* ctxt, const std::string& nam)
: Geant4Action(ctxt, nam) {
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
Geant4SteppingActionSequence::Geant4SteppingActionSequence(Geant4Context* ctxt, const std::string& nam)
: Geant4Action(ctxt, nam) {
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
