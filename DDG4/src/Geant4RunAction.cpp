// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4RunAction.h"
// C/C++ include files
#include <stdexcept>

using namespace DD4hep::Simulation;

/// Standard constructor
Geant4RunAction::Geant4RunAction(Geant4Context* context, const std::string& nam)
: Geant4Action(context,nam)  
{
  InstanceCount::increment(this);
}

/// Default destructor
Geant4RunAction::~Geant4RunAction()  {
  InstanceCount::decrement(this);
}

/// begin-of-run callback
void Geant4RunAction::begin(const G4Run* )  {
}

/// End-of-run callback
void Geant4RunAction::end(const G4Run* )  {
}

/// Standard constructor
Geant4RunActionSequence::Geant4RunActionSequence(Geant4Context* context, const std::string& name)
: Geant4Action(context,name)
{
  InstanceCount::increment(this);
}

/// Default destructor
Geant4RunActionSequence::~Geant4RunActionSequence()  {
  m_actors(&Geant4RunAction::release);
  m_actors.clear();
  m_begin.clear();
  m_end.clear();
  InstanceCount::decrement(this);
}

/// Add an actor responding to all callbacks. Sequence takes ownership.
void Geant4RunActionSequence::adopt(Geant4RunAction* action)  {
  if ( action )  {
    action->addRef();
    m_actors.add(action);
    return;
  }
  throw std::runtime_error("Geant4RunActionSequence: Attempt to add invalid actor!");
}

/// Pre-track action callback
void  Geant4RunActionSequence::begin(const G4Run* run)  { 
  m_actors(&Geant4RunAction::begin,run);
  m_begin(run);
}

/// Post-track action callback
void  Geant4RunActionSequence::end(const G4Run* run) { 
  m_end(run);
  m_actors(&Geant4RunAction::end,run);
}
