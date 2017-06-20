// $Id: $
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
#include "DDG4/Geant4StepHandler.h"
#include "DDG4/Geant4Mapping.h"

// C include files
#include <stdexcept>

/*
 *   dd4hep::sim namespace declaration
 */
namespace dd4hep {  namespace sim {

    /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    ///               Geant4GenericSD<Tracker>
    /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    struct Tracker {};
    /// Method for generating hit(s) using the information of G4Step object.
    template <> bool Geant4GenericSD<Tracker>::buildHits(G4Step* step,G4TouchableHistory* /*hist*/ ) {

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

      //      G4cout << "----------- Geant4GenericSD<Tracker>::buildHits : position : " << prePos << G4endl ;

      Geant4TrackerHit* hit =
        new Geant4TrackerHit(h.track->GetTrackID(),
                             h.track->GetDefinition()->GetPDGEncoding(),
                             step->GetTotalEnergyDeposit(),
                             h.track->GetGlobalTime());

      if ( hit )  {
        HitContribution contrib = Geant4Hit::extractContribution(step);
        hit->cellID  = getCellID( step ) ;
        hit->energyDeposit =  contrib.deposit ;
        hit->position = position;
        hit->momentum = direction;
        hit->length   = hit_len;
        collection(0)->insert(hit);
        return 1;
      }
      throw std::runtime_error("new() failed: Cannot allocate hit object");
    }
    typedef Geant4GenericSD<Tracker> Geant4Tracker;
  }}    // End namespace dd4hep::sim

DECLARE_GEANT4SENSITIVEDETECTOR_NS(dd4hep::sim,Geant4Tracker)
