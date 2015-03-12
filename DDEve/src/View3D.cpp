// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//  Original Author: Matevz Tadel 2009 (MultiView.C)
//
//====================================================================
// Framework include files
#include "DDEve/View3D.h"
#include "DDEve/Factories.h"

using namespace DD4hep;

ClassImp(View3D)
DECLARE_VIEW_FACTORY(View3D)

/// Initializing constructor
View3D::View3D(Display* eve, const std::string& nam) : View(eve, nam)
{
}

/// Default destructor
View3D::~View3D()  {
}

/// Build the projection view and map it to the given slot
View& View3D::Build(TEveWindow* slot)   {
  return CreateScenes().Map(slot);
}
