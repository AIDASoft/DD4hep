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
  InstanceCount::decrement(this);
}

bool ConditionsIOVPool::addKey(Condition c)   {
  // TODO: Should be: det.path()+'#'+c->name; instead of c->comment
  int hash = c->hash;
  c->flags |= Interna::ConditionObject::ACTIVE;
  keys.insert(hash);
  return true;
}

void ConditionsIOVPool::__find(DetElement detector,
                               const std::string& condition_name,
                               const IOV& req_validity,
                               RangeConditions& result)
{
  if ( !entries.empty() )  {
    const IOV::Key req_key = req_validity.key(); // 16 bytes => better copy!
    for(Entries::const_iterator i=entries.begin(); i!=entries.end(); ++i)  {
      if ( IOV::key_contains_range((*i).first, req_key) )  {
        (*i).second->select(detector, condition_name, result);
      }
    }
  }
}

void ConditionsIOVPool::__find_range(DetElement detector,
                                     const std::string& condition_name,
                                     const IOV& req_validity,
                                     RangeConditions& result)
{
  const IOV::Key range = req_validity.key();
  for(Entries::const_iterator i=entries.begin(); i!=entries.end(); ++i)  {
    const IOV::Key& key = (*i).first;
    if ( IOV::key_is_contained(key,range) )
      // IOV test contained in key. Take it!
      (*i).second->select(detector, condition_name, result);
    else if ( IOV::key_overlaps_lower_end(key,range) )
      // IOV overlap on test on the lower end of key
      (*i).second->select(detector, condition_name, result);
    else if ( IOV::key_overlaps_higher_end(key,range) )
      // IOV overlap of test on the higher end of key
      (*i).second->select(detector, condition_name, result);
  }
}

/// Remove all key based pools with an age beyon the minimum age
int ConditionsIOVPool::clean(int max_age)   {
  Entries rest;
  int count = 0;
  for(Entries::const_iterator i=entries.begin(); i!=entries.end(); ++i)  {
    ConditionsPool* pool = (*i).second;
    if ( pool->age_value >= max_age )   {
      count += pool->count();
      pool->print("Remove");
      delete pool;
    }
    else
      rest.insert(make_pair(pool->iov->keyData,pool));
  }
  entries = rest;
  return count;
}

/// Select all ACTIVE conditions, which do no longer match the IOV requirement
void ConditionsIOVPool::select(const IOV& required_validity, 
			       RangeConditions& valid,
			       RangeConditions& expired,
			       IOV& conditions_validity)
{
  if ( !entries.empty() )  {
    const IOV::Key req_key = required_validity.key(); // 16 bytes => better copy!
    for(Entries::const_iterator i=entries.begin(); i!=entries.end(); ++i)  {
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
