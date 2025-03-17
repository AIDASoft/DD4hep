
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

#include "Geant4UserParticleHandlerHelper.h"
#include <DDG4/Geant4Particle.h>
#include <DDG4/Geant4UserParticleHandler.h>

// using namespace dd4hep::sim;

namespace dd4hep::sim {

void setReason(Geant4Particle& p, bool starts_in_trk_vol, bool ends_in_trk_vol) {

  dd4hep::detail::ReferenceBitMask<int> reason(p.reason);

  if( reason.isSet(G4PARTICLE_PRIMARY) ) {
    //do nothing
  } else if( starts_in_trk_vol && ! reason.isSet(G4PARTICLE_ABOVE_ENERGY_THRESHOLD) )  {
    // created in tracking volume but below energy cut
    p.reason = 0;
    return;
  }

  // created and ended in calo but not primary particle
  //
  // we can have particles from the generator only in the calo, if we have a
  // long particle with preassigned decay, we need to keep the reason or the
  // MChistory will not be updated later on
  if( not reason.isSet(G4PARTICLE_PRIMARY) ) {
    if( !starts_in_trk_vol ) {
      if( !ends_in_trk_vol ){
	p.reason = 0;
      }
      //fg: dont keep backscatter that did not create a tracker hit
      else if( ! reason.isSet(G4PARTICLE_CREATED_TRACKER_HIT) ) {
	p.reason = 0;
      }
    }
  }
}

void setSimulatorStatus(Geant4Particle& p, bool starts_in_trk_vol, bool ends_in_trk_vol) {
  // Set the simulator status bits
  dd4hep::detail::ReferenceBitMask<int> simStatus(p.status);

  if( ends_in_trk_vol ) {
    simStatus.set(G4PARTICLE_SIM_DECAY_TRACKER);
  }

  // if the particle doesn't end in the tracker volume it must have ended in the calorimeter
  if( not ends_in_trk_vol && not simStatus.isSet(G4PARTICLE_SIM_LEFT_DETECTOR) ) {
    simStatus.set(G4PARTICLE_SIM_DECAY_CALO);
  }

  if( not starts_in_trk_vol && ends_in_trk_vol ) {
    simStatus.set(G4PARTICLE_SIM_BACKSCATTER);
  }
}

}