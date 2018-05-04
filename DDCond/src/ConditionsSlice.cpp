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
#include "DDCond/ConditionsSlice.h"
#include "DDCond/ConditionsIOVPool.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/Printout.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::cond;

/// Initializing constructor
ConditionsSlice::ConditionsSlice(ConditionsManager m) : manager(m)
{
  InstanceCount::increment(this);  
}

/// Initializing constructor
ConditionsSlice::ConditionsSlice(ConditionsManager m, const shared_ptr<ConditionsContent>& c)
  : manager(m), content(c)
{
  InstanceCount::increment(this);  
}

/// Copy constructor (Special, partial copy only. Hence no assignment!)
ConditionsSlice::ConditionsSlice(const ConditionsSlice& copy)
  : manager(copy.manager), content(copy.content)
{
  InstanceCount::increment(this);  
}

/// Default destructor. 
ConditionsSlice::~ConditionsSlice()   {
  reset();
  InstanceCount::decrement(this);  
}

/// Set flag to not reference the used pools during prepare (and drop possibly pending)
void ConditionsSlice::derefPools()     {
  used_pools.clear(); // Drop all refs possibly pending
  this->flags &= ~REF_POOLS;
}

/// Access the combined IOV of the slice from the pool
const IOV& ConditionsSlice::iov()  const    {
  if ( pool.get() ) return pool->validity();
  dd4hep::except("ConditionsSlice",
                 "pool-iov: Failed to access validity of non-existing pool.");
  return pool->validity();
}

/// Clear the conditions access and the user pool.
void ConditionsSlice::reset()   {
  derefPools();
  if ( pool.get() ) pool->clear();
}

/// Local optimization: Insert a set of conditions to the slice AND register them to the conditions manager.
bool ConditionsSlice::manage(ConditionsPool* p, Condition condition, ManageFlag flg)   {
  if ( condition.isValid() )  {
    bool ret = false;
    if ( flg&REGISTER_MANAGER )  {
      if ( !p )   {
        dd4hep::except("ConditionsSlice",
                       "manage_condition: Cannot access conditions pool according to IOV:%s.",
                       pool->validity().str().c_str());
      }
      ret = manager.registerUnlocked(*p,condition);
      if ( !ret )  {
        dd4hep::except("ConditionsSlice",
                       "manage_condition: Failed to register condition %016llx according to IOV:%s.",
                       condition->hash, pool->validity().str().c_str());
      }
    }
    if ( flg&REGISTER_POOL )  {
      ret = pool->insert(condition);
      if ( !ret )  {
        dd4hep::except("ConditionsSlice",
                       "manage_condition: Failed to register condition %016llx to user pool with IOV:%s.",
                       condition->hash, pool->validity().str().c_str());
      }
    }
    return ret;
  }
  dd4hep::except("ConditionsSlice",
                 "manage_condition: Cannot manage invalid condition!");
  return false;
}

/// Insert a set of conditions to the slice AND register them to the conditions manager.
bool ConditionsSlice::manage(Condition condition, ManageFlag flg)    {
  ConditionsPool* p = (flg&REGISTER_MANAGER) ? manager.registerIOV(pool->validity()) : 0;
  return manage(p, condition, flg);
}

/// Access all conditions from a given detector element
vector<Condition> ConditionsSlice::get(DetElement detector)  const  {
  return pool->get(detector,FIRST_ITEM,LAST_ITEM);
}

/// No ConditionsMap overload: Access all conditions within a key range in the interval [lower,upper]
vector<Condition> ConditionsSlice::get(DetElement detector,
                                       Condition::itemkey_type lower,
                                       Condition::itemkey_type upper)  const  {
  return pool->get(detector, lower, upper);
}

/// ConditionsMap overload: Add a condition directly to the slice
bool ConditionsSlice::insert(DetElement detector, Condition::itemkey_type key, Condition condition)   {
  if ( condition.isValid() )  {
    ConditionsPool* p = manager.registerIOV(pool->validity());
    if ( !p )   {
      dd4hep::except("ConditionsSlice",
                     "manage_condition: Cannot access conditions pool according to IOV:%s.",
                     pool->validity().str().c_str());
    }
    bool ret = manager.registerUnlocked(*p,condition);
    if ( !ret )  {
      dd4hep::except("ConditionsSlice",
                     "manage_condition: Failed to register condition %016llx according to IOV:%s.",
                     condition->hash, pool->validity().str().c_str());
    }
    return pool->insert(detector, key, condition);
  }
  dd4hep::except("ConditionsSlice",
                 "insert_condition: Cannot insert invalid condition to the user pool!");
  return false;
}

/// ConditionsMap overload: Access a condition
Condition ConditionsSlice::get(DetElement detector, Condition::itemkey_type key)  const   {
  return pool->get(detector, key);
}

/// ConditionsMap overload: Interface to scan data content of the conditions mapping
void ConditionsSlice::scan(const Condition::Processor& processor) const   {
  pool->scan(processor);
}

/// ConditionsMap overload: Interface to partially scan data content of the conditions mapping
void ConditionsSlice::scan(DetElement         detector,
                           Condition::itemkey_type       lower,
                           Condition::itemkey_type       upper,
                           const Condition::Processor&   processor) const  {
  pool->scan(detector, lower, upper, processor);
}

namespace  {
  
  struct SliceOper  : public ConditionsSelect  {
    ConditionsContent& content;
    SliceOper(ConditionsContent& c) : content(c) {}
    void operator()(const ConditionsIOVPool::Elements::value_type& v)    {
      v.second->select_all(*this);
    }
    bool operator()(Condition::Object* c)  const  {
      if ( 0 == (c->flags&Condition::DERIVED) )   {
        content.insertKey(c->hash,c->address);
        return true;
      }
      return true;
    }
    /// Return number of conditions selected
    virtual size_t size()  const { return content.conditions().size(); }
  };
}

/// Populate the conditions slice from the conditions manager (convenience)
void dd4hep::cond::fill_content(ConditionsManager mgr,
                                ConditionsContent& content,
                                const IOVType& typ)
{
  ConditionsIOVPool* iovPool = mgr.iovPool(typ);
  ConditionsIOVPool::Elements& pools = iovPool->elements;
  for_each(begin(pools),end(pools),SliceOper(content));
}

