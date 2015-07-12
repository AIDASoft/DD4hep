// $Id$
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
#include "DDEve/Calo3DProjection.h"
#include "DDEve/Factories.h"
#include "DD4hep/InstanceCount.h"

using namespace DD4hep;

ClassImp(Calo3DProjection)
DECLARE_VIEW_FACTORY(Calo3DProjection)

/// Initializing constructor
Calo3DProjection::Calo3DProjection(Display* eve, const std::string& nam)
: View(eve, nam)
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
