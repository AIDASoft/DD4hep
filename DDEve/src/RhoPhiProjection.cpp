//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
// Original Author: Matevz Tadel 2009 (MultiView.C)
//
//==========================================================================

// Framework include files
#include "DDEve/RhoPhiProjection.h"
#include "DDEve/Factories.h"

using namespace dd4hep;

ClassImp(RhoPhiProjection)
DECLARE_VIEW_FACTORY(RhoPhiProjection)

/// Initializing constructor
RhoPhiProjection::RhoPhiProjection(Display* eve, const std::string& nam)
: Projection(eve, nam)
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
