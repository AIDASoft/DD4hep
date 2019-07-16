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
#ifndef DD4HEP_DDG4_GEANT4TRACKINGACTION_H
#define DD4HEP_DDG4_GEANT4TRACKINGACTION_H

// Framework include files
#include "DDG4/Geant4Action.h"
#include "G4VUserTrackInformation.hh"

class G4TrackingManager;
class G4Track;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    // Forward declarations
    class Geant4TrackInformation;
    class Geant4TrackingAction;
    class Geant4SharedTrackingAction;
    class Geant4TrackingActionSequence;

    /// Default base class for all geant 4 tracking actions used in DDG4.
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4TrackingAction: public Geant4Action {
    public:
      typedef Geant4SharedTrackingAction shared_type;

      /// Define standard assignments and constructors
      DDG4_DEFINE_ACTION_CONSTRUCTORS(Geant4TrackingAction);
    public:
      /// Standard constructor
      Geant4TrackingAction(Geant4Context* context, const std::string& name = "");
      /// Default destructor
      virtual ~Geant4TrackingAction();
      /// Access the Geant4 tracking manager. Only use between tracking pre- and post action
      G4TrackingManager* trackMgr() const {
        return m_context ? m_context->trackMgr() : 0;
      }
      /// Mark the track to be kept for MC truth propagation
      void mark(const G4Track* track) const;
      /// Pre-track action callback
      virtual void begin(const G4Track* track);
      /// Post-track action callback
      virtual void end(const G4Track* track);
    };

    /// Implementation of the Geant4 shared track action
    /**
     * Wrapper to share single instances of track actions for
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
    class Geant4SharedTrackingAction : public Geant4TrackingAction {
    protected:
      /// Reference to the shared action
      Geant4TrackingAction* m_action = 0;

      /// Define standard assignments and constructors
      DDG4_DEFINE_ACTION_CONSTRUCTORS(Geant4SharedTrackingAction);
    public:
      /// Standard constructor
      Geant4SharedTrackingAction(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4SharedTrackingAction();
      /// Set or update client for the use in a new thread fiber
      virtual void configureFiber(Geant4Context* thread_context);
      /// Underlying object to be used during the execution of this thread
      virtual void use(Geant4TrackingAction* action);
      /// Begin-of-track callback
      virtual void begin(const G4Track* track);
      /// End-of-track callback
      virtual void end(const G4Track* track);
    };

    /// Concrete implementation of the Geant4 tracking action sequence
    /**
     * The sequence dispatches the callbacks for each tracking action
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
    class Geant4TrackingActionSequence: public Geant4Action {
    protected:
      /// Callback sequence for pre tracking action
      CallbackSequence             m_front;
      /// Callback sequence for pre tracking action
      CallbackSequence             m_begin;
      /// Callback sequence for post tracking action
      CallbackSequence             m_end;
      /// Callback sequence for pre tracking action
      CallbackSequence             m_final;
      /// The list of action objects to be called
      Actors<Geant4TrackingAction> m_actors;


      /// Define standard assignments and constructors
      DDG4_DEFINE_ACTION_CONSTRUCTORS(Geant4TrackingActionSequence);

    public:
      /// Standard constructor
      Geant4TrackingActionSequence(Geant4Context* context, const std::string& name);
      /// Default destructor
      virtual ~Geant4TrackingActionSequence();
      /// Set or update client context
      virtual void updateContext(Geant4Context* ctxt);
      /// Set or update client for the use in a new thread fiber
      virtual void configureFiber(Geant4Context* thread_context);
      /// Get an action by name
      Geant4TrackingAction* get(const std::string& name) const;
      /// Register Pre-track action callback before anything else
      template <typename Q, typename T>
      void callUpFront(Q* p, void (T::*f)(const G4Track*),
                       CallbackSequence::Location where=CallbackSequence::END) {
        m_front.add(p, f, where);
      }
      /// Register Pre-track action callback
      template <typename Q, typename T>
      void callAtBegin(Q* p, void (T::*f)(const G4Track*),
                       CallbackSequence::Location where=CallbackSequence::END) {
        m_begin.add(p, f, where);
      }
      /// Register Post-track action callback
      template <typename Q, typename T>
      void callAtEnd(Q* p, void (T::*f)(const G4Track*),
                     CallbackSequence::Location where=CallbackSequence::END) {
        m_end.add(p, f, where);
      }
      /// Register Post-track action callback
      template <typename Q, typename T>
      void callAtFinal(Q* p, void (T::*f)(const G4Track*),
                       CallbackSequence::Location where=CallbackSequence::END) {
        m_final.add(p, f, where);
      }
      /// Add an actor responding to all callbacks. Sequence takes ownership.
      void adopt(Geant4TrackingAction* action);
      /// Pre-tracking action callback
      virtual void begin(const G4Track* track);
      /// Post-tracking action callback
      virtual void end(const G4Track* track);
    };

  }    // End namespace sim
}      // End namespace dd4hep
#endif // DD4HEP_DDG4_GEANT4TRACKINGACTION_H
