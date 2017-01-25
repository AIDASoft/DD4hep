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
#include "DDCond/ConditionsSlice.h"
#include "DDCond/ConditionsIOVPool.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/Printout.h"

using namespace DD4hep::Conditions;

/// Default destructor. 
ConditionsLoadInfo::~ConditionsLoadInfo()  {
}

/// Initializing constructor
ConditionsDescriptor::ConditionsDescriptor(const ConditionKey& k, ConditionsLoadInfo* l)
  : key(k), loadinfo(l)
{
  InstanceCount::increment(this);  
}

/// Initializing constructor
ConditionsDescriptor::ConditionsDescriptor(ConditionDependency* dep, ConditionsLoadInfo* l)
  : key(dep->target), dependency(dep->addRef()), loadinfo(l)
{
  InstanceCount::increment(this);  
}

/// Default destructor. 
ConditionsDescriptor::~ConditionsDescriptor()  {
  releasePtr(dependency);
  InstanceCount::decrement(this);  
}

/// Initializing constructor
ConditionsSlice::ConditionsSlice(ConditionsManager m) : manager(m)
{
  InstanceCount::increment(this);  
}

/// Copy constructor (Special, partial copy only. Hence no assignment!)
ConditionsSlice::ConditionsSlice(const ConditionsSlice& copy)
  : manager(copy.manager)
{
  InstanceCount::increment(this);  
  for(const auto& c : copy.m_conditions )  {
    Descriptor* e = c.second->addRef();
    m_conditions.insert(std::make_pair(e->key.hash,e));
  }
  for(const auto& c : copy.m_derived )  {
    Descriptor* e = c.second->addRef();
    m_derived.insert(std::make_pair(e->key.hash,e));
  }
}

/// Default destructor. 
ConditionsSlice::~ConditionsSlice()   {
  releaseObjects(m_conditions);
  releaseObjects(m_derived);
  InstanceCount::decrement(this);  
}

/// Clear the container. Destroys the contained stuff
void ConditionsSlice::clear()   {
  releaseObjects(m_conditions);
  releaseObjects(m_derived);
}

/// Clear the conditions access and the user pool.
void ConditionsSlice::reset()   {
  if ( pool.get() ) pool->clear();
}

/// Add a new conditions dependency collection
void ConditionsSlice::insert(const ConditionsDependencyCollection& deps)   {
  for ( const auto& d : deps ) this->insert(d.second.get());
}

/// Add a new conditions dependency (shared)
bool ConditionsSlice::insert(Dependency* dependency)   {
  Descriptor* entry = new Descriptor(dependency,0);
  if ( m_derived.insert(std::make_pair(entry->key.hash,entry->addRef())).second )  {
    return true;
  }
  delete entry;
  return false;
}

/// Add a new entry
bool ConditionsSlice::insert_condition(Descriptor* entry)   {
  if ( entry->dependency )   {
    // ERROR: This call should not be invoked for derivatives!
    DD4hep::except("ConditionsSlice",
                   "insert_condition: Bad invokation. No dependency allowed here!");
  }
  if ( m_conditions.insert(std::make_pair(entry->key.hash,entry->addRef())).second )  {
    return true;
  }
  releasePtr(entry);
  return false;
}

/// Add a new condition to the user pool
bool ConditionsSlice::insert_condition(Condition condition)   {
  if ( condition.isValid() )  {
    return pool->insert(condition);
  }
  DD4hep::except("ConditionsSlice",
                 "insert_condition: Cannot insert invalid conditions to the user pool!");
  return false;
}

namespace  {
  
  struct SliceOper  : public ConditionsSelect  {
    ConditionsSlice* slice;
    SliceOper(ConditionsSlice* s) : slice(s) {}
    void operator()(const ConditionsIOVPool::Elements::value_type& v)    {
      v.second->select_all(*this);
    }
    bool operator()(Condition::Object* c)  const  {
      if ( 0 == (c->flags&Condition::DERIVED) )   {
        slice->insert(ConditionKey(c->name,c->hash),ConditionsSlice::loadInfo(c->address));
        return true;
      }
      //DD4hep::printout(DD4hep::INFO,"Slice","++ Ignore dependent condition: %s",c->name.c_str());
#if 0
      const ConditionsSlice::ConditionsProxy& cc=slice->conditions();
      auto i = cc.find(c->hash);
      const ConditionsSlice::LoadInfo<std::string>* info =
        (const ConditionsSlice::LoadInfo<std::string>*)(*i).second->loadinfo;
      std::string* address = info->data<std::string>();
#endif      
      return true;
    }
    /// Return number of conditions selected
    virtual size_t size()  const { return slice->conditions().size(); }
  };
}

/// Populate the conditions slice from the conditions manager (convenience)
ConditionsSlice*
DD4hep::Conditions::createSlice(ConditionsManager mgr, const IOVType& typ)  {
  dd4hep_ptr<ConditionsSlice> slice(new ConditionsSlice(mgr));
  Conditions::ConditionsIOVPool* iovPool = mgr.iovPool(typ);
  Conditions::ConditionsIOVPool::Elements& pools = iovPool->elements;
  for_each(begin(pools),end(pools),SliceOper(slice.get()));
  return slice.release();
}
