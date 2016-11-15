// $Id: $
//==========================================================================
//  AIDA Detector description implementation for LCD
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

#ifndef DD4HEP_GEANT4PRIMARY_H
#define DD4HEP_GEANT4PRIMARY_H

// Framework include files
#include "DD4hep/Memory.h"
#include "DDG4/Geant4Vertex.h"
#include "DDG4/Geant4Particle.h"

// C/C++ include files
#include <set>
#include <map>
#include <vector>
#include <memory>

// Forward declarations
class G4PrimaryParticle;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    // Forward declarations
    class Geant4Particle;
    class Geant4Vertex;

    /// Base structure to extend primary information with user data
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class PrimaryExtension  {
    public:
      /// Default constructor
      PrimaryExtension() = default;
      /// Default destructor
      virtual ~PrimaryExtension();
    };

    /// Data structure to map primaries to particles.
    /**
     *  This data structure is added to the Geant4Event data extensions
     *  by the Geant4GenerationInit action.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4PrimaryMap  {
    public:
      typedef std::map<const G4PrimaryParticle*,Geant4Particle*> Primaries;
    private:
      /// Mapping of primary particles of this event
      Primaries       m_primaryMap; //! not persistent

    public:
      /// Default constructor
      Geant4PrimaryMap() = default;
      /// Default destructor
      virtual ~Geant4PrimaryMap();
      /// Add a new object pair (G4 primary particle, DDG4 particle) into the maps
      void insert(G4PrimaryParticle* g4_particle,Geant4Particle* particle);
      /// Access DDG4 particle by G4 primary particle
      Geant4Particle* get(const G4PrimaryParticle* particle);
      /// Access DDG4 particle by G4 primary particle (const)
      const Geant4Particle* get(const G4PrimaryParticle* particle) const;
      /// Access primary map directly
      Primaries& primaries()  {  return m_primaryMap;  }
      /// Access primary map directly (const)
      const Primaries& primaries() const  {  return m_primaryMap;  }
    };


    /// Class modelling a single interaction with multiple primary vertices and particles
    /*
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4PrimaryInteraction   {
    private:
      /// Copy constructor
      Geant4PrimaryInteraction(const Geant4PrimaryInteraction& c) = delete;
      /// Assignment operator
      Geant4PrimaryInteraction& operator=(const Geant4PrimaryInteraction& c) = delete;

    public:
      typedef Geant4Particle                    Particle;
      typedef Geant4Vertex                      Vertex;
      typedef std::map<int,Particle*>           ParticleMap;
      typedef std::map<int,Vertex*>             VertexMap;
      typedef dd4hep_ptr<PrimaryExtension>      ExtensionHandle;

      /// The map of primary vertices for the particles.
      VertexMap       vertices;
      /// The map of particles participating in this primary interaction
      ParticleMap     particles;
      /// User data extension if required
      ExtensionHandle extension;
      /// User mask to flag the interaction. Also unique identifier
      int             mask = 0;
      /// Flag that the event is locked for G4 native generators
      int             locked = 0;
      /// Next PID indentifier
      int             next_particle_identifier = -1;
      
    public:
      /// Default constructor
      Geant4PrimaryInteraction() = default;
      /// Default destructor
      virtual ~Geant4PrimaryInteraction();
      /// Access a new particle identifier within the interaction
      int nextPID();
      /// Set the next PID value
      void setNextPID(int value);
      /// Apply mask to all contained vertices (max. 1) and particles
      bool applyMask();
    };

    /// Class modelling a complete primary event with multiple interactions
    /*
     *  Multiple interactions allow a simple handling of overlay events
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4PrimaryEvent   {
    private:
      /// Copy constructor
      Geant4PrimaryEvent(const Geant4PrimaryEvent& c) = delete;
      /// Assignment operator
      Geant4PrimaryEvent& operator=(const Geant4PrimaryEvent& c) = delete;

    public:
      typedef Geant4PrimaryInteraction     Interaction;
      typedef std::map<int,Interaction*>   Interactions;
      typedef dd4hep_ptr<PrimaryExtension> ExtensionHandle;

    protected:
      /// Set of primary interactions
      Interactions    m_interactions;

    public:
      /// User data extension if required
      ExtensionHandle extension;

    public:
      /// Default constructor
      Geant4PrimaryEvent() = default;
      /// Default destructor
      virtual ~Geant4PrimaryEvent();
      /// Add a new interaction object to the event
      void add(int id, Geant4PrimaryInteraction* interaction);
      /// Retrieve an interaction by it's ID
      Geant4PrimaryInteraction* get(int id) const;
      /// Number of interaction contained in the primary event
      size_t size()  const      {        return m_interactions.size();      }
      /// Retrieve all interactions
      std::vector<Geant4PrimaryInteraction*> interactions() const;
    };

  }    // End namespace Simulation
}      // End namespace DD4hep
#endif // DD4HEP_GEANT4PRIMARY_H
