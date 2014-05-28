// $Id: Geant4config.h 615 2013-06-18 11:13:35Z markus.frank $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Small helper file with abbreviations to write configurations
//  for CINT; also used when writing the XML configuration.
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DDG4_GEANT4USERPHYSICSLIST_H
#define DDG4_GEANT4USERPHYSICSLIST_H

#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4PhysicsList.h"
#include "G4VModularPhysicsList.hh"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {

    /** @class Geant4UserPhysicsList
     *
     * Concrete implementation of the Geant4 user physics list
     *
     * @author  M.Frank
     * @version 1.0
     */
    template <typename BASE_IMP> struct Geant4UserPhysicsList: virtual public Geant4UserPhysics, virtual public BASE_IMP {
    protected:
      /// Action container
      Geant4PhysicsListActionSequence* m_sequence;
    public:
      /// Standard constructor
      Geant4UserPhysicsList(Geant4PhysicsListActionSequence* seq, int verbose)
          : Geant4UserPhysics(), BASE_IMP(verbose), m_sequence(seq) {
        if (m_sequence)
          m_sequence->addRef();
        InstanceCount::increment(this);
      }
      /// Default destructor
      virtual ~Geant4UserPhysicsList() {
        if (m_sequence)
          m_sequence->release();
        m_sequence = 0;
        InstanceCount::decrement(this);
      }
      /// User construction callback
      virtual void ConstructProcess() {
        this->BASE_IMP::ConstructProcess();
        if (m_sequence)
          m_sequence->constructProcess(this);
      }
      /// User construction callback
      virtual void ConstructParticle() {
        this->BASE_IMP::ConstructParticle();
        if (m_sequence)
          m_sequence->constructParticles(this);
      }
      /// Enable transportation
      virtual void AddTransportation() {
        this->G4VUserPhysicsList::AddTransportation();
      }
      // Register Physics Constructor
      virtual void RegisterPhysics(G4VPhysicsConstructor* physics) {
        this->G4VModularPhysicsList::RegisterPhysics(physics);
      }
    };

  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DDG4_GEANT4USERPHYSICSLIST_H
