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
#include "DD4hep/ObjectExtensions.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/Primitives.h"

// C/C++ include files
#include <stdexcept>

using namespace DD4hep;

namespace {
  static int s_extensionID = 0;
  ObjectExtensions::ExtensionMap* extensionContainer(const std::type_info& typ) {
    static std::map<const std::type_info*, ObjectExtensions::ExtensionMap> s_map;
    return &s_map[&typ];
  }
}

/// Default constructor
ObjectExtensions::ObjectExtensions(const std::type_info& parent_type)    {
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

/// Internal object destructor: release extension object(s)
void ObjectExtensions::clear(bool destroy) {
  for (Extensions::iterator i = extensions.begin(); i != extensions.end(); ++i) {
    void* ptr = (*i).second;
    if (ptr) {
      ExtensionMap::iterator j = extensionMap->find((*i).first);
      if (j != extensionMap->end()) {
        Entry& e = (*j).second;
        if (destroy && e.destruct)
          (*(e.destruct))(ptr);
      }
    }
  }
  extensions.clear();
}

/// Copy object extensions from another object
void ObjectExtensions::copyFrom(const Extensions& ext, void* arg)  {
  for (Extensions::const_iterator i = ext.begin(); i != ext.end(); ++i) {
    const std::type_info* info = (*i).first;
    ExtensionMap::const_iterator j = extensionMap->find(info);
    const Entry& e = (*j).second;
    extensions[info] = (*(e.copy))((*i).second, arg);
  }
}

/// Add an extension object to the detector element
void* ObjectExtensions::addExtension(void* ptr, const std::type_info& info, destruct_t dtor)  {
  return addExtension(ptr, info, 0, dtor);
}

/// Add an extension object to the detector element
void* ObjectExtensions::addExtension(void* ptr, const std::type_info& info, copy_t ctor, destruct_t dtor)  {
  Extensions::iterator j = extensions.find(&info);
  if (j == extensions.end()) {
    ExtensionMap::iterator i = extensionMap->find(&info);
    if (i == extensionMap->end()) {
      Entry entry;
      entry.destruct = dtor;
      entry.copy = ctor;
      entry.id = ++s_extensionID;
      extensionMap->insert(make_pair(&info, entry));
      i = extensionMap->find(&info);
    }
    return extensions[&info] = ptr;
  }
  throw std::runtime_error("DD4hep: addExtension: Object already has an extension of type:" + typeName(info) + ".");
}

/// Remove an existing extension object from the instance
void* ObjectExtensions::removeExtension(const std::type_info& info, bool destroy)  {
  Extensions::iterator j = extensions.find(&info);
  if (j != extensions.end()) {
    void *ptr = (*j).second;
    if ( destroy )  {
      ExtensionMap::iterator i = extensionMap->find(&info);
      if (i != extensionMap->end()) {
        Entry& e = (*i).second;
        (*e.destruct)((*j).second);
        ptr = 0;
      }
    }
    extensions.erase(j);
    return ptr;
  }
  throw std::runtime_error("DD4hep: removeExtension: The object of type " + typeName(info) + " is not present.");
}

/// Access an existing extension object from the detector element
void* ObjectExtensions::extension(const std::type_info& info) const {
  Extensions::const_iterator j = extensions.find(&info);
  if (j != extensions.end()) {
    return (*j).second;
  }
  throw std::runtime_error("DD4hep: extension: Object has no extension of type:" + typeName(info) + ".");
}

/// Access an existing extension object from the detector element
void* ObjectExtensions::extension(const std::type_info& info, bool alert) const {
  Extensions::const_iterator j = extensions.find(&info);
  if (j != extensions.end()) {
    return (*j).second;
  }
  else if ( !alert )
    return 0;
  throw std::runtime_error("DD4hep: extension: Object has no extension of type:" + typeName(info) + ".");
}

