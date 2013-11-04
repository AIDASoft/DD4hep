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
#include "G4VSensitiveDetector.hh"

#include "DD4hep/Handle.h"
#include "DDG4/Geant4RunAction.h"
#include "DDG4/Geant4EventAction.h"
#include "DDG4/Geant4SteppingAction.h"
#include "DDG4/Geant4TrackingAction.h"
#include "DDG4/Geant4StackingAction.h"
#include "DDG4/Geant4GeneratorAction.h"
#include "DDG4/Geant4PhysicsList.h"
#include "DDG4/Geant4SensDetAction.h"
#include "DDG4/Geant4HitCollection.h"
#include "DDG4/Geant4Kernel.h"

#include <memory>
/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation   {

    template <typename T> struct _Seq  {
      typedef _Seq<T> Base;
      T* m_sequence;
      _Seq() : m_sequence(0) {                 }
      _Seq(T* seq)           {   _aquire(seq); }
      virtual ~_Seq()        { _release();     }
      void _aquire(T* s)  {
	InstanceCount::increment(this);
	m_sequence = s;
	m_sequence->addRef();
      }
      void _release()  {
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
    struct Geant4UserRunAction : public G4UserRunAction, public _Seq<Geant4RunActionSequence>    {
      /// Standard constructor
      Geant4UserRunAction(Geant4RunActionSequence* seq) : Base(seq)  {                    }
      /// Default destructor
      virtual ~Geant4UserRunAction()                           {                          }
      /// begin-of-run callback
      virtual void  BeginOfRunAction(const G4Run* run)         { m_sequence->begin(run);  }
      /// End-of-run callback
      virtual void  EndOfRunAction(const G4Run* run)           { m_sequence->end(run);    }
    };

    /** @class Geant4UserEventAction
     * 
     * Concrete implementation of the Geant4 event action
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct Geant4UserEventAction : public G4UserEventAction, public _Seq<Geant4EventActionSequence> {
      /// Standard constructor
      Geant4UserEventAction(Geant4EventActionSequence* seq) : Base(seq) {                 }
      /// Default destructor
      virtual ~Geant4UserEventAction()                         {                          }
      /// begin-of-event callback
      virtual void  BeginOfEventAction(const G4Event* evt)     { m_sequence->begin(evt);  }
      /// End-of-event callback
      virtual void  EndOfEventAction(const G4Event* evt)       { m_sequence->end(evt);    }
    };

    /** @class Geant4UserTrackingAction
     * 
     * Concrete implementation of the Geant4 tracking action
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct Geant4UserTrackingAction : public G4UserTrackingAction, public _Seq<Geant4TrackingActionSequence>  {
      /// Standard constructor
      Geant4UserTrackingAction(Geant4TrackingActionSequence* seq) : Base(seq)   {         }
      /// Default destructor
      virtual ~Geant4UserTrackingAction()                      {                          }
      /// Pre-track action callback
      virtual void  PreUserTrackingAction(const G4Track* trk)  { m_sequence->begin(trk);  }
      /// Post-track action callback
      virtual void  PostUserTrackingAction(const G4Track* trk) { m_sequence->end(trk);    }
    };

    /** @class Geant4UserStackingAction
     * 
     * Concrete implementation of the Geant4 stacking action sequence
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct Geant4UserStackingAction : public G4UserStackingAction, public _Seq<Geant4StackingActionSequence>  {
      /// Standard constructor
      Geant4UserStackingAction(Geant4StackingActionSequence* seq) : Base(seq)   {         }
      /// Default destructor
      virtual ~Geant4UserStackingAction()                      {                          }
      /// New-stage callback
      virtual void  NewStage()                                 { m_sequence->newStage();  }
      /// Preparation callback
      virtual void  PrepareNewEvent()                          { m_sequence->prepare();   }
    };

    /** @class Geant4UserGeneratorAction
     * 
     * Concrete implementation of the Geant4 generator action
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct Geant4UserGeneratorAction : public G4VUserPrimaryGeneratorAction, public _Seq<Geant4GeneratorActionSequence>    {
      /// Standard constructor
      Geant4UserGeneratorAction(Geant4GeneratorActionSequence* seq)
	: G4VUserPrimaryGeneratorAction(), Base(seq)           {                         }
      /// Default destructor
      virtual ~Geant4UserGeneratorAction()                     {                         }
      /// Generate primary particles
      virtual void GeneratePrimaries(G4Event* event)           { (*m_sequence)(event);   }
    };

    /** @class Geant4UserSteppingAction
     * 
     * Concrete implementation of the Geant4 stepping action
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct Geant4UserSteppingAction : public G4UserSteppingAction, public _Seq<Geant4SteppingActionSequence>  {
      /// Standard constructor
      Geant4UserSteppingAction(Geant4SteppingActionSequence* seq) : Base(seq)   {        }
      /// Default destructor
      virtual ~Geant4UserSteppingAction()                      {                         }
      /// User stepping callback
      virtual void UserSteppingAction(const G4Step* s) { (*m_sequence)(s,fpSteppingManager); }
    };

    /** @class Geant4UserPhysicsList
     * 
     * Concrete implementation of the Geant4 user physics list
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct Geant4UserPhysicsList : virtual public G4VModularPhysicsList,
				   virtual public Geant4UserPhysics,
				   virtual public _Seq<Geant4PhysicsListActionSequence>
    {
      /// Standard constructor
      Geant4UserPhysicsList(Geant4PhysicsListActionSequence* seq)
	: G4VModularPhysicsList(), Geant4UserPhysics(), Base(seq)   {                          }
      /// Default destructor
      virtual ~Geant4UserPhysicsList()   {                                                  }
      /// User construction callback
      virtual void ConstructProcess()    {  m_sequence->constructProcess(this);             }
      /// User construction callback
      virtual void ConstructParticle()   {  m_sequence->constructParticles(this);            }
      /// Enable transportation
      virtual void AddTransportation()   {  this->G4VUserPhysicsList::AddTransportation();  }
      // Register Physics Constructor 
      virtual void RegisterPhysics(G4VPhysicsConstructor* physics)
      { this->G4VModularPhysicsList::RegisterPhysics(physics);   }
    };

    struct Geant4SensDet : virtual public G4VSensitiveDetector, 
			   virtual public G4VSDFilter,
			   virtual public Geant4ActionSD,
			   virtual public _Seq<Geant4SensDetActionSequence>
    {
      /// Constructor. The detector element is identified by the name
      Geant4SensDet(const std::string& nam, Geometry::LCDD& lcdd)
	: G4VSensitiveDetector(nam), G4VSDFilter(nam), 
	  Geant4Action(0,nam), Geant4ActionSD(nam), Base()
      {
	Geant4Kernel& kernel = Geant4Kernel::access(lcdd);
	setContext(kernel.context());
	_aquire(kernel.sensitiveAction(nam));
	m_sequence->defineCollections(this);
	this->G4VSensitiveDetector::SetFilter(this);
      }

      /// Destructor
      virtual ~Geant4SensDet()        {                                 }
      /// Overload to avoid ambiguity between G4VSensitiveDetector and G4VSDFilter
      inline G4String GetName() const
      {  return this->G4VSensitiveDetector::SensitiveDetectorName;      }
      /// G4VSensitiveDetector internals: Access to the detector path name
      virtual std::string path()  const
      {  return this->G4VSensitiveDetector::GetPathName();              }
      /// G4VSensitiveDetector internals: Access to the detector path name
      virtual std::string fullPath()  const
      {  return this->G4VSensitiveDetector::GetFullPathName();          }
      /// Is the detector active?
      virtual bool isActive() const
      {  return this->G4VSensitiveDetector::isActive();                 }
      /// This is a utility method which returns the hits collection ID 
      virtual G4int GetCollectionID(G4int i)
      {  return this->G4VSensitiveDetector::GetCollectionID(i);         }
      /// Access to the readout geometry of the sensitive detector
      virtual G4VReadOutGeometry* readoutGeometry() const
      {  return this->G4VSensitiveDetector::GetROgeometry();            }
      /// Callback if the sequence should be accepted or filtered off
      virtual G4bool Accept(const G4Step* step) const
      {  return m_sequence->accept(step);                               }
      /// Method invoked at the begining of each event. 
      virtual void Initialize(G4HCofThisEvent* hce)
      {  m_sequence->begin(hce);                                        }
      /// Method invoked at the end of each event. 
      virtual void EndOfEvent(G4HCofThisEvent* hce)
      {  m_sequence->end(hce);                                          }
      /// Method for generating hit(s) using the information of G4Step object.
      virtual G4bool ProcessHits(G4Step* step,G4TouchableHistory* hist)
      {  return m_sequence->process(step,hist);                         }
      /// G4VSensitiveDetector interface: Method invoked if the event was aborted.
      virtual void clear()
      {  m_sequence->clear();                                           }
      /// Initialize the usage of a hit collection. Returns the collection identifier
      virtual size_t defineCollection(const std::string& coll)  {
	if ( coll.empty() ) {
	  except("Geant4Sensitive: No collection defined for %s [Invalid name]",c_name());
	}
	collectionName.push_back(coll);
	return collectionName.size()-1;
      }

    };
  }    // End namespace Simulation
}      // End namespace DD4hep


#include "DDG4/Factories.h"

typedef DD4hep::Simulation::Geant4SensDet Geant4SensDet;
typedef DD4hep::Simulation::Geant4SensDet Geant4tracker;
typedef DD4hep::Simulation::Geant4SensDet Geant4calorimeter;

DECLARE_GEANT4SENSITIVEDETECTOR(Geant4SensDet)
DECLARE_GEANT4SENSITIVEDETECTOR(Geant4tracker)
DECLARE_GEANT4SENSITIVEDETECTOR(Geant4calorimeter)


#include "DD4hep/LCDD.h"
#include "DDG4/Geant4DetectorConstruction.h"

#include "QGSP_BERT.hh"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;

// Geant4 include files
#include "G4RunManager.hh"

//--
#define G4VIS_USE_OPENGL
#include "G4UImanager.hh"
#include "G4UIsession.hh"
#ifdef G4VIS_USE_OPENGLX
#include "G4OpenGLImmediateX.hh"
#include "G4OpenGLStoredX.hh"
#endif
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"


/// Configure the simulation
int Geant4Exec::configure(Geant4Kernel& kernel)   {
  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
  Geometry::LCDD& lcdd = kernel.lcdd();

  // Construct the default run manager
  G4RunManager& runManager = kernel.runManager();

  // Check if the geometry was loaded
  if ( lcdd.detectors().size() <= 2 )  {
    cout << "Error, no geometry." << endl;
  }
  // Get the detector constructed
  Geant4DetectorConstruction* detector = new Geant4DetectorConstruction(lcdd);

  runManager.SetUserInitialization(detector);
  G4VUserPhysicsList* physics = 0;
  if ( kernel.physicsList(false) )
    physics = new Geant4UserPhysicsList(kernel.physicsList(false));
  else  {
    printout(INFO,"Geant4Exec","+++ Using Geant4 physics constructor QGSP_BERT");
    physics = new QGSP_BERT(); // physics from N04
  }
  runManager.SetUserInitialization(physics);
  
  // Set user generator action sequence
  if ( kernel.generatorAction(false) )  {
    Geant4UserGeneratorAction* action = new Geant4UserGeneratorAction(kernel.generatorAction(false));
    runManager.SetUserAction(action);
  }
  // Set the run action sequence
  if ( kernel.runAction(false) )  {
    Geant4UserRunAction* action = new Geant4UserRunAction(kernel.runAction(false));
    runManager.SetUserAction(action);
  }
  // Set the event action sequence
  if ( kernel.eventAction(false) )  {
    Geant4UserEventAction* action = new Geant4UserEventAction(kernel.eventAction(false));
    runManager.SetUserAction(action);
  }
  // Set the stepping action sequence
  if ( kernel.steppingAction(false) )  {
    Geant4UserSteppingAction* action = new Geant4UserSteppingAction(kernel.steppingAction(false));
    runManager.SetUserAction(action);
  }
  // Set the stacking action sequence
  if ( kernel.stackingAction(false) )  {
    Geant4UserStackingAction* action = new Geant4UserStackingAction(kernel.stackingAction(false));
    runManager.SetUserAction(action);
  }
  return 1;
}

/// Initialize the simulation
int Geant4Exec::initialize(Geant4Kernel& kernel)   {
  // Construct the default run manager
  G4RunManager& runManager = kernel.runManager();
  //
  // Initialize G4 engine
  //
  runManager.Initialize();
  return 1;
}

/// Run the simulation
int Geant4Exec::run(Geant4Kernel& )   {
  bool is_visual = false;
  string gui_setup, vis_setup, gui_type="tcsh";

  // Initialize visualization
  G4VisManager* visManager = 0;
  if ( is_visual )  {
    visManager = new G4VisExecutive();
#ifdef G4VIS_USE_OPENGLX
    //visManager->RegisterGraphicsSystem (new G4OpenGLImmediateX());
    //visManager->RegisterGraphicsSystem (new G4OpenGLStoredX());
#endif
    visManager->Initialize();
  }

  // Get the pointer to the User Interface manager  
  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  G4String command = "/control/execute run.mac";
  cout << "++++++++++++++++++++++++++++ executing command:" << command << endl;
  UImanager->ApplyCommand(command);

  G4UIExecutive* ui = 0;
  if ( !gui_type.empty() ) {  // interactive mode : define UI session    
    const char* args[] = {"cmd"};
    ui = new G4UIExecutive(1,(char**)args);//,gui_type);
    if ( is_visual && !vis_setup.empty() )   {
      UImanager->ApplyCommand("/control/execute vis.mac"); 
      cout << "++++++++++++++++++++++++++++ executed vis.mac" << endl;
    }
    if (ui->IsGUI()) {
      if ( !gui_setup.empty() )  {
	UImanager->ApplyCommand("/control/execute "+gui_setup);
	cout << "++++++++++++++++++++++++++++ executed gui.mac" << endl;
      }
    }
    ui->SessionStart();
    delete ui;
  }
  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  //                 owned and deleted by the run manager, so they should not
  //                 be deleted in the main() program !  
  if ( visManager ) delete visManager;
  return 1;
}

/// Run the simulation
int Geant4Exec::terminate(Geant4Kernel& )   {
  return 1;
}
