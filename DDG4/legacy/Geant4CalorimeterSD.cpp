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

// Framework include files
#include "DDG4/Geant4SensitiveDetector_inline.h"
#include "DDG4/Factories.h"

// Geant4 include files
#include "G4OpticalPhoton.hh"
#include "G4VProcess.hh"

using namespace std;
using namespace dd4hep::detail;

/*
 *   dd4hep::sim namespace declaration
 */
namespace dd4hep {  namespace sim {

    /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    ///               Geant4GenericSD<Calorimeter>
    /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    /// Legacy class. Deprecated. Not supported!  Not supported by the DDG4 kernel.
    /**  \deprecated
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct Calorimeter {};
    /// Method for generating hit(s) using the information of G4Step object.
    template <> bool Geant4GenericSD<Calorimeter>::buildHits(G4Step* step,G4TouchableHistory*) {
      StepHandler     h(step);
      Position        pos     = 0.5 * (h.prePos() + h.postPos());
      HitContribution contrib = Geant4Hit::extractContribution(step);
      Geant4CalorimeterHit* hit=find(collection(0),HitPositionCompare<Geant4CalorimeterHit>(pos));

      //    G4cout << "----------- Geant4GenericSD<Calorimeter>::buildHits : position : " << pos << G4endl;
      if ( !hit ) {
        hit = new Geant4CalorimeterHit(pos);
        hit->cellID  = getCellID( step );
        collection(0)->insert(hit);
      }
      hit->truth.push_back(contrib);
      hit->energyDeposit += contrib.deposit;

      return true;
    }
    typedef  Geant4GenericSD<Calorimeter> Geant4Calorimeter;
  }}    // End namespace dd4hep::sim

DECLARE_GEANT4SENSITIVEDETECTOR_NS(dd4hep::sim,Geant4Calorimeter)

/*
 *   dd4hep::sim namespace declaration
 */
namespace dd4hep {  namespace sim {

    /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    ///               Geant4GenericSD<OpticalCalorimeter>
    /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    enum { Edep_type=0, Cerenkov_type=1 };

    /// Legacy class. Deprecated. Not supported!  Not supported by the DDG4 kernel.
    /**  \deprecated
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct OpticalCalorimeter {};

    /// Legacy class. Deprecated. Not supported!  Not supported by the DDG4 kernel.
    /**  \deprecated
     *
     * @author  M.Frank
     * @version 1.0
     */
    template <> class Geant4GenericSD<OpticalCalorimeter> : public Geant4GenericSD<Calorimeter>  {
    public:
      /// Constructor. The sensitive detector element is identified by the detector name
      Geant4GenericSD(const string& nam, Detector& description_ref)
        : Geant4GenericSD<Calorimeter>(nam,description_ref) {          }

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
            hit->cellID  = getCellID( step ) ;
          }
          hit->energyDeposit += contrib.deposit;
          hit->truth.push_back(contrib);
          track->SetTrackStatus(fStopAndKill); // don't step photon any further
          return true;
        }
      }
    };
    typedef Geant4GenericSD<OpticalCalorimeter>  Geant4OpticalCalorimeter;
  }}    // End namespace dd4hep::sim

DECLARE_GEANT4SENSITIVEDETECTOR_NS(dd4hep::sim,Geant4OpticalCalorimeter)
