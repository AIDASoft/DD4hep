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
#include "DDEve/RhoPhiProjection.h"
#include "DDEve/Factories.h"

using namespace DD4hep;

ClassImp(RhoPhiProjection)
DECLARE_VIEW_FACTORY(RhoPhiProjection)

/// Initializing constructor
RhoPhiProjection::RhoPhiProjection(Display* eve, const std::string& name)
  : Projection(eve, name)
{
}

/// Default destructor
RhoPhiProjection::~RhoPhiProjection()  {
}

/// Build the projection view and map it to the given slot
View& RhoPhiProjection::Build(TEveWindow* slot)   {
  CreateScenes();
  CreateRhoPhiProjection().AddAxis();
  return Map(slot);
}
