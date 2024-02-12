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
#include <DDG4/Geant4StackingAction.h>

// Geant4 headers
#include <G4Threading.hh>
#include <G4AutoLock.hh>

using namespace dd4hep::sim;
namespace {
  G4Mutex action_mutex=G4MUTEX_INITIALIZER;
}

/// Standard constructor
Geant4StackingAction::Geant4StackingAction(Geant4Context* ctxt, const std::string& nam)
  : Geant4Action(ctxt, nam) {
  InstanceCount::increment(this);
}

/// Default destructor
Geant4StackingAction::~Geant4StackingAction() {
  InstanceCount::decrement(this);
}

/// Classify new track: The first call in the sequence returning non-null pointer wins!
TrackClassification 
Geant4StackingAction::classifyNewTrack(G4StackManager* /* manager */, const G4Track* /* track */)  {
  return TrackClassification();
}

/// Standard constructor
Geant4SharedStackingAction::Geant4SharedStackingAction(Geant4Context* ctxt, const std::string& nam)
  : Geant4StackingAction(ctxt, nam), m_action(0)
{
  InstanceCount::increment(this);
}

/// Default destructor
Geant4SharedStackingAction::~Geant4SharedStackingAction()   {
  detail::releasePtr(m_action);
  InstanceCount::decrement(this);
}

/// Set or update client for the use in a new thread fiber
void Geant4SharedStackingAction::configureFiber(Geant4Context* thread_context)   {
  m_action->configureFiber(thread_context);
}

/// Underlying object to be used during the execution of this thread
void Geant4SharedStackingAction::use(Geant4StackingAction* action)   {
  if (action) {
    action->addRef();
    m_properties.adopt(action->properties());
    m_action = action;
    return;
  }
  except("Attempt to use invalid actor!");
}

/// Begin-of-stacking callback
void Geant4SharedStackingAction::newStage(G4StackManager* stackManager)  {
  if ( m_action )  {
    G4AutoLock protection_lock(&action_mutex);    {
      ContextSwap swap(m_action,context());
      m_action->newStage(stackManager);
    }
  }
}

/// End-of-stacking callback
void Geant4SharedStackingAction::prepare(G4StackManager* stackManager)  {
  if ( m_action )  {
    G4AutoLock protection_lock(&action_mutex);  {
      ContextSwap swap(m_action,context());
      m_action->prepare(stackManager);
    }
  }
}

/// Classify new track with delegation
TrackClassification 
Geant4SharedStackingAction::classifyNewTrack(G4StackManager* stackManager,
                                             const G4Track* track)   {
  if ( m_action )  {
    G4AutoLock protection_lock(&action_mutex);  {
      ContextSwap swap(m_action,context());
      return m_action->classifyNewTrack(stackManager, track);
    }
  }
  return {};
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
  except("Attempt to add invalid actor!");
}

/// Set or update client context
void Geant4StackingActionSequence::updateContext(Geant4Context* ctxt)    {
  m_context = ctxt;
  m_actors.updateContext(ctxt);
}

/// Set or update client for the use in a new thread fiber
void Geant4StackingActionSequence::configureFiber(Geant4Context* thread_context)   {
  m_actors(&Geant4Action::configureFiber, thread_context);
}

/// Get an action by name
Geant4StackingAction* Geant4StackingActionSequence::get(const std::string& nam) const   {
  return m_actors.get(FindByName(TypeName::split(nam).second));
}

/// Pre-track action callback
void Geant4StackingActionSequence::newStage(G4StackManager* stackManager) {
  m_actors(&Geant4StackingAction::newStage, stackManager);
  m_newStage(stackManager);
}

/// Post-track action callback
void Geant4StackingActionSequence::prepare(G4StackManager* stackManager) {
  m_actors(&Geant4StackingAction::prepare, stackManager);
  m_prepare(stackManager);
}

/// Classify new track: The first call in the sequence returning non-null pointer wins!
TrackClassification 
Geant4StackingActionSequence::classifyNewTrack(G4StackManager* stackManager,
                                               const G4Track* track)   {
  for( auto a : m_actors )   {
    auto ret = a->classifyNewTrack(stackManager, track);
    if ( ret.type != NoTrackClassification )  {
      return ret;
    }
  }
  return {};
}
