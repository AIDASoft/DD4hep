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
//
//==========================================================================

// Framework include files
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#define  G__ROOT
#include "DDDigi/DigiData.h"

// C/C++ include files

using namespace std;
using namespace dd4hep;
using namespace dd4hep::digi;

/// Intializing constructor
DigiEvent::DigiEvent()
  : ObjectExtensions(typeid(DigiEvent))
{
  InstanceCount::increment(this);
}

/// Intializing constructor
DigiEvent::DigiEvent(int ev_num)
  : ObjectExtensions(typeid(DigiEvent)), eventNumber(ev_num)
{
  InstanceCount::increment(this);
}

/// Default destructor
DigiEvent::~DigiEvent()
{
  InstanceCount::decrement(this);
}
