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
#include "DD4hep/Handle.inl"
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/objects/DetectorInterna.h"
#include "DD4hep/objects/ConditionsInterna.h"

using namespace std;
using namespace DD4hep::Conditions;

DD4HEP_INSTANTIATE_HANDLE_NAMED(Interna::ConditionObject);
DD4HEP_INSTANTIATE_HANDLE_NAMED(Interna::ConditionContainer);

/// Standard initializing constructor
BlockData::BlockData() : Block(), destruct(0), copy(0), type(0)   {
  InstanceCount::increment(this);
}

/// Copy constructor
BlockData::BlockData(const BlockData& c) 
  : Block(c), destruct(c.destruct), copy(c.copy), type(c.type)   {
  grammar = 0;
  pointer = 0;
  this->bind(c.grammar,c.copy,c.destruct);
  this->copy(pointer,c.pointer);
  InstanceCount::increment(this);
}

/// Standard Destructor
BlockData::~BlockData()   {
  if ( destruct )  {
    (*destruct)(pointer);
    if ( (type&ALLOC_DATA) == ALLOC_DATA ) ::operator delete(pointer);
  }
  pointer = 0;
  grammar = 0;
  InstanceCount::decrement(this);
}

/// Move the data content: 'from' will be reset to NULL
bool BlockData::move(BlockData& from)   {
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
  return true;
}

/// Copy constructor
BlockData& BlockData::operator=(const BlockData& c)   {
  if ( this != &c )  {
    if ( this->grammar == c.grammar )   {
      if ( destruct )  {
	(*destruct)(pointer);
	if ( (type&ALLOC_DATA) == ALLOC_DATA ) ::operator delete(pointer);
      }
      pointer = 0;
      grammar = 0;
    }
    if ( this->grammar == 0 )  {
      this->Block::operator=(c);
      this->destruct = c.destruct;
      this->copy = c.copy;
      this->type = c.type;
      this->grammar = 0;
      this->bind(c.grammar,c.copy,c.destruct);
      this->copy(pointer,c.pointer);
      return *this;
    }
    except("Conditions","You may not bind condition data multiple times!");
  }
  return *this;
}

/// Set data value
bool BlockData::bind(const BasicGrammar* g, void (*ctor)(void*,const void*), void (*dtor)(void*))   {
  if ( !grammar )  {
    size_t len = g->sizeOf();
    grammar  = g;
    destruct = dtor;
    copy     = ctor;
    (len > sizeof(data))
      ? (pointer=::operator new(len),type=ALLOC_DATA)
      : (pointer=data,type=PLAIN_DATA);
    return true;
  }
  else if ( grammar == g )  {
    // We cannot ingore secondary requests for data bindings.
    // This leads to memory leaks in the caller!
    except("Conditions","You may not bind conditions multiple times!");
  }
  typeinfoCheck(grammar->type(),g->type(),"Conditions data blocks may not be assigned.");
  return false;
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
  : NamedObject(), value(), validity(), address(), comment(),
    data(), pool(0), iov(0), 
    hash(0), flags(0), refCount(0)
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
const Condition::iov_type* Interna::ConditionObject::iovData() const    {
  if ( iov ) return iov;
  invalidHandleError<IOV>();
  return 0;
}

/// Access safely the IOV-type
const IOVType* Interna::ConditionObject::iovType() const    {
  if ( iov && iov->iovType ) return iov->iovType;
  invalidHandleError<IOVType>();
  return 0;
}

/// Standard constructor
Interna::ConditionContainer::ConditionContainer(Geometry::DetElementObject* par)
  : NamedObject(), detector(par), keys()
{
  InstanceCount::increment(this);
}

/// Default destructor
Interna::ConditionContainer::~ConditionContainer() {
  InstanceCount::decrement(this);
}

/// Add a new key to the conditions access map
void Interna::ConditionContainer::addKey(const string& key_val)  {
  key_type hash = ConditionKey::hashCode(key_val);
  if ( !keys.insert(make_pair(hash,make_pair(hash,key_val))).second )   {
    except("ConditionContainer","++ Key[%08X]: %s already present. Duplicate insertions inhibited!",hash, key_val.c_str());
  }
}

/// Add a new key to the conditions access map: Allow for alias if key_val != data_val
void Interna::ConditionContainer::addKey(const string& key_val, const string& data_val)  {
  key_type key_hash = ConditionKey::hashCode(key_val);
  key_type val_hash = ConditionKey::hashCode(data_val);
  if ( !keys.insert(make_pair(key_hash,make_pair(val_hash,data_val))).second )   {
    except("ConditionContainer","++ Key[%08X]: %s already present. Duplicate insertions inhibited!",key_hash, key_val.c_str());
  }
}

/// Access to condition objects directly by their hash key. 
Condition Interna::ConditionContainer::get(const string& key_val, const iov_type& iov)   {
  key_type hash = ConditionKey::hashCode(key_val);
  Keys::const_iterator i=keys.find(hash);
  if ( i != keys.end() )  {
    const key_value& k = (*i).second;
    return detector->world()->getCondition(k.first, iov);
  }
  /// Last resort: Assume the key value is globally known:
  return detector->world()->getCondition(hash, iov);
}

/// Access to condition objects directly by their hash key. 
Condition Interna::ConditionContainer::get(key_type hash_key, const iov_type& iov)   {
  Keys::const_iterator i=keys.find(hash_key);
  if ( i != keys.end() )  {
    const key_value& k = (*i).second;
    return detector->world()->getCondition(k.first, iov);
  }
  /// Last resort: Assume the key value is globally known:
  return detector->world()->getCondition(hash_key, iov);
}

/// Access to condition objects directly by their hash key. 
Condition Interna::ConditionContainer::get(const string& key_val, const UserPool& pool)   {
  key_type hash = ConditionKey::hashCode(key_val);
  Keys::const_iterator i=keys.find(hash);
  if ( i != keys.end() )  {
    const key_value& k = (*i).second;
    return detector->world()->getCondition(k.first, pool);
  }
  /// Last resort: Assume the key value is globally known:
  return detector->world()->getCondition(hash, pool);
}

/// Access to condition objects directly by their hash key. 
Condition Interna::ConditionContainer::get(key_type hash_key, const UserPool& pool)   {
  Keys::const_iterator i=keys.find(hash_key);
  if ( i != keys.end() )  {
    const key_value& k = (*i).second;
    return detector->world()->getCondition(k.first, pool);
  }
  /// Last resort: Assume the key value is globally known:
  return detector->world()->getCondition(hash_key, pool);
}

/// Protected destructor
ConditionsLoader::~ConditionsLoader()   {
}
