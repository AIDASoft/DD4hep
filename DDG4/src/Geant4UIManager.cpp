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
#include "DDG4/Geant4UIManager.h"
#include "DDG4/Geant4Kernel.h"
#include "DD4hep/Primitives.h"
#include "DD4hep/Printout.h"

// Geant4 include files
#include "G4VisExecutive.hh"
#include "G4UImanager.hh"
#include "G4UIsession.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4RunManager.hh"

using namespace dd4hep::sim;
using namespace std;

namespace   {
  string make_cmd(const string& cmd)  {
    return string( "/control/execute "+cmd);
  }
}

/// Initializing constructor
Geant4UIManager::Geant4UIManager(Geant4Context* ctxt, const std::string& nam)
  : Geant4Action(ctxt,nam), m_vis(0), m_ui(0)
{
  declareProperty("SetupUI",     m_uiSetup="");
  declareProperty("SetupVIS",    m_visSetup="");
  declareProperty("SessionType", m_sessionType="tcsh");
  declareProperty("Macros",      m_macros);
  declareProperty("Commands",    m_commands);
  declareProperty("HaveVIS",     m_haveVis=false);
  declareProperty("HaveUI",      m_haveUI=true);
  declareProperty("Prompt",      m_prompt);
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
  if ( !m_prompt.empty() )  {
    ui->SetPrompt(m_prompt);
  }
  return ui;
}

/// Run UI
void Geant4UIManager::operator()(void* )   {
  start();
  stop();
}

/// Start manager & session
void Geant4UIManager::start() {
  typedef std::vector<std::string> _V;
  // Get the pointer to the User Interface manager
  G4UImanager* mgr = G4UImanager::GetUIpointer();
  bool executed_statements = false;

  // Start visualization
  if ( m_haveVis || !m_visSetup.empty() ) {
    m_vis = startVis();
    m_haveVis = true;   // If graphics setup, vis is always true
    m_haveUI = true;    // No graphics without UI!
  }
  // Start UI instance
  if ( m_haveUI ) {
    m_ui = startUI();
  }
  // Configure visualization instance
  if ( !m_visSetup.empty() ) {
    printout(INFO,"Geant4UIManager","++ Executing visualization setup:%s",m_visSetup.c_str());
    mgr->ApplyCommand(make_cmd(m_visSetup).c_str());
  }
  // Configure UI instance
  if ( !m_uiSetup.empty() )   {
    printout(INFO,"Geant4UIManager","++ Executing UI setup:%s",m_uiSetup.c_str());
    mgr->ApplyCommand(make_cmd(m_uiSetup).c_str());
    executed_statements = true;
  }
  // Execute the chained macro files
  for(_V::const_iterator i=m_macros.begin(); i!=m_macros.end(); ++i)   {
    printout(INFO,"Geant4UIManager","++ Executing Macro file:%s",(*i).c_str());
    mgr->ApplyCommand(make_cmd(*i).c_str());
    executed_statements = true;
  }
  // Execute the chained command statements
  for(_V::const_iterator i=m_commands.begin(); i!=m_commands.end(); ++i)   {
    printout(INFO,"Geant4UIManager","++ Executing Command statement:%s",(*i).c_str());
    mgr->ApplyCommand((*i).c_str());
    executed_statements = true;
  }
  // Start UI session if present
  if ( m_haveUI && m_ui )   {
    m_ui->SessionStart();
    return;
  }
  else if ( m_haveUI )   {
    printout(WARNING,"Geant4UIManager","++ No UI manager found. Exit.");
    return;
  }
  else if ( executed_statements )  {
    return;
  }

  // No UI. Pure batch mode: Simply execute requested number of events
  long numEvent = context()->kernel().property("NumEvents").value<long>();
  printout(INFO,"Geant4UIManager","++ Start run with %d events.",numEvent);
  context()->kernel().runManager().BeamOn(numEvent);
}

/// Stop and release resources
void Geant4UIManager::stop() {
  detail::deletePtr(m_vis);
  detail::deletePtr(m_ui);
}
