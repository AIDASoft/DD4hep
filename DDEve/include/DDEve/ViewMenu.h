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
#ifndef DD4HEP_DDEVE_VIEWMENU_H
#define DD4HEP_DDEVE_VIEWMENU_H

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

  /// class ViewMenu  ViewMenu.h DDEve/ViewMenu.h
  /*
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_EVE
   */
  class ViewMenu : public PopupMenu {
  public:

  protected:
    /// Reference to display manager
    Display* m_display;
    /// Menu title
    std::string m_title;
  public:
    /// Standard constructor
    ViewMenu(Display* eve, const std::string& title);
    /// Default destructor
    virtual ~ViewMenu();

    /// Add the menu to the menu bar
    virtual void Build(TGMenuBar* bar, int hints=kLHintsNormal)  override;

    /// Import Geometry data and build the view
    void BuildView(View *view)  const;
    /// Menu callback: Create a generic view using the user data
    void CreateView(TGMenuEntry* e, void* ud);

    /// Create a generic view of a given type with title
    View* CreateView(const std::string& type,const std::string& title);
    /// Specialization: Create a new 3D view
    View* CreateView3D(const std::string& title);
    /// Specialization: Create a new R-Z view
    View* CreateRhoZProjection(const std::string& title);
    /// Specialization: Create a new R-Phi view
    View* CreateRhoPhiProjection(const std::string& title);

    /// ROOT implementation macro
    ClassDefOverride(ViewMenu,0);
  };
}      /* End namespace dd4hep    */
#endif /* DD4HEP_DDEVE_VIEWMENU_H */

