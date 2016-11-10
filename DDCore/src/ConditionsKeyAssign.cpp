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

// Framework includes
#include "DD4hep/ConditionsKeyAssign.h"
#include "DD4hep/objects/ConditionsInterna.h"
#include "DD4hep/DetConditions.h"

using std::string;
using namespace DD4hep::Conditions;

/// Add a new key to the conditions access map
const ConditionsKeyAssign&
ConditionsKeyAssign::addKey(const string& key_value)  const
{
  DetConditions conds(detector);
  conds.conditions()->addKey(key_value);
  return *this;
}

/// Add a new key to the conditions access map: Allow for alias if key_val != data_val
const ConditionsKeyAssign&
ConditionsKeyAssign::addKey(const string& key_value, const string& data_value)  const
{
  DetConditions conds(detector);
  conds.conditions()->addKey(key_value, data_value);
  return *this;
}

 
