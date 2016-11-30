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
#include "DD4hep/Printout.h"

using namespace DD4hep::Conditions;

/// Default destructor. 
ConditionsSlice::Info::~Info()  {
}

/// Copy constructor (special!)
ConditionsSlice::Entry::Entry(const Entry& copy, Info* l)
  : key(copy.key), condition(0), loadinfo(l)
{
  if ( copy.dependency ) dependency = copy.dependency->addRef();
}

/// Initializing constructor
ConditionsSlice::Entry::Entry(const ConditionKey& k, Info* l)
  : key(k), condition(0), loadinfo(l)
{
}

/// Clone the entry
ConditionsSlice::Entry* ConditionsSlice::Entry::clone()   {
  Entry* e = new Entry(*this);
  return e;
}

/// Default destructor. 
ConditionsSlice::Entry::~Entry()  {
  releasePtr(dependency);
}

/// Copy constructor (Special, partial copy only. Hence no assignment!)
ConditionsSlice::ConditionsSlice(const ConditionsSlice& copy)
  : manager(copy.manager), m_iov(copy.m_iov.iovType)
{
  for(const auto& c : copy.m_conditions )  {
    Entry* e = new Entry(*c.second);
    m_conditions.insert(std::make_pair(e->key.hash,e));
  }
  for(const auto& c : copy.m_derived )  {
    Entry* e = new Entry(*c.second);
    if ( m_derived.insert(std::make_pair(e->key.hash,e)).second )  {
      if ( e->dependency ) m_dependencies.insert(e->dependency);
    }
  }
}

/// Default destructor. 
ConditionsSlice::~ConditionsSlice()   {
  destroyObjects(m_conditions);
  destroyObjects(m_derived);
  m_dependencies.clear();
}

/// Clear the container. Destroys the contained stuff
void ConditionsSlice::clear()   {
  destroyObjects(m_conditions);
  destroyObjects(m_derived);
  m_dependencies.clear();
}

/// Clear the conditions access and the user pool.
void ConditionsSlice::reset()   {
  for(const auto& e : m_conditions ) e.second->condition = 0;
  for(const auto& e : m_derived ) e.second->condition = 0;
  if ( pool().get() ) pool()->clear();
  m_iov.reset();
}

/// Add a new conditions dependency collection
void ConditionsSlice::insert(const Dependencies& deps)   {
  for ( const auto& d : deps ) insert(d.second.get());
}

/// Add a new conditions dependency (shared)
bool ConditionsSlice::insert(Dependency* dependency)   {
  Entry* entry = new Entry();
  entry->dependency = dependency->addRef();
  entry->mask |= Condition::DERIVED;
  entry->key = entry->dependency->target;
  if ( m_derived.insert(std::make_pair(entry->key.hash,entry)).second )  {
    if ( entry->dependency ) m_dependencies.insert(entry->dependency);
    return true;
  }
  deletePtr(entry);
  return false;
}

/// Add a new entry
bool ConditionsSlice::insert_condition(Entry* entry)   {
  if ( entry->dependency )   {
    // ERROR: This call should not be invoked for derivatives!
    DD4hep::except("ConditionsSlice",
                   "insert_condition: Bad invokation. No dependency allowed here!");
  }
  
  if ( m_conditions.insert(std::make_pair(entry->key.hash,entry)).second )  {
    return true;
  }
  deletePtr(entry);
  return false;
}

namespace  {
  
  struct SliceOper  : public ConditionsSelect  {
    struct LoadInfo : public ConditionsSlice::Info {
      std::string info;
      LoadInfo(const std::string& i) : info(i) {}
      virtual ~LoadInfo() {}
    };
    ConditionsSlice* slice;
    SliceOper(ConditionsSlice* s) : slice(s) {}
    void operator()(const ConditionsIOVPool::Elements::value_type& v)    {
      v.second->select_all(*this);
    }
    bool operator()(Condition::Object* c)  const  {
      slice->insert(ConditionKey(c->name,c->hash),LoadInfo(c->name));
      return true;
    }
    /// Return number of conditions selected
    virtual size_t size()  const { return slice->conditions().size(); }
  };
}

/// Populate the conditions slice from the conditions manager (convenience)
ConditionsSlice*
DD4hep::Conditions::createSlice(ConditionsManager mgr, const IOVType& typ)  {
  dd4hep_ptr<ConditionsSlice> slice(new ConditionsSlice(mgr, IOV(&typ)));
  Conditions::ConditionsIOVPool* iovPool = mgr.iovPool(typ);
  Conditions::ConditionsIOVPool::Elements& pools = iovPool->elements;
  for_each(begin(pools),end(pools),SliceOper(slice.get()));
  return slice.release();
}
