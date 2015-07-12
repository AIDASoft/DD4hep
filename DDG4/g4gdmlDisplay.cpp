// $Id: $
//==========================================================================
//  AIDA Detector description implementation for LCD
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
//#define G4UI_USE
//#define G4VIS_USE
//#define G4INTY_USE_XT
//#define G4VIS_USE_OPENGL
//#define G4UI_USE_TCSH

#include "G4PVPlacement.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIsession.hh"
#include "Randomize.hh"

#ifdef G4VIS_USE_OPENGLX
#include "G4OpenGLImmediateX.hh"
#include "G4OpenGLStoredX.hh"
#endif

#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4VUserPhysicsList.hh"
#include "G4ParticleTypes.hh"
#include "globals.hh"

#include "DDG4/Geant4GDMLDetector.h"
#include <cerrno>
#include <stdexcept>

using namespace std;
using namespace DD4hep::Simulation;

namespace {
  class EmptyPhysicsList: public G4VUserPhysicsList  {
  public:
    EmptyPhysicsList()       {                                       }
    ~EmptyPhysicsList()      {                                       }
    // Construct particle and physics process
    void ConstructParticle() {    G4Geantino::GeantinoDefinition();  }
    void ConstructProcess()  {    AddTransportation();               }
    void SetCuts()           {    SetCutsWithDefault();              }
  };
}

static const char* get_arg(int argc, char**  argv,int which)  {
  if ( which>0 && which < argc ) return argv[which];
  throw runtime_error("Invalid argument sequence");
}

int main(int argc, char** argv)   {
  string gdml = argv[1];
  string setup = argv[2];
  const char* args[] = {"cmd"};
  for(int i=1; i<argc;++i)   {
    string nam = get_arg(argc,argv,i);
    if ( argv[i][0]=='-' )     {
      string n = argv[i]+1;
      if ( ::strncmp(n.c_str(),"gdml",4) == 0 )
        gdml = get_arg(argc,argv,++i);
      else if ( ::strncmp(n.c_str(),"guisetup",3) == 0 )
        setup = get_arg(argc,argv,++i);
    }
  }
  if ( gdml.empty() || setup.empty() )  {
    cout << " usage: g4gdmlDisplay -gdml <file-name> -guisetup <g4 macro>" << endl;
    return EINVAL;
  }

  G4RunManager * run = new G4RunManager;
  run->SetUserInitialization(new Geant4GDMLDetector(gdml));
  run->SetUserInitialization(new EmptyPhysicsList());
  //
  // Initialize G4 kernel
  run->Initialize();
  //
  // Initialize visualization
  G4VisManager* vis = new G4VisExecutive;
  vis->Initialize();
  //
  // Get the pointer to the User Interface manager
  G4UImanager* uiman = G4UImanager::GetUIpointer();
  G4UIExecutive* ui = new G4UIExecutive(1,(char**)args);
  uiman->ApplyCommand("/control/execute "+setup);
  ui->SessionStart();
  // end ...
  delete ui;
  delete vis;
  delete run;
  return 0;
}
