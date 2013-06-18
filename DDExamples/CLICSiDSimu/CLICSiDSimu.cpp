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

#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "QGSP_BERT.hh"

#include "PrimaryGeneratorAction.h"
#include "RunAction.h"
#include "EventAction.h"
#include "SteppingAction.h"
#include "SteppingVerbose.h"


#include "DD4hep/LCDD.h"
#include "DDG4/Geant4DetectorConstruction.h"
#include <vector>
#include <algorithm>

using namespace std;
using namespace DD4hep::Geometry;
using namespace DD4hep::Simulation;


int main(int argc,char** argv)   {
  // Choose the Random engine
  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
  LCDD& lcdd = LCDD::getInstance();
  bool is_gui = false;
  bool is_visual = false;
  string gui_setup, vis_setup;
  vector<string>        macros;
  vector<pair<string,string> > plugins;
  for(int i=1; i<argc;++i)   {
    if ( argv[i][0]=='-' )     {
      string plug, nam = argv[i]+1;
      if ( strncmp(nam.c_str(),"gui_setup",5) == 0 )
	gui_setup = argv[++i];
      else if ( strncmp(nam.c_str(),"vis_setup",5) == 0 )
	vis_setup = argv[++i];
      // No else here!
      if ( strncmp(nam.c_str(),"gui",3) == 0 )
	is_gui = true;
      else if ( strncmp(nam.c_str(),"vis",3) == 0 ) 
	is_visual = true;
      else if ( strncmp(nam.c_str(),"macro",3) == 0 ) 
	macros.push_back(argv[++i]);
      else  {
	plug = nam;
	size_t idx = plug.find(':');
	if ( idx != string::npos && ::strncmp(plug.c_str(),"plug",4) == 0 ) 
	  plug = nam.substr(idx+1);
	else if ( ::strncmp(plug.c_str(),"compact",5) == 0 )
	  plug = "DD4hepXMLLoader";
	else if (  ::strncmp(plug.c_str(),"geant4XML",9) == 0 )
	  plug = "DD4hepXMLLoader";
	else if (  ::strncmp(plug.c_str(),"xml",3) == 0 )
	  plug = "DD4hepXMLLoader";
	nam = argv[++i];
	plugins.push_back(make_pair(plug,nam));
      }
    }
  }

  for(size_t j=0; j < plugins.size(); ++j)  {
    const pair<string,string>& p = plugins[j];
    // We need to construct the geometry at this level already
    char* arg = (char*)p.second.c_str();
    lcdd.apply(p.first.c_str(),1,&arg);
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
  G4VisManager* visManager = 0;
  if ( is_visual )  {
    visManager = new G4VisExecutive;
    visManager->Initialize();
  }

  // Get the pointer to the User Interface manager  
  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  for(size_t j=0; j<macros.size(); ++j)  {
    G4String command = "/control/execute "+macros[j];
    cout << "++++++++++++++++++++++++++++ executing command:" << command << endl;
    UImanager->ApplyCommand(command);
  }
  G4UIExecutive* ui = 0;
  if ( is_gui ) {  // interactive mode : define UI session    
    ui = new G4UIExecutive(argc,argv);
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
  delete runManager;  
  return 0;
}
