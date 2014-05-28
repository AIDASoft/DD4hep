// $Id: Geant4ActionPhase.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4ActionPhase.h"

using namespace std;
using namespace DD4hep::Simulation;

/// Standard constructor
Geant4ActionPhase::Geant4ActionPhase(Geant4Context* context, const string& nam, const type_info& arg_type0,
    const type_info& arg_type1, const type_info& arg_type2)
    : Geant4Action(context, nam) {
  m_argTypes[0] = &arg_type0;
  m_argTypes[1] = &arg_type1;
  m_argTypes[2] = &arg_type2;
  InstanceCount::increment(this);
}

/// Default destructor
Geant4ActionPhase::~Geant4ActionPhase() {
  m_members.clear();
  InstanceCount::decrement(this);
}

/// Add a new member to the phase
bool Geant4ActionPhase::add(Callback callback) {
  m_members.push_back(callback);
  return true;
}

/// Remove an existing member from the phase. If not existing returns false
bool Geant4ActionPhase::remove(Callback callback) {
  if (callback.func.first) {
    Members::iterator i = find(m_members.begin(), m_members.end(), callback);
    if (i != m_members.end()) {
      m_members.erase(i);
      return true;
    }
    return false;
  }
  size_t len = m_members.size();
  for (Members::iterator i = m_members.begin(); i != m_members.end(); ++i) {
    if ((*i).par == callback.par) {
      m_members.erase(i);
      i = m_members.begin();
    }
  }
  return (len > m_members.size());
}

/// Execute all members in the phase context
void Geant4ActionPhase::execute(void* argument) {
  for (Members::iterator i = m_members.begin(); i != m_members.end(); ++i) {
    (*i).execute((const void**) &argument);
  }
}

class G4HCofThisEvent;
class G4TouchableHistory;
#include "DDG4/Geant4RunAction.h"
#include "DDG4/Geant4EventAction.h"
#include "DDG4/Geant4TrackingAction.h"
#include "DDG4/Geant4SteppingAction.h"
#include "DDG4/Geant4GeneratorAction.h"
namespace DD4hep {
  namespace Simulation {
    /// Callback in Begin stacking action
    void Geant4ActionPhase::call() {
      this->execute(0);
    }
    /// Callback in BeginRun/EndRun
    template <> void Geant4ActionPhase::call<const G4Run*>(const G4Run* run) {
      this->execute((const void**) &run);
    }
    /// Callback in prepare primaries
    template <> void Geant4ActionPhase::call<G4Event*>(G4Event* event) {
      this->execute((const void**) &event);
    }
    /// Callback in BeginEvent/EndEvent
    template <> void Geant4ActionPhase::call<const G4Event*>(const G4Event* event) {
      this->execute((const void**) &event);
    }
    /// Callback in Begin/End tracking action
    template <> void Geant4ActionPhase::call<const G4Track*>(const G4Track* track) {
      this->execute((const void**) &track);
    }
    /// Callback in Begin/End stepping action
    template <> void Geant4ActionPhase::call<const G4Step*>(const G4Step* step) {
      this->execute((const void**) &step);
    }
    /// Callback for user stepping action
    template <> void Geant4ActionPhase::call<const G4Step*, G4SteppingManager*>(const G4Step* step, G4SteppingManager* mgr) {
      const void * args[] = { step, mgr };
      this->execute(args);
    }
    /// Callback for sensitives begin/end/clear
    template <> void Geant4ActionPhase::call<G4HCofThisEvent*>(G4HCofThisEvent* hce) {
      this->execute((const void**) &hce);
    }
    /// Callback for sensitives
    template <> void Geant4ActionPhase::call<G4Step*, G4TouchableHistory*>(G4Step* step, G4TouchableHistory* history) {
      const void * args[] = { step, history };
      this->execute(args);
    }
  }
}
