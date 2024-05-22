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
#ifndef DDG4_GEANT4FASTPHYSICS_H
#define DDG4_GEANT4FASTPHYSICS_H

// Framework include files
#include <DDG4/Geant4Action.h>
#include <DDG4/Geant4PhysicsList.h>

// Geant4 include files
#include <G4VModularPhysicsList.hh>
class G4FastSimulationPhysics;

// C/C++ include files
#include <vector>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep  {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim  {
    
    /// Wrapper for G4FastSimulationPhysics with properties
    /**
     *  Wrapper for G4FastSimulationPhysics with properties.
     *  The properties supposedly are used to fully configure the object
     *  in 'ConstructProcess'.
     *
     *  \author  M.Frank
     *
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4FastPhysics : public Geant4PhysicsList    {
    protected:
      /// Define standard assignments and constructors
      DDG4_DEFINE_ACTION_CONSTRUCTORS(Geant4FastPhysics);

      /// Vector of particle names for which fast simulation is enabled
      std::vector<std::string> m_enabledParticles;
      /// Property to set verbosity flag on G4FastSimulationPhysics
      bool m_verbose  { false };

      /// Reference to fast physics object
      G4FastSimulationPhysics* m_fastPhysics { nullptr };

    public:
      /// Standard constructor
      Geant4FastPhysics(Geant4Context* context, const std::string& nam);

      /// Default destructor
      virtual ~Geant4FastPhysics() = default;

      /// constructPhysics callback
      virtual void constructPhysics(G4VModularPhysicsList* physics)  override;
    };
  }     /* End namespace sim   */
}       /* End namespace dd4hep */
#endif // DDG4_GEANT4FASTPHYSICS_H

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

/// Framework include files
// #include <DDG4/Geant4FastPhysics.h>

// Geant4 include files
#include <G4FastSimulationPhysics.hh>

using namespace dd4hep::sim;

/// Standard constructor
Geant4FastPhysics::Geant4FastPhysics(Geant4Context* ctxt, const std::string& nam)
: Geant4PhysicsList(ctxt, nam)
{
  declareProperty("EnabledParticles", m_enabledParticles);
  declareProperty("BeVerbose",        m_verbose);
}

/// constructPhysics callback
void Geant4FastPhysics::constructPhysics(G4VModularPhysicsList* physics)    {
  /// Create and configure the fast simulation object according to properties
  m_fastPhysics = new G4FastSimulationPhysics(this->name());
  if ( this->m_verbose ) m_fastPhysics->BeVerbose();

  /// attach the particles the fast simulation object should act on
  for( const auto& part_name : m_enabledParticles )    {
    this->info("Enable fast simulation for particle type: %s", part_name.c_str());
    m_fastPhysics->ActivateFastSimulation(part_name);
  }
  /// -- Register this fastSimulationPhysics to the physicsList,
  /// -- when the physics list will be called by the run manager
  /// -- (will happen at initialization of the run manager)
  /// -- for physics process construction, the fast simulation
  /// -- configuration will be applied as well.
  physics->RegisterPhysics(m_fastPhysics);
  this->info("Constructed and initialized Geant4 Fast Physics [G4FastSimulationPhysics].");
}

#include <DDG4/Factories.h>
DECLARE_GEANT4ACTION(Geant4FastPhysics)
