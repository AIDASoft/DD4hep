// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#include "DD4hep/Printout.h"
#include "DD4hep/Primitives.h"
#include "DD4hep/InstanceCount.h"

#include "G4UserRunAction.hh"
#include "G4UserEventAction.hh"
#include "G4UserTrackingAction.hh"
#include "G4UserStackingAction.hh"
#include "G4UserSteppingAction.hh"
#include "G4UserReactionAction.hh"
#include "G4VUserPhysicsList.hh"
#include "G4VModularPhysicsList.hh"
#include "G4VUserPrimaryGeneratorAction.hh"

#include "DD4hep/Handle.h"
#include "DDG4/Geant4RunAction.h"
#include "DDG4/Geant4EventAction.h"
#include "DDG4/Geant4SteppingAction.h"
#include "DDG4/Geant4TrackingAction.h"
#include "DDG4/Geant4StackingAction.h"
#include "DDG4/Geant4GeneratorAction.h"
#include "DDG4/Geant4PhysicsList.h"
#include "DDG4/Geant4Kernel.h"
#include "DDG4/Geant4UIManager.h"

#include <memory>
#include <stdexcept>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {
    template <typename T> struct SequenceHdl {
      typedef SequenceHdl<T> Base;
      T* m_sequence;
      SequenceHdl()
	: m_sequence(0) {
      }
      SequenceHdl(T* seq) {
	_aquire(seq);
      }
      virtual ~SequenceHdl() {
	_release();
      }
      void _aquire(T* s) {
	InstanceCount::increment(this);
	m_sequence = s;
	m_sequence->addRef();
      }
      void _release() {
	releasePtr(m_sequence);
	InstanceCount::decrement(this);
      }
    };

    /** @class Geant4UserRunAction
     *
     * Concrete implementation of the Geant4 run action
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct Geant4UserRunAction : public G4UserRunAction, public SequenceHdl<Geant4RunActionSequence> {
      /// Standard constructor
      Geant4UserRunAction(Geant4RunActionSequence* seq)
          : Base(seq) {
      }
      /// Default destructor
      virtual ~Geant4UserRunAction() {
      }
      /// begin-of-run callback
      virtual void BeginOfRunAction(const G4Run* run) {
        m_sequence->begin(run);
      }
      /// End-of-run callback
      virtual void EndOfRunAction(const G4Run* run) {
        m_sequence->end(run);
      }
    };

    /** @class Geant4UserEventAction
     *
     * Concrete implementation of the Geant4 event action
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct Geant4UserEventAction : public G4UserEventAction, public SequenceHdl<Geant4EventActionSequence> {
      /// Standard constructor
      Geant4UserEventAction(Geant4EventActionSequence* seq)
          : Base(seq) {
      }
      /// Default destructor
      virtual ~Geant4UserEventAction() {
      }
      /// begin-of-event callback
      virtual void BeginOfEventAction(const G4Event* evt) {
        m_sequence->begin(evt);
      }
      /// End-of-event callback
      virtual void EndOfEventAction(const G4Event* evt) {
        m_sequence->end(evt);
      }
    };

    /** @class Geant4UserTrackingAction
     *
     * Concrete implementation of the Geant4 tracking action
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct Geant4UserTrackingAction : public G4UserTrackingAction, public SequenceHdl<Geant4TrackingActionSequence> {
      /// Standard constructor
      Geant4UserTrackingAction(Geant4TrackingActionSequence* seq)
          : Base(seq) {
      }
      /// Default destructor
      virtual ~Geant4UserTrackingAction() {
      }
      /// Pre-track action callback
      virtual void PreUserTrackingAction(const G4Track* trk) {
	m_sequence->context()->kernel().setTrackMgr(fpTrackingManager);
        m_sequence->begin(trk);
      }
      /// Post-track action callback
      virtual void PostUserTrackingAction(const G4Track* trk) {
        m_sequence->end(trk);
	m_sequence->context()->kernel().setTrackMgr(0);
      }
    };

    /** @class Geant4UserStackingAction
     *
     * Concrete implementation of the Geant4 stacking action sequence
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct Geant4UserStackingAction : public G4UserStackingAction, public SequenceHdl<Geant4StackingActionSequence> {
      /// Standard constructor
      Geant4UserStackingAction(Geant4StackingActionSequence* seq)
          : Base(seq) {
      }
      /// Default destructor
      virtual ~Geant4UserStackingAction() {
      }
      /// New-stage callback
      virtual void NewStage() {
        m_sequence->newStage();
      }
      /// Preparation callback
      virtual void PrepareNewEvent() {
        m_sequence->prepare();
      }
    };

    /** @class Geant4UserGeneratorAction
     *
     * Concrete implementation of the Geant4 generator action
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct Geant4UserGeneratorAction : public G4VUserPrimaryGeneratorAction, public SequenceHdl<Geant4GeneratorActionSequence> {
      /// Standard constructor
      Geant4UserGeneratorAction(Geant4GeneratorActionSequence* seq)
          : G4VUserPrimaryGeneratorAction(), Base(seq) {
      }
      /// Default destructor
      virtual ~Geant4UserGeneratorAction() {
      }
      /// Generate primary particles
      virtual void GeneratePrimaries(G4Event* event) {
        (*m_sequence)(event);
      }
    };

    /** @class Geant4UserSteppingAction
     *
     * Concrete implementation of the Geant4 stepping action
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct Geant4UserSteppingAction : public G4UserSteppingAction, public SequenceHdl<Geant4SteppingActionSequence> {
      /// Standard constructor
      Geant4UserSteppingAction(Geant4SteppingActionSequence* seq)
          : Base(seq) {
      }
      /// Default destructor
      virtual ~Geant4UserSteppingAction() {
      }
      /// User stepping callback
      virtual void UserSteppingAction(const G4Step* s) {
        (*m_sequence)(s, fpSteppingManager);
      }
    };

  }
}

#include "DD4hep/LCDD.h"
#include "DD4hep/Plugins.h"
#include "DDG4/Geant4DetectorConstruction.h"
#include "DDG4/Geant4UserPhysicsList.h"
#include "DDG4/Geant4Kernel.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;

// Geant4 include files
#include "G4RunManager.hh"

/// Configure the simulation
int Geant4Exec::configure(Geant4Kernel& kernel) {
  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
  Geometry::LCDD& lcdd = kernel.lcdd();

  // Construct the default run manager
  G4RunManager& runManager = kernel.runManager();

  // Check if the geometry was loaded
  if (lcdd.detectors().size() <= 2) {
    cout << "Error, no geometry." << endl;
  }
  // Get the detector constructed
  Geant4DetectorConstruction* detector = new Geant4DetectorConstruction(lcdd);

  runManager.SetUserInitialization(detector);
  G4VUserPhysicsList* physics = 0;
  Geant4PhysicsListActionSequence* seq = kernel.physicsList(false);
  if (seq) {
    string nam = kernel.physicsList().properties()["extends"].value<string>();
    if (nam.empty())
      nam = "EmptyPhysics";
    physics = PluginService::Create<G4VUserPhysicsList*>(nam, seq, int(1));
  }
  else {
    printout(INFO, "Geant4Exec", "+++ Using Geant4 physics constructor QGSP_BERT");
    physics = PluginService::Create<G4VUserPhysicsList*>(string("QGSP_BERT"), seq, int(1));
  }
  if (0 == physics) {
    throw runtime_error("Panic! No valid user physics list present!");
  }
  runManager.SetUserInitialization(physics);

  // Set user generator action sequence
  if (kernel.generatorAction(false)) {
    Geant4UserGeneratorAction* action = new Geant4UserGeneratorAction(kernel.generatorAction(false));
    runManager.SetUserAction(action);
  }
  // Set the run action sequence
  if (kernel.runAction(false)) {
    Geant4UserRunAction* action = new Geant4UserRunAction(kernel.runAction(false));
    runManager.SetUserAction(action);
  }
  // Set the event action sequence
  if (kernel.eventAction(false)) {
    Geant4UserEventAction* action = new Geant4UserEventAction(kernel.eventAction(false));
    runManager.SetUserAction(action);
  }
  // Set the tracking action sequence
  if (kernel.trackingAction(false)) {
    Geant4UserTrackingAction* action = new Geant4UserTrackingAction(kernel.trackingAction(false));
    runManager.SetUserAction(action);
  }
  // Set the stepping action sequence
  if (kernel.steppingAction(false)) {
    Geant4UserSteppingAction* action = new Geant4UserSteppingAction(kernel.steppingAction(false));
    runManager.SetUserAction(action);
  }
  // Set the stacking action sequence
  if (kernel.stackingAction(false)) {
    Geant4UserStackingAction* action = new Geant4UserStackingAction(kernel.stackingAction(false));
    runManager.SetUserAction(action);
  }
  return 1;
}

/// Initialize the simulation
int Geant4Exec::initialize(Geant4Kernel& kernel) {
  // Construct the default run manager
  G4RunManager& runManager = kernel.runManager();
  //
  // Initialize G4 engine
  //
  runManager.Initialize();
  return 1;
}

/// Run the simulation
int Geant4Exec::run(Geant4Kernel& kernel) {
  Property& p = kernel.property("UI");
  string value = p.value<string>();
  if ( !value.empty() )  {
    Geant4Action* ui = kernel.globalAction(value);
    if ( ui )  {
      Geant4Call* c = dynamic_cast<Geant4Call*>(ui);
      if ( c )  {
	(*c)(0);
	return 1;
      }
      ui->except("++ Geant4Exec: Failed to start UI interface.");
    }
    throw runtime_error(format("Geant4Exec","++ Failed to locate UI interface %s.",value.c_str()));
  }
  long nevt = kernel.property("NumEvents").value<long>();
  kernel.runManager().BeamOn(nevt);
  return 1;
}

/// Run the simulation
int Geant4Exec::terminate(Geant4Kernel&) {
  return 1;
}
