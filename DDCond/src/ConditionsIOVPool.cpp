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
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DDCond/ConditionsIOVPool.h"
#include "DDCond/ConditionsCleanup.h"
#include "DDCond/ConditionsDataLoader.h"

#include "DD4hep/detail/ConditionsInterna.h"

using namespace dd4hep;
using namespace dd4hep::cond;

/// Default constructor
ConditionsIOVPool::ConditionsIOVPool(const IOVType* typ) : type(typ)  {
  InstanceCount::increment(this);
}

/// Default destructor
ConditionsIOVPool::~ConditionsIOVPool()  {
  try {
    clean(-1);
  }
  catch(const std::exception& e)   {
    printout(ERROR,"ConditionsIOVPool","+++ Unexpected exception in destructor(ConditionsIOVPool): %s",e.what());
  }
  InstanceCount::decrement(this);
}

size_t ConditionsIOVPool::select(Condition::key_type key, const IOV& req_validity, RangeConditions& result)
{
  if ( !elements.empty() )  {
    size_t len = result.size();
    const IOV::Key req_key = req_validity.key(); // 16 bytes => better copy!
    for( const auto& e : elements )  {
      if ( IOV::key_contains_range(e.first, req_key) )  {
        e.second->select(key, result);
      }
    }
    return result.size() - len;
  }
  return 0;
}

size_t ConditionsIOVPool::selectRange(Condition::key_type key, const IOV& req_validity, RangeConditions& result)
{
  size_t len = result.size();
  const IOV::Key range = req_validity.key();
  for( const auto& e : elements )  {
    const IOV::Key& k = e.first;
    if ( IOV::key_is_contained(k,range) )
      // IOV test contained in key. Take it!
      e.second->select(key, result);
    else if ( IOV::key_overlaps_lower_end(k,range) )
      // IOV overlap on test on the lower end of key
      e.second->select(key, result);
    else if ( IOV::key_overlaps_higher_end(k,range) )
      // IOV overlap of test on the higher end of key
      e.second->select(key, result);
  }
  return result.size() - len;
}

/// Invoke cache cleanup with user defined policy
int ConditionsIOVPool::clean(const ConditionsCleanup& cleaner)   {
 Elements rest;
  int count = 0;
  for( const auto& e : elements )  {
    const ConditionsPool* p = e.second.get();
    if ( cleaner (*p) )   {
      count += e.second->size();
      e.second->print("Remove");
      continue;
    }
    rest.insert(e);
  }
  elements = std::move(rest);
  return count;  
}

/// Remove all key based pools with an age beyon the minimum age
int ConditionsIOVPool::clean(int max_age)   {
  Elements rest;
  int count = 0;
  for( const auto& e : elements )  {
    if ( e.second->age_value >= max_age )   {
      count += e.second->size();
      e.second->print("Remove");
    }
    else  {
      rest.insert(e);
    }
  }
  elements = std::move(rest);
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
    for( const auto& i : elements )  {
      if ( !IOV::key_contains_range(i.first, req_key) )  {
        ++i.second->age_value;
        continue;
      }
      cond_validity.iov_intersection(i.first);
      num_selected += i.second->select_all(valid);
      i.second->age_value = 0;
    }
  }
  return num_selected;
}

/// Select all ACTIVE conditions, which do match the IOV requirement
size_t ConditionsIOVPool::select(const IOV&              req_validity, 
                                 const ConditionsSelect& predicate_processor,
                                 IOV&                    cond_validity)
{
  size_t num_selected = 0, pool_selected = 0;
  if ( !elements.empty() )  {
    const IOV::Key req_key = req_validity.key(); // 16 bytes => better copy!
    for( const auto& i : elements )  {
      if ( !IOV::key_contains_range(i.first, req_key) )  {
        ++i.second->age_value;
        continue;
      }
      cond_validity.iov_intersection(i.first);
      pool_selected = i.second->select_all(predicate_processor);
      num_selected += pool_selected;
      i.second->age_value = 0;
    }
  }
  return num_selected;
}

/// Select all ACTIVE conditions, which do match the IOV requirement
size_t ConditionsIOVPool::select(const IOV& req_validity, 
                                 Elements&  valid,
                                 IOV&       cond_validity)
{
  size_t num_selected = select(req_validity, valid);
  cond_validity.invert().reset();
  for( const auto& i : valid )
    cond_validity.iov_intersection(*(i.second->iov));
  return num_selected;
}

/// Select all ACTIVE conditions, which do match the IOV requirement
size_t ConditionsIOVPool::select(const IOV& req_validity, Elements&  valid)
{
  size_t num_selected = 0;
  if ( !elements.empty() )   {
    const IOV::Key req_key = req_validity.key(); // 16 bytes => better copy!
    for( const auto& i : elements )  {
      if ( !IOV::key_contains_range(i.first, req_key) )  {
        continue;
      }
      valid[i.first] = i.second;
      ++num_selected;
    }
  }
  return num_selected;
}

/// Select all ACTIVE conditions, which do match the IOV requirement
size_t ConditionsIOVPool::select(const IOV&        req_validity, 
                                 std::vector<Element>&  valid,
                                 IOV&              cond_validity)
{
  size_t num_selected = select(req_validity, valid);
  cond_validity.invert().reset();
  for( const auto& i : valid )
    cond_validity.iov_intersection(*(i->iov));
  return num_selected;
}

/// Select all ACTIVE conditions, which do match the IOV requirement
size_t ConditionsIOVPool::select(const IOV& req_validity, std::vector<Element>& valid)
{
  size_t num_selected = 0;
  if ( !elements.empty() )   {
    const IOV::Key req_key = req_validity.key(); // 16 bytes => better copy!
    for( const auto& i : elements )  {
      if ( !IOV::key_contains_range(i.first, req_key) )  {
        continue;
      }
      valid.emplace_back(i.second);
      ++num_selected;
    }
  }
  return num_selected;
}
