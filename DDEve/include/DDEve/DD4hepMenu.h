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
#ifndef DDEVE_DD4HEPMENU_H
#define DDEVE_DD4HEPMENU_H

// Framework include files
#include "DDEve/Display.h"
#include "DDEve/PopupMenu.h"

// Forward declarations
class TGMenuBar;
class TGMenuEntry;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  // Forward declarations
  class View;
  class EventControl;

  /// dd4hep Menu for the ROOT browser
  /*
   * \author  M.Frank
   * \version 1.0
   * \ingroup DD4HEP_EVE
   */
  class DD4hepMenu : public PopupMenu {
  public:

  protected:
    /// Reference to display manager
    Display* m_display;
    EventControl* m_evtCtrl;
  public:
    /// Standard constructor
    DD4hepMenu(Display* eve);
    /// Default destructor
    virtual ~DD4hepMenu();

    /// Add the menu to the menu bar
    virtual void Build(TGMenuBar* bar, int hints=kLHintsNormal)  override;
    /// Callback when loading the configuration
    void OnLoadXML(TGMenuEntry* entry, void* ptr);
    /// Callback when loading the configuration
    void OnLoadRootGeometry(TGMenuEntry* entry, void* ptr);
    /// Callback to show the event I/O panel
    void OnCreateEventIO(TGMenuEntry* entry, void* ptr);
    /// Callback when loading a new event data file
    void OnOpenEventData(TGMenuEntry* entry, void* ptr);
    /// Callback when loading the next event
    void OnNextEvent(TGMenuEntry* entry, void* ptr);
    /// Callback when loading the previous event
    void OnPreviousEvent(TGMenuEntry* entry, void* ptr);
    /// Callback when exiting the display
    void OnExit(TGMenuEntry* entry, void* ptr);

    /// Callback when the geometry was loaded
    void OnGeometryLoaded();
    /// ROOT implementation macro
    ClassDefOverride(DD4hepMenu,0);
  };

}      /* End namespace dd4hep      */
#endif // DDEVE_DD4HEPMENU_H

