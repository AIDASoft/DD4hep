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
#include "DDCond/ConditionsPool.h"
#include "DDCond/ConditionsInterna.h"

using std::string;
using namespace DD4hep;
using namespace DD4hep::Conditions;

DD4HEP_INSTANTIATE_HANDLE_NAMED(ConditionsPool);

/// Default constructor
ConditionsPool::ConditionsPool(ConditionsManager mgr)
  : NamedObject(), m_manager(mgr), iovType(0), iov(0), age_value(AGE_NONE)
{
  InstanceCount::increment(this);
}

/// Default destructor
ConditionsPool::~ConditionsPool()   {
  // Should, but cannot clear here, since clear is a virtual overload.
  InstanceCount::decrement(this);
}

/// Print pool basics
void ConditionsPool::print(const string& opt)   const  {
  printout(INFO,"ConditionsPool","+++ %s Conditions for pool with IOV: %-32s age:%3d [%4d entries]",
	   opt.c_str(), iov->str().c_str(), age_value, count());
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
}

/// Default destructor
UpdatePool::~UpdatePool()   {
}

/// Default constructor
UserPool::UserPool(ConditionsManager mgr, ConditionsIOVPool* pool)
  : m_iov(0), m_manager(mgr), m_iovPool(pool)
{
  InstanceCount::increment(this);
}

/// Default destructor.
UserPool::~UserPool()   {
  InstanceCount::decrement(this);
}
