// $Id: Geant4Field.cpp 875 2013-11-04 16:15:14Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
// Framework include files
#include "DDG4/Geant4Field.h"

using namespace DD4hep::Simulation;
using namespace DD4hep::Geometry;
using namespace DD4hep;
using namespace std;

#if 0
#ifdef G4VIS_USE_OPENGLX
#include "G4OpenGLImmediateX.hh"
#include "G4OpenGLStoredX.hh"
#endif

void startX() {
  // Initialize visualization
  G4VisManager* visManager = 0;
  if ( is_visual ) {
    visManager = new G4VisExecutive();
#ifdef G4VIS_USE_OPENGLX
    //visManager->RegisterGraphicsSystem (new G4OpenGLImmediateX());
    //visManager->RegisterGraphicsSystem (new G4OpenGLStoredX());
#endif
    visManager->Initialize();
  }
}
#endif

#if 0
Geant4UIManager::Geant4UIManager()
{
  declareProperty("");
}

Geant4UIManager::operator() {
  // Get the pointer to the User Interface manager
  G4UImanager* mgr = G4UImanager::GetUIpointer();

  if ( !gui_type.empty() ) {   // interactive mode : define UI session
    G4UIExecutive* ui = 0;
    const char* args[] = {"cmd"};
    ui = new G4UIExecutive(1,(char**)args);
    if ( is_visual && !vis_setup.empty() ) {
      mgr->ApplyCommand("/control/execute vis.mac");
      cout << "++++++++++++++++++++++++++++ executed vis.mac" << endl;
    }

    if (ui->IsGUI()) {
      if ( !gui_setup.empty() ) {
        mgr->ApplyCommand("/control/execute "+gui_setup);
        cout << "++++++++++++++++++++++++++++ executed gui.mac" << endl;
      }
    }
    ui->SessionStart();
    delete ui;
  }
#endif
