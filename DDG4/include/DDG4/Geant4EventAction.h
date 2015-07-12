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

#ifndef DD4HEP_DDG4_GEANT4EVENTACTION_H
#define DD4HEP_DDG4_GEANT4EVENTACTION_H

// Framework include files
#include "DDG4/Geant4Action.h"

// Forward declarations
class G4Event;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

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
      /// Standard constructor
      Geant4EventAction(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4EventAction();
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
    public:
      /// Standard constructor
      Geant4EventActionSequence(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4EventActionSequence();
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

  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4EVENTACTION_H
