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
#include "DDG4/Geant4GeneratorAction.h"

// C/C++ include files
#include <stdexcept>

using namespace DD4hep::Simulation;

/// Standard constructor
Geant4GeneratorAction::Geant4GeneratorAction(Geant4Context* ctxt, const std::string& nam)
  : Geant4Action(ctxt, nam) {
  InstanceCount::increment(this);
}

/// Default destructor
Geant4GeneratorAction::~Geant4GeneratorAction() {
  InstanceCount::decrement(this);
}

/// Standard constructor
Geant4GeneratorActionSequence::Geant4GeneratorActionSequence(Geant4Context* ctxt, const std::string& nam)
  : Geant4Action(ctxt, nam) {
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
