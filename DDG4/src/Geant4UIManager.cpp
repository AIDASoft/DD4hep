// $Id: Geant4Field.cpp 875 2013-11-04 16:15:14Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DDG4/Geant4UIManager.h"
#include "DDG4/Geant4Kernel.h"
#include "DD4hep/Primitives.h"
#include "DD4hep/Printout.h"
#include "G4VisExecutive.hh"
#include "G4UImanager.hh"
#include "G4UIsession.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4RunManager.hh"


using namespace DD4hep::Simulation;
using namespace std;

/// Initializing constructor
Geant4UIManager::Geant4UIManager(Geant4Context* context, const std::string& name)
  : Geant4Action(context,name), m_vis(0), m_ui(0)
{
  declareProperty("SetupUI", m_uiSetup="");
  declareProperty("SetupVIS", m_visSetup="");
  declareProperty("SessionType", m_sessionType="tcsh");
  declareProperty("HaveVIS", m_haveVis=false);
  declareProperty("HaveUI", m_haveUI=true);
}

/// Default destructor
Geant4UIManager::~Geant4UIManager()   {
}

/// Start visualization
G4VisManager* Geant4UIManager::startVis()  {
  // Initialize visualization
  printout(INFO,"Geant4UIManager","+++ Starting G4VisExecutive ....");
  G4VisManager* vis = new G4VisExecutive();
  vis->Initialize();
  return vis;
}

/// Start UI
G4UIExecutive* Geant4UIManager::startUI()   {
  G4UIExecutive* ui = 0;
  const char* args[] = {"DDG4","",""};
  printout(INFO,"Geant4UIManager","+++ Starting G4UIExecutive '%s' of type %s....",
	   args[0], m_sessionType.c_str());

#if (G4VERSION_NUMBER >= 960)
  ui = new G4UIExecutive(1,(char**)args,m_sessionType.c_str());
#else
  ui = new G4UIExecutive(1,(char**)args );
#endif

  return ui;
}

/// Run UI
void Geant4UIManager::operator()(void* )   {
  start();
  stop();
}

/// Start manager & session
void Geant4UIManager::start() {
  // Get the pointer to the User Interface manager
  G4UImanager* mgr = G4UImanager::GetUIpointer();

  // Start visualization
  if ( m_haveVis || !m_visSetup.empty() ) {
    m_vis = startVis();
    m_haveVis = true;   // If graphics setup, vis is always true
    m_haveUI = true;    // No graphics without UI!
  }
  // Start UI instance
  if ( m_haveUI || !m_uiSetup.empty() ) {
    m_ui = startUI();
  }
  // Configure visualization instance
  if ( !m_visSetup.empty() ) {
    string cmd = "/control/execute "+m_visSetup;
    mgr->ApplyCommand(cmd.c_str());
    printout(INFO,"Geant4UIManager","++ Executed visualization setup:%s",m_visSetup.c_str());
  }
  // Configure UI instance
  if ( !m_uiSetup.empty() )   {
    string cmd = "/control/execute "+m_uiSetup;
    mgr->ApplyCommand(cmd.c_str());
    printout(INFO,"Geant4UIManager","++ Executed UI setup:%s",m_uiSetup.c_str());
  }
  // Start UI session if present
  if ( m_haveUI && m_ui )   {
    m_ui->SessionStart();
    return;
  }
  // No UI. Simply execute requested number of events
  long numEvent = context()->kernel().property("NumEvents").value<long>();
  printout(INFO,"Geant4UIManager","++ Start run with %d events.",numEvent);
  context()->kernel().runManager().BeamOn(numEvent);
}

/// Stop and release resources
void Geant4UIManager::stop() {
  // deletePtr(m_vis);
  //deletePtr(m_ui);
}
