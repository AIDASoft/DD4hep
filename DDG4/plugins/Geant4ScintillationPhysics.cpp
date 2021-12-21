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
#include "G4Version.hh"
#include "G4ParticleTableIterator.hh"
#include "G4ParticleDefinition.hh"
#include "G4LossTableManager.hh"
#include "G4ProcessManager.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"
#include "G4EmSaturation.hh"
#include "G4Threading.hh"

#if G4VERSION_NUMBER >= 1070
#include "G4OpticalParameters.hh"

/// This is a beta release problem:
#if G4VERSION_NUMBER == 110000
#include "G4OpticalParameters.hh"
#pragma message("Geant4 version 11 beta: temporary fix to be removed!")
void G4OpticalParameters::SetScintExcitationRatio(double) {}
void G4OpticalParameters::SetScintYieldFactor(double) {}
#endif

#endif

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
        declareProperty("ByParticleType",                m_byParticleType = false);
        declareProperty("TrackInfo",                     m_trackInfo = false);
        declareProperty("VerboseLevel",                  m_verbosity = 0);
        declareProperty("BoundaryInvokeSD",              m_boundaryInvokeSD = false);
        declareProperty("WLSTimeProfile",                m_WLSTimeProfile);
        declareProperty("WLS2TimeProfile",               m_WLS2TimeProfile);
        declareProperty("CerenkovMaxPhotonsPerStep",     m_CerenkovMaxPhotonsPerStep = -1);
        declareProperty("CerenkovMaxBetaChange",         m_CerenkovMaxBetaChange = -1.0);
        // declareProperty("ScintEnhancedTimeConstants",    m_ScintEnhancedTimeConstants=false);
      }
      /// Default destructor
      virtual ~Geant4ScintillationPhysics() = default;
      /// Callback to construct processes (uses the G4 particle table)
      virtual void constructProcesses(G4VUserPhysicsList* physics_list)   { 
        this->Geant4PhysicsList::constructProcesses(physics_list);
        info("+++ Constructing: yield:%f excitation:%f finite rise-time:%s "
             "stack photons:%s track secondaries:%s",
             m_scintillationYieldFactor, m_scintillationExcitationRatio,
             yes_no(m_finiteRiseTime), yes_no(m_stackPhotons),
             yes_no(m_trackSecondariesFirst));
        G4Scintillation* process = new G4Scintillation(name());
#if G4VERSION_NUMBER >= 1070
        G4OpticalParameters* params = G4OpticalParameters::Instance();
#if G4VERSION_NUMBER >= 1100
        if ( m_CerenkovMaxPhotonsPerStep > 0 )
          params->SetCerenkovMaxPhotonsPerStep(m_CerenkovMaxPhotonsPerStep);
        if ( m_CerenkovMaxBetaChange > 0e0 )
          params->SetCerenkovMaxBetaChange(m_CerenkovMaxBetaChange);
        if ( !m_WLSTimeProfile.empty()  )
          params->SetWLSTimeProfile(m_WLS2TimeProfile);
        if ( !m_WLS2TimeProfile.empty() )
          params->SetWLS2TimeProfile(m_WLS2TimeProfile);
        params->SetBoundaryInvokeSD(m_boundaryInvokeSD);
#endif
        params->SetScintVerboseLevel(m_verbosity);
        params->SetScintFiniteRiseTime(m_finiteRiseTime);
        params->SetScintStackPhotons(m_stackPhotons);
        params->SetScintTrackSecondariesFirst(m_trackSecondariesFirst);
        params->SetScintByParticleType(m_byParticleType);
        params->SetScintTrackInfo(m_trackInfo);
#else
        process->SetVerboseLevel(m_verbosity);
        process->SetFiniteRiseTime(m_finiteRiseTime);
#if G4VERSION_NUMBER >= 1030
        process->SetStackPhotons(m_stackPhotons);
#endif
        process->SetTrackSecondariesFirst(m_trackSecondariesFirst);
#if G4VERSION_NUMBER == 110000
        /// Not yet implemented in G4 11.0.0. Only declared in the header
        // params->SetScintEnhancedTimeConstants(m_ScintEnhancedTimeConstants);
        /// These were only in the beta release:
        process->SetScintillationYieldFactor(m_scintillationYieldFactor);
        process->SetScintillationExcitationRatio(m_scintillationExcitationRatio);
#endif
#if G4VERSION_NUMBER >= 940
        process->SetScintillationByParticleType(m_byParticleType);
#endif
#if G4VERSION_NUMBER >= 1030
        process->SetScintillationTrackInfo(m_trackInfo);
#endif
#endif
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
      /// G4 11 beta, then disappeared....
      double m_scintillationYieldFactor;
      /// G4 11 beta, then disappeared....
      double m_scintillationExcitationRatio;

      /// G4OpticalParameters: "VerboseLevel"
      int    m_verbosity;
      /// G4OpticalParameters: "CerenkovStackPhotons"
      bool   m_stackPhotons;
      /// G4OpticalParameters: "ScintFiniteRiseTime"
      bool   m_finiteRiseTime;
      /// G4OpticalParameters: "ScintTrackSecondariesFirst"
      bool   m_trackSecondariesFirst;
      /// G4OpticalParameters: "ScintByParticleType"
      bool   m_byParticleType;
      /// G4OpticalParameters: "ScintTrackInfo"
      bool   m_trackInfo;

      /// G4OpticalParameters: "BoundaryInvokeSD"
      bool m_boundaryInvokeSD;
      /// G4OpticalParameters: "WLSTimeProfile"
      std::string m_WLSTimeProfile;
      /// G4OpticalParameters: "WLS2TimeProfile";
      std::string m_WLS2TimeProfile;
      /// G4OpticalParameters: "CerenkovMaxBetaChange"
      double m_CerenkovMaxBetaChange;
      /// G4OpticalParameters: "CerenkovMaxPhotonsPerStep"
      int  m_CerenkovMaxPhotonsPerStep;
      /// G4OpticalParameters: "ScintEnhancedTimeConstants"
      bool m_ScintEnhancedTimeConstants;
    };
  }
}
#endif   // DDG4_GEANT4SCINTILLATIONPHYSICS_H

#include "DDG4/Factories.h"
using namespace dd4hep::sim;
DECLARE_GEANT4ACTION(Geant4ScintillationPhysics)
