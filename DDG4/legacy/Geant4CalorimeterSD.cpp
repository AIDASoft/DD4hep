// $Id: Handle.h 570 2013-05-17 07:47:11Z markus.frank $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation européenne pour la Recherche nucléaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

// Framework include files
#include "DDG4/Geant4SensitiveDetector_inline.h"
#include "DDG4/Factories.h"

// Geant4 include files
#include "G4OpticalPhoton.hh"
#include "G4VProcess.hh"

using namespace std;
using namespace DD4hep::Geometry;

/*
 *   DD4hep::Simulation namespace declaration
 */
namespace DD4hep {  namespace Simulation {

    /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    ///               Geant4GenericSD<Calorimeter>
    /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    struct Calorimeter {};
    /// Method for generating hit(s) using the information of G4Step object.
    template <> bool Geant4GenericSD<Calorimeter>::buildHits(G4Step* step,G4TouchableHistory*) {
      StepHandler     h(step);
      Position        pos     = 0.5 * (h.prePos() + h.postPos());
      HitContribution contrib = Geant4Hit::extractContribution(step);
      Geant4CalorimeterHit* hit=find(collection(0),HitPositionCompare<Geant4CalorimeterHit>(pos));

      //    G4cout << "----------- Geant4GenericSD<Calorimeter>::buildHits : position : " << pos << G4endl ;

      if ( !hit ) {

        hit = new Geant4CalorimeterHit(pos) ;

        hit->cellID  = getVolumeID( step ) ;

        collection(0)->insert(hit) ;
      }
      hit->truth.push_back(contrib);
      hit->energyDeposit += contrib.deposit;

      return true;
    }
    typedef  Geant4GenericSD<Calorimeter> Geant4Calorimeter;
  }}    // End namespace DD4hep::Simulation

DECLARE_GEANT4SENSITIVEDETECTOR_NS(DD4hep::Simulation,Geant4Calorimeter)

/*
 *   DD4hep::Simulation namespace declaration
 */
namespace DD4hep {  namespace Simulation {

    /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    ///               Geant4GenericSD<OpticalCalorimeter>
    /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    enum { Edep_type=0, Cerenkov_type=1 };
    struct OpticalCalorimeter {};
    template <> class Geant4GenericSD<OpticalCalorimeter> : public Geant4GenericSD<Calorimeter>  {
    public:
      /// Constructor. The sensitive detector element is identified by the detector name
      Geant4GenericSD(const string& nam, LCDD& lcdd_ref)
        : Geant4GenericSD<Calorimeter>(nam,lcdd_ref) {          }

      /// Initialize the sensitive detector for the usage of a single hit collection
      bool defineCollection(const string& coll_name) {
        Geant4SensitiveDetector::defineCollection("Edep_" + coll_name);
        Geant4SensitiveDetector::defineCollection("Ceren_" + coll_name);
        return true;
      }

      /// Method for generating hit(s) using the information of G4Step object.
      virtual G4bool ProcessHits(G4Step* step,G4TouchableHistory* history) {
        G4Track * track =  step->GetTrack();

        // check that particle is optical photon:
        if( track->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition() )  {
          return this->Geant4GenericSD<Calorimeter>::ProcessHits(step,history);
        }
        else if ( track->GetCreatorProcess()->G4VProcess::GetProcessName() != "Cerenkov")  {
          track->SetTrackStatus(fStopAndKill);
          return false;
        }
        else {
          StepHandler h(step);
          HitContribution contrib = Geant4Hit::extractContribution(step);
          Position        pos     = h.prePos();
          Geant4CalorimeterHit* hit=find(collection(Cerenkov_type),HitPositionCompare<Geant4CalorimeterHit>(pos));
          if ( !hit ) {
            collection(Cerenkov_type)->insert(hit=new Geant4CalorimeterHit(pos));
          }
          hit->energyDeposit += contrib.deposit;
          hit->truth.push_back(contrib);
          track->SetTrackStatus(fStopAndKill); // don't step photon any further
          return true;
        }
      }
    };
    typedef Geant4GenericSD<OpticalCalorimeter>  Geant4OpticalCalorimeter;
  }}    // End namespace DD4hep::Simulation

DECLARE_GEANT4SENSITIVEDETECTOR_NS(DD4hep::Simulation,Geant4OpticalCalorimeter)
