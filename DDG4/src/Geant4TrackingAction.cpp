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
#include "DDG4/Geant4TrackingAction.h"
#include "DDG4/Geant4MonteCarloTruth.h"
#include "DDG4/Geant4TrackInformation.h"

// Geant4 include files
#include "G4Track.hh"
#include "G4TrackingManager.hh"
#include "G4VUserTrackInformation.hh"

// C/C++ include files
#include <stdexcept>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;
class G4Step;
class G4TouchableHistory;

/// Standard constructor
Geant4TrackingActionSequence::Geant4TrackingActionSequence(Geant4Context* ctxt, const std::string& nam)
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

/// Add an actor responding to all callbacks. Sequence takes ownership.
void Geant4TrackingActionSequence::adopt(Geant4TrackingAction* action) {
  if (action) {
    action->addRef();
    m_actors.add(action);
    return;
  }
  throw std::runtime_error("Geant4TrackingActionSequence: Attempt to add invalid actor!");
}

/// Pre-track action callback
void Geant4TrackingActionSequence::begin(const G4Track* track) {
  m_actors(ContextUpdate(context()));
  m_front(track);
  m_actors(&Geant4TrackingAction::begin, track);
  m_begin(track);
}

/// Post-track action callback
void Geant4TrackingActionSequence::end(const G4Track* track) {
  m_end(track);
  m_actors(&Geant4TrackingAction::end, track);
  m_final(track);
  m_actors(ContextUpdate(0));
}

/// Standard constructor
Geant4TrackingAction::Geant4TrackingAction(Geant4Context* ctxt, const std::string& nam)
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

/// Get the valid Geant4 tarck information
Geant4TrackInformation* Geant4TrackingAction::trackInfo(G4Track* track) const {
  if (track) {
    Geant4TrackInformation* gau = 0;
    G4VUserTrackInformation* g4 = track->GetUserInformation();
    if (0 == g4)   {
      gau = new Geant4TrackInformation();
      track->SetUserInformation(gau);
      return gau;                                                 // RETURN
    }
    gau = fast_cast<Geant4TrackInformation*>(g4);
    if (!gau) {
      error("trackInfo: invalid cast to Geant4TrajckInformation");
    }
    return gau;
  }
  error("trackInfo: [Invalid G4Track]");
  return 0;
}

/// Mark all children of the track to be stored
bool Geant4TrackingAction::storeChildren() const {
  G4TrackVector* v = trackMgr()->GimmeSecondaries();
  if (v) {   // loop over all children
    for (G4TrackVector::const_iterator i = v->begin(); i != v->end(); ++i) {
      G4Track* t = *i;
      if (t) {
        storeChild(trackInfo(t));
      }
    }
    return true;
  }
  return false;
}

/// Mark a single child of the track to be stored
bool Geant4TrackingAction::storeChild(Geant4TrackInformation* track_info) const {
  if (0 != track_info) {
    if (!track_info->storeTrack()) {
      track_info->storeTrack(true);
    }
    return true;
  }
  return error(false, "storeChild: Geant4TrackInformation points to NULL!");
}
