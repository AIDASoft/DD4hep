// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
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
