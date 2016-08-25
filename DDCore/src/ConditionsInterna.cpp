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
#include "DD4hep/IOV.h"
#include "DD4hep/Handle.inl"
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/objects/DetectorInterna.h"
#include "DD4hep/objects/ConditionsInterna.h"

using namespace std;
using namespace DD4hep::Conditions;

DD4HEP_INSTANTIATE_HANDLE_NAMED(Interna::ConditionObject);
DD4HEP_INSTANTIATE_HANDLE_NAMED(Interna::ConditionContainer);

/// Standard constructor
Interna::ConditionObject::ConditionObject(const string& nam,const string& tit)
  : NamedObject(nam, tit), value(), validity(), address(), comment(),
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
const DD4hep::IOVType* Interna::ConditionObject::iovType() const    {
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
  ConditionsLoader& loader = detector->world().conditionsLoader();
  key_type hash = ConditionKey::hashCode(key_val);
  Keys::const_iterator i=keys.find(hash);
  if ( i != keys.end() )  {
    const key_value& k = (*i).second;
    return loader.get(k.first, iov);
  }
  /// Last resort: Assume the key value is globally known:
  return loader.get(hash, iov);
}

/// Access to condition objects directly by their hash key. 
Condition Interna::ConditionContainer::get(key_type hash_key, const iov_type& iov)   {
  ConditionsLoader& loader = detector->world().conditionsLoader();
  Keys::const_iterator i=keys.find(hash_key);
  if ( i != keys.end() )  {
    const key_value& k = (*i).second;
    return loader.get(k.first, iov);
  }
  /// Last resort: Assume the key value is globally known:
  return loader.get(hash_key, iov);
}

/// Access to condition objects directly by their hash key. 
Condition Interna::ConditionContainer::get(const string& key_val, const UserPool& pool)   {
  ConditionsLoader& loader = detector->world().conditionsLoader();
  key_type hash = ConditionKey::hashCode(key_val);
  Keys::const_iterator i=keys.find(hash);
  if ( i != keys.end() )  {
    const key_value& k = (*i).second;
    return loader.get(k.first, pool);
  }
  /// Last resort: Assume the key value is globally known:
  return loader.get(hash, pool);
}

/// Access to condition objects directly by their hash key. 
Condition Interna::ConditionContainer::get(key_type hash_key, const UserPool& pool)   {
  ConditionsLoader& loader = detector->world().conditionsLoader();
  Keys::const_iterator i=keys.find(hash_key);
  if ( i != keys.end() )  {
    const key_value& k = (*i).second;
    return loader.get(k.first, pool);
  }
  /// Last resort: Assume the key value is globally known:
  return loader.get(hash_key, pool);
}

/// Protected destructor
ConditionsLoader::~ConditionsLoader()   {
}
