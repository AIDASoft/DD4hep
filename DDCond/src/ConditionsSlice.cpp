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

/// Initializing constructor
ConditionsSlice::ConditionsSlice(ConditionsManager m) : manager(m)
{
  InstanceCount::increment(this);  
}

/// Copy constructor (Special, partial copy only. Hence no assignment!)
ConditionsSlice::ConditionsSlice(const ConditionsSlice& copy)
  : manager(copy.manager), content(copy.content)
{
  InstanceCount::increment(this);  
}

/// Default destructor. 
ConditionsSlice::~ConditionsSlice()   {
  InstanceCount::decrement(this);  
}

/// Clear the conditions access and the user pool.
void ConditionsSlice::reset()   {
  if ( pool.get() ) pool->clear();
}

/// ConditionsMap overload: Add a condition directly to the slice
bool ConditionsSlice::insert(DetElement detector, unsigned int key, Condition condition)   {
  if ( condition.isValid() )  {
    return pool->insert(detector, key, condition);
  }
  DD4hep::except("ConditionsSlice",
                 "insert_condition: Cannot insert invalid conditions to the user pool!");
  return false;
}

/// ConditionsMap overload: Access a condition
Condition ConditionsSlice::get(DetElement detector, unsigned int key)  const   {
  return pool->get(detector, key);
}

/// ConditionsMap overload: Interface to scan data content of the conditions mapping
void ConditionsSlice::scan(Condition::Processor& processor) const   {
  pool->scan(processor);
}
#if 0
namespace  {
  
  struct SliceOper  : public ConditionsSelect  {
    ConditionsSlice* slice;
    SliceOper(ConditionsSlice* s) : slice(s) {}
    void operator()(const ConditionsIOVPool::Elements::value_type& v)    {
      v.second->select_all(*this);
    }
    bool operator()(Condition::Object* c)  const  {
      if ( 0 == (c->flags&Condition::DERIVED) )   {
        slice->insert(ConditionKey(c->hash),ConditionsSlice::loadInfo(c->address));
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
#endif
