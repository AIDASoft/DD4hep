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
#ifndef DD4HEP_DDG4_GEANT4ESCAPECOUNTER_H
#define DD4HEP_DDG4_GEANT4ESCAPECOUNTER_H

// Framework include files
#include "DD4hep/Detector.h"
#include "DDG4/Geant4SensDetAction.h"
#include "DDG4/Geant4SteppingAction.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

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
      Geant4EscapeCounter(Geant4Context* ctxt, const std::string& name, DetElement det, LCDD& lcdd);
      /// Default destructor
      virtual ~Geant4EscapeCounter();
      /// G4VSensitiveDetector interface: Method for generating hit(s) using the information of G4Step object.
      virtual bool process(G4Step* step, G4TouchableHistory* history)  override;
    };

  }    // End namespace Simulation
}      // End namespace DD4hep

#endif /* DD4HEP_DDG4_GEANT4ESCAPECOUNTER_H */

//====================================================================
//  AIDA Detector description implementation for LCD
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
using namespace DD4hep;
using namespace DD4hep::Simulation;

/// Standard constructor
Geant4EscapeCounter::Geant4EscapeCounter(Geant4Context* ctxt, const string& nam, DetElement det, LCDD& lcdd_ref)
  : Geant4Sensitive(ctxt, nam, det, lcdd_ref)
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
bool Geant4EscapeCounter::process(G4Step* step, G4TouchableHistory* /* history */)   {
  Geant4StepHandler  h(step);
  Geant4TrackHandler th(h.track);
  Geant4TouchableHandler handler(step);
  string   hdlr_path  = handler.path();
  Position prePos     = h.prePos();
  HitCollection* coll = collection(m_collectionID);
  SimpleTracker::Hit* hit = new SimpleTracker::Hit(th.id(),th.pdgID(),h.deposit(),th.time());
  hit->cellID        = volumeID(step);
  hit->energyDeposit = th.energy();
  hit->position      = prePos;
  hit->momentum      = h.trkMom();
  hit->length        = 0;
  coll->add(hit);
  mark(h.track);

  print("+++ Track:%4d  %8.2f MeV [%s] %s Geant4 path:%s",
        h.trkID(),h.trkEnergy()/CLHEP::MeV,th.name().c_str(),
        th.creatorName().c_str(),hdlr_path.c_str());
  // Kill track, so that it does no longer participate in the propagation
  h.track->SetTrackStatus(fStopAndKill);
  return true;
}

#include "DDG4/Factories.h"
DECLARE_GEANT4SENSITIVE(Geant4EscapeCounter)

