// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4TESTACTIONS_H
#define DD4HEP_DDG4_GEANT4TESTACTIONS_H

// Framework include files
#include "DDG4/Geant4Handle.h"
#include "DDG4/Geant4Kernel.h"
#include "DDG4/Geant4GeneratorAction.h"
#include "DDG4/Geant4RunAction.h"
#include "DDG4/Geant4EventAction.h"
#include "DDG4/Geant4TrackingAction.h"
#include "DDG4/Geant4SteppingAction.h"
#include "DDG4/Geant4StackingAction.h"
#include "DDG4/Geant4ActionPhase.h"
#include "DDG4/Geant4SensDetAction.h"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {

    namespace Test {

      /// Common base class for test action
      /** 
       *
       * @author  M.Frank
       * @version 1.0
       */
      class Geant4TestBase {
      protected:
        int m_value1;
        double m_value2;
        std::string m_value3;
        std::string m_type;
      public:
        /// Standard constructor
        Geant4TestBase(Geant4Action* action, const std::string& typ);
        /// Default destructor
        virtual ~Geant4TestBase();
      };

      /// Example generator action doing nothing, but print
      /**
       * @author  M.Frank
       * @version 1.0
       */
      struct Geant4TestGeneratorAction: public Geant4GeneratorAction, public Geant4TestBase {
        /// Standard constructor with initializing arguments
        Geant4TestGeneratorAction(Geant4Context* c, const std::string& n);
        /// Default destructor
        virtual ~Geant4TestGeneratorAction();
	/// Callback to generate primary particles
	virtual void operator()(G4Event*);
      };

      /// Example run action doing nothing, but print
      /**
       * @author  M.Frank
       * @version 1.0
       */
      struct Geant4TestRunAction: public Geant4RunAction, public Geant4TestBase {
        /// Standard constructor with initializing arguments
        Geant4TestRunAction(Geant4Context* c, const std::string& n);
        /// Default destructor
        virtual ~Geant4TestRunAction();
        /// begin-of-run callback
        void begin(const G4Run*);
        /// End-of-run callback
        void end(const G4Run*);
        /// begin-of-event callback
        void beginEvent(const G4Event*);
        /// End-of-event callback
        void endEvent(const G4Event*);
      };

      /// Example event action doing nothing, but print
      /**
       * @author  M.Frank
       * @version 1.0
       */
      struct Geant4TestEventAction: public Geant4EventAction, public Geant4TestBase {
        /// Standard constructor with initializing arguments
        Geant4TestEventAction(Geant4Context* c, const std::string& n);
        /// Default destructor
        virtual ~Geant4TestEventAction();
        /// begin-of-event callback
        virtual void begin(const G4Event*);
        /// End-of-event callback
        virtual void end(const G4Event*);
        /// begin-of-run callback
        void beginRun(const G4Run*);
        /// End-of-run callback
        void endRun(const G4Run*);
      };

      /// Example tracking action doing nothing, but print
      /**
       * @author  M.Frank
       * @version 1.0
       */
      struct Geant4TestTrackAction: public Geant4TrackingAction, public Geant4TestBase {
        /// Standard constructor with initializing arguments
        Geant4TestTrackAction(Geant4Context* c, const std::string& n);
        /// Default destructor
        virtual ~Geant4TestTrackAction();
        /// Begin-of-tracking callback
        virtual void begin(const G4Track*);
        /// End-of-tracking callback
        virtual void end(const G4Track*);
      };

      /// Example stepping action doing nothing, but print
      /**
       * @author  M.Frank
       * @version 1.0
       */
      struct Geant4TestStepAction: public Geant4SteppingAction, public Geant4TestBase {
        /// Standard constructor with initializing arguments
        Geant4TestStepAction(Geant4Context* c, const std::string& n);
        /// Default destructor
        virtual ~Geant4TestStepAction();
        /// User stepping callback
        void operator()(const G4Step*, G4SteppingManager*);
      };

      /// Example sensitve detector action doing nothing, but print
      /**
       * @author  M.Frank
       * @version 1.0
       */
      struct Geant4TestSensitive: public Geant4Sensitive, public Geant4TestBase {
        size_t m_collectionID;
        /// Standard constructor with initializing arguments
        Geant4TestSensitive(Geant4Context* c, const std::string& n, DetElement det, LCDD& lcdd);
        /// Default destructor
        virtual ~Geant4TestSensitive();
        /// Begin-of-tracking callback
        virtual void begin(G4HCofThisEvent*);
        /// End-of-tracking callback
        virtual void end(G4HCofThisEvent*);
        /// Method for generating hit(s) using the information of G4Step object.
        virtual bool process(G4Step*, G4TouchableHistory*);
      };
    }   // End namespace Test
  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4TESTACTIONS_H
