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
#ifndef DDG4_GEANT4STEPPINGACTION_H
#define DDG4_GEANT4STEPPINGACTION_H

// Framework include files
#include "DDG4/Geant4Action.h"

// Forward declarations
class G4SteppingManager;
class G4Step;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    // Forward declarations
    class Geant4SteppingAction;
    class Geant4SharedSteppingAction;
    class Geant4SteppingActionSequence;

    /// Concrete implementation of the Geant4 stepping action sequence
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4SteppingAction: public Geant4Action {
    public:
      typedef Geant4SharedSteppingAction shared_type;
      /// Define standard assignments and constructors
      DDG4_DEFINE_ACTION_CONSTRUCTORS(Geant4SteppingAction);

    public:
      /// Standard constructor
      Geant4SteppingAction(Geant4Context* context, const std::string& name);
      /// Default destructor
      virtual ~Geant4SteppingAction();
      /// User stepping callback
      virtual void operator()(const G4Step* step, G4SteppingManager* mgr);
    };

    /// Implementation of the Geant4 shared stepping action
    /**
     * Wrapper to share single instances of stepping actions for
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
    class Geant4SharedSteppingAction : public Geant4SteppingAction {
    protected:
      /// Reference to the shared action
      Geant4SteppingAction* m_action = 0;

      /// Define standard assignments and constructors
      DDG4_DEFINE_ACTION_CONSTRUCTORS(Geant4SharedSteppingAction);

    public:
      /// Standard constructor
      Geant4SharedSteppingAction(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4SharedSteppingAction();
      /// Set or update client for the use in a new thread fiber
      virtual void configureFiber(Geant4Context* thread_context);
      /// Underlying object to be used during the execution of this thread
      virtual void use(Geant4SteppingAction* action);
      /// User stepping callback
      virtual void operator()(const G4Step* step, G4SteppingManager* mgr);
    };

    /// Concrete implementation of the Geant4 stepping action sequence
    /**
     * The sequence dispatches the callbacks for each stepping action
     * to all registered Geant4SteppingAction members and all
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
    class Geant4SteppingActionSequence: public Geant4Action {
    protected:
      /// Callback sequence for user stepping action calls
      CallbackSequence m_calls;
      /// The list of action objects to be called
      Actors<Geant4SteppingAction> m_actors;

      /// Define standard assignments and constructors
      DDG4_DEFINE_ACTION_CONSTRUCTORS(Geant4SteppingActionSequence);

    public:
      /// Standard constructor
      Geant4SteppingActionSequence(Geant4Context* context, const std::string& name);
      /// Default destructor
      virtual ~Geant4SteppingActionSequence();
      /// Set or update client context
      virtual void updateContext(Geant4Context* ctxt);
      /// Set or update client for the use in a new thread fiber
      virtual void configureFiber(Geant4Context* thread_context);
      /// Get an action by name
      Geant4SteppingAction* get(const std::string& name) const;
      /// Register stepping action callback. Types Q and T must be polymorph!
      template <typename Q, typename T>
      void call(Q* p, void (T::*f)(const G4Step*, G4SteppingManager*)) {
        m_calls.add(p, f);
      }
      /// Add an actor responding to all callbacks. Sequence takes ownership.
      void adopt(Geant4SteppingAction* action);
      /// User stepping callback
      virtual void operator()(const G4Step* step, G4SteppingManager* mgr);
    };

  }    // End namespace sim
}      // End namespace dd4hep

#endif // DDG4_GEANT4STEPPINGACTION_H
