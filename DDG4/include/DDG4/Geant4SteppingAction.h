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
#ifndef DD4HEP_DDG4_GEANT4STEPPINGACTION_H
#define DD4HEP_DDG4_GEANT4STEPPINGACTION_H

// Framework include files
#include "DDG4/Geant4Action.h"

// Forward declarations
class G4SteppingManager;
class G4Step;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    /// Concrete implementation of the Geant4 stepping action sequence
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4SteppingAction: public Geant4Action {
    public:
      /// Standard constructor
      Geant4SteppingAction(Geant4Context* context, const std::string& name);
      /// Default destructor
      virtual ~Geant4SteppingAction();
      /// User stepping callback
      virtual void operator()(const G4Step* step, G4SteppingManager* mgr);
    };

    /// Concrete implementation of the Geant4 stepping action sequence
    /**
     * The sequence dispatches the callbacks for each stepping action
     * to all registered Geant4SteppingAction members and all
     * registered callbacks.
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

    public:
      /// Standard constructor
      Geant4SteppingActionSequence(Geant4Context* context, const std::string& name);
      /// Default destructor
      virtual ~Geant4SteppingActionSequence();
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

  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4STEPPINGACTION_H
