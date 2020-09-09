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
#ifndef DDEVE_EVESHAPECONTEXTMENU_H
#define DDEVE_EVESHAPECONTEXTMENU_H

// ROOT include files
#include "DDEve/EveUserContextMenu.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// class EveShapeContextMenu  EveShapeContextMenu.h DDEve/EveShapeContextMenu.h
  /*
   * \author  M.Frank
   * \version 1.0
   * \ingroup DD4HEP_EVE
   */
  class EveShapeContextMenu : public EveUserContextMenu {
  protected:
    /// Initializing constructor
    EveShapeContextMenu(Display* m);
    /// Default destructor
    virtual ~EveShapeContextMenu();
  public:
    /// Instantiator
    static EveShapeContextMenu& install(Display* m);
    /// Root implementation macro
    ClassDefOverride(EveShapeContextMenu,0);
  };
}      /* End namespace dd4hep               */
#endif // DDEVE_EVESHAPECONTEXTMENU_H
