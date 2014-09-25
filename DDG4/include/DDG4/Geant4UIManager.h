// $Id: Geant4Field.cpp 875 2013-11-04 16:15:14Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
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
    /** @class Geant4UIManager Geant4UIManager.h DDG4/Geant4UIManager.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4UIManager : public Geant4Action, virtual public Geant4Call   {
    protected:
      /// Name of the default session type (="cmd")
      std::string m_sessionType;
      /// Property: Name of the UI macro file
      std::string m_uiSetup;
      /// Property: Name of the visualization macro file
      std::string m_visSetup;
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
