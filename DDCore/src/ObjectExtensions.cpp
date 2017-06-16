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
#include "DD4hep/ObjectExtensions.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/Primitives.h"

// C/C++ include files
#include <stdexcept>

using namespace std;
using namespace dd4hep;

namespace {
  static int s_extensionID = 0;
  map<const type_info*, ObjectExtensions::Entry>*
  extensionContainer(const type_info& typ) {
    static map<const type_info*,map<const type_info*,ObjectExtensions::Entry> > s_map;
    return &s_map[&typ];
  }
}

/// Default constructor
ObjectExtensions::ObjectExtensions(const type_info& parent_type)    {
  extensionMap = extensionContainer(parent_type);
  InstanceCount::increment(this);
}

/// Default destructor
ObjectExtensions::~ObjectExtensions()   {
  clear();
  InstanceCount::decrement(this);
}

/// Move extensions to target object
void ObjectExtensions::move(ObjectExtensions& source)   {
  extensions = source.extensions;
  source.extensions.clear();
}

/// detaill object destructor: release extension object(s)
void ObjectExtensions::clear(bool destroy) {
  for( const auto& i : extensions )  {
    if ( i.second ) {
      auto j = extensionMap->find(i.first);
      if (j != extensionMap->end()) {
        Entry& e = (*j).second;
        if (destroy && e.destruct)
          ( *(e.destruct) ) ( i.second );
      }
    }
  }
  extensions.clear();
}

/// Copy object extensions from another object
void ObjectExtensions::copyFrom(const map<const type_info*, void*>& ext, void* arg)  {
  for( const auto& i : ext )  {
    const auto j = extensionMap->find(i.first);
    const Entry& e = (*j).second;
    extensions[i.first] = ( *(e.copy) )(i.second, arg);
  }
}

/// Add an extension object to the detector element
void* ObjectExtensions::addExtension(void* ptr, const type_info& info, void  (*dtor)(void*))  {
  return addExtension(ptr, info, 0, dtor);
}

/// Add an extension object to the detector element
void* ObjectExtensions::addExtension(void* ptr, const type_info& info,
                                     void* (*ctor)(const void*, void* arg),
                                     void  (*dtor)(void*))
{
  auto j = extensions.find(&info);
  if (j == extensions.end()) {
    auto i = extensionMap->find(&info);
    if (i == extensionMap->end()) {
      Entry entry;
      entry.destruct = dtor;
      entry.copy     = ctor;
      entry.id       = ++s_extensionID;
      extensionMap->insert(make_pair(&info, entry));
      i = extensionMap->find(&info);
    }
    return extensions[&info] = ptr;
  }
  throw runtime_error("dd4hep: addExtension: Object already has an extension of type:" + typeName(info) + ".");
}

/// Remove an existing extension object from the instance
void* ObjectExtensions::removeExtension(const type_info& info, bool destroy)  {
  auto j = extensions.find(&info);
  if (j != extensions.end()) {
    void *ptr = (*j).second;
    if ( destroy )  {
      const auto i = extensionMap->find(&info);
      if (i != extensionMap->end()) {
        const Entry& e = (*i).second;
        (*e.destruct)((*j).second);
        ptr = 0;
      }
    }
    extensions.erase(j);
    return ptr;
  }
  throw runtime_error("dd4hep: removeExtension: The object of type " + typeName(info) + " is not present.");
}

/// Access an existing extension object from the detector element
void* ObjectExtensions::extension(const type_info& info) const {
  const auto j = extensions.find(&info);
  if (j != extensions.end()) {
    return (*j).second;
  }
  throw runtime_error("dd4hep: extension: Object has no extension of type:" + typeName(info) + ".");
}

/// Access an existing extension object from the detector element
void* ObjectExtensions::extension(const type_info& info, bool alert) const {
  const auto j = extensions.find(&info);
  if (j != extensions.end()) {
    return (*j).second;
  }
  else if ( !alert )
    return 0;
  throw runtime_error("dd4hep: extension: Object has no extension of type:" + typeName(info) + ".");
}

