// $Id$
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
#ifndef DD4HEP_DDEVE_EVEUSERCONTEXTMENU_H
#define DD4HEP_DDEVE_EVEUSERCONTEXTMENU_H

// ROOT include files
#include "TClass.h"

// Forward declarations
class TEveManager;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  // Forward declarations
  class Display;

  /// class EveUserContextMenu  EveUserContextMenu.h DDEve/EveUserContextMenu.h
  /*
   * \author  M.Frank
   * \version 1.0
   * \ingroup DD4HEP_EVE
   */
  class EveUserContextMenu  {
  protected:
    Display* m_display;
  public:
    /// Initializing constructor
    EveUserContextMenu(Display* m);
    /// Default destructor
    virtual ~EveUserContextMenu();    
    /// EVE display
    Display& display() const  { return *m_display; }
    /// EVE manager
    TEveManager& manager() const;
    /// Set everything (in-) visible
    void VisibleAll(bool yes_no, TObject* target, void* user_param);
    /// Set self (in-) visible
    void VisibleSelf(bool yes_no, TObject* target, void* user_param);
    /// Set the children (in-) visible
    void VisibleChildren(bool yes_no, TObject* target, void* user_param);

    /// Show all
    void ShowAll(TObject* target, void* user_param);
    /// Show all
    void HideAll(TObject* target, void* user_param);
    /// Show shape self
    void ShowSelf(TObject* target, void* user_param);
    /// Show shape self
    void HideSelf(TObject* target, void* user_param);
    /// Show shape children
    void ShowChildren(TObject* target, void* user_param);
    /// Show shape children
    void HideChildren(TObject* target, void* user_param);
    /// Load next level children and add them to all scenes
    void LoadChildren(TObject* target, void* user_param);
    /// Load all children and add them to all scenes
    void DeepLoadChildren(TObject* target, void* user_param);

    /// Install the geometry context menu
    void InstallGeometryContextMenu(TClass* cl);

    /// Root implementation macro
    ClassDef(EveUserContextMenu,0);
  };
} /* End namespace DD4hep   */

#endif /* DD4HEP_DDEVE_EVEUSERCONTEXTMENU_H */
