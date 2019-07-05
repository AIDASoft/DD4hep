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
#include "DD4hep/Primitives.h"
#include "DD4hep/InstanceCount.h"
#define  G__ROOT
#include "DDDigi/DigiData.h"

// C/C++ include files

using namespace std;
using namespace dd4hep;
using namespace dd4hep::digi;

Key::Key()    {
  key = 0;
}

Key::Key(const Key& copy)   {
  key = copy.key;
}

Key::Key(mask_type mask, itemkey_type item)   {
  key = 0;
  values.mask = mask;
  values.item = item;
}

Key::Key(mask_type mask, const std::string& item)  {
  key = 0;
  values.mask = mask;
  values.item = detail::hash32(item);
}

Key& Key::operator=(const Key& copy)   {
  key = copy.key;
  return *this;
}

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
