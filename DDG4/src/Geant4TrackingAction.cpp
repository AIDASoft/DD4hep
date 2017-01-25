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
#include "DDG4/Geant4TrackingAction.h"
#include "DDG4/Geant4MonteCarloTruth.h"
#include "DDG4/Geant4TrackInformation.h"

// Geant4 include files
#include "G4Track.hh"
#include "G4Threading.hh"
#include "G4AutoLock.hh"
#include "G4TrackingManager.hh"
#include "G4VUserTrackInformation.hh"

// C/C++ include files
#include <stdexcept>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;
class G4Step;
class G4TouchableHistory;
namespace {
  G4Mutex action_mutex=G4MUTEX_INITIALIZER;
}

/// Standard constructor
Geant4TrackingActionSequence::Geant4TrackingActionSequence(Geant4Context* ctxt, const string& nam)
  : Geant4Action(ctxt, nam) {
  m_needsControl = true;
  InstanceCount::increment(this);
}

/// Default destructor
Geant4TrackingActionSequence::~Geant4TrackingActionSequence() {
  m_actors(&Geant4TrackingAction::release);
  m_actors.clear();
  m_front.clear();
  m_final.clear();
  m_begin.clear();
  m_end.clear();
  InstanceCount::decrement(this);
}

/// Set or update client context
void Geant4TrackingActionSequence::updateContext(Geant4Context* ctxt)    {
  m_context = ctxt;
  m_actors.updateContext(ctxt);
}

/// Set or update client for the use in a new thread fiber
void Geant4TrackingActionSequence::configureFiber(Geant4Context* thread_context)   {
  m_actors(&Geant4Action::configureFiber, thread_context);
}

/// Get an action by name
Geant4TrackingAction* Geant4TrackingActionSequence::get(const string& nam) const   {
  return m_actors.get(FindByName(TypeName::split(nam).second));
}

/// Add an actor responding to all callbacks. Sequence takes ownership.
void Geant4TrackingActionSequence::adopt(Geant4TrackingAction* action) {
  if (action) {
    G4AutoLock protection_lock(&action_mutex);
    action->addRef();
    m_actors.add(action);
    return;
  }
  throw runtime_error("Geant4TrackingActionSequence: Attempt to add invalid actor!");
}

/// Pre-track action callback
void Geant4TrackingActionSequence::begin(const G4Track* track) {
  m_front(track);
  m_actors(&Geant4TrackingAction::begin, track);
  m_begin(track);
}

/// Post-track action callback
void Geant4TrackingActionSequence::end(const G4Track* track) {
  m_end(track);
  m_actors(&Geant4TrackingAction::end, track);
  m_final(track);
}

/// Standard constructor
Geant4TrackingAction::Geant4TrackingAction(Geant4Context* ctxt, const string& nam)
  : Geant4Action(ctxt, nam) {
  InstanceCount::increment(this);
}

/// Default destructor
Geant4TrackingAction::~Geant4TrackingAction() {
  InstanceCount::decrement(this);
}

/// Pre-track action callback
void Geant4TrackingAction::begin(const G4Track*) {
}

/// Post-track action callback
void Geant4TrackingAction::end(const G4Track*) {
}

/// Mark the track to be kept for MC truth propagation
void Geant4TrackingAction::mark(const G4Track* track) const    {
  Geant4MonteCarloTruth* truth = context()->event().extension<Geant4MonteCarloTruth>(false);
  if ( truth ) truth->mark(track,true);
}

/// Standard constructor
Geant4SharedTrackingAction::Geant4SharedTrackingAction(Geant4Context* ctxt, const string& nam)
  : Geant4TrackingAction(ctxt, nam), m_action(0)
{
  InstanceCount::increment(this);
}

/// Default destructor
Geant4SharedTrackingAction::~Geant4SharedTrackingAction()   {
  releasePtr(m_action);
  InstanceCount::decrement(this);
}

/// Set or update client for the use in a new thread fiber
void Geant4SharedTrackingAction::configureFiber(Geant4Context* thread_context)   {
  m_action->configureFiber(thread_context);
}

/// Underlying object to be used during the execution of this thread
void Geant4SharedTrackingAction::use(Geant4TrackingAction* action)   {
  if (action) {
    action->addRef();
    m_properties.adopt(action->properties());
    m_action = action;
    return;
  }
  throw runtime_error("Geant4SharedTrackingAction: Attempt to use invalid actor!");
}

/// Begin-of-track callback
void Geant4SharedTrackingAction::begin(const G4Track* track)   {
  if ( m_action )  {
    G4AutoLock protection_lock(&action_mutex);    {
      ContextSwap swap(m_action,context());
      m_action->begin(track);
    }
  }
}

/// End-of-track callback
void Geant4SharedTrackingAction::end(const G4Track* track)   {
  if ( m_action )  {
    G4AutoLock protection_lock(&action_mutex);  {
      ContextSwap swap(m_action,context());
      m_action->end(track);
    }
  }
}
