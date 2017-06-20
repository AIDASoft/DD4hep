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
#include "DD4hep/PluginTester.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/Primitives.h"

// C/C++ include files
#include <stdexcept>

using namespace dd4hep;

namespace {
  static int s_extensionID = 0;
  PluginTester::ExtensionMap* extensionContainer(const std::type_info& typ) {
    static std::map<const std::type_info*, PluginTester::ExtensionMap> s_map;
    return &s_map[&typ];
  }
}

/// Default constructor
PluginTester::PluginTester()    {
  extensionMap = extensionContainer(typeid(*this));
  InstanceCount::increment(this);
}

/// Default destructor
PluginTester::~PluginTester()   {
  clear();
  InstanceCount::decrement(this);
}

/// Internal object destructor: release extension object(s)
void PluginTester::clear(bool destroy)    {
  for (Extensions::iterator i = extensions.begin(); i != extensions.end(); ++i) {
    void* ptr = (*i).second;
    if (ptr) {
      ExtensionMap::iterator j = extensionMap->find((*i).first.first);
      if (j != extensionMap->end()) {
        Entry& e = (*j).second;
        if (destroy && e.destruct)
          (*(e.destruct))(ptr);
      }
    }
  }
  extensions.clear();
}

/// Add an extension object to the detector element
void* PluginTester::addExtension(void* ptr, const std::string& name, const std::type_info& info, destruct_t dtor)  {
  key_type key(&info,name);
  Extensions::iterator j = extensions.find(key);
  if (j == extensions.end()) {
    ExtensionMap::iterator i = extensionMap->find(&info);
    if (i == extensionMap->end()) {
      Entry entry;
      entry.destruct = dtor;
      entry.id = ++s_extensionID;
      extensionMap->insert(make_pair(&info, entry));
    }
    return extensions[key] = ptr;
  }
  throw std::runtime_error("dd4hep: addExtension: Object already has an extension "+name+
                           " of type:" + typeName(info) + ".");
}

/// Remove an existing extension object from the instance
void* PluginTester::removeExtension(const std::string& name, const std::type_info& info, bool destroy)  {
  key_type key(&info,name);
  Extensions::iterator j = extensions.find(key);
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
  throw std::runtime_error("dd4hep: removeExtension: The object "+name+
                           " of type " + typeName(info) + " is not present.");
}

/// Access an existing extension object from the detector element
void* PluginTester::extension(const std::string& name, const std::type_info& info, bool alert) const {
  key_type key(&info,name);
  Extensions::const_iterator j = extensions.find(key);
  if (j != extensions.end()) {
    return (*j).second;
  }
  else if ( !alert )
    return 0;
  throw std::runtime_error("dd4hep: extension: Object has no extension "+name+
                           " of type:" + typeName(info) + ".");
}

