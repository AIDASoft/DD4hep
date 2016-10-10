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
#pragma push_macro("Gaudi")
#undef Gaudi
#include <string>
#include <sstream>
#include <map>
#include <set>
#include <typeinfo>
#include <utility>

#define private public
// This define will give us a version of the gaudi plugin manager,
// which will NOT clash with Gaudi! It of course has a correspondance in the
// compiler options of the GaudiPluginService package.
//#define Gaudi DD4hep_Flavor
#include "Gaudi/PluginService.h"

extern "C"  {
  /// Access debug level
  int dd4hep_pluginmgr_getdebug()   {
    return (int)Gaudi::PluginService::Debug();
  }
  /// Set debug level
  int dd4hep_pluginmgr_setdebug(int value)   {
    int debug = dd4hep_pluginmgr_getdebug();
    Gaudi::PluginService::SetDebug(value);
    return debug;
  }
  /// Access factory by name
  void* dd4hep_pluginmgr_create(const char* id, const char* sig)   {
    return Gaudi::PluginService::Details::getCreator(id,sig);
  }
  /// Add a new factory to the registry
  void dd4hep_pluginmgr_add_factory(const char* id, void* stub, const char* sig, const char* ret)   {
    Gaudi::PluginService::Details::Registry::instance().add(id,stub,sig,ret,id);
  }
}
#pragma pop_macro("Gaudi")

