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
#include <string>
#include <sstream>
#include <map>
#include <set>
#include <typeinfo>
#include <utility>
#if __cplusplus >= 201703
#  include <any>
#else
#  include <boost/any.hpp>
namespace std {
  using boost::any;
  using boost::any_cast;
  using boost::bad_any_cast;
} // namespace std
#endif

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wkeyword-macro"
#pragma clang diagnostic pop
#endif

#define GAUDI_PLUGIN_SERVICE_V2 1

// This define will give us a version of the gaudi plugin manager,
// which will NOT clash with Gaudi! It of course has a correspondance in the
// compiler options of the GaudiPluginService package.
#include <Gaudi/PluginService.h>

#ifdef GAUDI_PLUGIN_SERVICE_USE_V2
using namespace Gaudi::PluginService::v2;
#endif

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
  std::any dd4hep_pluginmgr_create(const char* id, const char* /* sig */)   {
#ifdef GAUDI_PLUGIN_SERVICE_USE_V2
    const Details::Registry::FactoryInfo& info = Details::Registry::instance().getInfo(id, true);
    return info.factory;
#else
    return Gaudi::PluginService::Details::getCreator(id,sig);
#endif
  }
#ifdef GAUDI_PLUGIN_SERVICE_USE_V2
  /// Add a new factory to the registry
  void dd4hep_pluginmgr_add_factory(const char* id, std::any&& stub, const char* /* sig */, const char* /* ret */)   {
    Details::Registry::Properties props = {};
    std::string lib_name = "";
    Details::Registry::instance().add( id, {lib_name, std::move( stub ), std::move( props )} );
  }
#else
  /// Add a new factory to the registry
  void dd4hep_pluginmgr_add_factory(const char* id, void* stub, const char* sig, const char* ret)   {
    Gaudi::PluginService::Details::Registry::instance().add(id,stub,sig,ret,id);
  }
#endif
}

