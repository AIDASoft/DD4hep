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
#ifndef DD4HEP_DDEVE_EVEPGONSETPROJECTEDCONTEXTMENU_H
#define DD4HEP_DDEVE_EVEPGONSETPROJECTEDCONTEXTMENU_H

// Framework include files
#include "DDEve/EveUserContextMenu.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// class EvePgonSetProjectedContextMenu  EvePgonSetProjectedContextMenu.h DDEve/EvePgonSetProjectedContextMenu.h
  /*
   * \author  M.Frank
   * \version 1.0
   * \ingroup DD4HEP_EVE
   */
  class EvePgonSetProjectedContextMenu : public EveUserContextMenu  {
  protected:
    /// Initializing constructor
    EvePgonSetProjectedContextMenu(Display* m);
    /// Default destructor
    virtual ~EvePgonSetProjectedContextMenu();
  public:
    /// Instantiator
    static EvePgonSetProjectedContextMenu& install(Display* m);
    /// Root implementation macro
    ClassDefOverride(EvePgonSetProjectedContextMenu,0);
  };
}      /* End namespace DD4hep                          */
#endif /* DD4HEP_DDEVE_EVEPGONSETPROJECTEDCONTEXTMENU_H */
