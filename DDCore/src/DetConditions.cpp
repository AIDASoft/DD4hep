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
#include "DD4hep/objects/DetectorInterna.h"
#include "DD4hep/objects/ConditionsInterna.h"
#include "DD4hep/DetConditions.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/World.h"
#include "DD4hep/LCDD.h"

using namespace std;
using namespace DD4hep::Conditions;

/// Check if conditions are at all present
bool DetConditions::hasConditions()  const   {
  Object* o = access();
  if ( o->conditions.isValid() && !o->conditions->keys.empty() ) 
    return true;
  return false;
}

/// Access to the conditions information
Container DetConditions::conditions() const  {
  Object* o = access();
  if ( o->conditions.isValid() ) return o->conditions;
  o->conditions.assign(new Container::Object(o),"conditions","");
  return o->assign_conditions();
}

/// Access to condition objects.
Condition DetConditions::get(const std::string& key, const Condition::iov_type& iov)   {
  return conditions().get(key, iov);
}

/// Access to condition objects.
Condition DetConditions::get(Condition::key_type key, const Condition::iov_type& iov)   {
  return conditions().get(key, iov);
}

/// Access to condition objects from a given pool. Only conditions in the pool are accessed.
Condition DetConditions::get(const std::string& key,  const UserPool& pool)   {
  return conditions().get(key, pool);
}

/// Access to condition objects from a given pool. Only conditions in the pool are accessed.
Condition DetConditions::get(Condition::key_type key, const UserPool& pool)   {
  return conditions().get(key, pool);
}

