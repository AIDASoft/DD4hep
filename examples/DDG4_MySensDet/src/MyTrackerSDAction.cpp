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
#include "MyTrackerHit.h"
#include "DDG4/Geant4SensDetAction.inl"
#include "DDG4/Factories.h"

//using namespace CLHEP;

namespace SomeExperiment {

  class MyTrackerSD {
  public:
    typedef MyTrackerHit Hit;
    // If we need special data to personalize the action, be put it here
    int mumDeposits = 0;
    double integratedDeposit = 0;
  };
}

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim   {

    using namespace SomeExperiment;
    
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
      m_collectionID = declareReadoutFilteredCollection<MyTrackerSD::Hit>();
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
      MyTrackerSD::Hit* hit = new MyTrackerSD::Hit(h.trkID(), h.trkPdgID(), h.deposit(), h.track->GetGlobalTime());
      Geant4HitData::MonteCarloContrib contrib = Geant4HitData::extractContribution(step);
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
