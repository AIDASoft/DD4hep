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
#ifndef DD4HEP_DDG4_GEANT4STACKINGACTION_H
#define DD4HEP_DDG4_GEANT4STACKINGACTION_H

// Framework include files
#include "DDG4/Geant4Action.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    // Forward declarations
    class Geant4StackingAction;
    class Geant4SharedStackingAction;
    class Geant4StackingActionSequence;

    /// Concrete implementation of the Geant4 stacking action base class
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4StackingAction: public Geant4Action {
    public:
      typedef Geant4SharedStackingAction shared_type;
    public:
      /// Standard constructor
      Geant4StackingAction(Geant4Context* ctxt, const std::string& name);
      /// Default destructor
      virtual ~Geant4StackingAction();
      /// New-stage callback
      virtual void newStage() {
      }
      /// Preparation callback
      virtual void prepare() {
      }
    };

    /// Implementation of the Geant4 shared stacking action
    /**
     * Wrapper to share single instances of stacking actions for
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
    class Geant4SharedStackingAction : public Geant4StackingAction {
    protected:
      /// Reference to the shared action
      Geant4StackingAction* m_action;
    public:
      /// Standard constructor
      Geant4SharedStackingAction(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4SharedStackingAction();
      /// Set or update client for the use in a new thread fiber
      virtual void configureFiber(Geant4Context* thread_context);
      /// Underlying object to be used during the execution of this thread
      virtual void use(Geant4StackingAction* action);
      /// New-stage callback
      virtual void newStage();
      /// Preparation callback
      virtual void prepare();
    };

    /// Concrete implementation of the Geant4 stacking action sequence
    /**
     * The sequence dispatches the callbacks for each stepping action
     * to all registered Geant4StackingAction members and all
     * registered callbacks.
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
    class Geant4StackingActionSequence: public Geant4Action {
    protected:
      /// Callback sequence for the newStage call
      CallbackSequence m_newStage;
      // Callback sequence for the prepare call
      CallbackSequence m_prepare;
      /// The list of action objects to be called
      Actors<Geant4StackingAction> m_actors;
    public:
      /// Standard constructor
      Geant4StackingActionSequence(Geant4Context* ctxt, const std::string& name);
      /// Default destructor
      virtual ~Geant4StackingActionSequence();
      /// Set or update client context
      virtual void updateContext(Geant4Context* ctxt);
      /// Set or update client for the use in a new thread fiber
      virtual void configureFiber(Geant4Context* thread_context);
      /// Get an action by name
      Geant4StackingAction* get(const std::string& name) const;
      /// Register begin-of-event callback. Types Q and T must be polymorph!
      template <typename T> void callAtNewStage(T* p, void (T::*f)()) {
        m_newStage.add(p, f);
      }
      /// Register end-of-event callback. Types Q and T must be polymorph!
      template <typename T> void callAtPrepare(T* p, void (T::*f)()) {
        m_prepare.add(p, f);
      }
      /// Add an actor responding to all callbacks. Sequence takes ownership.
      void adopt(Geant4StackingAction* action);
      /// New-stage callback
      virtual void newStage();
      /// Preparation callback
      virtual void prepare();
    };

  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4STACKINGACTION_H
