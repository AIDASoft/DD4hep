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
#include "DD4hep/Printout.h"
#include "DD4hep/Primitives.h"
#include "DD4hep/InstanceCount.h"

#include "DDG4/Geant4ActionContainer.h"
#include "DDG4/Geant4RunAction.h"
#include "DDG4/Geant4PhysicsList.h"
#include "DDG4/Geant4EventAction.h"
#include "DDG4/Geant4SteppingAction.h"
#include "DDG4/Geant4TrackingAction.h"
#include "DDG4/Geant4StackingAction.h"
#include "DDG4/Geant4GeneratorAction.h"
#include "DDG4/Geant4DetectorConstruction.h"
#include "DDG4/Geant4UserInitialization.h"
#include "DDG4/Geant4SensDetAction.h"

// C/C++ include files
#include <stdexcept>
#include <algorithm>

using namespace std;
using namespace DD4hep::Simulation;

/// Standard constructor
Geant4ActionContainer::Geant4ActionContainer(Geant4Context* ctxt)
  : m_context(ctxt), 
    m_generatorAction(0), m_runAction(0), m_eventAction(0), m_trackingAction(0),
    m_steppingAction(0), m_stackingAction(0), m_constructionAction(0),
    m_sensDetActions(0), m_physicsList(0), m_userInit(0)
{
  m_sensDetActions = new Geant4SensDetSequences();
  InstanceCount::increment(this);
}

/// Default destructor
Geant4ActionContainer::~Geant4ActionContainer() {
  this->Geant4ActionContainer::terminate();
  InstanceCount::decrement(this);
}

/// Terminate all associated action instances
int Geant4ActionContainer::terminate() {
  releasePtr (m_physicsList);
  releasePtr (m_constructionAction);
  releasePtr (m_stackingAction);
  releasePtr (m_steppingAction);
  releasePtr (m_trackingAction);
  releasePtr (m_eventAction);
  releasePtr (m_generatorAction);
  releasePtr (m_runAction);
  deletePtr  (m_sensDetActions);
  deletePtr  (m_context);
  return 1;
}

Geant4Context* Geant4ActionContainer::workerContext()   {
  if ( m_context ) return m_context;
  throw runtime_error(format("Geant4Kernel", "DDG4: Master kernel object has no thread context! [Invalid Handle]"));
}

/// Set the thread's context
void Geant4ActionContainer::setContext(Geant4Context* ctxt)    {
  m_context = ctxt;
}

template <class C> bool Geant4ActionContainer::registerSequence(C*& seq, const std::string& name) {
  if (!name.empty()) {
    seq = new C(m_context, name);
    seq->installMessengers();
    return true;
  }
  throw runtime_error(format("Geant4ActionContainer", "DDG4: The action '%s' not found. [Action-NotFound]", name.c_str()));
}

/// Access generator action sequence
Geant4GeneratorActionSequence* Geant4ActionContainer::generatorAction(bool create) {
  if (!m_generatorAction && create)
    registerSequence(m_generatorAction, "GeneratorAction");
  return m_generatorAction;
}

/// Access run action sequence
Geant4RunActionSequence* Geant4ActionContainer::runAction(bool create) {
  if (!m_runAction && create)
    registerSequence(m_runAction, "RunAction");
  return m_runAction;
}

/// Access event action sequence
Geant4EventActionSequence* Geant4ActionContainer::eventAction(bool create) {
  if (!m_eventAction && create)
    registerSequence(m_eventAction, "EventAction");
  return m_eventAction;
}

/// Access stepping action sequence
Geant4SteppingActionSequence* Geant4ActionContainer::steppingAction(bool create) {
  if (!m_steppingAction && create)
    registerSequence(m_steppingAction, "SteppingAction");
  return m_steppingAction;
}

/// Access tracking action sequence
Geant4TrackingActionSequence* Geant4ActionContainer::trackingAction(bool create) {
  if (!m_trackingAction && create)
    registerSequence(m_trackingAction, "TrackingAction");
  return m_trackingAction;
}

/// Access stacking action sequence
Geant4StackingActionSequence* Geant4ActionContainer::stackingAction(bool create) {
  if (!m_stackingAction && create)
    registerSequence(m_stackingAction, "StackingAction");
  return m_stackingAction;
}

/// Access detector construcion action sequence (geometry+sensitives+field)
Geant4DetectorConstructionSequence* Geant4ActionContainer::detectorConstruction(bool create)  {
  if (!m_constructionAction && create)
    registerSequence(m_constructionAction, "StackingAction");
  return m_constructionAction;
}

/// Access to the sensitive detector sequences from the kernel object
Geant4SensDetSequences& Geant4ActionContainer::sensitiveActions() const {
  return *m_sensDetActions;
}

/// Access to the sensitive detector action from the kernel object
Geant4SensDetActionSequence* Geant4ActionContainer::sensitiveAction(const string& nam) {
  Geant4SensDetActionSequence* ptr = m_sensDetActions->find(nam);
  if (ptr)   {
    return ptr;
  }
  ptr = new Geant4SensDetActionSequence(workerContext(), nam);
  m_sensDetActions->insert(nam, ptr);
  return ptr;
}

/// Access to the physics list
Geant4PhysicsListActionSequence* Geant4ActionContainer::physicsList(bool create) {
  if (!m_physicsList && create)
    registerSequence(m_physicsList, "PhysicsList");
  return m_physicsList;
}

/// Access to the physics list
Geant4UserInitializationSequence* Geant4ActionContainer::userInitialization(bool create) {
  if (!m_userInit && create)   {
    registerSequence(m_userInit, "UserInitialization");
  }
  return m_userInit;
}
