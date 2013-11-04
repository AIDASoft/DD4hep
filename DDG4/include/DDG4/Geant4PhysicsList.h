// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4PHYSICSLIST_H
#define DD4HEP_DDG4_GEANT4PHYSICSLIST_H

// Framework include files
#include "DDG4/Geant4Action.h"

// C/C++ include files
#include <map>
#include <vector>

// Forward declarations
class G4VPhysicsConstructor;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation   {

    /** @class Geant4UserPhysics Geant4PhysicsList.h DDG4/Geant4PhysicsList.h
     * 
     * Interface class exposing some of the G4VUserPhysicsList class.
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4UserPhysics  {
    protected:
      /// Standard constructor with initailization parameters
      Geant4UserPhysics();
      /// Default destructor
      virtual ~Geant4UserPhysics();
    public:
      /// Enable transportation
      virtual void AddTransportation() = 0;
      /// Register physics constructor
      virtual void RegisterPhysics(G4VPhysicsConstructor* physics) = 0;
    };

    /** @class Geant4PhysicsList Geant4PhysicsList.h DDG4/Geant4PhysicsList.h
     * 
     * Concrete basic implementation of the Geant4 event action 
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4PhysicsList : public Geant4Action  {
    public:

      /** @class PhysicsConstructor Geant4PhysicsList.h DDG4/Geant4PhysicsList.h
       * 
       * Image of a physics constructor holding all stub information to attach
       * the concrete process contributing to the user physics list.
       *
       * @author  M.Frank
       * @version 1.0
       */
      struct Process  {
	std::string name;
	int         ordAtRestDoIt, ordAlongSteptDoIt, ordPostStepDoIt;
	/// Default constructor
	Process();
	/// Copy constructor
	Process(const Process& p);
	/// Assignment operator
	Process& operator=(const Process& p);
      };
      struct ParticleConstructor : public std::string  {
      ParticleConstructor() : std::string() {}
      ParticleConstructor(const std::string& s) : std::string(s) {}
	~ParticleConstructor() {}
      };
      struct PhysicsConstructor : public std::string {
      PhysicsConstructor() : std::string() {}
      PhysicsConstructor(const std::string& s) : std::string(s) {}
	~PhysicsConstructor() {}
      };

      typedef std::vector<Process>  ParticleProcesses;
      typedef std::map<std::string, ParticleProcesses > PhysicsProcesses;

      PhysicsProcesses       m_processes;

      typedef std::vector<ParticleConstructor> ParticleConstructors;
      ParticleConstructors   m_particles;

      typedef std::vector<PhysicsConstructor> PhysicsConstructors;
      PhysicsConstructors    m_physics;

    public:
      /// Standard constructor with initailization parameters
      Geant4PhysicsList(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4PhysicsList();
      
      /// Access all physics processes
      PhysicsProcesses& processes()                   {  return m_processes;       }
      /// Access all physics processes
      const PhysicsProcesses& processes() const       {  return m_processes;       }
      /// Access processes for one particle type
      ParticleProcesses& processes(const std::string& part_name);
      /// Access processes for one particle type (CONST)
      const ParticleProcesses& processes(const std::string& part_name) const;
      /// Access all physics particles
      ParticleConstructors& particles()               {  return m_particles;       }
      /// Access all physics particles
      const ParticleConstructors& particles() const   {  return m_particles;       }
      /// Access all physics constructors
      PhysicsConstructors& physics()                  {  return m_physics;         }
      /// Access all physics constructors
      const PhysicsConstructors& physics() const      {  return m_physics;         }

      /// Callback to construct the physics list
      virtual void constructProcess(Geant4UserPhysics* interface);
      /// constructParticle callback
      virtual void constructParticles(Geant4UserPhysics* particle);
      /// constructPhysics callback
      virtual void constructPhysics(Geant4UserPhysics* physics);
      /// Callback to construct processes (uses the G4 particle table)
      virtual void constructProcesses(Geant4UserPhysics* physics);
    };

 
    /** @class Geant4PhysicsListActionSequence Geant4Action.h DDG4/Geant4Action.h
     * 
     * Concrete implementation of the Geant4 physics list sequence.
     * A list to setup the physics content in a modular form
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4PhysicsListActionSequence : public Geant4Action    {
    protected:
      /// Callback sequence for event finalization action
      CallbackSequence m_process;
      /// Callback sequence for event finalization action
      CallbackSequence m_particle;
      /// The list of action objects to be called
      Actors<Geant4PhysicsList> m_actors;
      /// Callback to construct particle decays
      virtual void constructDecays(Geant4UserPhysics* physics);
    public:
      /// Flag if particle transportation is to be added
      bool             m_transportation;
      /// Flag if particle decays are to be added
      bool             m_decays;
    public:
      /// Standard constructor
      Geant4PhysicsListActionSequence(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4PhysicsListActionSequence();
      /// Update transportation flag
      void setTransportation(bool value)              {  m_transportation = value; }
      /// Access the transportation flag
      bool transportation() const                     {  return m_transportation;  }
      /// Register process construction callback
      template <typename Q, typename T> 
	void constructProcess(Q* p, void (T::*f)(Geant4UserPhysics*)) { m_process.add(p,f);}
      /// Register particle construction callback
      template <typename Q, typename T> 
	void constructParticle(Q* p, void (T::*f)(Geant4UserPhysics*)) { m_particle.add(p,f);}
      /// Add an actor responding to all callbacks. Sequence takes ownership.
      void adopt(Geant4PhysicsList* action);
      /// begin-of-event callback
      void constructProcess(Geant4UserPhysics* physics);
      /// begin-of-event callback
      void constructParticles(Geant4UserPhysics* physics);
    };

  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4PHYSICSLIST_H
