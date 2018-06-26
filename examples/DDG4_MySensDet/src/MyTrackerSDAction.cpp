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
#include "DDG4/Geant4SensDetAction.inl"
#include "DDG4/Factories.h"

using namespace CLHEP;

namespace {
  class MyTrackerSD {
  public:
    /// This is the hit definition.
    /** I took here the same definition of the default Geant4Tracker class,
     *  (see DDG4/Geant4Data.h)  but it could be anything else as well
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class MyHit : public dd4hep::sim::Geant4HitData {
    public:
      /// Hit position
      dd4hep::Position      position;
      /// Hit direction
      dd4hep::Direction     momentum;
      /// Length of the track segment contributing to this hit
      double        length = 0;
      /// Monte Carlo / Geant4 information
      Contribution  truth;
      /// Energy deposit in the tracker hit
      double        energyDeposit = 0;
    public:
      /// Default constructor
      MyHit() = default;
      /// Initializing constructor
      MyHit(int track_id, int pdg_id, double deposit, double time_stamp)
      : length(0.0), truth(track_id, pdg_id, deposit, time_stamp, 0.), energyDeposit(deposit) {}
      /// Default destructor
      virtual ~MyHit() = default;
      /// Assignment operator
      MyHit& operator=(const MyHit& c)  {
        if ( &c != this )  {
          position = c.position;
          momentum = c.momentum;
          length = c.length;
          truth = c.truth;
        }
        return *this;
      }
      /// Clear hit content
      MyHit& clear() {
        position.SetXYZ(0, 0, 0);
        momentum.SetXYZ(0, 0, 0);
        length = 0.0;
        truth.clear();
        return *this;
      }

      /// Store Geant4 point and step information into tracker hit structure.
      MyHit& storePoint(const G4Step* step, const G4StepPoint* pnt) {
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
    
    // If we need special data to personalize the action, be put it here
    int mumDeposits = 0;
    double integratedDeposit = 0;
  };
}

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim   {

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //               Geant4SensitiveAction<MyTrackerSD>
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    /** \addtogroup Geant4SDActionPlugin
     *
     * @{
     * \package MyTrackerSDAction
     * \brief Sensitive detector meant for tracking detectors, will produce one hit per step
     *
     * @}
     */

    /// Define collections created by this sensitivie action object
    template <> void Geant4SensitiveAction<MyTrackerSD>::defineCollections()    {
      m_collectionID = declareReadoutFilteredCollection<MyTrackerSD::MyHit>();
    }

    /// Method for generating hit(s) using the information of G4Step object.
    template <> bool Geant4SensitiveAction<MyTrackerSD>::process(G4Step* step,G4TouchableHistory* /*hist*/ ) {
      Geant4StepHandler h(step);
      Position prePos    = h.prePos();
      Position postPos   = h.postPos();
      Position direction = postPos - prePos;
      Position position  = mean_direction(prePos,postPos);
      double   hit_len   = direction.R();

      // Somehow extract here the physics you want
      MyTrackerSD::MyHit* hit = new MyTrackerSD::MyHit(h.trkID(), h.trkPdgID(), h.deposit(), h.track->GetGlobalTime());
      HitContribution contrib = MyTrackerSD::MyHit::extractContribution(step);
      hit->cellID        = cellID(step);
      hit->energyDeposit = contrib.deposit;
      hit->position      = position;
      hit->momentum      = 0.5*(h. preMom() + h.postMom());
      hit->length        = hit_len;
      collection(m_collectionID)->add(hit);
      mark(h.track);
      if ( 0 == hit->cellID )  {
        hit->cellID        = volumeID(step);
        except("+++ Invalid CELL ID for hit!");
      }
      printP1("Hit with deposit:%f  Pos:%f %f %f ID=%016X",
            step->GetTotalEnergyDeposit(),position.X(),position.Y(),position.Z(),
            (void*)hit->cellID);
      Geant4TouchableHandler handler(step);
      print("    Geant4 path:%s",handler.path().c_str());

      // Do something with my personal data (can be also something more clever ;-):
      m_userData.integratedDeposit += contrib.deposit;
      ++m_userData.mumDeposits;
      return true;
    }

  }
}

//--- Factory declaration
namespace dd4hep { namespace sim {
    typedef Geant4SensitiveAction<MyTrackerSD> MyTrackerSDAction;
  }}
DECLARE_GEANT4SENSITIVE(MyTrackerSDAction)
