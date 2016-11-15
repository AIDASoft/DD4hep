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

#ifndef DD4HEP_DDG4_GEANT4RUNACTION_H
#define DD4HEP_DDG4_GEANT4RUNACTION_H

// Framework include files
#include "DDG4/Geant4Action.h"

// Forward declaration
class G4Run;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    // Forward declarations
    class Geant4RunAction;
    class Geant4SharedRunAction;
    class Geant4RunActionSequence;

    /// Concrete basic implementation of the Geant4 run action base class.
    /**
     * The Run Action is called once per start and end of a run.
     * i.e. a series of generated events. These two callbacks
     * allow clients to define run-dependent actions such as statistics
     * summaries etc.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4RunAction: public Geant4Action {
    public:
      typedef Geant4SharedRunAction shared_type;
    public:
      /// Inhibit default constructor
      Geant4RunAction() = delete;
      /// Inhibit copy constructor
      Geant4RunAction(const Geant4RunAction& copy) = delete;
      /// Standard constructor
      Geant4RunAction(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4RunAction();
      /// Begin-of-run callback
      virtual void begin(const G4Run* run);
      /// End-of-run callback
      virtual void end(const G4Run* run);
    };

    /// Implementation of the Geant4 shared run action
    /**
     * Wrapper to share single instances of run actions for
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
    class Geant4SharedRunAction : public Geant4RunAction {
    protected:
      /// Reference to the shared action
      Geant4RunAction* m_action;
    public:
      /// Inhibit default constructor
      Geant4SharedRunAction() = delete;
      /// Inhibit default constructor
      Geant4SharedRunAction(const Geant4SharedRunAction& copy) = delete;
      /// Standard constructor
      Geant4SharedRunAction(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4SharedRunAction();
      /// Set or update client for the use in a new thread fiber
      virtual void configureFiber(Geant4Context* thread_context);
      /// Underlying object to be used during the execution of this thread
      virtual void use(Geant4RunAction* action);
      /// Begin-of-run callback
      virtual void begin(const G4Run* run);
      /// End-of-run callback
      virtual void end(const G4Run* run);
    };

    /// Concrete basic implementation of the Geant4 run action sequencer.
    /**
     * Concrete implementation of the Geant4 run action sequence.
     * The sequence dispatches the callbacks at the beginning and the and
     * of a run to all registered Geant4RunAction members and all
     * registered callbacks.
     *
     * Note Multi-Threading issue:
     * Callbacks and the action list is protected against multiple 
     * threads calling the Geant4 callbacks!
     * 
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4RunActionSequence: public Geant4Action {
    protected:
      /// Callback sequence for begin-run action
      CallbackSequence m_begin;
      /// Callback sequence for end-run action
      CallbackSequence m_end;
      /// The list of action objects to be called
      Actors<Geant4RunAction> m_actors;
    public:
      /// Inhibit default constructor
      Geant4RunActionSequence() = delete;
      /// Inhibit copy constructor
      Geant4RunActionSequence(const Geant4RunActionSequence& copy) = delete;
      /// Standard constructor
      Geant4RunActionSequence(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4RunActionSequence();
      /// Set or update client context
      virtual void updateContext(Geant4Context* ctxt);
      /// Set or update client for the use in a new thread fiber
      virtual void configureFiber(Geant4Context* thread_context);
      /// Get an action by name
      Geant4RunAction* get(const std::string& name) const;
      /// Register begin-of-run callback. Types Q and T must be polymorph!
      template <typename Q, typename T>
      void callAtBegin(Q* p, void (T::*f)(const G4Run*)) {
        m_begin.add(p, f);
      }
      /// Register end-of-run callback. Types Q and T must be polymorph!
      template <typename Q, typename T>
      void callAtEnd(Q* p, void (T::*f)(const G4Run*)) {
        m_end.add(p, f);
      }
      /// Add an actor responding to all callbacks. Sequence takes ownership.
      void adopt(Geant4RunAction* action);
      /// Begin-of-run callback
      virtual void begin(const G4Run* run);
      /// End-of-run callback
      virtual void end(const G4Run* run);
    };

  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4RUNACTION_H
