//==========================================================================
//  AIDA Detector description implementation 
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
#include <DD4hep/InstanceCount.h>
#include <DDG4/Geant4GeneratorAction.h>
#include <DDG4/Geant4Kernel.h>

// Geant4 headers
#include <G4Threading.hh>
#include <G4AutoLock.hh>

using namespace dd4hep::sim;

namespace {
  G4Mutex action_mutex=G4MUTEX_INITIALIZER;
}

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
Geant4SharedGeneratorAction::Geant4SharedGeneratorAction(Geant4Context* ctxt, const std::string& nam)
  : Geant4GeneratorAction(ctxt, nam), m_action(0)
{
  InstanceCount::increment(this);
}

/// Default destructor
Geant4SharedGeneratorAction::~Geant4SharedGeneratorAction()   {
  detail::releasePtr(m_action);
  InstanceCount::decrement(this);
}

/// Set or update client for the use in a new thread fiber
void Geant4SharedGeneratorAction::configureFiber(Geant4Context* thread_context)   {
  m_action->configureFiber(thread_context);
}

/// Underlying object to be used during the execution of this thread
void Geant4SharedGeneratorAction::use(Geant4GeneratorAction* action)   {
  if (action) {
    action->addRef();
    m_properties.adopt(action->properties());
    m_action = action;
    return;
  }
  throw std::runtime_error("Geant4SharedGeneratorAction: Attempt to use invalid actor!");
}

/// User generator callback
void Geant4SharedGeneratorAction::operator()(G4Event* event)  {
  if ( m_action )  {
    G4AutoLock protection_lock(&action_mutex);    {
      ContextSwap swap(m_action,context());
      (*m_action)(event);
    }
  }
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

/// Set or update client context
void Geant4GeneratorActionSequence::updateContext(Geant4Context* ctxt)    {
  m_context = ctxt;
  m_actors.updateContext(ctxt);
}

/// Set or update client for the use in a new thread fiber
void Geant4GeneratorActionSequence::configureFiber(Geant4Context* thread_context)   {
  m_actors(&Geant4Action::configureFiber, thread_context);
}

/// Get an action by name
Geant4GeneratorAction* Geant4GeneratorActionSequence::get(const std::string& nam) const   {
  return m_actors.get(FindByName(TypeName::split(nam).second));
}

/// Add an actor responding to all callbacks. Sequence takes ownership.
void Geant4GeneratorActionSequence::adopt(Geant4GeneratorAction* action) {
  if (action) {
    G4AutoLock protection_lock(&action_mutex);
    action->addRef();
    m_actors.add(action);
    return;
  }
  except("Attempt to add invalid actor!");
}

/// Generator callback
void Geant4GeneratorActionSequence::operator()(G4Event* event) {
  if ( context()->kernel().processEvents() )  {
    m_actors(&Geant4GeneratorAction::operator(), event);
    m_calls(event);
    return;
  }
  throw DD4hep_Stop_Processing();
}
