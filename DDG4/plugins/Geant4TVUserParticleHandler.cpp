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

/** \addtogroup Geant4Action
 *
 @{
   \package Geant4TVUserParticleHandler
 * \brief Rejects to keep particles, which are created outside a tracking cylinder.
 *
 *
@}
 */

#ifndef DD4HEP_DDG4_GEANT4TVUSERPARTICLEHANDLER_H
#define DD4HEP_DDG4_GEANT4TVUSERPARTICLEHANDLER_H

// Framework include files
#include <DD4hep/Primitives.h>
#include <DD4hep/Volumes.h>
#include <DDG4/Geant4UserParticleHandler.h>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    ///  Rejects to keep particles, which are created outside a tracking volume.
    /** Geant4TVUserParticleHandler
     *
     *  TV stands for TrackingVolume ;-)
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4TVUserParticleHandler : public Geant4UserParticleHandler  {
      Volume m_trackingVolume;

    public:
      /// Standard constructor
      Geant4TVUserParticleHandler(Geant4Context* context, const std::string& nam);

      /// Default destructor
      virtual ~Geant4TVUserParticleHandler() {}

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
  }    // End namespace sim
}      // End namespace dd4hep

#endif // DD4HEP_DDG4_GEANT4TVUSERPARTICLEHANDLER_H

//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
// Framework include files
//#include <DDG4/Geant4TVUserParticleHandler.h>
#include <DDG4/Factories.h>
#include <DDG4/Geant4Particle.h>
#include <DDG4/Geant4Kernel.h>


using namespace dd4hep::sim;
DECLARE_GEANT4ACTION(Geant4TVUserParticleHandler)

/// Standard constructor
Geant4TVUserParticleHandler::Geant4TVUserParticleHandler(Geant4Context* ctxt, const std::string& nam)
: Geant4UserParticleHandler(ctxt,nam)
{
  m_trackingVolume = ctxt->kernel().detectorDescription().trackingVolume();
}

/// Post-track action callback
void Geant4TVUserParticleHandler::end(const G4Track* /* track */, Particle& p)  {

  std::array<double, 3> start_point = {p.vsx, p.vsy, p.vsz};
  bool starts_in_trk_vol = m_trackingVolume.ptr()->Contains(start_point.data());

  dd4hep::detail::ReferenceBitMask<int> reason(p.reason);

  if( reason.isSet(G4PARTICLE_PRIMARY) ) {
    //do nothing
  } else if( starts_in_trk_vol && ! reason.isSet(G4PARTICLE_ABOVE_ENERGY_THRESHOLD) )  {
    // created in tracking volume but below energy cut
    p.reason = 0;
    return;
  }

  std::array<double, 3> end_point = {p.vex, p.vey, p.vez};
  bool ends_in_trk_vol = m_trackingVolume.ptr()->Contains(end_point.data());

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

  return ;

}

/// Post-event action callback
void Geant4TVUserParticleHandler::end(const G4Event* /* event */)   {

}

