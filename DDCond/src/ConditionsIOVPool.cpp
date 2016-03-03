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

void ConditionsIOVPool::__find(DetElement detector,
                               const std::string& condition_name,
                               const IOV& req_validity,
                               RangeConditions& result)
{
  if ( !entries.empty() )  {
    const IOV::Key req_key = req_validity.key(); // 8 bytes => better copy!
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

/// Select all ACTIVE conditions, which do no longer match the IOV requirement
void ConditionsIOVPool::__select_expired(const IOV& required_validity, RangeConditions& result)  {
  if ( !entries.empty() )  {
    const IOV::Key req_key = required_validity.key(); // 8 bytes => better copy!
    for(Entries::const_iterator i=entries.begin(); i!=entries.end(); ++i)  {
      ConditionsPool* pool = (*i).second;
      if ( !IOV::key_contains_range((*i).first, req_key) )  {
        if ( pool->age_value == ConditionsPool::AGE_NONE ) {
          // Now check the content:
          pool->select_used(result);
        }
        ++pool->age_value;
        continue;
      }
      pool->age_value = 0;
    }
  }
}

/// Select all ACTIVE conditions, which do no longer match the IOV requirement
void ConditionsIOVPool::__update_expired(ConditionsDataLoader& loader,
                                         ConditionsPool& new_pool,
                                         RangeConditions& updates,
                                         const IOV& required_validity)  {

  RangeConditions upda;
  for(RangeConditions::const_iterator i=updates.begin(); i!=updates.end(); ++i)  {
    Condition::Object* condition = (*i).ptr();
    Condition c = new_pool.exists(condition);
    if ( c.isValid() )  {
      upda.push_back(c);
      continue;
    }
    size_t items = loader.load(condition->detector,condition->name,required_validity,upda);
    if ( items < 1 )  {
      // Error: no such condition
      except("ConditionsManager","+++ update_expired: Cannot update condition %s.%s [%s] to iov:%s.",
             condition->detector.path().c_str(), condition->name.c_str(),
             condition->iov->str().c_str(), required_validity.str().c_str());
    }
  }
  updates = upda;
}
