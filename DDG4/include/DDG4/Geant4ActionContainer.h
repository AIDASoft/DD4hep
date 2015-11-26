// $Id$
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
#ifndef DD4HEP_DDG4_GEANT4ACTIONCONTAINER_H
#define DD4HEP_DDG4_GEANT4ACTIONCONTAINER_H

// Framework include files
#include "DD4hep/Printout.h"
#include "DDG4/Geant4Primitives.h"
#include "DDG4/Geant4Action.h"

// C/C++ include files
#include <map>
#include <string>
#include <typeinfo>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    // Forward declarations
    class Geant4Context;
    class Geant4Action;
    class Geant4RunAction;
    class Geant4EventAction;
    class Geant4SteppingAction;
    class Geant4TrackingAction;
    class Geant4StackingAction;
    class Geant4GeneratorAction;
    class Geant4PhysicsList;
    class Geant4RunActionSequence;
    class Geant4EventActionSequence;
    class Geant4SteppingActionSequence;
    class Geant4TrackingActionSequence;
    class Geant4StackingActionSequence;
    class Geant4GeneratorActionSequence;
    class Geant4PhysicsListActionSequence;
    class Geant4DetectorConstructionSequence;
    class Geant4UserInitializationSequence;
    class Geant4SensDetActionSequence;
    class Geant4SensDetSequences;

    /// Class, which allows all Geant4Action to be stored
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4ActionContainer {
    protected:

      /// Geant4 worker context (thread specific)
      Geant4Context*                      m_context;

      /// Reference to the Geant4 primary generator action
      Geant4GeneratorActionSequence*      m_generatorAction;
      /// Reference to the Geant4 run action
      Geant4RunActionSequence*            m_runAction;
      /// Reference to the Geant4 event action
      Geant4EventActionSequence*          m_eventAction;
      /// Reference to the Geant4 track action
      Geant4TrackingActionSequence*       m_trackingAction;
      /// Reference to the Geant4 step action
      Geant4SteppingActionSequence*       m_steppingAction;
      /// Reference to the Geant4 stacking action
      Geant4StackingActionSequence*       m_stackingAction;
      /// Reference to the Geant4 detector construction sequence
      Geant4DetectorConstructionSequence* m_constructionAction;

      /// Reference to the Geant4 sensitive action sequences
      Geant4SensDetSequences*             m_sensDetActions;
      /// Reference to the Geant4 physics list
      Geant4PhysicsListActionSequence*    m_physicsList;
      /// Reference to the user initialization object
      Geant4UserInitializationSequence*   m_userInit;

      /// Helper to register an action sequence
      template <typename C> bool registerSequence(C*& seq, const std::string& name);

      /// Standard constructor
      Geant4ActionContainer(Geant4Context* ctxt=0);

      /// Default destructor
      virtual ~Geant4ActionContainer();

      /// Set the thread's context
      void setContext(Geant4Context* ctxt);

    public:
      /// Thread's Geant4 execution context
      Geant4Context* workerContext();

      /// Terminate all associated action instances
      virtual int terminate();

      /// Access generator action sequence
      Geant4GeneratorActionSequence* generatorAction(bool create);
      /// Access generator action sequence
      Geant4GeneratorActionSequence& generatorAction() {
        return *generatorAction(true);
      }

      /// Access run action sequence
      Geant4RunActionSequence* runAction(bool create);
      /// Access run action sequence
      Geant4RunActionSequence& runAction() {
        return *runAction(true);
      }

      /// Access run action sequence
      Geant4EventActionSequence* eventAction(bool create);
      /// Access run action sequence
      Geant4EventActionSequence& eventAction() {
        return *eventAction(true);
      }

      /// Access stepping action sequence
      Geant4SteppingActionSequence* steppingAction(bool create);
      /// Access stepping action sequence
      Geant4SteppingActionSequence& steppingAction() {
        return *steppingAction(true);
      }

      /// Access tracking action sequence
      Geant4TrackingActionSequence* trackingAction(bool create);
      /// Access tracking action sequence
      Geant4TrackingActionSequence& trackingAction() {
        return *trackingAction(true);
      }

      /// Access stacking action sequence
      Geant4StackingActionSequence* stackingAction(bool create);
      /// Access stacking action sequence
      Geant4StackingActionSequence& stackingAction() {
        return *stackingAction(true);
      }

      /// Access detector construcion action sequence (geometry+sensitives+field)
      Geant4DetectorConstructionSequence* detectorConstruction(bool create);
      /// Access detector construcion action sequence (geometry+sensitives+field)
      Geant4DetectorConstructionSequence& detectorConstruction() {
        return *detectorConstruction(true);
      }

      /// Access to the sensitive detector sequences from the actioncontainer object
      Geant4SensDetSequences& sensitiveActions() const;
      /// Access to the sensitive detector action from the actioncontainer object
      Geant4SensDetActionSequence* sensitiveAction(const std::string& name);

      /// Access to the physics list
      Geant4PhysicsListActionSequence* physicsList(bool create);
      /// Access to the physics list
      Geant4PhysicsListActionSequence& physicsList() {
        return *physicsList(true);
      }
      /// Access to the user initialization object
      Geant4UserInitializationSequence* userInitialization(bool create);
      /// Access to the user initialization object
      Geant4UserInitializationSequence& userInitialization() {
        return *userInitialization(true);
      }
    };

  }    // End namespace Simulation
}      // End namespace DD4hep
#endif // DD4HEP_DDG4_GEANT4KERNEL_H
