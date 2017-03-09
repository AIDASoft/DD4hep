// $Id: $
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
#ifndef DD4HEP_DDG4_GEANT4TCUSERPARTICLEHANDLER_H
#define DD4HEP_DDG4_GEANT4TCUSERPARTICLEHANDLER_H

// Framework include files
#include "DD4hep/Primitives.h"
#include "DDG4/Geant4UserParticleHandler.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    ///  Rejects to keep particles, which are created outside a tracking cylinder.
    /** Geant4TCUserParticleHandler
     *
     *  TC stands for TrackingCylinder ;-)
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4TCUserParticleHandler : public Geant4UserParticleHandler  {
      double m_zTracker, m_rTracker;
    public:
      /// Standard constructor
      Geant4TCUserParticleHandler(Geant4Context* context, const std::string& nam);

      /// Default destructor
      virtual ~Geant4TCUserParticleHandler() {}

      /// Post-track action callback
      /** Allow the user to force the particle handling in the post track action
       *  set the reason mask to NULL in order to drop the particle.
       *  The parent's reasoning mask will be or'ed with the particle's mask
       *  to preserve the MC truth for the hit creation.
       *  The default implementation is empty.
       *
       *  Note: The particle passed is a temporary and will be copied if kept.
       */
      virtual void end(const G4Track* track, Particle& particle);

      /// Post-event action callback: avoid warning (...) was hidden [-Woverloaded-virtual]
      virtual void end(const G4Event* event);

    };
  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4TCUSERPARTICLEHANDLER_H

// $Id: Geant4Field.cpp 888 2013-11-14 15:54:56Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
// Framework include files
//#include "DDG4/Geant4TCUserParticleHandler.h"
#include "DDG4/Geant4Particle.h"
#include "DDG4/Factories.h"


using namespace DD4hep::Simulation;
DECLARE_GEANT4ACTION(Geant4TCUserParticleHandler)

/// Standard constructor
Geant4TCUserParticleHandler::Geant4TCUserParticleHandler(Geant4Context* ctxt, const std::string& nam)
: Geant4UserParticleHandler(ctxt,nam)
{
  declareProperty("TrackingVolume_Zmax",m_zTracker=1e100);
  declareProperty("TrackingVolume_Rmax",m_rTracker=1e100);
}

/// Post-track action callback
void Geant4TCUserParticleHandler::end(const G4Track* /* track */, Particle& p)  {

  double r_prod = std::sqrt(p.vsx*p.vsx + p.vsy*p.vsy);
  double z_prod = std::fabs(p.vsz);
  bool starts_in_trk_vol = ( r_prod <= m_rTracker && z_prod <= m_zTracker )  ;

  DD4hep::ReferenceBitMask<int> reason(p.reason);

  if( reason.isSet(G4PARTICLE_PRIMARY) ) {
    //do nothing
  } else if( starts_in_trk_vol && ! reason.isSet(G4PARTICLE_ABOVE_ENERGY_THRESHOLD) )  {
    // created in tracking volume but below energy cut
    p.reason = 0;
    return;
  }

  double r_end  = std::sqrt(p.vex*p.vex + p.vey*p.vey);
  double z_end  = std::fabs(p.vez);
  bool ends_in_trk_vol =  ( r_end <= m_rTracker && z_end <= m_zTracker ) ;

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

  // Set the simulator status bits
  DD4hep::ReferenceBitMask<int> simStatus(p.status);

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

  return ;

}

/// Post-event action callback
void Geant4TCUserParticleHandler::end(const G4Event* /* event */)   {

}

