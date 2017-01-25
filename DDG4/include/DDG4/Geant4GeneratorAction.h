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

    // Forward declarations
    class Geant4GeneratorAction;
    class Geant4SharedGeneratorAction;
    class Geant4GeneratorActionSequence;

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
    public:
      typedef Geant4SharedGeneratorAction shared_type;
    protected:
      Callback m_calls;
    public:
      /// Inhibit default constructor
      Geant4GeneratorAction() = delete;
      /// Standard constructor
      Geant4GeneratorAction(Geant4Context* context, const std::string& name);
      /// Default destructor
      virtual ~Geant4GeneratorAction();
      /// Callback to generate primary particles
      virtual void operator()(G4Event*) {
      }
    };

    /// Implementation of the Geant4 shared generator action
    /**
     * Wrapper to share single instances of generator actions for
     * multi-threaded purposes. The wrapper ensures the locking
     * of the basic actions to avoid race conditions.
     *
     * Shared action should be 'fast'. The global lock otherwise
     * inhibits the efficient use of the multiple threads.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4SharedGeneratorAction : public Geant4GeneratorAction {
    protected:
      /// Reference to the shared action
      Geant4GeneratorAction* m_action;
    public:
      /// Inhibit default constructor
      Geant4SharedGeneratorAction() = delete;
      /// Standard constructor
      Geant4SharedGeneratorAction(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4SharedGeneratorAction();
      /// Set or update client for the use in a new thread fiber
      virtual void configureFiber(Geant4Context* thread_context);
      /// Underlying object to be used during the execution of this thread
      virtual void use(Geant4GeneratorAction* action);
      /// User generator callback
      virtual void operator()(G4Event* event);
    };

    /// Concrete implementation of the Geant4 generator action sequence
    /**
     * The sequence dispatches the callbacks at the beginning
     * of an event to all registered Geant4GeneratorAction members and all
     * registered callbacks.
     *
     * The callback signature is: void operator()(G4Event* event)
     *
     * Note Multi-Threading issue:
     * Neither callbacks not the action list is protected against multiple 
     * threads calling the Geant4 callbacks!
     * These must be protected in the user actions themselves.
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
      /// Inhibit default constructor
      Geant4GeneratorActionSequence() = delete;
      /// Standard constructor
      Geant4GeneratorActionSequence(Geant4Context* context, const std::string& name);
      /// Default destructor
      virtual ~Geant4GeneratorActionSequence();
      /// Set or update client context
      virtual void updateContext(Geant4Context* ctxt);
      /// Set or update client for the use in a new thread fiber
      virtual void configureFiber(Geant4Context* thread_context);
      /// Get an action by name
      Geant4GeneratorAction* get(const std::string& name) const;
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
