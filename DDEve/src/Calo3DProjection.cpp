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
#include "DDEve/Calo3DProjection.h"
#include "DDEve/Factories.h"
#include "DD4hep/InstanceCount.h"

using namespace DD4hep;

ClassImp(Calo3DProjection)
DECLARE_VIEW_FACTORY(Calo3DProjection)

/// Initializing constructor
Calo3DProjection::Calo3DProjection(Display* eve, const std::string& name)
: View(eve, name)
{
  InstanceCount::increment(this);
}

/// Default destructor
Calo3DProjection::~Calo3DProjection()  {
  InstanceCount::decrement(this);
}

/// Build the projection view and map it to the given slot
View& Calo3DProjection::Build(TEveWindow* slot)   {
  CreateScenes();
  AddToGlobalItems(name());
  return Map(slot);
}
