// $Id$
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
#include "DDG4/Geant4UserInitialization.h"

// C/C++ include files
#include <stdexcept>

using namespace std;
using namespace DD4hep::Simulation;

/// Standard constructor
Geant4UserInitialization::Geant4UserInitialization(Geant4Context* ctxt, const string& nam)
  : Geant4Action(ctxt, nam) {
  m_needsControl = false;
  InstanceCount::increment(this);
}

/// Default destructor
Geant4UserInitialization::~Geant4UserInitialization() {
  InstanceCount::decrement(this);
}

/// Callback function to build setup for the MT worker thread
void Geant4UserInitialization::build()  const  {
}

/// Callback function to build setup for the MT master thread
void Geant4UserInitialization::buildMaster()  const  {
}

/// Standard constructor
Geant4UserInitializationSequence::Geant4UserInitializationSequence(Geant4Context* ctxt, const string& nam)
  : Geant4UserInitialization(ctxt, nam) {
  m_needsControl = false;
  InstanceCount::increment(this);
}

/// Default destructor
Geant4UserInitializationSequence::~Geant4UserInitializationSequence() {
  InstanceCount::decrement(this);
  m_workerCalls.clear();
  m_masterCalls.clear();
  m_actors(&Geant4Action::release);
  m_actors.clear();
}

/// Add an actor responding to all callbacks. Sequence takes ownership.
void Geant4UserInitializationSequence::adopt(Geant4UserInitialization* action)   {
  if (action) {
    action->addRef();
    m_actors.add(action);
    return;
  }
  throw runtime_error("Geant4UserInitializationSequence: Attempt to add invalid actor!");
}

/// Callback function to build setup for the MT worker thread
void Geant4UserInitializationSequence::build()  const  {
  m_actors(&Geant4UserInitialization::build);
  m_workerCalls();
}

/// Callback function to build setup for the MT master thread
void Geant4UserInitializationSequence::buildMaster()  const  {
  m_actors(&Geant4UserInitialization::buildMaster);
  m_masterCalls();
}

/// Set client context
void Geant4UserInitializationSequence::updateContext(Geant4Context* ctxt)   {
  m_context = ctxt;
  m_actors.updateContext(ctxt);
}

