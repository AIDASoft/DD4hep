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

/** \addtogroup Geant4PhysicsConstructor
 *
 * @{
 * \package Geant4Optical PhotonPhysics
 *
 * \brief PhysicsConstructor for Optical Photon physics

 This plugin allows to enable Optical Photon physics in the physics list

 *
 * @}
 */
#ifndef DDG4_GEANT4OPTICALPHOTONPHYSICS_H
#define DDG4_GEANT4OPTICALPHOTONPHYSICS_H 1

// Framework include files
#include <DDG4/Geant4PhysicsList.h>

/// Geant4 include files
#include <G4OpAbsorption.hh>
#include <G4OpRayleigh.hh>
#include <G4OpMieHG.hh>
#include <G4OpBoundaryProcess.hh>
#include <G4OpWLS.hh>
#include <G4OpWLS2.hh>
#include <G4ParticleDefinition.hh>
#include <G4ParticleTypes.hh>
#include <G4ParticleTable.hh>
#include <G4ProcessManager.hh>
#include <G4Version.hh>

#if G4VERSION_NUMBER >= 1070
#include <G4OpticalParameters.hh>
#endif

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    /// Geant4 physics list action to enable OpticalPhoton physics
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4OpticalPhotonPhysics : public Geant4PhysicsList    {
    public:
      /// Default constructor
      Geant4OpticalPhotonPhysics() = delete;
      /// Copy constructor
      Geant4OpticalPhotonPhysics(const Geant4OpticalPhotonPhysics&) = delete;
      /// Initializing constructor
      Geant4OpticalPhotonPhysics(Geant4Context* ctxt, const std::string& nam)
        : Geant4PhysicsList(ctxt, nam)
      {
        declareProperty("VerboseLevel", m_verbosity = 0);
        declareProperty("BoundaryInvokeSD", m_boundaryInvokeSD = false);
      }
      /// Default destructor
      virtual ~Geant4OpticalPhotonPhysics() = default;
      /// Callback to construct processes (uses the G4 particle table)
      virtual void constructProcesses(G4VUserPhysicsList* physics_list)   { 
        this->Geant4PhysicsList::constructProcesses(physics_list);
        info("+++ Constructing optical_photon processes:");
        G4ParticleTable*      table = G4ParticleTable::GetParticleTable();
        G4ParticleDefinition* particle = table->FindParticle("opticalphoton");
        if (0 == particle) {
          except("++ Cannot resolve 'opticalphoton' particle definition!");
        }

        G4ProcessManager* pmanager = particle->GetProcessManager();

#if G4VERSION_NUMBER >= 1070
        G4OpticalParameters* params = G4OpticalParameters::Instance();
        params->SetBoundaryVerboseLevel(m_verbosity);
        params->SetBoundaryInvokeSD(m_boundaryInvokeSD);

        // Always add boundary process (controls surface interactions)
        pmanager->AddDiscreteProcess(new G4OpBoundaryProcess());

        // Add remaining processes only if activated in G4OpticalParameters.
        // This allows callers to disable unused processes (e.g. OpMieHG when no
        // material defines MIE scattering tables) to reduce GPIL overhead.
        auto addIf = [&](const G4String& key, G4VDiscreteProcess* proc, G4int verbosity) {
          if (params->GetProcessActivation(key)) {
            proc->SetVerboseLevel(verbosity);
            pmanager->AddDiscreteProcess(proc);
          } else {
            delete proc;
          }
        };
        addIf("OpAbsorption", new G4OpAbsorption(), m_verbosity);
        addIf("OpRayleigh",   new G4OpRayleigh(),   m_verbosity);
        addIf("OpMieHG",      new G4OpMieHG(),      m_verbosity);
        addIf("OpWLS",        new G4OpWLS(),        m_verbosity);
        addIf("OpWLS2",       new G4OpWLS2(),       m_verbosity);

        // Log which processes are actually registered
        std::string active = "+++              G4OpBoundaryProcess";
        for (const char* key : {"OpAbsorption", "OpRayleigh", "OpMieHG", "OpWLS", "OpWLS2"}) {
          if (params->GetProcessActivation(key)) active += std::string(" G4") + key;
        }
        info("%s", active.c_str());
#else
        info("+++              G4OpAbsorption G4OpRayleigh G4OpMieHG G4OpBoundaryProcess");
        G4OpBoundaryProcess*  fBoundaryProcess           = new G4OpBoundaryProcess();
        G4OpAbsorption*       fAbsorptionProcess         = new G4OpAbsorption();
        G4OpRayleigh*         fRayleighScatteringProcess = new G4OpRayleigh();
        G4OpMieHG*            fMieHGScatteringProcess    = new G4OpMieHG();
        fAbsorptionProcess->SetVerboseLevel(m_verbosity);
        fRayleighScatteringProcess->SetVerboseLevel(m_verbosity);
        fMieHGScatteringProcess->SetVerboseLevel(m_verbosity);
        fBoundaryProcess->SetVerboseLevel(m_verbosity);
#if G4VERSION_NUMBER >= 1000
        fBoundaryProcess->SetInvokeSD(m_boundaryInvokeSD);
#endif
        pmanager->AddDiscreteProcess(fAbsorptionProcess);
        pmanager->AddDiscreteProcess(fRayleighScatteringProcess);
        pmanager->AddDiscreteProcess(fMieHGScatteringProcess);
        pmanager->AddDiscreteProcess(fBoundaryProcess);
#endif
      }
    private:
      int         m_verbosity;
      bool        m_boundaryInvokeSD;
    };
  }
}
#endif   // DDG4_GEANT4OPTICALPHOTONPHYSICS_H

#include <DDG4/Factories.h>
using namespace dd4hep::sim;
DECLARE_GEANT4ACTION(Geant4OpticalPhotonPhysics)
