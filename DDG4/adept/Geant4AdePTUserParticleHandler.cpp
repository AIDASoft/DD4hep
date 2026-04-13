//==========================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//==========================================================================

/// @file Geant4AdePTUserParticleHandler.cpp
/// @brief DDG4 user particle handler that repairs the MC truth parent chain
///        for GPU-returned AdePT tracks.
///
/// When AdePT's callUserTrackingAction is false (the default for performance),
/// AdePT does not maintain CPU-side HostTrackData for GPU-tracked particles.
/// Tracks returned from the GPU (leaked EM tracks and hadronic secondaries
/// produced on the GPU) carry trackID=0 and parentID=0 (from the dummy
/// HostTrackData).  Without correction, these tracks and their CPU-side
/// daughters have g4Parent=0, which is not in m_equivalentTracks, causing
/// "No real particle parent" FATALs and a failed consistency check in
/// Geant4ParticleHandler.
///
/// With callUserTrackingAction=false:
///  - Leaked EM tracks (e-/e+/gamma) are handled by AdePTTrackingManager which
///    does NOT call Pre/PostUserTrackingAction for them.  They are re-offloaded
///    to the GPU immediately and never appear in begin()/end().
///  - Hadronic secondaries produced on the GPU are returned as new Geant4
///    tracks with parentID=0 (dummy) and are tracked by the standard
///    G4TrackingManager (since AdePTTrackingManager is only registered for
///    e-/e+/gamma).  They DO go through begin()/end().
///
/// This handler fixes the parent chain at the moment the track is first seen
/// (PreUserTrackingAction -> begin()):
///  - It records the G4 track ID of the entering primary (G4PARTICLE_PRIMARY).
///  - For hadronic secondaries with parentID=0 (GPU dummy data), it replaces
///    particle.g4Parent with the entering primary's G4 track ID.  This gives
///    them a valid ancestor that is already in m_particleMap (the primary was
///    stored there by the PostUserTrackingAction fix in AdePTTrackingManager).
///
/// Adopt this handler in the steering file alongside Geant4ParticleHandler:
/// @code
///   part = DDG4.GeneratorAction(kernel, "Geant4ParticleHandler/ParticleHandler")
///   ...
///   user = DDG4.Action(kernel, "Geant4AdePTUserParticleHandler/AdePTParticleHandler")
///   part.adopt(user)
/// @endcode

#include <DDG4/Factories.h>
#include <DDG4/Geant4Particle.h>
#include <DDG4/Geant4UserParticleHandler.h>
#include <Parsers/Primitives.h>

#include <G4Track.hh>

namespace dd4hep { namespace sim {

using PropertyMask = dd4hep::detail::ReferenceBitMask<int>;

  class Geant4AdePTUserParticleHandler : public Geant4UserParticleHandler {
  public:
    Geant4AdePTUserParticleHandler(Geant4Context* ctxt, std::string const& name)
      : Geant4UserParticleHandler(ctxt, name) {}

    DDG4_DEFINE_ACTION_CONSTRUCTORS(Geant4AdePTUserParticleHandler);

    /// Called at the start of every track (PreUserTrackingAction).
    ///
    /// Two cases are handled here:
    ///
    ///  1. The entering primary (G4PARTICLE_PRIMARY):
    ///     Record its G4 track ID as m_primaryG4Id so we can use it as the
    ///     surrogate parent for GPU-produced hadronic secondaries.
    ///
    ///  2. Hadronic secondaries of GPU tracks (parentID=0, not a real primary):
    ///     With callUserTrackingAction=false, GPU-produced hadronic secondaries
    ///     are returned to Geant4 with parentID=0 (from the dummy HostTrackData).
    ///     These tracks go through the standard G4TrackingManager (since
    ///     AdePTTrackingManager only handles e-/e+/gamma) and are seen in begin().
    ///     We replace g4Parent with the entering primary's G4 track ID so that
    ///     Geant4ParticleHandler can walk to a stored ancestor and the
    ///     consistency check succeeds.
    void begin(const G4Track* track, Particle& particle) override {
      PropertyMask mask(particle.reason);
      if (mask.isSet(G4PARTICLE_PRIMARY)) {
        m_primaryG4Id = track->GetTrackID();
      } else if (track->GetParentID() == 0) {
        // Hadronic secondary returned from GPU with dummy parentID=0
        particle.g4Parent = m_primaryG4Id;
      }
    }

  private:
    int m_primaryG4Id{0};
  };

}} // namespace dd4hep::sim

DECLARE_GEANT4ACTION(Geant4AdePTUserParticleHandler)
