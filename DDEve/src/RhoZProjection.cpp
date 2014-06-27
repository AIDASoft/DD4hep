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
#include "DDEve/RhoZProjection.h"
#include "DDEve/Factories.h"

using namespace DD4hep;

ClassImp(RhoZProjection)

DECLARE_VIEW_FACTORY(RhoZProjection)

/// Initializing constructor
RhoZProjection::RhoZProjection(Display* eve, const std::string& name)
  : Projection(eve, name)
{
}

/// Default destructor
RhoZProjection::~RhoZProjection()  {
}

/// Build the projection view and map it to the given slot
View& RhoZProjection::Build(TEveWindow* slot)   {
  CreateScenes();
  CreateRhoZProjection().AddAxis();
  return Map(slot);
}
