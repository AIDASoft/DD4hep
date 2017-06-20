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
#include "DD4hep/detail/ConditionsInterna.h"
#include "DDCond/ConditionsDataLoader.h"

using namespace dd4hep;
using namespace dd4hep::cond;

/// Default constructor
ConditionsIOVPool::ConditionsIOVPool(const IOVType* typ) : type(typ)  {
  InstanceCount::increment(this);
}

/// Default destructor
ConditionsIOVPool::~ConditionsIOVPool()  {
  clean(-1);
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
  size_t num_selected = 0;
  if ( !elements.empty() )  {
    const IOV::Key req_key = req_validity.key(); // 16 bytes => better copy!
    for( const auto& i : elements )  {
      if ( !IOV::key_contains_range(i.first, req_key) )  {
        ++i.second->age_value;
        continue;
      }
      cond_validity.iov_intersection(i.first);
      num_selected += i.second->select_all(predicate_processor);
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
  size_t num_selected = 0;
  if ( !elements.empty() )   {
    const IOV::Key req_key = req_validity.key(); // 16 bytes => better copy!
    for( const auto& i : elements )  {
      if ( !IOV::key_contains_range(i.first, req_key) )  {
        ++i.second->age_value;
        continue;
      }
      cond_validity.iov_intersection(i.first);
      valid[i.first] = i.second;
      i.second->age_value = 0;
      ++num_selected;
    }
  }
  return num_selected;
}
