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

// Framework includes
#include "DD4hep/IOV.h"
#include "DD4hep/Handle.inl"

#include "DD4hep/World.h"
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/objects/DetectorInterna.h"
#include "DD4hep/objects/AlignmentsInterna.h"

using namespace std;
using namespace DD4hep::Alignments;
using namespace DD4hep::Alignments::Interna;

DD4HEP_INSTANTIATE_HANDLE_NAMED(AlignmentConditionObject);
DD4HEP_INSTANTIATE_HANDLE_NAMED(AlignmentNamedObject);
DD4HEP_INSTANTIATE_HANDLE_NAMED(AlignmentContainer);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(AlignmentData);

/// Standard constructor
AlignmentConditionObject::AlignmentConditionObject(const string& nam, const string& tit)
  : ConditionObject(nam, tit), alignment_data(0)
{
  InstanceCount::increment(this);
  flags = Conditions::Condition::ALIGNMENT;
}

/// Standard Destructor
AlignmentConditionObject::~AlignmentConditionObject()  {
  InstanceCount::decrement(this);
}

/// Clear data content on demand.
void AlignmentConditionObject::clear()   {
  AlignmentCondition a(this);
  Alignment::Data& d = a.data();
  d.trToWorld = Transform3D();
  d.detectorTrafo.Clear();
  d.worldTrafo.Clear();
  d.nodes.clear();
  flags = Conditions::Condition::ALIGNMENT;
}

/// Standard constructor
AlignmentContainer::AlignmentContainer(Geometry::DetElementObject* par)
  : NamedObject(), detector(par), keys()
{
  InstanceCount::increment(this);
}

/// Default destructor
AlignmentContainer::~AlignmentContainer() {
  InstanceCount::decrement(this);
}

/// Add a new key to the alignments access map
void AlignmentContainer::addKey(const string& key_val)  {
  key_type hash = Alignment::hashCode(key_val);
  if ( !keys.insert(make_pair(hash,make_pair(hash,key_val))).second )   {
    except("AlignmentContainer","++ Key[%08X]: %s already present. "
           "Duplicate insertions inhibited!",hash, key_val.c_str());
  }
}

/// Add a new key to the alignments access map: Allow for alias if key_val != data_val
void AlignmentContainer::addKey(const string& key_val, const string& data_val)  {
  key_type key_hash = Alignment::hashCode(key_val);
  key_type val_hash = Alignment::hashCode(data_val);
  if ( !keys.insert(make_pair(key_hash,make_pair(val_hash,data_val))).second )   {
    except("AlignmentContainer","++ Key[%08X]: %s already present. "
           "Duplicate insertions inhibited!",key_hash, key_val.c_str());
  }
}

/// Access to alignment objects directly by their hash key. 
Alignment AlignmentContainer::get(const string& key_val, const iov_type& iov)   {
  AlignmentsLoader& loader = detector->world().alignmentsLoader();
  key_type hash = Alignment::hashCode(key_val);
  Keys::const_iterator i=keys.find(hash);
  if ( i != keys.end() )  {
    const key_value& k = (*i).second;
    return loader.get(k.first, iov);
  }
  /// Last resort: Assume the key value is globally known:
  return loader.get(hash, iov);
}

/// Access to alignment objects directly by their hash key. 
Alignment AlignmentContainer::get(key_type hash_key, const iov_type& iov)   {
  AlignmentsLoader& loader = detector->world().alignmentsLoader();
  Keys::const_iterator i=keys.find(hash_key);
  if ( i != keys.end() )  {
    const key_value& k = (*i).second;
    return loader.get(k.first, iov);
  }
  /// Last resort: Assume the key value is globally known:
  return loader.get(hash_key, iov);
}

/// Access to alignment objects directly by their hash key. 
Alignment AlignmentContainer::get(const string& key_val, const UserPool& pool)   {
  AlignmentsLoader& loader = detector->world().alignmentsLoader();
  key_type hash = Alignment::hashCode(key_val);
  Keys::const_iterator i=keys.find(hash);
  if ( i != keys.end() )  {
    const key_value& k = (*i).second;
    return loader.get(k.first, pool);
  }
  /// Last resort: Assume the key value is globally known:
  return loader.get(hash, pool);
}

/// Access to alignment objects directly by their hash key. 
Alignment AlignmentContainer::get(key_type hash_key, const UserPool& pool)   {
  AlignmentsLoader& loader = detector->world().alignmentsLoader();
  Keys::const_iterator i=keys.find(hash_key);
  if ( i != keys.end() )  {
    const key_value& k = (*i).second;
    return loader.get(k.first, pool);
  }
  /// Last resort: Assume the key value is globally known:
  return loader.get(hash_key, pool);
}

/// Protected destructor
AlignmentsLoader::~AlignmentsLoader()   {
}

/// Default destructor
AlignmentNamedObject::~AlignmentNamedObject()   {
}

