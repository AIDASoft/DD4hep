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
#ifndef DDG4_GEANT4STACKINGACTION_H
#define DDG4_GEANT4STACKINGACTION_H

/// Framework include files
#include <DDG4/Geant4Action.h>

/// Geant4 include files
#include <G4ClassificationOfNewTrack.hh>

/// Forward declarations
class G4StackManager;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    // Forward declarations
    class Geant4StackingAction;
    class Geant4SharedStackingAction;
    class Geant4StackingActionSequence;

    enum Geant4StackingActionTrackClassification {
      NoTrackClassification = 0xFEED
    };
    union TrackClassification {
      G4ClassificationOfNewTrack value;
      int                        type;
      TrackClassification() { type = NoTrackClassification; }
      TrackClassification(G4ClassificationOfNewTrack val) { value = val; }
    };

    /// Concrete implementation of the Geant4 stacking action base class
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4StackingAction: public Geant4Action {
    public:
      friend class Geant4StackingActionSequence;
      typedef Geant4SharedStackingAction shared_type;
    public:
      /// Define standard assignments and constructors
      DDG4_DEFINE_ACTION_CONSTRUCTORS(Geant4StackingAction);

    public:
      /// Standard constructor
      Geant4StackingAction(Geant4Context* ctxt, const std::string& name);
      /// Default destructor
      virtual ~Geant4StackingAction();
      /// New-stage callback
      virtual void newStage(G4StackManager* /* stackManager */)   {
      }
      /// Preparation callback
      virtual void prepare(G4StackManager* /* stackManager */)   {
      }
      /// Return TrackClassification with enum G4ClassificationOfNewTrack or NoTrackClassification
      virtual TrackClassification 
	classifyNewTrack(G4StackManager* /* stackManager */, const G4Track* track);
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
      /// Define standard assignments and constructors
      DDG4_DEFINE_ACTION_CONSTRUCTORS(Geant4SharedStackingAction);
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
      virtual void newStage(G4StackManager* stackManager)  override;
      /// Preparation callback
      virtual void prepare(G4StackManager* stackManager)  override;
      /// Return TrackClassification with enum G4ClassificationOfNewTrack or NoTrackClassification
      virtual TrackClassification 
	classifyNewTrack(G4StackManager* stackManager, const G4Track* track)  override;
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

      /// Define standard assignments and constructors
      DDG4_DEFINE_ACTION_CONSTRUCTORS(Geant4StackingActionSequence);
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
      template <typename T> void callAtNewStage(T* p, void (T::*f)(G4StackManager*)) {
        m_newStage.add(p, f);
      }
      /// Register end-of-event callback. Types Q and T must be polymorph!
      template <typename T> void callAtPrepare(T* p, void (T::*f)(G4StackManager*)) {
        m_prepare.add(p, f);
      }
      /// Add an actor responding to all callbacks. Sequence takes ownership.
      void adopt(Geant4StackingAction* action);
      /// New-stage callback
      virtual void newStage(G4StackManager* stackManager);
      /// Preparation callback
      virtual void prepare(G4StackManager* stackManager);
      /// Classify new track: The first call in the sequence returning non-null pointer wins!
      virtual TrackClassification 
	classifyNewTrack(G4StackManager* stackManager, const G4Track* track);
    };

  }    // End namespace sim
}      // End namespace dd4hep
#endif // DDG4_GEANT4STACKINGACTION_H
