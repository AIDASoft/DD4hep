// $Id$
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : F.Gaede, DESY
//
//==========================================================================

// Framework include files
#include "IMPL/LCCollectionVec.h"
#include "IMPL/SimTrackerHitImpl.h"
#include "IMPL/SimCalorimeterHitImpl.h"
#include "IMPL/MCParticleImpl.h"
#include "UTIL/Operators.h"
#include "UTIL/ILDConf.h"

#include "DDG4/Geant4SensDetAction.h"
#include "DDG4/Geant4Data.h"
#include "DDG4/Geant4StepHandler.h"

#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"

using namespace DD4hep::Simulation;
using namespace DD4hep;

/// Test namespace.
namespace  Tests {

  // copied from Geant4SDActions.cpp (why is this not a public class ??????)

  /// Deprecated: Simple SensitiveAction class ...
  /**
   *  \deprecated
   *  \author  M.Frank
   *  \version 1.0
   */
  template <typename T> class Geant4SensitiveAction : public Geant4Sensitive  {
  protected:
    typedef Geant4Sensitive Base;
    /// Collection identifiers
    size_t m_collectionID;

    // properties:
    bool _detailedHitsStoring ;

  public:
    //    typedef SimpleHit::Contribution HitContribution;
    // Standard , initializing constructor
    Geant4SensitiveAction(Geant4Context* ctxt, const std::string& nam, DetElement det, LCDD& lcdd_ref)
      : Geant4Sensitive(ctxt,nam,det,lcdd_ref), m_collectionID(0) {
      declareProperty("detailedHitsStoring", _detailedHitsStoring ) ;
      defineCollections();
      InstanceCount::increment(this);
    }
    /// Default destructor
    virtual ~Geant4SensitiveAction(){
      InstanceCount::decrement(this);
    }
    /// Define collections created by this sensitivie action object
    virtual void defineCollections() {}
    /// G4VSensitiveDetector interface: Method invoked at the begining of each event.
    virtual void begin(G4HCofThisEvent* hce) {
      Base::begin(hce);
    }
    /// G4VSensitiveDetector interface: Method invoked at the end of each event.
    virtual void end(G4HCofThisEvent* hce) {
      Base::end(hce);
    }
    /// G4VSensitiveDetector interface: Method for generating hit(s) using the G4Step object.
    virtual bool process(G4Step* step,G4TouchableHistory* history)  {
      return Base::process(step,history);
    }
    /// G4VSensitiveDetector interface: Method invoked if the event was aborted.
    virtual void clear(G4HCofThisEvent* hce) {
      Base::clear(hce);
    }
  };


  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  //               Geant4SensitiveAction<SimpleTracker>
  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  /// Deprecated: Simple SensitiveAction class ...
  /**
   *  \deprecated
   *  \author  M.Frank
   *  \version 1.0
   */
  class LcioTestTracker {};

  /// Define collections created by this sensitivie action object
  template <> void Geant4SensitiveAction<LcioTestTracker>::defineCollections() {
    m_collectionID = Base::defineCollection<lcio::SimTrackerHitImpl>(m_sensitive.readout().name());
  }

  /// Method for generating hit(s) using the information of G4Step object.
  template <> bool Geant4SensitiveAction<LcioTestTracker>::process(G4Step* step,G4TouchableHistory* /*hist*/ ) {
    StepHandler h(step);

    Position prePos    = h.prePos();
    Position postPos   = h.postPos();
    Position direction = postPos - prePos;
    Position position  = mean_direction(prePos,postPos);
    double   hit_len   = direction.R();

    if (hit_len > 0) {
      double new_len = mean_length(h.preMom(),h.postMom())/hit_len;
      direction *= new_len/hit_len;
    }

    lcio::SimTrackerHitImpl* hit = new lcio::SimTrackerHitImpl;
    //    (h.track->GetTrackID(),
    // h.track->GetDefinition()->GetPDGEncoding(),
    // step->GetTotalEnergyDeposit(),
    // h.track->GetGlobalTime());

    // HitContribution contrib = Hit::extractContribution(step);

    VolumeID cell = volumeID( step ) ;
    hit->setCellID0( cell & 0xffffffff   ) ;
    hit->setCellID1( ( cell >> 32 ) & 0xffffffff   ) ;

    printout(INFO,"LcioTestTracker","%s> Add hit with deposit:%f  Pos:%f %f %f - cellID0: 0x%x cellID1: 0x%x",
             c_name(),step->GetTotalEnergyDeposit(),position.X(),position.Y(),position.Z() , hit->getCellID0() ,hit->getCellID1() );

    double pos[3] = {position.x(), position.y(), position.z()};
    hit->setPosition( pos  ) ;

    // hit->energyDeposit = contrib.deposit ;
    // hit->position      = position;
    // hit->momentum      = direction;
    // hit->length        = hit_len;
    collection(m_collectionID)->add(hit);
    return hit != 0;
  }

  typedef Geant4SensitiveAction<LcioTestTracker> LcioTestTrackerAction;
} // namespace

#include "DDG4/Factories.h"
DECLARE_GEANT4SENSITIVE_NS(Tests,LcioTestTrackerAction)
