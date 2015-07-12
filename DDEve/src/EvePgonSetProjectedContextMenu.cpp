// $Id: run_plugin.h 1663 2015-03-20 13:54:53Z gaede $
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

// Framework include files
#include "DDEve/ContextMenu.h"
#include "DDEve/EvePgonSetProjectedContextMenu.h"

// ROOT include files
#include "TEvePolygonSetProjected.h"

// C/C++ include files
#include <stdexcept>

using namespace std;
using namespace DD4hep;

ClassImp(EvePgonSetProjectedContextMenu)

/// Instantiator
EvePgonSetProjectedContextMenu& EvePgonSetProjectedContextMenu::install(Display* m)   {
  static EvePgonSetProjectedContextMenu s(m);
  return s;
}

/// Initializing constructor
EvePgonSetProjectedContextMenu::EvePgonSetProjectedContextMenu(Display* mgr)
  : EveUserContextMenu(mgr)
{
  InstallGeometryContextMenu(TEvePolygonSetProjected::Class());
}

/// Default destructor
EvePgonSetProjectedContextMenu::~EvePgonSetProjectedContextMenu()  {
}
