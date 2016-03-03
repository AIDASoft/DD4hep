// $Id: $
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

// Framework includes
#include "DD4hep/Mutex.h"
#include "DD4hep/Handle.inl"
#include "DD4hep/Printout.h"
#include "DD4hep/Detector.h"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/objects/ConditionsInterna.h"

using namespace std;
using namespace DD4hep::Conditions;

namespace {
  DD4hep::dd4hep_mutex_t s_conditionsMutex;
}

DD4HEP_INSTANTIATE_HANDLE_NAMED(Interna::ConditionObject);
DD4HEP_INSTANTIATE_HANDLE_NAMED(Interna::ConditionContainer);

/// Standard initializing constructor
BlockData::BlockData() : Block(), destruct(0), copy(0), type(0)   {
}

/// Standard Destructor
BlockData::~BlockData()   {
  if ( destruct )  {
    (*destruct)(pointer);
    if ( (type&ALLOC_DATA) == ALLOC_DATA ) ::operator delete(pointer);
  }
  pointer = 0;
  grammar = 0;
}
#if 0
/// Move the data content: 'from' will be reset to NULL
void BlockData::move(BlockData& from)   {
  pointer = from.pointer;
  grammar = from.grammar;
  ::memcpy(data,from.data,sizeof(data));
  destruct = from.destruct;
  copy = from.copy;
  type = from.type;
  ::memset(from.data,0,sizeof(data));
  from.type = PLAIN_DATA;
  from.destruct = 0;
  from.copy = 0;
  from.pointer = 0;
  from.grammar = 0;
}
#endif
/// Set data value
void BlockData::bind(const BasicGrammar* g, void (*ctor)(void*,const void*), void (*dtor)(void*))   {
  if ( !grammar )  {
    size_t len = g->sizeOf();
    grammar  = g;
    destruct = dtor;
    copy     = ctor;
    (len > sizeof(data))
      ? (pointer=::operator new(len),type=ALLOC_DATA)
      : (pointer=data,type=PLAIN_DATA);
    return;
  }
  else if ( grammar == g )  {
    // We cannot ingore secondary requests for data bindings.
    // This leads to memory leaks in the caller!
    except("Conditions","You may not bind conditions multiple times!");
  }
  typeinfoCheck(grammar->type(),g->type(),"Conditions data blocks may not be assigned.");
}

/// Set data value
void BlockData::assign(const void* ptr, const type_info& typ)  {
  if ( !grammar )   {
    throw runtime_error("Conditions data block is unbound. Cannot copy data.");
  }
  else if ( grammar->type() != typ )  {
    throw runtime_error("bad binding");
  }
  (*copy)(pointer,ptr);
}

/// Standard constructor
Interna::ConditionObject::ConditionObject()
  : NamedObject(), detector(), data(), iov(0), hash(0), flags(0), refCount(0)
{
  InstanceCount::increment(this);
}

/// Standard Destructor
Interna::ConditionObject::~ConditionObject()  {
  InstanceCount::decrement(this);
}

/// Move data content: 'from' will be reset to NULL
Interna::ConditionObject& Interna::ConditionObject::move(ConditionObject& /* from */)   {
  return *this;
}

/// Access safely the IOV
const IOV* Interna::ConditionObject::iov_data() const    {
  if ( iov ) return iov;
  invalidHandleError<IOV>();
  return 0;
}

/// Access safely the IOV-type
const IOVType* Interna::ConditionObject::iov_type() const    {
  if ( iov && iov->iovType ) return iov->iovType;
  invalidHandleError<IOVType>();
  return 0;
}

/// Standard constructor
Interna::ConditionContainer::ConditionContainer() : NamedObject(), entries() {
  InstanceCount::increment(this);
}

/// Default destructor
Interna::ConditionContainer::~ConditionContainer() {
  removeElements();
  InstanceCount::decrement(this);
}

/// Clear all attached conditions.
void Interna::ConditionContainer::removeElements()    {
  dd4hep_lock_t locked_call(s_conditionsMutex);
  for(Entries::iterator i=entries.begin(); i!=entries.end();++i)
    (*i).second->flags &= ~(ConditionObject::ACTIVE);
  entries.clear();
}

void Interna::ConditionContainer::add(Condition condition)   {
  ConditionObject* o = condition.ptr();
  if ( o )  {
    if ( !o->hash ) o->hash = hash32(condition.name());
    dd4hep_lock_t locked_call(s_conditionsMutex);
    Entries::iterator i=entries.find(o->hash);
    o->flags |= ConditionObject::ACTIVE;
    if ( i != entries.end() )  {
      (*i).second->flags &= ~(ConditionObject::ACTIVE);
      (*i).second = o;
      return;
    }
    entries[o->hash] = o;
    return;
  }
  invalidHandleError<Condition>();
}

void Interna::ConditionContainer::remove(int condition_hash)   {
  dd4hep_lock_t locked_call(s_conditionsMutex);
  Entries::iterator i=entries.find(condition_hash);
  if ( i != entries.end() )  {
    (*i).second->flags &= ~(ConditionObject::ACTIVE);
  }
}

void Interna::ConditionContainer::lock()   {
  s_conditionsMutex.lock();
}

void Interna::ConditionContainer::unlock()   {
  s_conditionsMutex.unlock();
}

/// Protected destructor
ConditionsLoader::~ConditionsLoader()   {
}
