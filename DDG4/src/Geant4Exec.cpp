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

    namespace {
      Geant4Context* s_globalContext = 0;
    }

    Geant4Context* ddg4_globalContext()   {
      return s_globalContext;
    }


    template <typename T> struct SequenceHdl {
      typedef SequenceHdl<T> Base;
      T* m_sequence;
      Geant4Context* m_activeContext;
      SequenceHdl()
	: m_sequence(0), m_activeContext(0) {
      }
      SequenceHdl(Geant4Context* context, T* seq) : m_sequence(0), m_activeContext(context)  {
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
      void setContextToClients()   {
	(Geant4Action::ContextUpdate(m_activeContext))(m_sequence);
      }
      void releaseContextFromClients()  {
	Geant4Action::ContextUpdate(0)(m_sequence);
      }
      void releaseContextFromClients2()  {
	Geant4Action::ContextUpdate(0)(m_sequence);
      }
      void createClientContext(const G4Run* run)   {
	Geant4Run* r = new Geant4Run(run);
	m_activeContext->setRun(r);
	setContextToClients();
      }
      void destroyClientContext(const G4Run*)   {
	Geant4Run* r = m_activeContext->runPtr();
	releaseContextFromClients();
	if ( r )  {
	  m_activeContext->setRun(0);
	  deletePtr(r);
	}
      }
      void createClientContext(const G4Event* evt)   {
	Geant4Event* e = new Geant4Event(evt);
	m_activeContext->setEvent(e);
	setContextToClients();
      }
      void destroyClientContext(const G4Event*)   {
	Geant4Event* e = m_activeContext->eventPtr();
	releaseContextFromClients();
	if ( e )  {
	  m_activeContext->setEvent(0);
	  deletePtr(e);
	}
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
      Geant4UserRunAction(Geant4Context* context, Geant4RunActionSequence* seq)
	: Base(context, seq) {
      }
      /// Default destructor
      virtual ~Geant4UserRunAction() {
      }
      /// begin-of-run callback
      virtual void BeginOfRunAction(const G4Run* run) {
	createClientContext(run);
        m_sequence->begin(run);
      }
      /// End-of-run callback
      virtual void EndOfRunAction(const G4Run* run) {
        m_sequence->end(run);
	destroyClientContext(run);
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
      Geant4UserEventAction(Geant4Context* context, Geant4EventActionSequence* seq)
	: Base(context, seq) {
      }
      /// Default destructor
      virtual ~Geant4UserEventAction() {
      }
      /// begin-of-event callback
      virtual void BeginOfEventAction(const G4Event* evt) {
	setContextToClients();
        m_sequence->begin(evt);
      }
      /// End-of-event callback
      virtual void EndOfEventAction(const G4Event* evt) {
        m_sequence->end(evt);
	destroyClientContext(evt);
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
      Geant4UserTrackingAction(Geant4Context* context, Geant4TrackingActionSequence* seq)
	: Base(context, seq) {
      }
      /// Default destructor
      virtual ~Geant4UserTrackingAction() {
      }
      /// Pre-track action callback
      virtual void PreUserTrackingAction(const G4Track* trk) {
	setContextToClients();
	m_sequence->context()->kernel().setTrackMgr(fpTrackingManager);
        m_sequence->begin(trk);
      }
      /// Post-track action callback
      virtual void PostUserTrackingAction(const G4Track* trk) {
        m_sequence->end(trk);
	m_sequence->context()->kernel().setTrackMgr(0);
	releaseContextFromClients2();	//Let's leave this out for now...Frank has dirty tricks.
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
      Geant4UserStackingAction(Geant4Context* context, Geant4StackingActionSequence* seq)
	: Base(context, seq) {
      }
      /// Default destructor
      virtual ~Geant4UserStackingAction() {
      }
      /// New-stage callback
      virtual void NewStage() {
	setContextToClients();
        m_sequence->newStage();
	releaseContextFromClients2();	//Let's leave this out for now...Frank has dirty tricks.
      }
      /// Preparation callback
      virtual void PrepareNewEvent() {
	setContextToClients();
        m_sequence->prepare();
	releaseContextFromClients2();	//Let's leave this out for now...Frank has dirty tricks.
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
      Geant4UserGeneratorAction(Geant4Context* context, Geant4GeneratorActionSequence* seq)
	: G4VUserPrimaryGeneratorAction(), Base(context, seq) {
      }
      /// Default destructor
      virtual ~Geant4UserGeneratorAction() {
      }
      /// Generate primary particles
      virtual void GeneratePrimaries(G4Event* event) {
	createClientContext(event);
        (*m_sequence)(event);
	releaseContextFromClients2();	//Let's leave this out for now...Frank has dirty tricks.
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
      Geant4UserSteppingAction(Geant4Context* context, Geant4SteppingActionSequence* seq)
	: Base(context, seq) {
      }
      /// Default destructor
      virtual ~Geant4UserSteppingAction() {
      }
      /// User stepping callback
      virtual void UserSteppingAction(const G4Step* s) {
	setContextToClients();
        (*m_sequence)(s, fpSteppingManager);
	releaseContextFromClients2();	//Let's leave this out for now...Frank has dirty tricks.
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
  Geant4Context* ctx = s_globalContext = new Geant4Context(&kernel);

  // Construct the default run manager
  G4RunManager& runManager = kernel.runManager();

  // Check if the geometry was loaded
  if (lcdd.detectors().size() <= 1) {
    printout(WARNING, "Geant4Exec", "+++ Only %d subdetectors present. "
	     "You sure you loaded the geometry properly?",int(lcdd.detectors().size()));
  }
  // Get the detector constructed
  Geant4DetectorConstruction* detector = new Geant4DetectorConstruction(kernel);

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
  // Set user generator action sequence. Not optional, since event context is defined inside
  Geant4UserGeneratorAction* gen_action = 
    new Geant4UserGeneratorAction(ctx,kernel.generatorAction(false));
  runManager.SetUserAction(gen_action);

  // Set the run action sequence. Not optional, since run context is defined/destroyed inside
  Geant4UserRunAction* run_action = 
    new Geant4UserRunAction(ctx,kernel.runAction(false));
  runManager.SetUserAction(run_action);

  // Set the event action sequence. Not optional, since event context is destroyed inside
  Geant4UserEventAction* evt_action =
    new Geant4UserEventAction(ctx,kernel.eventAction(false));
  runManager.SetUserAction(evt_action);

  // Set the tracking action sequence
  if (kernel.trackingAction(false)) {
    Geant4UserTrackingAction* action = new Geant4UserTrackingAction(ctx,kernel.trackingAction(false));
    runManager.SetUserAction(action);
  }
  // Set the stepping action sequence
  if (kernel.steppingAction(false)) {
    Geant4UserSteppingAction* action = new Geant4UserSteppingAction(ctx,kernel.steppingAction(false));
    runManager.SetUserAction(action);
  }
  // Set the stacking action sequence
  if (kernel.stackingAction(false)) {
    Geant4UserStackingAction* action = new Geant4UserStackingAction(ctx,kernel.stackingAction(false));
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
  deletePtr(s_globalContext);
  return 1;
}
