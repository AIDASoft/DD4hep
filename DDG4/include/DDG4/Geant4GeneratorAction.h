// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4GENERATORACTION_H
#define DD4HEP_DDG4_GEANT4GENERATORACTION_H

// Framework include files
#include "DDG4/Geant4Action.h"

// Forward declaration
class G4Event;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    /// Concrete implementation of the Geant4 generator action base class
    /**
     * The Geant4GeneratorAction is called for every event.
     * During the callback all particles are created which form the
     * microscopic kinematic action of the particle collision.
     * This input may either origin directly from an event generator program
     * or come from file.
     *
     * The callback signature is: void operator()(G4Event* event)
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4GeneratorAction : public Geant4Action {
    protected:
      Callback m_calls;
    public:
      /// Standard constructor
      Geant4GeneratorAction(Geant4Context* context, const std::string& name);
      /// Default destructor
      virtual ~Geant4GeneratorAction();
      /// Callback to generate primary particles
      virtual void operator()(G4Event*) {
      }
    };

    /// Concrete implementation of the Geant4 generator action sequence
    /**
     * The sequence dispatches the callbacks at the beginning
     * of an event to all registered Geant4GeneratorAction members and all
     * registered callbacks.
     *
     * The callback signature is: void operator()(G4Event* event)
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4GeneratorActionSequence : public Geant4Action {
    protected:
      /// Callback sequence to generate primary particles
      CallbackSequence m_calls;
      /// The list of action objects to be called
      Actors<Geant4GeneratorAction> m_actors;
    public:
      /// Standard constructor
      Geant4GeneratorActionSequence(Geant4Context* context, const std::string& name);
      /// Default destructor
      virtual ~Geant4GeneratorActionSequence();
      /// Register primary particle generation callback. Types Q and T must be polymorph!
      template <typename Q, typename T>
        void call(Q* p, void (T::*f)(G4Event*)) {
        m_calls.add(p, f);
      }
      /// Add an actor responding to all callbacks. Sequence takes ownership.
      void adopt(Geant4GeneratorAction* action);
      /// Callback to generate primary particles
      virtual void operator()(G4Event* event);
    };

  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4GENERATORACTION_H
