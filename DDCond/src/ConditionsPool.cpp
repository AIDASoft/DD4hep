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
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/objects/ConditionsInterna.h"

#include "DDCond/ConditionsPool.h"
#include "DDCond/ConditionsEntry.h"
#include "DDCond/ConditionsInterna.h"

using std::string;
using namespace DD4hep;
using namespace DD4hep::Conditions;

DD4HEP_INSTANTIATE_HANDLE_NAMED(ConditionsPool);

/// Default constructor
ConditionsPool::ConditionsPool(ConditionsManager mgr)
  : NamedObject(), m_manager(mgr),
    iovType(0), iov(0), age_value(AGE_NONE), pool_type(NO_POOL_TYPE)
{
  InstanceCount::increment(this);
}

/// Default destructor
ConditionsPool::~ConditionsPool()   {
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

/// Print pool basics
void ConditionsPool::print(const string& opt)   const  {
  const IOVType* iov_type = iov->iovType;
  if ( iov_type->name == "epoch" )  {
    time_t since = iov->key().first;
    time_t until = iov->key().second;
    char c_since[64], c_until[64];
    struct tm time_buff;
    ::strftime(c_since,sizeof(c_since),"%d-%m-%Y %H:%M:%S",::gmtime_r(&since,&time_buff));
    ::strftime(c_until,sizeof(c_until),"%d-%m-%Y %H:%M:%S",::gmtime_r(&until,&time_buff));
    printout(INFO,"Conditions","+++ %s Conditions for pool with IOV: %s(%d) age:%3d [%4d entries] [%s -> %s] ",
	     opt.c_str(), iov_type->name.c_str(), iov_type->type, age_value, count(), c_since, c_until);
  }
  else  {
    printout(INFO,"Example","+++ %s Conditions for pool with IOV: %-32s age:%3d [%4d entries]",
	     opt.c_str(), iov->str().c_str(), age_value, count());
  }
}

/// Listener invocation when a condition is registered to the cache
void ConditionsPool::onRegister(Condition condition)   {
  m_manager.ptr()->onRegister(condition);
}

/// Listener invocation when a condition is deregistered from the cache
void ConditionsPool::onRemove(Condition condition)   {
  m_manager.ptr()->onRemove(condition);
}

/// Default constructor
UpdatePool::UpdatePool(ConditionsManager mgr) : ConditionsPool(mgr)
{
  pool_type = UPDATE_POOL_TYPE;
}

/// Default destructor
UpdatePool::~UpdatePool()   {
}

/// Default constructor
UserPool::UserPool(ConditionsManager mgr)  : ConditionsPool(mgr) {
  pool_type = USER_POOL_TYPE;
}

/// Default destructor.
UserPool::~UserPool()   {
}
