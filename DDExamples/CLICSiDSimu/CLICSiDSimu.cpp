//#define G4UI_USE
//#define G4VIS_USE
//#define G4INTY_USE_XT
//#define G4VIS_USE_OPENGL
//#define G4UI_USE_TCSH
//#define G4VIS_USE_OPENGLX

#include "G4PVPlacement.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIsession.hh"
#include "Randomize.hh"

#ifdef G4VIS_USE
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#endif
#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

#include "QGSP_BERT.hh"

#include "PrimaryGeneratorAction.h"
#include "RunAction.h"
#include "EventAction.h"
#include "SteppingAction.h"
#include "SteppingVerbose.h"


#include "DD4hep/LCDD.h"
#include "DDG4/Geant4DetectorConstruction.h"


using namespace std;
using namespace DD4hep::Geometry;
using namespace DD4hep::Simulation;


int main(int argc,char** argv)   {
  // Choose the Random engine
  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
  LCDD& lcdd = LCDD::getInstance();
  
  for(int i=1; i<argc-1;++i) {
    // We need to construct the geometry at this level already
    lcdd.fromCompact(argv[i]);
  }

  // User Verbose output class
  G4VSteppingVerbose::SetInstance(new SteppingVerbose);
  
  // Construct the default run manager
  G4RunManager* runManager = new G4RunManager;

  // Get the detector constructed
  Geant4DetectorConstruction* detector = new Geant4DetectorConstruction(lcdd);

  runManager->SetUserInitialization(detector);
  
  //
  G4VUserPhysicsList* physics = new QGSP_BERT(); // physics from N04
  runManager->SetUserInitialization(physics);
  
  // Set user action classes
  G4VUserPrimaryGeneratorAction* gen_action = new PrimaryGeneratorAction(lcdd);
  runManager->SetUserAction(gen_action);
  //
  RunAction* run_action = new RunAction;  
  runManager->SetUserAction(run_action);
  //
  EventAction* event_action = new EventAction(run_action);
  runManager->SetUserAction(event_action);

  //
  G4UserSteppingAction* stepping_action = new SteppingAction(event_action);
  runManager->SetUserAction(stepping_action);
  
  // Initialize G4 kernel
  //
  runManager->Initialize();
  
  // Initialize visualization
#ifdef G4VIS_USE
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();
#endif

  // Get the pointer to the User Interface manager  
  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  if ( argc > 2 ) {   // batch mode
    G4String command = "/control/execute ";
    G4String fileName = argv[argc-1];
    UImanager->ApplyCommand(command+fileName);    
  }
  else {  // interactive mode : define UI session    
#ifdef G4UI_USE
    G4UIExecutive* ui = new G4UIExecutive(argc-1, &argv[1]);
    if (ui->IsGUI()) {
      UImanager->ApplyCommand("/control/execute gui.mac");
      cout << "++++++++++++++++++++++++++++ executed gui.mac" << endl;
    }
#ifdef G4VIS_USE
      UImanager->ApplyCommand("/control/execute vis.mac"); 
      cout << "++++++++++++++++++++++++++++ executed vis.mac" << endl;
#endif
    ui->SessionStart();
    delete ui;
#endif
  }
  
  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  //                 owned and deleted by the run manager, so they should not
  //                 be deleted in the main() program !
  
#ifdef G4VIS_USE
  delete visManager;
#endif
  delete runManager;  
  return 0;
}
