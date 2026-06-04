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
 * Woodcock tracking for photons can be enabled per G4Region via the
 * WoodcockRegions property.  Woodcock tracking allows photons to bypass
 * fine-grained geometry navigation (e.g. individual fibres in a ScFi
 * calorimeter) by sampling interactions against a majorant cross-section.
 *
 * Usage in a ddsim/npsim steering file:
 * \code{.py}
 *   SIM.physics.list = "FTFP_BERT"
 *
 *   def setupHepEm(kernel):
 *     from DDG4 import PhysicsList
 *     seq = kernel.physicsList()
 *     hepem = PhysicsList(kernel, 'Geant4HepEmTrackingPhysics/HepEmPhysics')
 *     hepem.WoodcockRegions = ['EcalBarrelRegion']
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
#include <G4Gamma.hh>
#include <G4Positron.hh>
#include <G4VUserPhysicsList.hh>

/// G4HepEm include files
#include <G4HepEmConfig.hh>
#include <G4HepEmTrackingManager.hh>

/// C++ include files
#include <string>
#include <vector>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    /// DDG4 physics constructor that installs the G4HepEm vectorised EM tracking manager
    /**
     * Replaces the Geant4 stepping loop for e-, e+ and gamma with
     * G4HepEmTrackingManager, providing SIMD-vectorised EM physics.
     * A single tracking manager instance is shared among the three particle
     * types, which is the standard G4HepEm usage pattern.
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

    public:
      /// Standard constructor
      Geant4HepEmTrackingPhysics(Geant4Context* context, const std::string& nam)
        : Geant4PhysicsList(context, nam) {
        declareProperty("WoodcockRegions", m_woodcockRegions);
      }

      /// Default destructor
      virtual ~Geant4HepEmTrackingPhysics() = default;

      /// Callback to install the G4HepEmTrackingManager on e-, e+ and gamma
      /**
       * Called from within G4VPhysicsConstructor::ConstructProcess() by the DDG4
       * action sequence, after ConstructParticle() has been executed.
       */
      virtual void constructProcesses(G4VUserPhysicsList* /* physics_list */) override {
        auto* tm = new G4HepEmTrackingManager();

        for (const auto& region : m_woodcockRegions) {
          info("+++ Enabling Woodcock photon tracking in G4Region: %s", region.c_str());
          tm->GetConfig()->SetWoodcockTrackingRegion(region);
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
