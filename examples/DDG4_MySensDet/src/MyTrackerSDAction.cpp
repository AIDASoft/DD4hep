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
#include <DDG4/Geant4SensDetAction.inl>
#include <DDG4/Geant4ParticleInformation.h>
#include <DDG4/Factories.h>


namespace SomeExperiment {

  class MyTrackerSD {
  public:
    typedef MyTrackerHit Hit;
    // If we need special data to personalize the action, be put it here
    bool   haveCellID = true;
    int    mumDeposits = 0;
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
    template <>
    Geant4SensitiveAction<MyTrackerSD>::Geant4SensitiveAction(Geant4Context* ctxt,
                                                              const std::string& nam,
                                                              DetElement det,
                                                              Detector& description_ref)
      : Geant4Sensitive(ctxt,nam,det,description_ref), m_collectionName(), m_collectionID(0)
    {
      declareProperty("HaveCellID",     m_userData.haveCellID = true);
      declareProperty("ReadoutName",    m_readoutName);
      declareProperty("CollectionName", m_collectionName);
      initialize();
      InstanceCount::increment(this);
    }
    
    /// Define collections created by this sensitivie action object
    template <> void Geant4SensitiveAction<MyTrackerSD>::defineCollections()    {
      m_collectionID = declareReadoutFilteredCollection<MyTrackerSD::Hit>();
    }

    /// Method for generating hit(s) using the information of G4Step object.
    template <> bool Geant4SensitiveAction<MyTrackerSD>::process(const G4Step* step,G4TouchableHistory* /*hist*/ ) {
      Geant4StepHandler h(step);
      Position  prePos    = h.prePos();
      Position  postPos   = h.postPos();
      Position  direction = postPos - prePos;
      Position  pos       = mean_direction(prePos,postPos);
      Direction mom       = 0.5 * (h.preMom() + h.postMom());
      double    hit_len   = direction.R();
      double    depo      = h.deposit();
      double    tim       = h.track->GetGlobalTime();
      // Somehow extract here the physics you want
      MyTrackerSD::Hit* hit = 
        new MyTrackerSD::Hit(h.trkID(), h.trkPdgID(), depo, tim, hit_len, pos, mom);
      Geant4HitData::MonteCarloContrib contrib = Geant4HitData::extractContribution(step);
      hit->cellID        = this->m_userData.haveCellID ? cellID(step) : 0;
      hit->step_length   = hit_len;
      hit->prePos        = prePos;
      hit->postPos       = postPos;
      collection(m_collectionID)->add(hit);
      mark(h.track);
      if ( this->m_userData.haveCellID && 0 == hit->cellID )  {
        hit->cellID = volumeID(step);
        except("+++ Invalid CELL ID for hit!");
      }
      printP1("Hit with deposit:%f  Pos:%f %f %f ID=%016X",
              depo, pos.X(), pos.Y(), pos.Z(), (void*)hit->cellID);
      Geant4TouchableHandler handler(step);
      print("    Geant4 path:%s", handler.path().c_str());

      // Do something with my personal data (can be also something more clever ;-):
      m_userData.integratedDeposit += contrib.deposit;
      ++m_userData.mumDeposits;

      /// Let's play with the Geant4TrackInformation
      /// See issue https://github.com/AIDASoft/DD4hep/issues/1073
      if ( nullptr == h.track->GetUserInformation() )   {
        auto data = std::make_unique<ParticleUserData>();
        data->absolute_momentum = h.track->GetMomentum().mag();
        h.track->SetUserInformation(new Geant4ParticleInformation(std::move(data)));
      }
      return true;
    }

  }
}

//--- Factory declaration
namespace dd4hep { namespace sim {
    typedef Geant4SensitiveAction<MyTrackerSD> MyTrackerSDAction;
  }}
DECLARE_GEANT4SENSITIVE(MyTrackerSDAction)
