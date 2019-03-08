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
#include "DDG4/Geant4PhysicsList.h"

/// Geant4 include files
#include "G4OpAbsorption.hh"
#include "G4OpRayleigh.hh"
#include "G4OpMieHG.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"
#include "G4ProcessManager.hh"

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
      }
      /// Default destructor
      virtual ~Geant4OpticalPhotonPhysics() = default;
      /// Callback to construct processes (uses the G4 particle table)
      virtual void constructProcesses(G4VUserPhysicsList* physics_list)   { 
        this->Geant4PhysicsList::constructProcesses(physics_list);
        info("+++ Constructing optical_photon processes:");
        info("+++              G4OpAbsorption G4OpRayleigh G4OpMieHG G4OpBoundaryProcess");
        G4ParticleTable*      table = G4ParticleTable::GetParticleTable();
        G4ParticleDefinition* particle = table->FindParticle("opticalphoton");
        if (0 == particle) {
          except("++ Cannot resolve 'opticalphoton' particle definition!");
        }

        G4OpBoundaryProcess*  fBoundaryProcess           = new G4OpBoundaryProcess();
        G4OpAbsorption*       fAbsorptionProcess         = new G4OpAbsorption();
        G4OpRayleigh*         fRayleighScatteringProcess = new G4OpRayleigh();
        G4OpMieHG*            fMieHGScatteringProcess    = new G4OpMieHG();

        fAbsorptionProcess->SetVerboseLevel(m_verbosity);
        fRayleighScatteringProcess->SetVerboseLevel(m_verbosity);
        fMieHGScatteringProcess->SetVerboseLevel(m_verbosity);
        fBoundaryProcess->SetVerboseLevel(m_verbosity);

        G4ProcessManager* pmanager = particle->GetProcessManager();
        pmanager->AddDiscreteProcess(fAbsorptionProcess);
        pmanager->AddDiscreteProcess(fRayleighScatteringProcess);
        pmanager->AddDiscreteProcess(fMieHGScatteringProcess);
        pmanager->AddDiscreteProcess(fBoundaryProcess);
      }
    private:
      int         m_verbosity;
    };
  }
}
#endif   // DDG4_GEANT4OPTICALPHOTONPHYSICS_H

#include "DDG4/Factories.h"
using namespace dd4hep::sim;
DECLARE_GEANT4ACTION(Geant4OpticalPhotonPhysics)
