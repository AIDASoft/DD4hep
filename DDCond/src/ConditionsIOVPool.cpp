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
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DDCond/ConditionsIOVPool.h"
#include "DD4hep/objects/ConditionsInterna.h"
#include "DDCond/ConditionsDataLoader.h"

using namespace DD4hep;
using namespace DD4hep::Conditions;

/// Default constructor
ConditionsIOVPool::ConditionsIOVPool()  {
  InstanceCount::increment(this);
}

/// Default destructor
ConditionsIOVPool::~ConditionsIOVPool()  {
  clean(-1);
  InstanceCount::decrement(this);
}

void ConditionsIOVPool::select(Condition::key_type key, const Condition::iov_type& req_validity, RangeConditions& result)
{
  if ( !elements.empty() )  {
    const IOV::Key req_key = req_validity.key(); // 16 bytes => better copy!
    for(Elements::const_iterator i=elements.begin(); i!=elements.end(); ++i)  {
      if ( IOV::key_contains_range((*i).first, req_key) )  {
        (*i).second->select(key, result);
      }
    }
  }
}

void ConditionsIOVPool::selectRange(Condition::key_type key, const Condition::iov_type& req_validity, RangeConditions& result)
{
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
}

/// Remove all key based pools with an age beyon the minimum age
int ConditionsIOVPool::clean(int max_age)   {
  Elements rest;
  int count = 0;
  for(Elements::const_iterator i=elements.begin(); i!=elements.end(); ++i)  {
    ConditionsPool* pool = (*i).second;
    if ( pool->age_value >= max_age )   {
      count += pool->count();
      pool->print("Remove");
      delete pool;
    }
    else
      rest.insert(make_pair(pool->iov->keyData,pool));
  }
  elements = rest;
  return count;
}

/// Select all ACTIVE conditions, which do no longer match the IOV requirement
void ConditionsIOVPool::select(const Condition::iov_type& required_validity, 
                               RangeConditions& valid,
                               RangeConditions& expired,
                               Condition::iov_type& conditions_validity)
{
  if ( !elements.empty() )  {
    const IOV::Key req_key = required_validity.key(); // 16 bytes => better copy!
    for(Elements::const_iterator i=elements.begin(); i!=elements.end(); ++i)  {
      ConditionsPool* pool = (*i).second;
      if ( !IOV::key_contains_range((*i).first, req_key) )  {
        if ( pool->age_value == ConditionsPool::AGE_NONE ) {
          // Now check the content:
          pool->select_used(expired);
        }
        ++pool->age_value;
        continue;
      }
      conditions_validity.iov_intersection((*i).first);
      pool->select_used(valid);
      pool->age_value = 0;
    }
  }
}
