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
#ifndef DDCOND_CONDITIONSMAPPEDUSERPOOL_H
#define DDCOND_CONDITIONSMAPPEDUSERPOOL_H

// Framework include files
#include "DDCond/ConditionsPool.h"

// C/C++ include files
#include <map>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Conditions {

    /// Class implementing the conditions user pool for a given IOV type
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    template<typename MAPPING> 
    class ConditionsMappedUserPool : public UserPool
    {
      typedef MAPPING Mapping;
      Mapping        m_conditions;

      Condition::Object* i_findCondition(key_type key)  const;

    public:
      /// Default constructor
      ConditionsMappedUserPool(ConditionsManager mgr, ConditionsIOVPool* pool);
      /// Default destructor
      virtual ~ConditionsMappedUserPool();
      /// Print pool content
      virtual void print(const std::string& opt)   const;
      /// Total entry count
      virtual size_t count()  const;
      /// Full cleanup of all managed conditions.
      virtual void clear();
      /// Check a condition for existence
      virtual bool exists(key_type hash)  const;
      /// Check a condition for existence
      virtual bool exists(const ConditionKey& key)  const;
      /// Check if a condition exists in the pool and return it to the caller
      virtual Condition get(key_type hash)  const;
      /// Check if a condition exists in the pool and return it to the caller     
      virtual Condition get(const ConditionKey& key)  const;
      /// Remove condition by key from pool.
      virtual bool remove(key_type hash_key);
      /// Remove condition by key from pool.
      virtual bool remove(const ConditionKey& key);
      /// Register a new condition to this pool
      virtual bool insert(Condition cond);
      /// Prepare user pool for usage (load, fill etc.) according to required IOV
      virtual long prepare(const IOV& required);
      /// Evaluate and register all derived conditions from the dependency list
      virtual long compute(const Dependencies& dependencies, void* user_param);
    };

  } /* End namespace Conditions             */
} /* End namespace DD4hep                   */
#endif // DDCOND_CONDITIONSMAPPEDUSERPOOL_H

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
//#include "DDCond/ConditionsMappedPool.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Factories.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/objects/ConditionsInterna.h"
#include "DDCond/ConditionsInterna.h"
#include "DDCond/ConditionsIOVPool.h"
#include "DDCond/ConditionsSelectors.h"
#include "DDCond/ConditionsDependencyHandler.h"

using namespace DD4hep::Conditions;
using namespace std;

namespace {
  template <typename T> struct Inserter {
    T& m;
    Inserter(T& o) : m(o) {}
    void operator()(Condition& c)  {
      Condition::Object* o = c.ptr();
      m.insert(make_pair(o->hash,o));
    }
  };
}

/// Default constructor
template<typename MAPPING>
ConditionsMappedUserPool<MAPPING>::ConditionsMappedUserPool(ConditionsManager mgr, ConditionsIOVPool* pool) 
  : UserPool(mgr, pool)
{
  InstanceCount::increment(this);
}

/// Default destructor
template<typename MAPPING>
ConditionsMappedUserPool<MAPPING>::~ConditionsMappedUserPool()  {
  clear();
  InstanceCount::decrement(this);
}

template<typename MAPPING> inline Condition::Object* 
ConditionsMappedUserPool<MAPPING>::i_findCondition(key_type key)  const {
  typename MAPPING::const_iterator i=m_conditions.find(key);
  return i != m_conditions.end() ? (*i).second : 0;
}

/// Total entry count
template<typename MAPPING>
size_t ConditionsMappedUserPool<MAPPING>::count()  const  {
  return  m_conditions.size();
}

/// Print pool content
template<typename MAPPING>
void ConditionsMappedUserPool<MAPPING>::print(const std::string& opt)   const  {
  const IOV* iov = &m_iov;
  printout(INFO,"UserPool","+++ %s Conditions for USER pool with IOV: %-32s [%4d entries]",
           opt.c_str(), iov->str().c_str(), count());
  if ( opt == "*" ) {
    typename MAPPING::const_iterator i=m_conditions.begin();
    for( ; i != m_conditions.end(); ++i)   {
      Condition c = (*i).second;
      printout(INFO,"UserPool","++ %08X/%08X Val:%s %s",(*i).first, c->hash, c->value.c_str(), c.str().c_str());
    }
  }
}

/// Full cleanup of all managed conditions.
template<typename MAPPING>
void ConditionsMappedUserPool<MAPPING>::clear()   {
  m_iov = IOV(0);
  m_conditions.clear();
}

/// Check a condition for existence
template<typename MAPPING>
bool ConditionsMappedUserPool<MAPPING>::exists(key_type hash)  const   {
  return i_findCondition(hash) != 0;
}

/// Check a condition for existence
template<typename MAPPING>
bool ConditionsMappedUserPool<MAPPING>::exists(const ConditionKey& key)  const   {
  return i_findCondition(key.hash) != 0;
}

/// Check if a condition exists in the pool and return it to the caller
template<typename MAPPING>
Condition ConditionsMappedUserPool<MAPPING>::get(key_type hash)  const   {
  return i_findCondition(hash);
}

/// Check if a condition exists in the pool and return it to the caller     
template<typename MAPPING>
Condition ConditionsMappedUserPool<MAPPING>::get(const ConditionKey& key)  const   {
  return i_findCondition(key.hash);
}

/// Register a new condition to this pool
template<typename MAPPING>
bool ConditionsMappedUserPool<MAPPING>::insert(Condition cond)   {
  Condition::Object* o = cond.ptr();
  pair<typename MAPPING::iterator,bool> res = m_conditions.insert(make_pair(o->hash,o));
  return res.second;
}

/// Remove condition by key from pool.
template<typename MAPPING>
bool ConditionsMappedUserPool<MAPPING>::remove(const ConditionKey& key)   {
  return remove(key.hash);
}

/// Remove condition by key from pool.
template<typename MAPPING>
bool ConditionsMappedUserPool<MAPPING>::remove(key_type hash_key)    {
  typename MAPPING::iterator i=m_conditions.find(hash_key);
  if ( i != m_conditions.end() ) {
    m_conditions.erase(i);
    return true;
  }
  return false;
}

/// Prepare user pool for usage (load, fill etc.) according to required IOV
template<typename MAPPING>
long ConditionsMappedUserPool<MAPPING>::prepare(const IOV& required)   {
  if ( m_iovPool )   {
    RangeConditions valid, expired;
    IOV pool_iov(required.iovType);
    pool_iov.reset().invert();
    m_iovPool->select(required, valid, expired, pool_iov);
    clear();
    for_each(valid.begin(),valid.end(),Inserter<MAPPING>(m_conditions));
    long num_expired = (long)expired.size();
    if ( num_expired > 0 )  {
      m_manager->loader()->update(required, expired, pool_iov);
      for_each(expired.begin(),expired.end(),Inserter<MAPPING>(m_conditions));
    }
    m_iov = pool_iov;
    return num_expired;
  }
  DD4hep::except("ConditionsMappedUserPool","++ Invalid reference to iov pool! [Internal Error]");
  return -1;
}

/// Evaluate and register all derived conditions from the dependency list
template<typename MAPPING>
long ConditionsMappedUserPool<MAPPING>::compute(const Dependencies& deps, void* user_param)  {
  long num_updates = 0;
  if ( !deps.empty() )  {
    typedef Dependencies _D;
    ConditionsDependencyHandler handler(m_manager.ptr(), *this, deps, user_param);
    ConditionsPool* pool = m_manager->registerIOV(*m_iov.iovType, m_iov.keyData);
    // Loop over the dependencies and check if they have to be upgraded
    for(_D::const_iterator i = deps.begin(); i!=deps.end(); ++i)  {
      key_type key = (*i).first;
      typename MAPPING::iterator j = m_conditions.find(key);
      if ( j != m_conditions.end() )  {
        Condition::Object* cond = (*j).second;
        if ( IOV::key_is_contained(m_iov.keyData,cond->iov->keyData) )
          continue;
        /// This condition is no longer valid. remove it! Will be added again afterwards.
        m_conditions.erase(j);
      }
      const ConditionDependency* d = (*i).second.get();
      Condition::Object* cond = handler(d);
      m_manager->registerUnlocked(pool, cond); // Would bulk update be more efficient?
      ++num_updates;
    }
  }
  return num_updates;
}

namespace {
  void* create_user_pool(DD4hep::Geometry::LCDD&, int argc, char** argv)  {
    if ( argc > 1 )  {
      ConditionsManager::Object* m = (ConditionsManager::Object*)argv[0];
      ConditionsIOVPool* p = (ConditionsIOVPool*)argv[1];
      UserPool* pool = new ConditionsMappedUserPool<std::map<Condition::key_type,Condition::Object*> >(m, p);
      return pool;
    }
    DD4hep::except("ConditionsMappedUserPool","++ Insufficient arguments: arg[0] = ConditionManager!");
    return 0;
  }
}
DECLARE_LCDD_CONSTRUCTOR(DD4hep_ConditionsMapUserPool, create_user_pool)
