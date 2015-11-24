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
// Geant4 headers
#include "G4Threading.hh"
#include "G4AutoLock.hh"
// C/C++ include files
#include <stdexcept>

using namespace std;
using namespace DD4hep::Simulation;

namespace {
  G4Mutex event_action_mutex=G4MUTEX_INITIALIZER;
}

/// Standard constructor
Geant4EventAction::Geant4EventAction(Geant4Context* ctxt, const string& nam)
  : Geant4Action(ctxt, nam) 
{
  InstanceCount::increment(this);
}

/// Default destructor
Geant4EventAction::~Geant4EventAction() {
  InstanceCount::decrement(this);
}

/// begin-of-event callback
void Geant4EventAction::begin(const G4Event* ) {
}

/// End-of-event callback
void Geant4EventAction::end(const G4Event* ) {
}

/// Standard constructor
Geant4SharedEventAction::Geant4SharedEventAction(Geant4Context* ctxt, const string& nam)
  : Geant4EventAction(ctxt, nam)
{
  InstanceCount::increment(this);
}

/// Default destructor
Geant4SharedEventAction::~Geant4SharedEventAction()   {
  releasePtr(m_action);
  InstanceCount::decrement(this);
}

/// Set or update client for the use in a new thread fiber
void Geant4SharedEventAction::configureFiber(Geant4Context* thread_context)   {
  m_action->configureFiber(thread_context);
}

/// Underlying object to be used during the execution of this thread
void Geant4SharedEventAction::use(Geant4EventAction* action)   {
  if (action) {
    action->addRef();
    m_properties.adopt(action->properties());
    m_action = action;
    return;
  }
  throw runtime_error("Geant4SharedEventAction: Attempt to use invalid actor!");
}

/// Begin-of-event callback
void Geant4SharedEventAction::begin(const G4Event* event)   {
  if ( m_action )  {
    G4AutoLock protection_lock(&event_action_mutex);    {
      ContextSwap swap(m_action,context());
      m_action->begin(event);
    }
  }
}

/// End-of-event callback
void Geant4SharedEventAction::end(const G4Event* event)   {
  if ( m_action )  {
    G4AutoLock protection_lock(&event_action_mutex);  {
      ContextSwap swap(m_action,context());
      m_action->end(event);
    }
  }
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

/// Set or update client context
void Geant4EventActionSequence::updateContext(Geant4Context* ctxt)    {
  m_context = ctxt;
  m_actors.updateContext(ctxt);
}

/// Set or update client for the use in a new thread fiber
void Geant4EventActionSequence::configureFiber(Geant4Context* thread_context)   {
  m_actors(&Geant4Action::configureFiber, thread_context);
}

/// Get an action by name
Geant4EventAction* Geant4EventActionSequence::get(const string& nam) const   {
  return m_actors.get(FindByName(TypeName::split(nam).second));
}

/// Add an actor responding to all callbacks. Sequence takes ownership.
void Geant4EventActionSequence::adopt(Geant4EventAction* action) {
  if (action) {
    G4AutoLock protection_lock(&event_action_mutex);
    action->addRef();
    m_actors.add(action);
    return;
  }
  throw runtime_error("Geant4EventActionSequence: Attempt to add invalid actor!");
}

/// Pre-track action callback
void Geant4EventActionSequence::begin(const G4Event* event)   {
  m_actors(&Geant4EventAction::begin, event);
  m_begin(event);
}

/// Post-track action callback
void Geant4EventActionSequence::end(const G4Event* event)   {
  m_end(event);
  m_actors(&Geant4EventAction::end, event);
  m_final(event);
}
