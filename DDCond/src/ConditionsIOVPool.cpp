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
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DDCond/ConditionsIOVPool.h"
#include "DD4hep/detail/ConditionsInterna.h"
#include "DDCond/ConditionsDataLoader.h"

using namespace DD4hep;
using namespace DD4hep::Conditions;

/// Default constructor
ConditionsIOVPool::ConditionsIOVPool(const IOVType* typ) : type(typ)  {
  InstanceCount::increment(this);
}

/// Default destructor
ConditionsIOVPool::~ConditionsIOVPool()  {
  clean(-1);
  InstanceCount::decrement(this);
}

size_t ConditionsIOVPool::select(Condition::key_type key, const Condition::iov_type& req_validity, RangeConditions& result)
{
  if ( !elements.empty() )  {
    size_t len = result.size();
    const IOV::Key req_key = req_validity.key(); // 16 bytes => better copy!
    for(Elements::const_iterator i=elements.begin(); i!=elements.end(); ++i)  {
      if ( IOV::key_contains_range((*i).first, req_key) )  {
        (*i).second->select(key, result);
      }
    }
    return result.size() - len;
  }
  return 0;
}

size_t ConditionsIOVPool::selectRange(Condition::key_type key, const Condition::iov_type& req_validity, RangeConditions& result)
{
  size_t len = result.size();
  const IOV::Key range = req_validity.key();
  for(Elements::const_iterator i=elements.begin(); i!=elements.end(); ++i)  {
    const IOV::Key& k = (*i).first;
    if ( IOV::key_is_contained(k,range) )
      // IOV test contained in key. Take it!
      (*i).second->select(key, result);
    else if ( IOV::key_overlaps_lower_end(k,range) )
      // IOV overlap on test on the lower end of key
      (*i).second->select(key, result);
    else if ( IOV::key_overlaps_higher_end(k,range) )
      // IOV overlap of test on the higher end of key
      (*i).second->select(key, result);
  }
  return result.size() - len;
}

/// Remove all key based pools with an age beyon the minimum age
int ConditionsIOVPool::clean(int max_age)   {
  Elements rest;
  int count = 0;
  for(Elements::const_iterator i=elements.begin(); i!=elements.end(); ++i)  {
    ConditionsPool* pool = (*i).second;
    if ( pool->age_value >= max_age )   {
      count += pool->size();
      pool->print("Remove");
      delete pool;
    }
    else
      rest.insert(make_pair(pool->iov->keyData,pool));
  }
  elements = rest;
  return count;
}

/// Select all ACTIVE conditions, which do match the IOV requirement
size_t ConditionsIOVPool::select(const IOV&        req_validity, 
                                 RangeConditions&  valid,
                                 IOV&              cond_validity)
{
  size_t num_selected = 0;
  if ( !elements.empty() )  {
    const IOV::Key req_key = req_validity.key(); // 16 bytes => better copy!
    for(Elements::const_iterator i=elements.begin(); i!=elements.end(); ++i)  {
      ConditionsPool* pool = (*i).second;
      if ( !IOV::key_contains_range((*i).first, req_key) )  {
        ++pool->age_value;
        continue;
      }
      cond_validity.iov_intersection((*i).first);
      num_selected += pool->select_all(valid);
      pool->age_value = 0;
    }
  }
  return num_selected;
}

/// Select all ACTIVE conditions, which do match the IOV requirement
size_t ConditionsIOVPool::select(const IOV&              req_validity, 
                                 const ConditionsSelect& predicate_processor,
                                 IOV&                    cond_validity)
{
  size_t num_selected = 0;
  if ( !elements.empty() )  {
    const IOV::Key req_key = req_validity.key(); // 16 bytes => better copy!
    for(const auto& i : elements )  {
      ConditionsPool* pool = i.second;
      if ( !IOV::key_contains_range(i.first, req_key) )  {
        ++pool->age_value;
        continue;
      }
      cond_validity.iov_intersection(i.first);
      num_selected += pool->select_all(predicate_processor);
      pool->age_value = 0;
    }
  }
  return num_selected;
}

/// Select all ACTIVE conditions, which do match the IOV requirement
size_t ConditionsIOVPool::select(const IOV& req_validity, 
                                 Elements&  valid,
                                 IOV&       cond_validity)
{
  size_t num_selected = 0;
  if ( !elements.empty() )   {
    const IOV::Key req_key = req_validity.key(); // 16 bytes => better copy!
    for(const auto& i : elements )  {
      ConditionsPool* pool = i.second;
      if ( !IOV::key_contains_range(i.first, req_key) )  {
        ++pool->age_value;
        continue;
      }
      cond_validity.iov_intersection(i.first);
      valid[i.first] = pool;
      pool->age_value = 0;
      ++num_selected;
    }
  }
  return num_selected;
}
