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

/// Framework include files
#include <DDG4/Geant4UIManager.h>
#include <DDG4/Geant4Kernel.h>
#include <DDG4/Geant4UIMessenger.h>
#include <DD4hep/Primitives.h>

/// Geant4 include files
#include <G4Version.hh>
#include <G4VisExecutive.hh>
#include <G4UImanager.hh>
#include <G4UIsession.hh>
#include <G4VisExecutive.hh>
#include <G4UIExecutive.hh>
#include <G4RunManager.hh>

/// C/C++ include files
#include <cstdlib>
#include <functional>

using namespace dd4hep::sim;

namespace   {
  std::string make_cmd(const std::string& cmd)  {
    return std::string( "/control/execute "+cmd);
  }
}

/// Initializing constructor
Geant4UIManager::Geant4UIManager(Geant4Context* ctxt, const std::string& nam)
  : Geant4Action(ctxt,nam), m_vis(0), m_ui(0)
{
  declareProperty("SetupUI",            m_uiSetup="");
  declareProperty("SetupVIS",           m_visSetup="");
  declareProperty("SessionType",        m_sessionType="tcsh");
  declareProperty("Macros",             m_macros);
  declareProperty("ConfigureCommands",  m_configureCommands);
  declareProperty("InitializeCommands", m_initializeCommands);
  declareProperty("TerminateCommands",  m_terminateCommands);
  declareProperty("Commands",           m_preRunCommands);
  declareProperty("PreRunCommands",     m_preRunCommands);
  declareProperty("PostRunCommands",    m_postRunCommands);
  declareProperty("HaveVIS",            m_haveVis=false);
  declareProperty("HaveUI",             m_haveUI=true);
  declareProperty("Prompt",             m_prompt);
  context()->kernel().register_configure(std::bind(&Geant4UIManager::configure,this));
  context()->kernel().register_initialize(std::bind(&Geant4UIManager::initialize,this));
  context()->kernel().register_terminate(std::bind(&Geant4UIManager::terminate,this));
  enableUI();
}

/// Default destructor
Geant4UIManager::~Geant4UIManager()   {
}

/// Configure the object
void Geant4UIManager::configure()   {
  /// Get the pointer to the User Interface manager
  G4UImanager* mgr = G4UImanager::GetUIpointer();
  /// Start UI instance
  if ( m_haveUI ) {
    m_ui = startUI();
  }
  /// Execute the chained command statements
  for(const auto& c : m_configureCommands)  {
    info("++ Executing configure command: %s",c.c_str());
    G4int ret = mgr->ApplyCommand(c.c_str());
    if ( ret != 0 )  {
      except("Failed to execute command: %s",c.c_str());
    }
  }
}

/// Initialize the object
void Geant4UIManager::initialize()   {
  /// Get the pointer to the User Interface manager
  G4UImanager* mgr = G4UImanager::GetUIpointer();
  /// Execute the chained command statements
  for(const auto& c : m_initializeCommands)  {
    info("++ Executing initialization command: %s",c.c_str());
    G4int ret = mgr->ApplyCommand(c.c_str());
    if ( ret != 0 )  {
      except("Failed to execute command: %s",c.c_str());
    }
  }
}

/// Callback on terminate
void Geant4UIManager::terminate() {
  /// Get the pointer to the User Interface manager
  G4UImanager* mgr = G4UImanager::GetUIpointer();
  /// Execute the chained command statements
  for(const auto& c : m_terminateCommands)  {
    info("++ Executing finalization command: %s",c.c_str());
    G4int ret = mgr->ApplyCommand(c.c_str());
    if ( ret != 0 )  {
      except("Failed to execute command: %s",c.c_str());
    }
  }
}

/// Apply single command
void Geant4UIManager::applyCommand(const std::string& command)   {
  /// Get the pointer to the User Interface manager
  G4UImanager* mgr = G4UImanager::GetUIpointer();
  if ( mgr )    {
    info("++ Executing G4 command: %s",command.c_str());
    G4int ret = mgr->ApplyCommand(command.c_str());
    if ( ret == 0 )  {
      return;
    }
    except("Failed to execute command: %s",command.c_str());
  }
  except("No UI reference present. Too early to interact with Geant4!");
}

/// Install command control messenger to write GDML file from command prompt.
void Geant4UIManager::installCommandMessenger()   {
  m_control->addCall("exit", "Force exiting this process",
                     Callback(this).make(&Geant4UIManager::forceExit),0);
  m_control->addCall("terminate", "Regular exit this process",
                     Callback(this).make(&Geant4UIManager::regularExit),0);
}

/// Force exiting this process without calling atexit handlers
void Geant4UIManager::forceExit()   {
  std::_Exit(0);
}

/// Regularly exiting this process without calling atexit handlers
void Geant4UIManager::regularExit()   {
  info("++ End of processing requested.");
  context()->kernel().terminate();
  forceExit();
}

/// Start visualization
G4VisManager* Geant4UIManager::startVis()  {
  /// Initialize visualization
  info("+++ Starting G4VisExecutive ....");
  G4VisManager* vis = new G4VisExecutive();
  vis->Initialize();
  return vis;
}

/// Start UI
G4UIExecutive* Geant4UIManager::startUI()   {
  G4UIExecutive* ui = 0;
  const char* args[] = {"DDG4","",""};
  info("+++ Starting G4UIExecutive '%s' of type %s....", args[0], m_sessionType.c_str());
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
  /// Get the pointer to the User Interface manager
  G4UImanager* mgr = G4UImanager::GetUIpointer();
  bool executed_statements = false;

  /// Start visualization
  if ( m_haveVis || !m_visSetup.empty() ) {
    m_vis = startVis();
    m_haveVis = true;   /// If graphics setup, vis is always true
    m_haveUI = true;    /// No graphics without UI!
  }
  /// Configure visualization instance
  if ( !m_visSetup.empty() ) {
    info("++ Executing visualization setup: %s",m_visSetup.c_str());
    mgr->ApplyCommand(make_cmd(m_visSetup).c_str());
  }
  /// Configure UI instance
  if ( !m_uiSetup.empty() )   {
    info("++ Executing UI setup: %s",m_uiSetup.c_str());
    mgr->ApplyCommand(make_cmd(m_uiSetup).c_str());
    executed_statements = true;
  }
  /// Execute the chained macro files
  for(const auto& m : m_macros)  {
    info("++ Executing Macro file: %s",m.c_str());
    mgr->ApplyCommand(make_cmd(m.c_str()));
    executed_statements = true;
  }
  /// Execute the chained pre-run command statements
  for(const auto& c : m_preRunCommands)  {
    info("++ Executing pre-run statement: %s",c.c_str());
    G4int ret = mgr->ApplyCommand(c.c_str());
    if ( ret != 0 )  {
      except("Failed to execute command: %s",c.c_str());
    }
    executed_statements = true;
  }
  /// Start UI session if present
  if ( m_haveUI && m_ui )   {
    m_ui->SessionStart();
    /// Execute the chained post-run command statements
    for(const auto& c : m_postRunCommands)  {
      info("++ Executing post-run statement: %s",c.c_str());
      G4int ret = mgr->ApplyCommand(c.c_str());
      if ( ret != 0 )  {
        except("Failed to execute command: %s",c.c_str());
      }
      executed_statements = true;
    }
    return;
  }
  else if ( m_haveUI )   {
    warning("++ No UI manager found. Exit.");
    return;
  }
  else if ( executed_statements )  {
    /// Execute the chained post-run command statements
    for(const auto& c : m_postRunCommands)  {
      info("++ Executing post-run statement: %s",c.c_str());
      G4int ret = mgr->ApplyCommand(c.c_str());
      if ( ret != 0 )  {
        except("Failed to execute command: %s",c.c_str());
      }
    }
    return;
  }

  /// No UI. Pure batch mode: Simply execute requested number of events
  long numEvent = context()->kernel().property("NumEvents").value<long>();
  if(numEvent < 0) numEvent = std::numeric_limits<int>::max();
  info("++ Start run with %d events.",numEvent);
  try {
    context()->kernel().runManager().BeamOn(numEvent);
  } catch (DD4hep_End_Of_File& e) {
    info("++ End of file reached, ending run...");
    context()->kernel().runManager().RunTermination();
  }
}

/// Stop and release resources
void Geant4UIManager::stop() {
  detail::deletePtr(m_vis);
  detail::deletePtr(m_ui);
}
