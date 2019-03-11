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
 * \package Geant4ScintillationPhysics
 * \brief PhysicsConstructor for Scintillation physics

   This plugin allows to enable Scintillation physics in the physics list

 * @}
 */
#ifndef DDG4_GEANT4SCINTILLATIONPHYSICS_H
#define DDG4_GEANT4SCINTILLATIONPHYSICS_H 1

/// Framework include files
#include "DDG4/Geant4PhysicsList.h"

/// Geant4 include files
#include "G4ParticleTableIterator.hh"
#include "G4ParticleDefinition.hh"
#include "G4LossTableManager.hh"
#include "G4ProcessManager.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"
#include "G4EmSaturation.hh"
#include "G4Threading.hh"

#include "G4Scintillation.hh"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    /// Geant4 physics list action to enable Scintillation physics
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4ScintillationPhysics : public Geant4PhysicsList    {
    public:
      /// Default constructor
      Geant4ScintillationPhysics() = delete;
      /// Copy constructor
      Geant4ScintillationPhysics(const Geant4ScintillationPhysics&) = delete;
      /// Initializing constructor
      Geant4ScintillationPhysics(Geant4Context* ctxt, const std::string& nam)
        : Geant4PhysicsList(ctxt, nam)
      {
        declareProperty("ScintillationYieldFactor",      m_scintillationYieldFactor = 1.0);
        declareProperty("ScintillationExcitationRatio",  m_scintillationExcitationRatio = 1.0);
        declareProperty("FiniteRiseTime",                m_finiteRiseTime = false);
        declareProperty("TrackSecondariesFirst",         m_trackSecondariesFirst = false);
        declareProperty("StackPhotons",                  m_stackPhotons = true);
        declareProperty("VerboseLevel",                  m_verbosity = 0);
      }
      /// Default destructor
      virtual ~Geant4ScintillationPhysics() = default;
      /// Callback to construct processes (uses the G4 particle table)
      virtual void constructProcesses(G4VUserPhysicsList* physics_list)   { 
        this->Geant4PhysicsList::constructProcesses(physics_list);
        info("+++ Constructing: yield:%f excitation:%f rise-time:%s stack photons:%s",
             m_scintillationYieldFactor, m_scintillationExcitationRatio,
             yes_no(m_finiteRiseTime), yes_no(m_stackPhotons));
        G4Scintillation* process = new G4Scintillation(name());
        process->SetVerboseLevel(m_verbosity);
        process->SetFiniteRiseTime(m_finiteRiseTime);
#if G4VERSION_NUMBER>1030
        process->SetStackPhotons(m_stackPhotons);
#endif
        process->SetTrackSecondariesFirst(m_trackSecondariesFirst);
        process->SetScintillationYieldFactor(m_scintillationYieldFactor);
        process->SetScintillationExcitationRatio(m_scintillationExcitationRatio);
        // Use Birks Correction in the Scintillation process
        if ( G4Threading::IsMasterThread() )  {
          G4EmSaturation* emSaturation =
            G4LossTableManager::Instance()->EmSaturation();
          process->AddSaturation(emSaturation);
        }

        auto pit = G4ParticleTable::GetParticleTable()->GetIterator();
        pit->reset();
        while( (*pit)() ){
          G4ParticleDefinition* particle = pit->value();
          if (process->IsApplicable(*particle)) {
            G4ProcessManager* pmanager = particle->GetProcessManager();
            pmanager->AddProcess(process);
            pmanager->SetProcessOrdering(process,idxPostStep);
          }
        }
      }

    private:
      double m_scintillationYieldFactor;
      double m_scintillationExcitationRatio;
      int    m_verbosity;
      bool   m_stackPhotons;
      bool   m_finiteRiseTime;
      bool   m_trackSecondariesFirst;
    };
  }
}
#endif   // DDG4_GEANT4SCINTILLATIONPHYSICS_H

#include "DDG4/Factories.h"
using namespace dd4hep::sim;
DECLARE_GEANT4ACTION(Geant4ScintillationPhysics)
