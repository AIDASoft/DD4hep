// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include <G4Version.hh>

#include "DD4hep/Printout.h"
#include "DD4hep/Primitives.h"
#include "DD4hep/InstanceCount.h"

#include "G4UserRunAction.hh"
#include "G4UserEventAction.hh"
#include "G4UserTrackingAction.hh"
#include "G4UserStackingAction.hh"
#include "G4UserSteppingAction.hh"
#if G4VERSION_NUMBER < 1000
#include "G4UserReactionAction.hh"
#endif
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
#include "DDG4/Geant4UIManager.h"
#include "DDG4/Geant4Kernel.h"
#include "DDG4/Geant4Random.h"

#include <memory>
#include <stdexcept>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    namespace {
      Geant4Context* s_globalContext = 0;
      Geant4Random*  s_globalRandom = 0;
    }

    Geant4Context* ddg4_globalContext()   {
      return s_globalContext;
    }

    /// Sequence handler implementing common actions to all sequences.
    template <typename T> class SequenceHdl {
    public:
      typedef SequenceHdl<T> Base;
      T* m_sequence;
      Geant4Context* m_activeContext;
      /// Default constructor
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
      Geant4Context* context() const  {  
	return m_activeContext;
      }
      Geant4Kernel& kernel()  const  {
	return context()->kernel();
      }

      void setContextToClients()   {
        (Geant4Action::ContextUpdate(m_activeContext))(m_sequence);
      }
      void releaseContextFromClients()  {
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
        Geant4Event* e = new Geant4Event(evt,s_globalRandom);
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

    class Geant4UserRunAction;
    class Geant4UserEventAction;

    /// Concrete implementation of the Geant4 run action
    /** @class Geant4UserRunAction
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4UserRunAction : public G4UserRunAction, public SequenceHdl<Geant4RunActionSequence> {
    public:
      Geant4UserEventAction* eventAction;
      /// Standard constructor
      Geant4UserRunAction(Geant4Context* context, Geant4RunActionSequence* seq)
        : Base(context, seq), eventAction(0)  {
      }
      /// Default destructor
      virtual ~Geant4UserRunAction() {
      }
      /// Begin-of-run callback
      virtual void BeginOfRunAction(const G4Run* run);
      /// End-of-run callback
      virtual void EndOfRunAction(const G4Run* run);
    };

    /// Concrete implementation of the Geant4 event action
    /** @class Geant4UserEventAction
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4UserEventAction : public G4UserEventAction, public SequenceHdl<Geant4EventActionSequence> {
    public:
      Geant4UserRunAction* runAction;
      /// Standard constructor
      Geant4UserEventAction(Geant4Context* context, Geant4EventActionSequence* seq)
        : Base(context, seq), runAction(0)  {
      }
      /// Default destructor
      virtual ~Geant4UserEventAction() {
      }
      /// Begin-of-event callback
      virtual void BeginOfEventAction(const G4Event* evt);
      /// End-of-event callback
      virtual void EndOfEventAction(const G4Event* evt);
    };

    /// Concrete implementation of the Geant4 tracking action
    /** @class Geant4UserTrackingAction
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4UserTrackingAction : public G4UserTrackingAction, public SequenceHdl<Geant4TrackingActionSequence> {
    public:
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
        releaseContextFromClients();    //Let's leave this out for now...Frank has dirty tricks.
      }
    };

    /// Concrete implementation of the Geant4 stacking action sequence
    /** @class Geant4UserStackingAction
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4UserStackingAction : public G4UserStackingAction, public SequenceHdl<Geant4StackingActionSequence> {
    public:
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
        releaseContextFromClients();    //Let's leave this out for now...Frank has dirty tricks.
      }
      /// Preparation callback
      virtual void PrepareNewEvent() {
        setContextToClients();
        m_sequence->prepare();
        releaseContextFromClients();    //Let's leave this out for now...Frank has dirty tricks.
      }
    };

    /// Concrete implementation of the Geant4 generator action
    /** @class Geant4UserGeneratorAction
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4UserGeneratorAction : public G4VUserPrimaryGeneratorAction, public SequenceHdl<Geant4GeneratorActionSequence> {
    public:
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
        releaseContextFromClients();    //Let's leave this out for now...Frank has dirty tricks.
      }
    };

    /// Concrete implementation of the Geant4 stepping action
    /** @class Geant4UserSteppingAction
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4UserSteppingAction : public G4UserSteppingAction, public SequenceHdl<Geant4SteppingActionSequence> {
    public:
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
        releaseContextFromClients();    //Let's leave this out for now...Frank has dirty tricks.
      }
    };

    /// Begin-of-run callback
    void Geant4UserRunAction::BeginOfRunAction(const G4Run* run) {
      createClientContext(run);
      kernel().executePhase("begin-run",(const void**)&run);
      eventAction->setContextToClients();
      m_sequence->begin(run);
    }

    /// End-of-run callback
    void Geant4UserRunAction::EndOfRunAction(const G4Run* run) {
      m_sequence->end(run);
      kernel().executePhase("end-run",(const void**)&run);
      eventAction->releaseContextFromClients();
      destroyClientContext(run);
    }

    /// Begin-of-event callback
    void Geant4UserEventAction::BeginOfEventAction(const G4Event* evt) {
      runAction->setContextToClients();
      setContextToClients();
      kernel().executePhase("begin-event",(const void**)&evt);
      m_sequence->begin(evt);
    }

    /// End-of-event callback
    void Geant4UserEventAction::EndOfEventAction(const G4Event* evt) {
      m_sequence->end(evt);
      runAction->releaseContextFromClients();
      kernel().executePhase("end-event",(const void**)&evt);
      destroyClientContext(evt);
    }
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
  // For now do this:
  /* Geant4Random* rnd = */ s_globalRandom = new Geant4Random();

  kernel.executePhase("configure",0);

  // Construct the default run manager
  G4RunManager& runManager = kernel.runManager();

  // Check if the geometry was loaded
  if (lcdd.detectors().size() <= 1) {
    printout(WARNING, "Geant4Exec", "+++ Only %d subdetectors present. "
             "You sure you loaded the geometry properly?",int(lcdd.detectors().size()));
  }
  // Get the detector constructed
  Geant4DetectorConstruction* detector = Geant4DetectorConstruction::instance(kernel);

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
  Geant4UserRunAction* run_action = new Geant4UserRunAction(ctx,kernel.runAction(false));
  runManager.SetUserAction(run_action);

  // Set the event action sequence. Not optional, since event context is destroyed inside
  Geant4UserEventAction* evt_action = new Geant4UserEventAction(ctx,kernel.eventAction(false));
  runManager.SetUserAction(evt_action);
  run_action->eventAction = evt_action;
  evt_action->runAction = run_action;

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
  kernel.executePhase("initialize",0);
  runManager.Initialize();
  return 1;
}

/// Run the simulation
int Geant4Exec::run(Geant4Kernel& kernel) {
  Property& p = kernel.property("UI");
  string value = p.value<string>();

  kernel.executePhase("start",0);
  if ( !value.empty() )  {
    Geant4Action* ui = kernel.globalAction(value);
    if ( ui )  {
      Geant4Call* c = dynamic_cast<Geant4Call*>(ui);
      if ( c )  {
        (*c)(0);
	kernel.executePhase("stop",0);
        return 1;
      }
      ui->except("++ Geant4Exec: Failed to start UI interface.");
    }
    throw runtime_error(format("Geant4Exec","++ Failed to locate UI interface %s.",value.c_str()));
  }
  long nevt = kernel.property("NumEvents").value<long>();
  kernel.runManager().BeamOn(nevt);
  kernel.executePhase("stop",0);
  return 1;
}

/// Run the simulation
int Geant4Exec::terminate(Geant4Kernel& kernel) {
  kernel.executePhase("terminate",0);
  deletePtr(s_globalContext);
  return 1;
}
