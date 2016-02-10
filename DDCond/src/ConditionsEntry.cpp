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
#include "DD4hep/InstanceCount.h"
#include "DDCond/ConditionsEntry.h"

using std::string;
using namespace DD4hep::Conditions;

/// Initializing constructor
Entry::Entry(const DD4hep::Geometry::DetElement& det, const string& nam, const string& typ, const string& valid, int h)
  : NamedObject(nam,typ), detector(det), value(), validity(valid), hash(h)
{
  InstanceCount::increment(this);
}

/// Copy constructor
Entry::Entry(const Entry& c)
  : NamedObject(c), detector(c.detector), value(c.value), validity(c.validity)
{
  InstanceCount::increment(this);
}

/// Default destructor
Entry::~Entry()   {
  InstanceCount::decrement(this);
}

/// Assignment operator
Entry& Entry::operator=(const Entry& c)   {
  if ( this != &c )  {
    this->NamedObject::operator=(c);
    detector = c.detector;
    value = c.value;
    validity = c.validity;
  }
  return *this;
}
