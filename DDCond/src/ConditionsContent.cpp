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
#include "DDCond/ConditionsContent.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/Printout.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::cond;

/// Default constructor
ConditionsLoadInfo::ConditionsLoadInfo()   {
  InstanceCount::increment(this);  
}

/// Default destructor. 
ConditionsLoadInfo::~ConditionsLoadInfo()  {
  InstanceCount::decrement(this);  
}

/// Initializing constructor
ConditionsContent::ConditionsContent()
{
  InstanceCount::increment(this);  
}

/// Default destructor. 
ConditionsContent::~ConditionsContent()   {
  detail::releaseObjects(m_derived);
  detail::releaseObjects(m_conditions);
  InstanceCount::decrement(this);  
}

/// Clear the container. Destroys the contained stuff
void ConditionsContent::clear()   {
  detail::releaseObjects(m_derived);
  detail::releaseObjects(m_conditions);
}

/// Merge the content of "to_add" into the this content
void ConditionsContent::merge(const ConditionsContent& to_add)    {
  auto& cond  = to_add.conditions();
  auto& deriv = to_add.derived();
  for( const auto& c : cond )   {
    auto ret = m_conditions.emplace(c);
    if ( ret.second )  {
      c.second->addRef();
      continue;
    }
    // Need error handling here ?
    ConditionKey key(c.first);
    printout(WARNING,"ConditionsContent",
             "++ Condition %s already present in content. Not merged",key.toString().c_str());
             
  }
  for( const auto& d : deriv )   {
    auto ret = m_derived.emplace(d);
    if ( ret.second )  {
      d.second->addRef();
      continue;
    }
    // Need error handling here ?
    ConditionKey key(d.first);
    printout(WARNING,"ConditionsContent",
             "++ Dependency %s already present in content. Not merged",key.toString().c_str());
  }
}

/// Remove a new shared condition
bool ConditionsContent::remove(Condition::key_type hash)   {
  auto i = m_conditions.find(hash);
  if ( i != m_conditions.end() )  {
    detail::releasePtr((*i).second);
    m_conditions.erase(i);
    return true;
  }
  auto j = m_derived.find(hash);
  if ( j != m_derived.end() )  {
    detail::releasePtr((*j).second);
    m_derived.erase(j);
    return true;
  }
  return false;
}

pair<Condition::key_type, ConditionsLoadInfo*>
ConditionsContent::insertKey(Condition::key_type hash)   {
  auto ret = m_conditions.emplace(hash,(ConditionsLoadInfo*)0);
  if ( ret.second )  return pair<Condition::key_type, ConditionsLoadInfo*>(hash,0);
  return pair<Condition::key_type, ConditionsLoadInfo*>(0,0);
}

/// Add a new conditions key. T must inherit from class ConditionsContent::Info
pair<Condition::key_type, ConditionsLoadInfo*>
ConditionsContent::addLocationInfo(Condition::key_type hash, ConditionsLoadInfo* info)   {
  if ( info )   {
    auto ret = m_conditions.emplace(hash,info);
    if ( ret.second )  {
      info->addRef();
      return *(ret.first);
    }
    info->release();
  }
  return pair<Condition::key_type, ConditionsLoadInfo*>(0,0);
}

/// Add a new shared conditions dependency
pair<Condition::key_type, ConditionDependency*>
ConditionsContent::addDependency(ConditionDependency* dep)
{
  auto ret = m_derived.emplace(dep->key(),dep);
  if ( ret.second )  {
    dep->addRef();
    return *(ret.first);
  }
  ConditionKey::KeyMaker km(dep->target.hash);
#if defined(DD4HEP_CONDITIONS_DEBUG)
  DetElement             de(dep->detector);
  const char* path = de.isValid() ? de.path().c_str() : "(global)";
#else
  const char* path = "";
#endif
  dep->release();
  except("DeConditionsRequests",
         "++ Dependency already exists: %s [%08X] [%016llX]",
         path, km.values.item_key, km.hash);
  return pair<Condition::key_type, ConditionDependency*>(0,0);
}

/// Add a new conditions dependency (Built internally from arguments)
std::pair<Condition::key_type, ConditionDependency*>
ConditionsContent::addDependency(DetElement de,
                                 Condition::itemkey_type item,
                                 std::shared_ptr<ConditionUpdateCall> callback)
{
  ConditionDependency* dep = new ConditionDependency(de, item, callback);
  return addDependency(dep);
}

