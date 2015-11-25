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
#include "DDG4/Geant4RunAction.h"
// Geant4 headers
#include "G4Threading.hh"
#include "G4AutoLock.hh"
// C/C++ include files
#include <stdexcept>

using namespace std;
using namespace DD4hep::Simulation;

namespace {
  G4Mutex action_mutex=G4MUTEX_INITIALIZER;
  G4Mutex sequence_mutex=G4MUTEX_INITIALIZER;
}

/// Standard constructor
Geant4RunAction::Geant4RunAction(Geant4Context* ctxt, const string& nam)
  : Geant4Action(ctxt, nam) {
  InstanceCount::increment(this);
}

/// Default destructor
Geant4RunAction::~Geant4RunAction() {
  InstanceCount::decrement(this);
}

/// begin-of-run callback
void Geant4RunAction::begin(const G4Run*) {
}

/// End-of-run callback
void Geant4RunAction::end(const G4Run*) {
}

/// Standard constructor
Geant4SharedRunAction::Geant4SharedRunAction(Geant4Context* ctxt, const string& nam)
  : Geant4RunAction(ctxt, nam)
{
  InstanceCount::increment(this);
}

/// Default destructor
Geant4SharedRunAction::~Geant4SharedRunAction()   {
  releasePtr(m_action);
  InstanceCount::decrement(this);
}

/// Set or update client for the use in a new thread fiber
void Geant4SharedRunAction::configureFiber(Geant4Context* thread_context)   {
  m_action->configureFiber(thread_context);
}

/// Underlying object to be used during the execution of this thread
void Geant4SharedRunAction::use(Geant4RunAction* action)   {
  if (action) {
    action->addRef();
    m_properties.adopt(action->properties());
    m_action = action;
    return;
  }
  throw runtime_error("Geant4SharedRunAction: Attempt to use invalid actor!");
}

/// Begin-of-run callback
void Geant4SharedRunAction::begin(const G4Run* run)   {
  if ( m_action )  {
    G4AutoLock protection_lock(&action_mutex);    {
      ContextSwap swap(m_action,context());
      m_action->begin(run);
    }
  }
}

/// End-of-run callback
void Geant4SharedRunAction::end(const G4Run* run)   {
  if ( m_action )  {
    G4AutoLock protection_lock(&action_mutex);  {
      ContextSwap swap(m_action,context());
      m_action->end(run);
    }
  }
}

/// Standard constructor
Geant4RunActionSequence::Geant4RunActionSequence(Geant4Context* ctxt, const string& nam)
  : Geant4Action(ctxt, nam) {
  m_needsControl = true;
  InstanceCount::increment(this);
}

/// Default destructor
Geant4RunActionSequence::~Geant4RunActionSequence() {
  m_actors(&Geant4RunAction::release);
  m_actors.clear();
  m_begin.clear();
  m_end.clear();
  InstanceCount::decrement(this);
}

/// Set or update client context
void Geant4RunActionSequence::updateContext(Geant4Context* ctxt)    {
  m_context = ctxt;
  m_actors.updateContext(ctxt);
}

/// Set or update client for the use in a new thread fiber
void Geant4RunActionSequence::configureFiber(Geant4Context* thread_context)   {
  m_actors(&Geant4Action::configureFiber, thread_context);
}

/// Get an action by name
Geant4RunAction* Geant4RunActionSequence::get(const string& nam) const   {
  return m_actors.get(FindByName(TypeName::split(nam).second));
}

/// Add an actor responding to all callbacks. Sequence takes ownership.
void Geant4RunActionSequence::adopt(Geant4RunAction* action) {
  if (action) {
    G4AutoLock protection_lock(&action_mutex);
    action->addRef();
    m_actors.add(action);
    return;
  }
  throw runtime_error("Geant4RunActionSequence: Attempt to add invalid actor!");
}

/// Pre-track action callback
void Geant4RunActionSequence::begin(const G4Run* run) {
  G4AutoLock protection_lock(&sequence_mutex);
  m_actors(&Geant4RunAction::begin, run);
  m_begin(run);
}

/// Post-track action callback
void Geant4RunActionSequence::end(const G4Run* run) {
  G4AutoLock protection_lock(&sequence_mutex);
  m_end(run);
  m_actors(&Geant4RunAction::end, run);
}
