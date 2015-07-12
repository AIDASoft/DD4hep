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
#include "DDG4/Geant4EventAction.h"
// C/C++ include files
#include <stdexcept>

using namespace std;
using namespace DD4hep::Simulation;

/// Standard constructor
Geant4EventAction::Geant4EventAction(Geant4Context* ctxt, const string& nam)
  : Geant4Action(ctxt, nam) {
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
Geant4EventActionSequence::Geant4EventActionSequence(Geant4Context* ctxt, const string& nam)
  : Geant4Action(ctxt, nam) {
  m_needsControl = true;
  InstanceCount::increment(this);
}

/// Default destructor
Geant4EventActionSequence::~Geant4EventActionSequence() {
  m_actors(&Geant4Action::release);
  m_actors.clear();
  m_begin.clear();
  m_end.clear();
  m_final.clear();
  InstanceCount::decrement(this);
}

/// Add an actor responding to all callbacks. Sequence takes ownership.
void Geant4EventActionSequence::adopt(Geant4EventAction* action) {
  if (action) {
    action->addRef();
    m_actors.add(action);
    return;
  }
  throw runtime_error("Geant4EventActionSequence: Attempt to add invalid actor!");
}

/// Pre-track action callback
void Geant4EventActionSequence::begin(const G4Event* event) {
  m_actors(ContextUpdate(context()));
  m_actors(&Geant4EventAction::begin, event);
  m_begin(event);
}

/// Post-track action callback
void Geant4EventActionSequence::end(const G4Event* event) {
  m_end(event);
  m_actors(&Geant4EventAction::end, event);
  m_final(event);
  m_actors(ContextUpdate());
}
