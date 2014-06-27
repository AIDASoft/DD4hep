// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
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
EveShapeContextMenu::EveShapeContextMenu(Display* display)
  : EveUserContextMenu(display)
{
  InstallGeometryContextMenu(TEveGeoShape::Class());
}

/// Default destructor
EveShapeContextMenu::~EveShapeContextMenu()  {
}
