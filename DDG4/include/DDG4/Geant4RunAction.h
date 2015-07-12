// $Id: Handle.h 570 2013-05-17 07:47:11Z markus.frank $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation européenne pour la Recherche nucléaire (CERN)
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
      /// Standard constructor
      Geant4RunAction(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4RunAction();
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
      /// Standard constructor
      Geant4RunActionSequence(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4RunActionSequence();
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
