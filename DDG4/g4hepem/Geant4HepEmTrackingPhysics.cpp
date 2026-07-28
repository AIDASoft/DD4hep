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

/** \addtogroup Geant4PhysicsConstructor
 *
 * @{
 * \package Geant4HepEmTrackingPhysics
 * \brief PhysicsConstructor enabling G4HepEm vectorised EM tracking for e-/e+/gamma
 *
 * This plugin replaces the Geant4 stepping loop for electrons, positrons and
 * photons with the G4HepEmTrackingManager, which provides vectorised EM physics
 * using SIMD intrinsics (AVX2/AVX-512).  Hadronic physics from the base physics
 * list (e.g. FTFP_BERT) is unaffected: G4VTrackingManager::SetTrackingManager()
 * takes priority over the standard process manager for the three EM particles,
 * while all other particles continue to use the default stepping loop.
 *
 * HepEm can be restricted to selected G4Region names via the HepEmRegions
 * property. If HepEmRegions is empty (default), HepEm applies in all regions; if
 * non-empty but no regions are resolved, HepEm tracking is disabled (with a warning).
 * Woodcock tracking for photons can be enabled per G4Region via the
 * WoodcockRegions property.  Woodcock tracking allows photons to bypass
 * fine-grained geometry navigation (e.g. individual fibres in a ScFi
 * calorimeter) by sampling interactions against a majorant cross-section.
 * When HepEmRegions is non-empty, WoodcockRegions is applied only to the
 * intersection with HepEmRegions; non-overlapping Woodcock regions are ignored
 * with a warning.
 *
 * Usage in a ddsim/npsim steering file:
 * \code{.py}
 *   SIM.physics.list = "FTFP_BERT"
 *
 *   def setupHepEm(kernel):
 *     from DDG4 import PhysicsList
 *     seq = kernel.physicsList()
 *     hepem = PhysicsList(kernel, 'Geant4HepEmTrackingPhysics/HepEmPhysics')
 *     hepem.HepEmRegions = ['EcalBarrelScFiLayerRegion']
 *     hepem.VerboseLevel = 1
 *     hepem.WoodcockRegions = ['EcalBarrelScFiLayerRegion']
 *     hepem.enableUI()
 *     seq.adopt(hepem)
 *
 *   SIM.physics.setupUserPhysics(setupHepEm)
 * \endcode
 *
 * The DDG4HepEm plugin library must be in LD_LIBRARY_PATH.
 *
 * @}
 */

#ifndef DDG4_GEANT4HEPEMTRACKINGPHYSICS_H
#define DDG4_GEANT4HEPEMTRACKINGPHYSICS_H 1

/// Framework include files
#include <DDG4/Geant4PhysicsList.h>

/// Geant4 include files
#include <G4Electron.hh>
#include <G4EventManager.hh>
#include <G4Gamma.hh>
#include <G4Positron.hh>
#include <G4RegionStore.hh>
#include <G4TrackStatus.hh>
#include <G4VUserPhysicsList.hh>

/// G4HepEm include files
#include <G4HepEmConfig.hh>
#include <G4HepEmTrackingManager.hh>

/// C++ include files
#include <memory>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    namespace {

      /// Tracking manager wrapper that applies HepEm only in selected regions.
      class Geant4RegionSelectiveHepEmTrackingManager : public G4VTrackingManager {
      public:
        Geant4RegionSelectiveHepEmTrackingManager(bool trackInAllRegions,
                                                  std::unordered_set<G4int> hepEmRegionIDs,
                                                  G4int verbose)
          : fTrackInAllRegions(trackInAllRegions)
          , fHepEmRegionIDs(std::move(hepEmRegionIDs))
          , fHepEmTrackingManager(new G4HepEmTrackingManager(verbose)) {}

        virtual ~Geant4RegionSelectiveHepEmTrackingManager() = default;

        G4HepEmConfig* GetConfig() { return fHepEmTrackingManager->GetConfig(); }

        void BuildPhysicsTable(const G4ParticleDefinition& part) override {
          fHepEmTrackingManager->BuildPhysicsTable(part);
        }

        void PreparePhysicsTable(const G4ParticleDefinition& part) override {
          fHepEmTrackingManager->PreparePhysicsTable(part);
        }

        void FlushEvent() override {
          fHepEmTrackingManager->FlushEvent();
        }

        void HandOverOneTrack(G4Track* aTrack) override {
          if (useHepEm(aTrack)) {
            fHepEmTrackingManager->HandOverOneTrack(aTrack);
            return;
          }
          trackWithGeant4(aTrack);
        }

      private:
        bool useHepEm(const G4Track* aTrack) const {
          if (fTrackInAllRegions) {
            return true;
          }
          const G4VPhysicalVolume* volume = aTrack->GetVolume();
          if (volume == nullptr && aTrack->GetTouchableHandle()) {
            volume = aTrack->GetTouchableHandle()->GetVolume();
          }
          if (volume == nullptr) {
            return false;
          }
          const G4LogicalVolume* logical = volume->GetLogicalVolume();
          if (logical == nullptr) {
            return false;
          }
          const G4Region* region = logical->GetRegion();
          if (region == nullptr) {
            return false;
          }
          return fHepEmRegionIDs.find(region->GetInstanceID()) != fHepEmRegionIDs.end();
        }

        void trackWithGeant4(G4Track* track) {
          auto* eventManager = G4EventManager::GetEventManager();
          auto* trackManager = eventManager->GetTrackingManager();
          auto* stackManager = eventManager->GetStackManager();

          trackManager->ProcessOneTrack(track);

          G4TrackStatus track_status = track->GetTrackStatus();
          G4VTrajectory* trajectory =
              trackManager->GetStoreTrajectory() == 0 ? nullptr : trackManager->GimmeTrajectory();
          G4TrackVector* secondaries = trackManager->GimmeSecondaries();

          switch (track_status) {
          case fStopButAlive:
          case fSuspend:
          case fSuspendAndWait:
            stackManager->PushOneTrack(track, trajectory);
            trajectory = nullptr;
            eventManager->StackTracks(secondaries);
            break;
          case fPostponeToNextEvent:
            stackManager->PushOneTrack(track);
            eventManager->StackTracks(secondaries);
            delete trajectory;
            break;
          case fStopAndKill:
            eventManager->StackTracks(secondaries);
            delete trajectory;
            delete track;
            break;
          case fKillTrackAndSecondaries:
            if (secondaries != nullptr) {
              for (auto& secondary : *secondaries) {
                delete secondary;
              }
              secondaries->clear();
            }
            delete trajectory;
            delete track;
            break;
          default:
            G4cerr << "+++ Geant4RegionSelectiveHepEmTrackingManager: unexpected track status from "
                      "G4TrackingManager, killing track."
                   << G4endl;
            eventManager->StackTracks(secondaries);
            delete trajectory;
            delete track;
            break;
          }
        }

        bool fTrackInAllRegions;
        std::unordered_set<G4int> fHepEmRegionIDs;
        std::unique_ptr<G4HepEmTrackingManager> fHepEmTrackingManager;
      };

    }  // namespace

    /// DDG4 physics constructor that installs the G4HepEm vectorised EM tracking manager
    /**
     * Replaces the Geant4 stepping loop for e-, e+ and gamma with
     * G4HepEmTrackingManager, providing SIMD-vectorised EM physics.
     *
     * A single G4HepEmTrackingManager instance is created per call to
     * constructProcesses() and registered with all three particle types
     * (e-, e+, gamma).  This is the canonical G4HepEm usage pattern:
     * G4HepEmTrackingManager::HandOverOneTrack() dispatches internally on
     * particle type, so one shared instance handles all three species.
     *
     * Geant4 owns and deletes the tracking manager.
     *
     * \author  W. Deconinck
     * \version 1.0
     * \ingroup DD4HEP_SIMULATION
     */
    class Geant4HepEmTrackingPhysics : public Geant4PhysicsList {
    protected:
      /// Define standard assignments and constructors
      DDG4_DEFINE_ACTION_CONSTRUCTORS(Geant4HepEmTrackingPhysics);

      /// G4Region names in which Woodcock tracking is activated for photons
      std::vector<std::string> m_woodcockRegions;
      /// G4Region names in which HepEm tracking manager is activated
      std::vector<std::string> m_hepEmRegions;
      /// Verbosity forwarded to the underlying G4HepEmTrackingManager
      G4int m_verbosity;

    public:
      /// Standard constructor
      Geant4HepEmTrackingPhysics(Geant4Context* context, const std::string& nam)
        : Geant4PhysicsList(context, nam) {
        declareProperty("WoodcockRegions", m_woodcockRegions);
        declareProperty("HepEmRegions", m_hepEmRegions);
        declareProperty("VerboseLevel", m_verbosity = 0);
      }

      /// Default destructor
      virtual ~Geant4HepEmTrackingPhysics() = default;

      /// Callback to install the G4HepEmTrackingManager on e-, e+ and gamma
      /**
       * Called from within G4VPhysicsConstructor::ConstructProcess() by the DDG4
       * action sequence, after ConstructParticle() has been executed.
       *
       * Creates one G4HepEmTrackingManager and registers it with all three EM
       * particle types.  Ownership is transferred to Geant4: the pointer is
       * stored (non-owning) inside each particle's TLS data slot, and
       * G4VUserPhysicsList::RemoveTrackingManager() will delete it at thread /
       * run teardown.
       */
      virtual void constructProcesses(G4VUserPhysicsList* /* physics_list */) override {
        const bool trackInAllRegions = m_hepEmRegions.empty();
        std::unordered_set<G4int> hepEmRegionIDs;
        if (trackInAllRegions) {
          info("+++ HepEm region selection: all regions");
        } else {
          for (const auto& regionName : m_hepEmRegions) {
            G4Region* region = G4RegionStore::GetInstance()->GetRegion(regionName, false);
            if (region == nullptr) {
              warning("+++ HepEm region not found in G4RegionStore: %s", regionName.c_str());
              continue;
            }
            hepEmRegionIDs.insert(region->GetInstanceID());
            info("+++ HepEm enabled in G4Region: %s", regionName.c_str());
          }
          if (hepEmRegionIDs.empty()) {
            warning("+++ No valid HepEmRegions were resolved; HepEm tracking will be disabled.");
          }
        }
        const std::unordered_set<G4int> resolvedHepEmRegionIDs = hepEmRegionIDs;

        auto* tm =
            new Geant4RegionSelectiveHepEmTrackingManager(trackInAllRegions, std::move(hepEmRegionIDs), m_verbosity);

        G4int configuredWoodcockRegions = 0;
        for (const auto& region : m_woodcockRegions) {
          G4Region* woodcockRegion = G4RegionStore::GetInstance()->GetRegion(region, false);
          if (woodcockRegion == nullptr) {
            warning("+++ Woodcock region not found in G4RegionStore: %s", region.c_str());
            continue;
          }
          if (!trackInAllRegions
              && resolvedHepEmRegionIDs.find(woodcockRegion->GetInstanceID()) == resolvedHepEmRegionIDs.end()) {
            warning("+++ Ignoring Woodcock region '%s': region is outside HepEmRegions selection", region.c_str());
            continue;
          }
          info("+++ Enabling Woodcock photon tracking in G4Region: %s", region.c_str());
          tm->GetConfig()->SetWoodcockTrackingRegion(region);
          ++configuredWoodcockRegions;
        }
        if (!trackInAllRegions && !m_woodcockRegions.empty() && configuredWoodcockRegions == 0) {
          warning("+++ No WoodcockRegions remain after applying HepEmRegions intersection.");
        }

        // Warn if another custom tracking manager is already installed —
        // SetTrackingManager() is exclusive and will silently replace it.
        auto warnIfConflict = [&](G4ParticleDefinition* particle) {
          if (particle->GetTrackingManager() != nullptr) {
            warning("+++ Replacing existing tracking manager for %s with G4HepEmTrackingManager",
                    particle->GetParticleName().c_str());
          }
        };
        warnIfConflict(G4Electron::Definition());
        warnIfConflict(G4Positron::Definition());
        warnIfConflict(G4Gamma::Definition());

        G4Electron::Definition()->SetTrackingManager(tm);
        G4Positron::Definition()->SetTrackingManager(tm);
        G4Gamma::Definition()   ->SetTrackingManager(tm);

        info("+++ Installed G4HepEmTrackingManager for e-/e+/gamma");
      }
    };
  }  // namespace sim
}  // namespace dd4hep

#endif  // DDG4_GEANT4HEPEMTRACKINGPHYSICS_H

#include <DDG4/Factories.h>
using namespace dd4hep::sim;
DECLARE_GEANT4ACTION(Geant4HepEmTrackingPhysics)
