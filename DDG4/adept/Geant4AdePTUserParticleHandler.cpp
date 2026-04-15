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
/// @brief DDG4 user particle handler for AdePT GPU-accelerated EM transport.
///
/// AdePT offloads e-/e+/γ tracks to the GPU via AdePTTrackingManager.  When
/// CallUserTrackingAction=true, AdePT calls PreUserTrackingAction at step 0
/// (begin()) and PostUserTrackingAction at the last GPU step (end()).  Between
/// these two calls many CPU-side hadronic tracks may run, each overwriting
/// Geant4ParticleHandler::m_currTrack.  By the time end() is invoked for the
/// GPU-returned track, m_currTrack is stale.
///
/// This handler corrects this by:
///
///  1. Caching bookkeeping fields of every e-/e+/γ track at begin() time.
///  2. In end(), if m_currTrack appears stale (particle.id differs from the
///     cached id), restoring the bookkeeping fields so that
///     part->get_data(m_currTrack) at line 418 of Geant4ParticleHandler.cpp
///     stores the correct values.
///     Note: pex/pey/pez and vex/vey/vez are NOT restored — they are already
///     set correctly from the G4Track before end() is called.
///
///  3. (Safety net for hadronic secondaries from GPU with parentID=0):
///     Records the entering primary's G4 track ID and remaps g4Parent for
///     tracks that arrive with parentID=0 (GPU dummy data).

#include <DDG4/Factories.h>
#include <DDG4/Geant4Particle.h>
#include <DDG4/Geant4UserParticleHandler.h>
#include <Parsers/Primitives.h>

#include <G4Event.hh>
#include <G4Track.hh>

#include <unordered_map>

namespace dd4hep { namespace sim {

using PropertyMask = dd4hep::detail::ReferenceBitMask<int>;

  class Geant4AdePTUserParticleHandler : public Geant4UserParticleHandler {
  public:
    Geant4AdePTUserParticleHandler(Geant4Context* ctxt, std::string const& name)
      : Geant4UserParticleHandler(ctxt, name) {}

    DDG4_DEFINE_ACTION_CONSTRUCTORS(Geant4AdePTUserParticleHandler);

    void begin(const G4Event* /*event*/) override { m_trackCache.clear(); }
    void end  (const G4Event* /*event*/) override { m_trackCache.clear(); }

    /// Called at PreUserTrackingAction.
    ///
    /// Caches bookkeeping fields for e-/e+/γ tracks so they can be restored
    /// in end() if m_currTrack was overwritten by intervening CPU tracks.
    /// Also remaps g4Parent=0 for GPU-produced hadronic secondaries that carry
    /// dummy parentID from the GPU HostTrackData.
    void begin(const G4Track* track, Particle& particle) override {
      PropertyMask mask(particle.reason);
      if (mask.isSet(G4PARTICLE_PRIMARY)) {
        m_primaryG4Id = track->GetTrackID();
      } else if (track->GetParentID() == 0) {
        // Hadronic secondary returned from GPU with dummy parentID=0
        particle.g4Parent = m_primaryG4Id;
      }

      // Cache bookkeeping for e-/e+/γ (the particles AdePT handles).
      // They may be returned from the GPU long after begin() set m_currTrack.
      int pdg = track->GetParticleDefinition()->GetPDGEncoding();
      if (pdg == 11 || pdg == -11 || pdg == 22) {
        m_trackCache.emplace(track->GetTrackID(), CachedState(particle));
      }
    }

    /// Called at PostUserTrackingAction (inside Geant4ParticleHandler::end()).
    ///
    /// If m_currTrack is stale (particle.id != cached id for this G4 track ID),
    /// restores the bookkeeping fields saved at begin() time.
    /// pex/pey/pez and vex/vey/vez are intentionally NOT restored since they
    /// are already set correctly from the G4Track before this callback.
    void end(const G4Track* track, Particle& particle) override {
      auto it = m_trackCache.find(track->GetTrackID());
      if (it == m_trackCache.end()) return;

      if (it->second.id != particle.id) {
        it->second.restoreTo(particle);
      }
      m_trackCache.erase(it);
    }

  private:
    /// Bookkeeping snapshot saved at begin() for one e-/e+/γ track.
    /// Fields that are set from the G4Track in end() (pex/pey/pez, vex/vey/vez)
    /// and fields that grow during tracking (daughters) or are user-owned
    /// (extension) are intentionally excluded.
    struct CachedState {
      int    id{0}, originalG4ID{0}, g4Parent{0};
      int    reason{0}, mask{0}, status{0};
      int    steps{0}, secondaries{0}, pdgID{0};
      unsigned short genStatus{0};
      char   charge{0};
      double vsx{0}, vsy{0}, vsz{0};
      double psx{0}, psy{0}, psz{0};
      double mass{0}, time{0}, properTime{0};
      const G4VProcess* process{nullptr};
      Particle::Particles parents;

      CachedState() = default;
      explicit CachedState(const Particle& p)
        : id(p.id), originalG4ID(p.originalG4ID), g4Parent(p.g4Parent),
          reason(p.reason), mask(p.mask), status(p.status),
          steps(p.steps), secondaries(p.secondaries), pdgID(p.pdgID),
          genStatus(p.genStatus), charge(p.charge),
          vsx(p.vsx), vsy(p.vsy), vsz(p.vsz),
          psx(p.psx), psy(p.psy), psz(p.psz),
          mass(p.mass), time(p.time), properTime(p.properTime),
          process(p.process), parents(p.parents) {}

      void restoreTo(Particle& p) const {
        p.id          = id;
        p.originalG4ID= originalG4ID;
        p.g4Parent    = g4Parent;
        p.reason      = reason;
        p.mask        = mask;
        p.status      = status;
        p.steps       = steps;
        p.secondaries = secondaries;
        p.pdgID       = pdgID;
        p.genStatus   = genStatus;
        p.charge      = charge;
        p.vsx = vsx; p.vsy = vsy; p.vsz = vsz;
        p.psx = psx; p.psy = psy; p.psz = psz;
        p.mass        = mass;
        p.time        = time;
        p.properTime  = properTime;
        p.process     = process;
        p.parents     = parents;
      }
    };

    int m_primaryG4Id{0};
    std::unordered_map<int, CachedState> m_trackCache;
  };

}} // namespace dd4hep::sim

DECLARE_GEANT4ACTION(Geant4AdePTUserParticleHandler)
