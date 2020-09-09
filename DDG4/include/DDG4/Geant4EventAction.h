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

#ifndef DDG4_GEANT4EVENTACTION_H
#define DDG4_GEANT4EVENTACTION_H

// Framework include files
#include "DDG4/Geant4Action.h"

// Forward declarations
class G4Event;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    // Forward declarations
    class Geant4EventAction;
    class Geant4SharedEventAction;
    class Geant4EventActionSequence;

    /// Concrete basic implementation of the Geant4 event action
    /**
     * The EventAction is called for every event.
     *
     * This class is the base class for all user actions, which have
     * to hook into the begin- and end-of-event actions.
     * Typical use cases are the collection/computation of event
     * related properties.
     *
     * Examples of this functionality may include for example:
     * - Reset variables summing event related information in the
     *   begin-event callback.
     * - Monitoring activities such as filling histograms
     *   from hits collected during the end-event action.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4EventAction : public Geant4Action {
    public:
      typedef Geant4SharedEventAction shared_type;
      
    protected:
      /// Define standard assignments and constructors
      DDG4_DEFINE_ACTION_CONSTRUCTORS(Geant4EventAction);

    public:
      /// Standard constructor
      Geant4EventAction(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4EventAction();
      /// Begin-of-event callback
      virtual void begin(const G4Event* event);
      /// End-of-event callback
      virtual void end(const G4Event* event);
    };

    /// Implementation of the Geant4 shared event action
    /**
     * Wrapper to share single instances of event actions for
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
    class Geant4SharedEventAction : public Geant4EventAction {
    protected:
      /// Reference to the shared action
      Geant4EventAction* m_action = 0;

    protected:
      /// Define standard assignments and constructors
      DDG4_DEFINE_ACTION_CONSTRUCTORS(Geant4SharedEventAction);

    public:
      /// Standard constructor
      Geant4SharedEventAction(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4SharedEventAction();
      /// Set or update client for the use in a new thread fiber
      virtual void configureFiber(Geant4Context* thread_context);
      /// Underlying object to be used during the execution of this thread
      virtual void use(Geant4EventAction* action);
      /// Begin-of-event callback
      virtual void begin(const G4Event* event);
      /// End-of-event callback
      virtual void end(const G4Event* event);
    };

    /// Concrete implementation of the Geant4 event action sequence
    /**
     * The sequence dispatches the callbacks at the beginning and the and
     * of an event to all registered Geant4EventAction members and all
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
    class Geant4EventActionSequence : public Geant4Action {
    protected:
      /// Callback sequence for event initialization action
      CallbackSequence m_begin;
      /// Callback sequence for event finalization action
      CallbackSequence m_end;
      /// Callback sequence for event finalization action
      CallbackSequence m_final;
      /// The list of action objects to be called
      Actors<Geant4EventAction> m_actors;
      
    protected:
      /// Define standard assignments and constructors
      DDG4_DEFINE_ACTION_CONSTRUCTORS(Geant4EventActionSequence);

    public:
      /// Standard constructor
      Geant4EventActionSequence(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4EventActionSequence();
      /// Set or update client context
      virtual void updateContext(Geant4Context* ctxt);
      /// Set or update client for the use in a new thread fiber
      virtual void configureFiber(Geant4Context* thread_context);
      /// Get an action by name
      Geant4EventAction* get(const std::string& name) const;
      /// Register begin-of-event callback
      template <typename Q, typename T>
      void callAtBegin(Q* p, void (T::*f)(const G4Event*)) {
        m_begin.add(p, f);
      }
      /// Register end-of-event callback
      template <typename Q, typename T>
      void callAtEnd(Q* p, void (T::*f)(const G4Event*)) {
        m_end.add(p, f);
      }
      /// Register event-cleanup callback (after end-of-event callback -- unordered)
      template <typename Q, typename T>
      void callAtFinal(Q* p, void (T::*f)(const G4Event*)) {
        m_final.add(p, f);
      }
      /// Add an actor responding to all callbacks. Sequence takes ownership.
      void adopt(Geant4EventAction* action);
      /// Begin-of-event callback
      virtual void begin(const G4Event* event);
      /// End-of-event callback
      virtual void end(const G4Event* event);
    };

  }    // End namespace sim
}      // End namespace dd4hep
#endif // DDG4_GEANT4EVENTACTION_H
