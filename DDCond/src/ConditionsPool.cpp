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
#include "DD4hep/Handle.inl"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/objects/ConditionsInterna.h"

#include "DDCond/ConditionsPool.h"
#include "DDCond/ConditionsEntry.h"

using std::string;
using namespace DD4hep::Conditions;

DD4HEP_INSTANTIATE_HANDLE_NAMED(ConditionsPool);

/// Default constructor
ConditionsPool::ConditionsPool()
  : NamedObject(), iovType(0), iov(0), updates(0), age_value(AGE_NONE)
{
  InstanceCount::increment(this);
}

/// Default destructor
ConditionsPool::~ConditionsPool()   {
  DD4hep::deletePtr(updates);
  // Should, but cannot clear here, since clear is a virtual overload.
  InstanceCount::decrement(this);
}

/// Unconditionally create a new condition from the input data
Condition ConditionsPool::create(ConditionsPool* pool, const Entry* entry)   {
  Condition condition(entry->name,entry->type);
  Condition::Object* c = condition.ptr();
  //c->name = entry->name;
  //  c->type = entry->type;
  c->value = entry->value;
  c->comment = "----";
  c->address = "----";
  c->validity = entry->validity;
  c->detector = entry->detector;
  c->iov  = pool->iov;
  c->pool = pool;
  return c;
}

/// Default constructor
UpdatePool::UpdatePool() : ConditionsPool()
{
}

/// Default destructor
UpdatePool::~UpdatePool()   {
}

/// Default constructor
ReplacementPool::ReplacementPool()  : ConditionsPool() {
}

/// Default destructor.
ReplacementPool::~ReplacementPool()   {
}
