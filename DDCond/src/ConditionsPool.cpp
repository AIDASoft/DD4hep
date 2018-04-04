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
#include "DDCond/ConditionsPool.h"
#include "DDCond/ConditionsManagerObject.h"
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/detail/Handle.inl"
#include "DD4hep/ConditionsPrinter.h"
#include "DD4hep/detail/ConditionsInterna.h"

using std::string;
using namespace dd4hep;
using namespace dd4hep::cond;

DD4HEP_INSTANTIATE_HANDLE_NAMED(UpdatePool);
DD4HEP_INSTANTIATE_HANDLE_NAMED(ConditionsPool);

/// Default constructor
ConditionsPool::ConditionsPool(ConditionsManager mgr, IOV* i)
  : NamedObject(), m_manager(mgr), iov(i), age_value(AGE_NONE)
{
  InstanceCount::increment(this);
}

/// Default destructor
ConditionsPool::~ConditionsPool()   {
  // Should, but cannot clear here, since clear is a virtual overload.
  InstanceCount::decrement(this);
}

/// Print pool basics
void ConditionsPool::print()   const  {
  printout(INFO,"ConditionsPool","+++ Conditions for pool with IOV: %-32s age:%3d [%4d entries]",
           GetName(), age_value, size());
}

/// Print pool basics
void ConditionsPool::print(const string& opt)   const  {
  printout(INFO,"ConditionsPool","+++ %s Conditions for pool with IOV: %-32s age:%3d [%4d entries]",
           opt.c_str(), GetName(), age_value, size());
  if ( opt == "*" || opt == "ALL" )   {
    ConditionsPrinter printer(0);
    RangeConditions   range;
    printer.summary    = false;
    printer.lineLength = 132;
    const_cast<ConditionsPool*>(this)->select_all(range);
    for( auto c : range )
      printer(c);
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
UpdatePool::UpdatePool(ConditionsManager mgr, IOV* i) : ConditionsPool(mgr, i)
{
}

/// Default destructor
UpdatePool::~UpdatePool()   {
}

/// Default constructor
UserPool::UserPool(ConditionsManager mgr)
  : m_iov(0), m_manager(mgr)
{
  InstanceCount::increment(this);
}

/// Default destructor.
UserPool::~UserPool()   {
  InstanceCount::decrement(this);
}
