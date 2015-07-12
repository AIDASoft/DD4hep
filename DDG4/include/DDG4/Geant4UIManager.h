// $Id: Handle.h 570 2013-05-17 07:47:11Z markus.frank $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation européenne pour la Recherche nucléaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================
#ifndef DD4HEP_DDG4_GEANT4UIMANAGER_H
#define DD4HEP_DDG4_GEANT4UIMANAGER_H

// Framework include files
#include "DDG4/Geant4Call.h"
#include "DDG4/Geant4Action.h"

/// Forward declarations
class G4VisManager;
class G4UImanager;
class G4UIExecutive;


/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    /// Standard UI interface implementation with configuration using property options
    /** The {\tt{Geant4UIManager}} is a component attached to the {\tt{Geant4Kernel}} object.
     *
     *  All properties of all {\tt{Geant4Action}} instances may be exported to
     *  Geant4 messengers and {\em{may}} hence be accessible directly from the Geant4
     *  prompt. To export properties from any action, call the {\tt{enableUI()}}
     *  method of the action.
     *
     *  Please note: 
     *
     *  Any type of commands may be put together, but the overall execution order
     *  is as follows:
     *  1. Execution of the visualization setup (Property: SetupVIS)
     *  2. Execution of the user interface setup (Property: SetupUI)
     *  3. Execution of the macro file list (Property: Macros)
     *  4. Execution of the command list (Property: Commands)
     *
     *
     * \author  M.Frank
     * \version 1.0
     */
    class Geant4UIManager : public Geant4Action, virtual public Geant4Call   {
    protected:
      /// Name of the default session type (="cmd")
      std::string m_sessionType;
      /// Property: Name of the UI macro file
      std::string m_uiSetup;
      /// Property: Name of the visualization macro file
      std::string m_visSetup;
      /// Property: Array of macro files to be chained
      std::vector<std::string> m_commands;
      /// Property: Array of commands to be chained
      std::vector<std::string> m_macros;
      /// Property: New prompt if the user wants to change it. (Default is do nothing)
      std::string m_prompt;
      /// Property: Flag to instantiate Vis manager (default=false, unless m_visSetup set)
      bool        m_haveVis;
      /// Property: Flag to instantiate UI (default=true)
      bool        m_haveUI;
      /// Reference to Geant4 visualtion manager
      G4VisManager* m_vis;
      /// Reference to Geant4 UI manager
      G4UIExecutive*  m_ui;
    public:
      /// Initializing constructor
      Geant4UIManager(Geant4Context* context, const std::string& name);
      /// Default destructor
      virtual ~Geant4UIManager();
      /// Start visualization
      G4VisManager* startVis();
      /// Start UI
      G4UIExecutive* startUI();
      /// Start manager & session
      void start();
      /// Stop and release resources
      void stop();
      /// Run UI
      virtual void operator()(void* param);
    };

  }    // End namespace Simulation
}      // End namespace DD4hep
#endif // DD4HEP_DDG4_GEANT4UIMANAGER_H
