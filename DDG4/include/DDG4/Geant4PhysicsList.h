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

#ifndef DD4HEP_DDG4_GEANT4PHYSICSLIST_H
#define DD4HEP_DDG4_GEANT4PHYSICSLIST_H

// Framework include files
#include "DDG4/Geant4Action.h"

// C/C++ include files
#include <map>
#include <vector>

// Forward declarations
class G4VPhysicsConstructor;
class G4VModularPhysicsList;
class G4VUserPhysicsList;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    /// Concrete basic implementation of a Geant4 physics list action
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4PhysicsList: public Geant4Action {
    public:

      /// Structure describing a G4 process
      /**
       * Image of a physics constructor holding all stub information to attach
       * the concrete process contributing to the user physics list.
       *
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_SIMULATION
       */
      class Process {
      public:
        std::string name;
        int ordAtRestDoIt, ordAlongSteptDoIt, ordPostStepDoIt;
        /// Default constructor
        Process();
        /// Copy constructor
        Process(const Process& p);
        /// Assignment operator
        Process& operator=(const Process& p);
      };
      typedef std::vector<Process> ParticleProcesses;
      typedef std::map<std::string, ParticleProcesses> PhysicsProcesses;

      /// Structure describing a G4 particle constructor
      class ParticleConstructor: public std::string {
      public:
        /// Default constructor
        ParticleConstructor()
          : std::string() {
        }
        /// Initalizing constructor
        ParticleConstructor(const std::string& s)
          : std::string(s) {
        }
        /// Default destructor
        ~ParticleConstructor() {
        }
      };
      typedef std::vector<ParticleConstructor> ParticleConstructors;

      /// Structure describing a G4 physics constructor
      /**
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_SIMULATION
       */
      class PhysicsConstructor: public std::string {
      public:
        /// Pointer to physics constructor object
        G4VPhysicsConstructor* pointer;
      public:
        /// Default constructor
        PhysicsConstructor()
          : std::string(), pointer(0) {
        }
        /// Copy constructor
        PhysicsConstructor(const PhysicsConstructor& c)
          : std::string(c), pointer(c.pointer)  {
        }
        /// Initalizing constructor
        PhysicsConstructor(const std::string& s)
          : std::string(s), pointer(0)  {
        }
        /// Default destructor
        ~PhysicsConstructor() {
        }
      };
      typedef std::vector<PhysicsConstructor> PhysicsConstructors;

      PhysicsProcesses     m_processes;
      ParticleConstructors m_particles;
      PhysicsConstructors  m_physics;

    public:
      /// Standard constructor with initailization parameters
      Geant4PhysicsList(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4PhysicsList();
      /// Dump content to stdout
      void dump();
      /// Install command control messenger if wanted
      virtual void installCommandMessenger();
      /// Access all physics processes
      PhysicsProcesses& processes() {
        return m_processes;
      }
      /// Access all physics processes
      const PhysicsProcesses& processes() const {
        return m_processes;
      }
      /// Access processes for one particle type
      ParticleProcesses& processes(const std::string& part_name);
      /// Access processes for one particle type (CONST)
      const ParticleProcesses& processes(const std::string& part_name) const;
      /// Access all physics particles
      ParticleConstructors& particles() {
        return m_particles;
      }
      /// Access all physics particles
      const ParticleConstructors& particles() const {
        return m_particles;
      }
      /// Access all physics constructors
      PhysicsConstructors& physics() {
        return m_physics;
      }
      /// Access all physics constructors
      const PhysicsConstructors& physics() const {
        return m_physics;
      }

      /// Add physics particle constructor by name
      void addParticleConstructor(const std::string& part_name);
      /// Add particle process by name with arguments
      void addParticleProcess(const std::string& part_name, const std::string& proc_name,
                              int ordAtRestDoIt,int ordAlongSteptDoIt,int ordPostStepDoIt);
      /// Add PhysicsConstructor by name
      /** This constructor is used for intrinsic Geant4 defined physics constructors.
       *  Such physics constructors are only created by the factory and attached
       *  to the physics list object.
       */
      void addPhysicsConstructor(const std::string& physics_name);
      /// Add PhysicsConstructor as Geant4Action object
      /** The action object must bve a sub-class of type G4VPhysicsConstructor.
       *  -- The Geant4Action object to supports properties.
       *  -- Specific user actions may be implemented in the 
       *     base class calls to 'ConstructParticle' or 'ConstructProcess'.
       *     Both calls are invoked by the framework when the physics list
       *     is configured for the use in the run-manager.
       */
      void adoptPhysicsConstructor(Geant4Action* action);

      /// constructParticle callback
      virtual void constructParticles(G4VUserPhysicsList* particle);
      /// constructPhysics callback
      virtual void constructPhysics(G4VModularPhysicsList* physics);
      /// Callback to construct processes (uses the G4 particle table)
      virtual void constructProcesses(G4VUserPhysicsList* physics);
    };

    /// The implementation of the single Geant4 physics list action sequence
    /**
     * Concrete implementation of the Geant4 physics list sequence.
     * A list to setup the physics content in a modular form
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4PhysicsListActionSequence: public Geant4Action {
    public:

    protected:
      /// Callback sequence for G4 physics constructors
      CallbackSequence m_physics;
      /// Callback sequence for G4 process constructors
      CallbackSequence m_process;
      /// Callback sequence for G4 particle constructors
      CallbackSequence m_particle;
      /// The list of action objects to be called
      Actors<Geant4PhysicsList> m_actors;

      /// Callback to construct particle decays
      virtual void constructDecays(G4VUserPhysicsList* physics);
    public:
      /// Flag if particle transportation is to be added
      bool m_transportation;
      /// Flag if particle decays are to be added
      bool m_decays;
      /// Property: Store name of basic predefined Geant4 physics list
      std::string m_extends;
      /// global range cut for secondary productions
      double m_rangecut;

    public:
      /// Standard constructor
      Geant4PhysicsListActionSequence(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4PhysicsListActionSequence();
      /// Dump content to stdout
      void dump();
      /// Install command control messenger if wanted
      virtual void installCommandMessenger();
      /// Update transportation flag
      void setTransportation(bool value) {
        m_transportation = value;
      }
      /// Access the transportation flag
      bool transportation() const {
        return m_transportation;
      }
      /// Register physics construction callback
      template <typename Q, typename T>
      void constructPhysics(Q* p, void (T::*f)(G4VModularPhysicsList*)) {
        m_physics.add(p, f);
      }
      /// Register process construction callback
      template <typename Q, typename T>
      void constructProcess(Q* p, void (T::*f)(G4VUserPhysicsList*)) {
        m_process.add(p, f);
      }
      /// Register particle construction callback
      template <typename Q, typename T>
      void constructParticle(Q* p, void (T::*f)(G4VUserPhysicsList*)) {
        m_particle.add(p, f);
      }
      /// Add an actor responding to all callbacks. Sequence takes ownership.
      void adopt(Geant4PhysicsList* action);

      /// Execute sequence of G4 physics constructors
      virtual void constructProcesses(G4VUserPhysicsList* physics);
      /// Execute sequence of G4 particle constructors
      virtual void constructParticles(G4VUserPhysicsList* physics);
      /// Execute sequence of G4  physics constructors
      virtual void constructPhysics(G4VModularPhysicsList* physics);
      /// Extend physics list from factory:
      G4VUserPhysicsList* extensionList();
    };

  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4PHYSICSLIST_H
