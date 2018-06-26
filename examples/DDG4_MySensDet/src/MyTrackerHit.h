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
#ifndef SOMEEXPERIMENT_MYTRACKERHIT_H
#define SOMEEXPERIMENT_MYTRACKERHIT_H

#include "DDG4/Geant4Data.h"
#include "G4Track.hh"
#include "G4Step.hh"

namespace SomeExperiment {

  /// This is the hit definition.
  /** I took here the same definition of the default Geant4Tracker class,
   *  (see DDG4/Geant4Data.h)  but it could be anything else as well.
   *
   *  Please note:
   *  ============
   *  The MC truth handling as implemented in the Geant4ParticleHandler
   *  will not work with this class if the object(s) are saved with 
   *  the standard Geant4Output2ROOT event action. If the hit is 
   *  specialized, the output writing also must be specialized if
   *  MC truth handling should be supported.
   *  Otherwise it is sufficient to provide a ROOT dictionary as long as the
   *  base class dd4hep::sim::Geant4HitData is kept.
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_SIMULATION
   */
  class MyTrackerHit /* : public dd4hep::sim::Geant4HitData  */ {

  public:
    /// dd4hep::sim::Geant4HitData: cellID
    long long int cellID = 0;
    /// dd4hep::sim::Geant4HitData: User flag to classify hits
    long flag = 0;
    /// dd4hep::sim::Geant4HitData: Original Geant 4 track identifier of the creating track (debugging)
    long g4ID = -1;

    
    /// Hit position
    dd4hep::Position      position;
    /// Hit direction
    dd4hep::Direction     momentum;
    /// Length of the track segment contributing to this hit
    double        length = 0;
    /// Monte Carlo / Geant4 information
    dd4hep::sim::Geant4HitData::MonteCarloContrib truth;
    /// Energy deposit in the tracker hit
    double        energyDeposit = 0;

  public:
    /// Default constructor
    MyTrackerHit() = default;
    /// Initializing constructor
    MyTrackerHit(int track_id, int pdg_id, double deposit, double time_stamp)
      : length(0.0), truth(track_id, pdg_id, deposit, time_stamp, 0.), energyDeposit(deposit) {}
    /// Default destructor
    virtual ~MyTrackerHit() = default;
    /// Assignment operator
    MyTrackerHit& operator=(const MyTrackerHit& c)  {
      if ( &c != this )  {
        position = c.position;
        momentum = c.momentum;
        length = c.length;
        truth = c.truth;
      }
      return *this;
    }
    /// Clear hit content
    MyTrackerHit& clear() {
      position.SetXYZ(0, 0, 0);
      momentum.SetXYZ(0, 0, 0);
      length = 0.0;
      truth.clear();
      return *this;
    }

    /// Store Geant4 point and step information into tracker hit structure.
    MyTrackerHit& storePoint(const G4Step* step, const G4StepPoint* pnt) {
      G4Track* trk = step->GetTrack();
      G4ThreeVector pos = pnt->GetPosition();
      G4ThreeVector mom = pnt->GetMomentum();

      truth.trackID = trk->GetTrackID();
      truth.pdgID   = trk->GetDefinition()->GetPDGEncoding();
      truth.deposit = step->GetTotalEnergyDeposit();
      truth.time    = trk->GetGlobalTime();
      position.SetXYZ(pos.x(), pos.y(), pos.z());
      momentum.SetXYZ(mom.x(), mom.y(), mom.z());
      length = 0;
      return *this;
    }
  };
}

// CINT configuration
#if defined(__CINT__) || defined(__MAKECINT__) || defined(__CLING__) || defined(__ROOTCLING__)
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

/// Define namespaces
#pragma link C++ namespace dd4hep;
#pragma link C++ namespace dd4hep::sim;
#pragma link C++ namespace SomeExperiment;
#pragma link C++ class     SomeExperiment::MyTrackerHit+;
#endif

#endif /* SOMEEXPERIMENT_MYTRACKERHIT_H */
