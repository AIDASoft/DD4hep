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
 * \package Geant4CerenkovPhysics
 * \brief PhysicsConstructor for Cerenkov physics

 This plugin allows to enable Cerenkov physics in the physics list

 * @}
 */
#ifndef DDG4_GEANT4CERENKOVPHYSICS_H
#define DDG4_GEANT4CERENKOVPHYSICS_H 1

/// Framework include files
#include "DDG4/Geant4PhysicsList.h"

/// Geant4 include files
#include "G4ParticleTableIterator.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"
#include "G4ProcessManager.hh"

#include "G4Cerenkov.hh"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    /// Geant4 physics list action to enable Cerenkov physics
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4CerenkovPhysics : public Geant4PhysicsList    {
    public:
      /// Default constructor
      Geant4CerenkovPhysics() = delete;
      /// Copy constructor
      Geant4CerenkovPhysics(const Geant4CerenkovPhysics&) = delete;
      /// Initializing constructor
      Geant4CerenkovPhysics(Geant4Context* ctxt, const std::string& nam)
        : Geant4PhysicsList(ctxt, nam)
      {
        declareProperty("MaxNumPhotonsPerStep",  m_maxNumPhotonsPerStep = 0);
        declareProperty("MaxBetaChangePerStep",  m_maxBetaChangePerStep = 0.0);
        declareProperty("TrackSecondariesFirst", m_trackSecondariesFirst = false);
        declareProperty("StackPhotons",          m_stackPhotons = true);
        declareProperty("VerboseLevel",          m_verbosity = 0);
      }
      /// Default destructor
      virtual ~Geant4CerenkovPhysics() = default;
      /// Callback to construct processes (uses the G4 particle table)
      virtual void constructProcesses(G4VUserPhysicsList* physics_list)   { 
        this->Geant4PhysicsList::constructProcesses(physics_list);
        info("+++ Constructing: maxNumPhotonsPerStep:%d maxBeta:%f "
             "track secondaries:%s stack photons:%s",
             m_maxNumPhotonsPerStep, m_maxBetaChangePerStep,
             yes_no(m_trackSecondariesFirst), yes_no(m_stackPhotons));
        G4Cerenkov* process = new G4Cerenkov(name());
        process->SetVerboseLevel(m_verbosity);
        process->SetMaxNumPhotonsPerStep(m_maxNumPhotonsPerStep);
        process->SetMaxBetaChangePerStep(m_maxBetaChangePerStep);
        process->SetTrackSecondariesFirst(m_trackSecondariesFirst);
        process->SetStackPhotons(m_stackPhotons);

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
      double      m_maxBetaChangePerStep;
      int         m_maxNumPhotonsPerStep;
      int         m_verbosity;
      bool        m_trackSecondariesFirst;
      bool        m_stackPhotons;
    };
  }
}
#endif   // DDG4_GEANT4CERENKOVPHYSICS_H

#include "DDG4/Factories.h"
using namespace dd4hep::sim;
DECLARE_GEANT4ACTION(Geant4CerenkovPhysics)
