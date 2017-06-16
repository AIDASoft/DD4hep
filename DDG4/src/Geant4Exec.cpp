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

// Framework include files
#include "DD4hep/Printout.h"
#include "DD4hep/Primitives.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/Handle.h"
#include "DDG4/Geant4RunAction.h"
#include "DDG4/Geant4EventAction.h"
#include "DDG4/Geant4SteppingAction.h"
#include "DDG4/Geant4TrackingAction.h"
#include "DDG4/Geant4StackingAction.h"
#include "DDG4/Geant4GeneratorAction.h"
#include "DDG4/Geant4UserInitialization.h"
#include "DDG4/Geant4DetectorConstruction.h"
#include "DDG4/Geant4PhysicsList.h"
#include "DDG4/Geant4UIManager.h"
#include "DDG4/Geant4Kernel.h"
#include "DDG4/Geant4Random.h"

// Geant4 include files
#include "G4Version.hh"
#include "G4UserRunAction.hh"
#include "G4UserEventAction.hh"
#include "G4UserTrackingAction.hh"
#include "G4UserStackingAction.hh"
#include "G4UserSteppingAction.hh"
#include "G4VUserPhysicsList.hh"
#include "G4VModularPhysicsList.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4VUserActionInitialization.hh"
#include "G4VUserDetectorConstruction.hh"

// C/C++ include files
#include <memory>
#include <stdexcept>

namespace {
  G4Mutex action_mutex=G4MUTEX_INITIALIZER;
}

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    /// Sequence handler implementing common actions to all sequences.
    template <typename T> class SequenceHdl {
    public:
      typedef SequenceHdl<T> Base;
      T* m_sequence;
      mutable Geant4Context* m_activeContext;
      /// Default constructor
      SequenceHdl()
        : m_sequence(0), m_activeContext(0) {
      }
      SequenceHdl(Geant4Context* ctxt, T* seq) : m_sequence(0), m_activeContext(ctxt)  {
        _aquire(seq);
      }
      virtual ~SequenceHdl() {
        _release();
      }
      void _aquire(T* s) {
        InstanceCount::increment(this);
        m_sequence = s;
        if ( m_sequence ) m_sequence->addRef();
      }
      void updateContext(Geant4Context* ctxt)   {
        m_activeContext = ctxt;
        if ( m_sequence )  {
          m_sequence->updateContext(ctxt);
        }
      }
      void _release() {
        detail::releasePtr(m_sequence);
        InstanceCount::decrement(this);
      }
      Geant4Context* context() const  {  
        return m_activeContext;
      }
      Geant4Kernel& kernel()  const  {
        return context()->kernel();
      }
      void configureFiber(Geant4Context* ctxt)   {
        if ( m_sequence )  {
          m_sequence->configureFiber(ctxt);
        }
      }
      void createClientContext(const G4Run* run)   {
        Geant4Run* r = new Geant4Run(run);
        m_activeContext->setRun(r);
      }
      void destroyClientContext(const G4Run*)   {
        Geant4Run* r = m_activeContext->runPtr();
        if ( r )  {
          m_activeContext->setRun(0);
          detail::deletePtr(r);
        }
      }
      void createClientContext(const G4Event* evt)   {
        Geant4Event* e = new Geant4Event(evt,Geant4Random::instance());
        m_activeContext->setEvent(e);
      }
      void destroyClientContext(const G4Event*)   {
        Geant4Event* e = m_activeContext->eventPtr();
        if ( e )  {
          m_activeContext->setEvent(0);
          detail::deletePtr(e);
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
    class Geant4UserRunAction :
      public G4UserRunAction,
      public SequenceHdl<Geant4RunActionSequence>
    {
    public:
      Geant4UserEventAction* eventAction;
      /// Standard constructor
      Geant4UserRunAction(Geant4Context* ctxt, Geant4RunActionSequence* seq)
        : Base(ctxt, seq), eventAction(0)  {
        updateContext(ctxt);
        configureFiber(ctxt);
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
    class Geant4UserEventAction :
      public G4UserEventAction, 
      public SequenceHdl<Geant4EventActionSequence>
    {
    public:
      Geant4UserRunAction* runAction;
      /// Standard constructor
      Geant4UserEventAction(Geant4Context* ctxt, Geant4EventActionSequence* seq)
        : Base(ctxt, seq), runAction(0)  {
        updateContext(ctxt);
        configureFiber(ctxt);
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
    class Geant4UserTrackingAction :
      public G4UserTrackingAction,
      public SequenceHdl<Geant4TrackingActionSequence>
    {
    public:
      /// Standard constructor
      Geant4UserTrackingAction(Geant4Context* ctxt, Geant4TrackingActionSequence* seq)
        : Base(ctxt, seq) {
        updateContext(ctxt);
        configureFiber(ctxt);
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

    /// Concrete implementation of the Geant4 stacking action sequence
    /** @class Geant4UserStackingAction
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4UserStackingAction :
      public G4UserStackingAction,
      public SequenceHdl<Geant4StackingActionSequence>
    {
    public:
      /// Standard constructor
      Geant4UserStackingAction(Geant4Context* ctxt, Geant4StackingActionSequence* seq)
        : Base(ctxt, seq) {
        updateContext(ctxt);
        configureFiber(ctxt);
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


    /// Concrete implementation of the Geant4 generator action
    /** @class Geant4UserGeneratorAction
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4UserGeneratorAction : 
      public G4VUserPrimaryGeneratorAction,
      public SequenceHdl<Geant4GeneratorActionSequence> 
    {
    public:
      /// Standard constructor
      Geant4UserGeneratorAction(Geant4Context* ctxt, Geant4GeneratorActionSequence* seq)
        : G4VUserPrimaryGeneratorAction(), Base(ctxt, seq) {
        updateContext(ctxt);
        configureFiber(ctxt);
      }
      /// Default destructor
      virtual ~Geant4UserGeneratorAction() {
      }
      /// Generate primary particles
      virtual void GeneratePrimaries(G4Event* event);
    };

    /// Concrete implementation of the Geant4 stepping action
    /** @class Geant4UserSteppingAction
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4UserSteppingAction :
      public G4UserSteppingAction,
      public SequenceHdl<Geant4SteppingActionSequence>
    {
    public:
      /// Standard constructor
      Geant4UserSteppingAction(Geant4Context* ctxt, Geant4SteppingActionSequence* seq)
        : Base(ctxt, seq) {
        updateContext(ctxt);
        configureFiber(ctxt);
      }
      /// Default destructor
      virtual ~Geant4UserSteppingAction() {
      }
      /// User stepping callback
      virtual void UserSteppingAction(const G4Step* s) {
        (*m_sequence)(s, fpSteppingManager);
      }
    };

    /// Concrete implementation of the Geant4 user detector construction action sequence
    /** @class Geant4UserDetectorConstruction
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4UserDetectorConstruction : 
      public G4VUserDetectorConstruction, 
      public SequenceHdl<Geant4DetectorConstructionSequence>
    {
    protected:
      Geant4DetectorConstructionContext m_ctxt;
    public:
      /// Standard constructor
      Geant4UserDetectorConstruction(Geant4Context* ctxt, Geant4DetectorConstructionSequence* seq)
        : G4VUserDetectorConstruction(), Base(ctxt, seq), 
          m_ctxt(ctxt->kernel().detectorDescription(), this) 
      {
      }
      /// Default destructor
      virtual ~Geant4UserDetectorConstruction() {
      }
      /// Call the actions for the construction of the sensitive detectors and the field
      virtual void ConstructSDandField();
      /// Call the actions to construct the detector geometry
      virtual G4VPhysicalVolume* Construct();
    };

    /// Concrete implementation of the Geant4 user initialization action sequence
    /** @class Geant4UserActionInitialization
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4UserActionInitialization : 
      public G4VUserActionInitialization, 
      public SequenceHdl<Geant4UserInitializationSequence>
    {
    public:
      /// Standard constructor
      Geant4UserActionInitialization(Geant4Context* ctxt, Geant4UserInitializationSequence* seq)
        : G4VUserActionInitialization(), Base(ctxt, seq) {
      }
      /// Default destructor
      virtual ~Geant4UserActionInitialization() {
      }
      /// Build the actions for the worker thread
      virtual void Build()  const;
      /// Build the action sequences for the master thread
      virtual void BuildForMaster()  const;
    };

    /// Begin-of-run callback
    void Geant4UserRunAction::BeginOfRunAction(const G4Run* run) {
      createClientContext(run);
      kernel().executePhase("begin-run",(const void**)&run);
      if ( m_sequence ) m_sequence->begin(run); // Action not mandatory
    }

    /// End-of-run callback
    void Geant4UserRunAction::EndOfRunAction(const G4Run* run) {
      if ( m_sequence ) m_sequence->end(run); // Action not mandatory
      kernel().executePhase("end-run",(const void**)&run);
      destroyClientContext(run);
    }

    /// Begin-of-event callback
    void Geant4UserEventAction::BeginOfEventAction(const G4Event* evt) {
      kernel().executePhase("begin-event",(const void**)&evt);
      if ( m_sequence ) m_sequence->begin(evt); // Action not mandatory
    }

    /// End-of-event callback
    void Geant4UserEventAction::EndOfEventAction(const G4Event* evt) {
      if ( m_sequence ) m_sequence->end(evt); // Action not mandatory
      kernel().executePhase("end-event",(const void**)&evt);
      destroyClientContext(evt);
    }

    /// Generate primary particles
    void Geant4UserGeneratorAction::GeneratePrimaries(G4Event* event) {
      createClientContext(event);
      if ( m_sequence )  {
        (*m_sequence)(event);
        return;
      }
      throw std::runtime_error("GeneratePrimaries: Panic! No action sequencer defined. "
                               "No primary particles can be produced.");
    }

    /// Instantiate Geant4 sensitive detectors and electro-magnetic field
    void Geant4UserDetectorConstruction::ConstructSDandField()  {
      G4AutoLock protection_lock(&action_mutex);
      Geant4Context* ctx = m_sequence->context();
      Geant4Kernel&  krnl = kernel().worker(Geant4Kernel::thread_self(),true);
      updateContext(krnl.workerContext());
      m_sequence->constructField(&m_ctxt);
      m_sequence->constructSensitives(&m_ctxt);
      updateContext(ctx);
    }

    /// Construct electro magnetic field entity from the dd4hep field
    G4VPhysicalVolume* Geant4UserDetectorConstruction::Construct()    {
      // The G4TransportationManager is thread-local. 
      // Thus, regardless of whether the field class object is global or local 
      // to a certain volume, a field object must be assigned to G4FieldManager.
      G4AutoLock protection_lock(&action_mutex);
      updateContext(m_sequence->context());
      m_sequence->constructGeo(&m_ctxt);
      if ( 0 == m_ctxt.world )    {
        m_sequence->except("+++ Executing G4 detector construction did not result in a valid world volume!");
      }
      return m_ctxt.world;
    }

    /// Build the actions for the worker thread
    void Geant4UserActionInitialization::Build()  const   {
      G4AutoLock protection_lock(&action_mutex);
      Geant4Kernel&  krnl = kernel().worker(Geant4Kernel::thread_self(),true);
      Geant4Context* ctx  = krnl.workerContext();

      if ( m_sequence )  {
        Geant4Context* old = m_sequence->context();
        m_sequence->info("+++ Executing Geant4UserActionInitialization::Build. "
                         "Context:%p Kernel:%p [%ld]", (void*)ctx, (void*)&krnl, krnl.id());
      
        m_sequence->updateContext(ctx);
        m_sequence->build();
        m_sequence->updateContext(old);
      }
      // Set user generator action sequence. Not optional, since event context is defined inside
      Geant4UserGeneratorAction* gen_action = new Geant4UserGeneratorAction(ctx,krnl.generatorAction(false));
      SetUserAction(gen_action);

      // Set the run action sequence. Not optional, since run context is defined/destroyed inside
      Geant4UserRunAction* run_action = new Geant4UserRunAction(ctx,krnl.runAction(false));
      SetUserAction(run_action);

      // Set the event action sequence. Not optional, since event context is destroyed inside
      Geant4UserEventAction* evt_action = new Geant4UserEventAction(ctx,krnl.eventAction(false));
      run_action->eventAction = evt_action;
      evt_action->runAction = run_action;
      SetUserAction(evt_action);

      // Set the tracking action sequence
      Geant4TrackingActionSequence* trk_action = krnl.trackingAction(false);
      if ( trk_action ) {
        Geant4UserTrackingAction* action = new Geant4UserTrackingAction(ctx, trk_action);
        SetUserAction(action);
      }
      // Set the stepping action sequence
      Geant4SteppingActionSequence* stp_action = krnl.steppingAction(false);
      if ( stp_action ) {
        Geant4UserSteppingAction* action = new Geant4UserSteppingAction(ctx, stp_action);
        SetUserAction(action);
      }
      // Set the stacking action sequence
      Geant4StackingActionSequence* stk_action = krnl.stackingAction(false);
      if ( stk_action ) {
        Geant4UserStackingAction* action = new Geant4UserStackingAction(ctx, stk_action);
        SetUserAction(action);
      }
      
    }

    /// Build the action sequences for the master thread
    void Geant4UserActionInitialization::BuildForMaster()  const  {
      if ( m_sequence )   {
        m_sequence->info("+++ Executing Geant4UserActionInitialization::BuildForMaster....");
        m_sequence->buildMaster();
      }
    }
  }
}

#include "DD4hep/Detector.h"
#include "DD4hep/Plugins.h"
#include "DDG4/Geant4DetectorConstruction.h"
#include "DDG4/Geant4Kernel.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::sim;

// Geant4 include files
#include "G4RunManager.hh"
#include "G4PhysListFactory.hh"

class Geant4Compatibility {
public:
  Geant4Compatibility() {}
  Geant4DetectorConstructionSequence* buildDefaultDetectorConstruction(Geant4Kernel& kernel);
};
Geant4DetectorConstructionSequence* Geant4Compatibility::buildDefaultDetectorConstruction(Geant4Kernel& kernel)  {
  Geant4Action* cr;
  Geant4DetectorConstruction* det_cr;
  Geant4Context* ctx = kernel.workerContext();
  Geant4DetectorConstructionSequence* seq = kernel.detectorConstruction(true);
  printout(WARNING, "Geant4Exec", "+++ Building default Geant4DetectorConstruction for single threaded compatibility.");

  // Attach first the geometry converter from dd4hep to Geant4
  cr = PluginService::Create<Geant4Action*>("Geant4DetectorGeometryConstruction",ctx,string("ConstructGeometry"));
  det_cr = dynamic_cast<Geant4DetectorConstruction*>(cr);
  if ( det_cr ) 
    seq->adopt(det_cr);
  else
    throw runtime_error("Panic! Failed to build Geant4DetectorGeometryConstruction.");
  // Attach the sensitive detector manipulator:
  cr = PluginService::Create<Geant4Action*>("Geant4DetectorSensitivesConstruction",ctx,string("ConstructSensitives"));
  det_cr = dynamic_cast<Geant4DetectorConstruction*>(cr);
  if ( det_cr )
    seq->adopt(det_cr);
  else
    throw runtime_error("Panic! Failed to build Geant4DetectorSensitivesConstruction.");
  return seq;
}

/// Configure the simulation
int Geant4Exec::configure(Geant4Kernel& kernel) {
  Detector& description = kernel.detectorDescription();
  Geant4Context* ctx = kernel.workerContext();
  Geant4Random* rndm = Geant4Random::instance(false);
  
  if ( !rndm )  {
    rndm = new Geant4Random(ctx, "Geant4Random");
    /// Initialize the engine etc.
    rndm->initialize();
  }
  Geant4Random::setMainInstance(rndm);
  kernel.executePhase("configure",0);

  // Construct the default run manager
  G4RunManager& runManager = kernel.runManager();

  // Check if the geometry was loaded
  if (description.sensitiveDetectors().size() <= 1) {
    printout(WARNING, "Geant4Exec", "+++ Only %d subdetectors present. "
             "You sure you loaded the geometry properly?",
             int(description.sensitiveDetectors().size()));
  }

  // Get the detector constructed
  Geant4DetectorConstructionSequence* user_det = kernel.detectorConstruction(false);
  if ( 0 == user_det && kernel.isMultiThreaded() )   {
    throw runtime_error("Panic! No valid detector construction sequencer present. [Mandatory MT]");
  }
  if ( 0 == user_det && !kernel.isMultiThreaded() )   {
    user_det = Geant4Compatibility().buildDefaultDetectorConstruction(kernel);
  }
  Geant4UserDetectorConstruction* det_seq = new Geant4UserDetectorConstruction(ctx,user_det);
  runManager.SetUserInitialization(det_seq);

  // Get the physics list constructed
  Geant4PhysicsListActionSequence* phys_seq = kernel.physicsList(false);
  if ( 0 == phys_seq )   {
    string phys_model = "QGSP_BERT";
    phys_seq = kernel.physicsList(true);
    phys_seq->property("extends").set(phys_model);
  }
  G4VUserPhysicsList* physics = phys_seq->extensionList();
  if (0 == physics) {
    throw runtime_error("Panic! No valid user physics list present!");
  }
#if 0
  /// Not here: Use seperate object to do this!
  printout(INFO, "Geant4Exec", "+++ PhysicsList RangeCut: %f", phys_seq->m_rangecut );
  physics->SetDefaultCutValue(phys_seq->m_rangecut);
  physics->SetCuts();
  if( DEBUG == printLevel() ) physics->DumpCutValuesTable();
#endif
  runManager.SetUserInitialization(physics);

  // Construct the remaining user initialization in multi-threaded mode
  Geant4UserInitializationSequence* user_init = kernel.userInitialization(false);
  if ( 0 == user_init && kernel.isMultiThreaded() )   {
    throw runtime_error("Panic! No valid user initialization sequencer present. [Mandatory MT]");
  }
  else if ( 0 == user_init && !kernel.isMultiThreaded() )  {
    // Use default actions registered to the default kernel. Will do the right thing...
    user_init = kernel.userInitialization(true);
  }
  Geant4UserActionInitialization* init = new Geant4UserActionInitialization(ctx,user_init);
  runManager.SetUserInitialization(init);
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
  return 1;
}
