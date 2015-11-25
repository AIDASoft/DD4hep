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
// Geant4 headers
#include "G4Threading.hh"
#include "G4AutoLock.hh"
// C/C++ include files
#include <stdexcept>

using namespace std;
using namespace DD4hep::Simulation;
namespace {
  G4Mutex action_mutex=G4MUTEX_INITIALIZER;
}

/// Standard constructor
Geant4SteppingAction::Geant4SteppingAction(Geant4Context* ctxt, const string& nam)
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
Geant4SharedSteppingAction::Geant4SharedSteppingAction(Geant4Context* ctxt, const string& nam)
  : Geant4SteppingAction(ctxt, nam)
{
  InstanceCount::increment(this);
}

/// Default destructor
Geant4SharedSteppingAction::~Geant4SharedSteppingAction()   {
  releasePtr(m_action);
  InstanceCount::decrement(this);
}

/// Underlying object to be used during the execution of this thread
void Geant4SharedSteppingAction::use(Geant4SteppingAction* action)   {
  if (action) {
    action->addRef();
    m_properties.adopt(action->properties());
    m_action = action;
    return;
  }
  throw runtime_error("Geant4SharedSteppingAction: Attempt to use invalid actor!");
}

/// Set or update client for the use in a new thread fiber
void Geant4SharedSteppingAction::configureFiber(Geant4Context* thread_context)   {
  m_action->configureFiber(thread_context);
}

/// User stepping callback
void Geant4SharedSteppingAction::operator()(const G4Step* s, G4SteppingManager* m) {
  if ( m_action )  {
    G4AutoLock protection_lock(&action_mutex);    {
      ContextSwap swap(m_action,context());
      (*m_action)(s,m);
    }
  }
}

/// Standard constructor
Geant4SteppingActionSequence::Geant4SteppingActionSequence(Geant4Context* ctxt, const string& nam)
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

/// Set or update client for the use in a new thread fiber
void Geant4SteppingActionSequence::configureFiber(Geant4Context* thread_context)   {
  m_actors(&Geant4Action::configureFiber, thread_context);
}

/// Set or update client context
void Geant4SteppingActionSequence::updateContext(Geant4Context* ctxt)    {
  m_context = ctxt;
  m_actors.updateContext(ctxt);
}

/// Get an action by name
Geant4SteppingAction* Geant4SteppingActionSequence::get(const string& nam) const   {
  return m_actors.get(FindByName(TypeName::split(nam).second));
}

/// Pre-track action callback
void Geant4SteppingActionSequence::operator()(const G4Step* step, G4SteppingManager* mgr) {
  m_actors(&Geant4SteppingAction::operator(), step, mgr);
  m_calls(step, mgr);
}

/// Add an actor responding to all callbacks. Sequence takes ownership.
void Geant4SteppingActionSequence::adopt(Geant4SteppingAction* action) {
  if (action) {
    G4AutoLock protection_lock(&action_mutex);
    action->addRef();
    m_actors.add(action);
    return;
  }
  throw runtime_error("Geant4SteppingActionSequence: Attempt to add invalid actor!");
}
