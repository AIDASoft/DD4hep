//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id: exampleN04.cc,v 1.19 2010-06-04 17:50:22 vnivanch Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4RunManager.hh"
#include "G4UImanager.hh"

#include "ExN04DetectorConstruction.hh"
#include "QGSP_BERT.hh"
#include "ExN04PrimaryGeneratorAction.hh"
#include "ExN04RunAction.hh"
#include "ExN04EventAction.hh"
#include "ExN04StackingAction.hh"
#include "ExN04TrackingAction.hh"
#include "ExN04SteppingAction.hh"
#include "ExN04SteppingVerbose.hh"

#include "DDG4/Geant4DetectorConstruction.h"
#include "DD4hep/LCDD.h"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

int main(int argc,char** argv)
{
  // User Verbose output class
  //
  G4VSteppingVerbose* verbosity = new ExN04SteppingVerbose;
  G4VSteppingVerbose::SetInstance(verbosity);
  
  // Run manager
  //
  G4RunManager* runManager = new G4RunManager;
  DD4hep::Geometry::LCDD& lcdd = DD4hep::Geometry::LCDD::getInstance();

  if( argc < 3 ){
    std::cout << " --- Usage: \n " 
	      << "  [1] exampleN04      file:../compact/N04.xml file:../compact/geant4.xml ../exampleN04.in \n"
	      << "  [2] exampleN04  -i  file:../compact/N04.xml file:../compact/geant4.xml \n"
	      << "  [1]: batch mode - [2]: interactive " << std::endl ;
    exit( 0 ) ;
  }



//**************************************************************

  bool isBatchMode = ( std::string( argv[1] ) != "-i" )  ; 

  int argStart = ( isBatchMode ?       1 : 2 ) ; 
  int argEnd   = ( isBatchMode ?  argc-1 : argc ) ; 

  for(int i=argStart; i < argEnd ; ++i ) {
    
    // We need to construct the geometry at this level already
    lcdd.fromCompact(argv[i]);
  }

  //**************************************************************

  // Get the detector constructed

  // User Initialization classes (mandatory)
  //
#if 0 // use original EXN04 detector construction
  G4VUserDetectorConstruction* detector = new ExN04DetectorConstruction;
#else // use DD4Hep
  G4VUserDetectorConstruction* detector  =  new DD4hep::Simulation::Geant4DetectorConstruction( lcdd )   ;
#endif

  runManager->SetUserInitialization(detector);
  //
  G4VUserPhysicsList* physics = new QGSP_BERT();
  runManager->SetUserInitialization(physics);

  runManager->Initialize();

  // User Action classes
  //
  G4VUserPrimaryGeneratorAction* gen_action = new ExN04PrimaryGeneratorAction;
  runManager->SetUserAction(gen_action);
  //
  G4UserRunAction* run_action = new ExN04RunAction;
  runManager->SetUserAction(run_action);
  //
  G4UserEventAction* event_action = new ExN04EventAction;
  runManager->SetUserAction(event_action);
  //
  G4UserStackingAction* stacking_action = new ExN04StackingAction;
  runManager->SetUserAction(stacking_action);
  //
  G4UserTrackingAction* tracking_action = new ExN04TrackingAction;
  runManager->SetUserAction(tracking_action);
  //
  G4UserSteppingAction* stepping_action = new ExN04SteppingAction;
  runManager->SetUserAction(stepping_action);
  
#ifdef G4VIS_USE
      G4VisManager* visManager = new G4VisExecutive;
      visManager->Initialize();
#endif    
     
  //get the pointer to the User Interface manager   
  G4UImanager* UImanager = G4UImanager::GetUIpointer();  

  if (isBatchMode)   // batch mode  
    {
      G4String command = "/control/execute ";
      G4String fileName = argv[argc-1];
      UImanager->ApplyCommand(command+fileName);
    }
  else           // interactive mode : define UI session
    { 
#ifdef G4UI_USE
      G4UIExecutive * ui = new G4UIExecutive(argc,argv);
#ifdef G4VIS_USE
      UImanager->ApplyCommand("/control/execute vis.mac");     
#endif
      ui->SessionStart();
      delete ui;
#endif
     
#ifdef G4VIS_USE
      delete visManager;
#endif     
    }
    
  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  //                 owned and deleted by the run manager, so they should not
  //                 be deleted in the main() program !
  delete runManager;
  delete verbosity;

  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
