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
#ifndef DD4HEP_DDG4_GEANT4ESCAPECOUNTER_H
#define DD4HEP_DDG4_GEANT4ESCAPECOUNTER_H

// Framework include files
#include "DD4hep/DetElement.h"
#include "DDG4/Geant4SensDetAction.h"
#include "DDG4/Geant4SteppingAction.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    /// Class to measure the energy of escaping tracks
    /** Class to measure the energy of escaping tracks of a detector using Geant 4
     * Measure escaping energy....
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4EscapeCounter : /* virtual public Geant4SteppingAction, virtual */ public Geant4Sensitive {
      /// Collection identifiers
      size_t m_collectionID;
      /// Detector name set
      std::vector<std::string> m_detectorNames;
    public:
      /// Standard constructor
      Geant4EscapeCounter(Geant4Context* ctxt, const std::string& name, DetElement det, Detector& description);
      /// Default destructor
      virtual ~Geant4EscapeCounter();
      /// G4VSensitiveDetector interface: Method for generating hit(s) using the information of G4Step object.
      virtual bool process(const G4Step* step, G4TouchableHistory* history)  override;
    };

  }    // End namespace sim
}      // End namespace dd4hep

#endif /* DD4HEP_DDG4_GEANT4ESCAPECOUNTER_H */

//====================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//====================================================================
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"

#include "DDG4/Geant4TouchableHandler.h"
#include "DDG4/Geant4TrackHandler.h"
#include "DDG4/Geant4StepHandler.h"
#include "DDG4/Geant4Mapping.h"
#include "DDG4/Geant4Data.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "G4VProcess.hh"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::sim;

/// Standard constructor
Geant4EscapeCounter::Geant4EscapeCounter(Geant4Context* ctxt, const string& nam, DetElement det, Detector& description_ref)
  : Geant4Sensitive(ctxt, nam, det, description_ref)
{
  string coll_name = name()+"Hits";
  m_needsControl = true;
  declareProperty("Shells",m_detectorNames);
  m_collectionID = defineCollection<SimpleTracker::Hit>(coll_name);
  InstanceCount::increment(this);
}

/// Default destructor
Geant4EscapeCounter::~Geant4EscapeCounter() {
  InstanceCount::decrement(this);
}

/// G4VSensitiveDetector interface: Method for generating hit(s) using the information of G4Step object.
bool Geant4EscapeCounter::process(const G4Step* step, G4TouchableHistory* /* history */)   {
  Geant4StepHandler  h(step);
  Geant4TrackHandler th(h.track);
  Geant4TouchableHandler handler(step);
  string   hdlr_path  = handler.path();
  Position prePos     = h.prePos();
  Position postPos    = h.postPos();
  Geant4HitCollection* coll = collection(m_collectionID);
  SimpleTracker::Hit*  hit = new SimpleTracker::Hit();
  hit->g4ID          = th.id();
  hit->energyDeposit = h.deposit();
  hit->cellID        = volumeID(step);
  hit->energyDeposit = th.energy();
  hit->position      = prePos;
  hit->momentum      = h.trkMom();
  hit->length        = (postPos-prePos).R();
  hit->truth.trackID = th.id();
  hit->truth.deposit = h.deposit();
  hit->truth.pdgID   = th.pdgID();
  hit->truth.deposit = h.deposit();
  hit->truth.time    = th.time();
  hit->truth.length  = hit->length;
  hit->truth.x       = hit->position.x();
  hit->truth.y       = hit->position.y();
  hit->truth.z       = hit->position.z();
  coll->add(hit);
  mark(h.track);

  print("+++ Track:%4d  %8.2f MeV [%s] %s Geant4 path:%s",
        h.trkID(),h.trkEnergy()/CLHEP::MeV,th.name().c_str(),
        th.creatorName().c_str(),hdlr_path.c_str());
  // Kill track, so that it does no longer participate in the propagation
  step->GetTrack()->SetTrackStatus(fStopAndKill);
  return true;
}

#include "DDG4/Factories.h"
DECLARE_GEANT4SENSITIVE(Geant4EscapeCounter)

