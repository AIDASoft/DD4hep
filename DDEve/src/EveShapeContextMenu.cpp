// $Id: $
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

// Framework include files
#include "DD4hep/Printout.h"
#include "DDEve/EveShapeContextMenu.h"

// ROOT include files
#include "TEveGeoShape.h"
#include "TEveManager.h"

// C/C++ include files
#include <stdexcept>

using namespace std;
using namespace DD4hep;

ClassImp(EveShapeContextMenu)

/// Instantiator
EveShapeContextMenu& EveShapeContextMenu::install(Display* m)   {
  static EveShapeContextMenu s(m);
  return s;
}

/// Initializing constructor
EveShapeContextMenu::EveShapeContextMenu(Display* disp)
  : EveUserContextMenu(disp)
{
  InstallGeometryContextMenu(TEveGeoShape::Class());
}

/// Default destructor
EveShapeContextMenu::~EveShapeContextMenu()  {
}
