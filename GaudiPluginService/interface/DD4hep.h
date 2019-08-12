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

#if GAUDI_PLUGIN_SERVICE_VERSION == 2
#define GAUDI_PLUGIN_SERVICE_V2 1
#include <Gaudi/PluginService.h>
#elif GAUDI_PLUGIN_SERVICE_VERSION == 1
#define private public
#define GAUDI_PLUGIN_SERVICE_V1 1
#include <Gaudi/PluginService.h>
#undef private
#endif

#define MAKE_GAUDI_PLUGIN_SERVICE_ENTRY(n,v)     dd4hep_pluginmgr_##n##_V##v
#define MAKE_FUNC(name,version)  MAKE_GAUDI_PLUGIN_SERVICE_ENTRY(name,version)

extern "C"  {
  /// Access debug level
  int MAKE_FUNC(getdebug,GAUDI_PLUGIN_SERVICE_VERSION) ()   {
    return (int)Gaudi::PluginService::Debug();
  }
  /// Set debug level
  int MAKE_FUNC(setdebug,GAUDI_PLUGIN_SERVICE_VERSION)(int value)   {
    int debug = (int)Gaudi::PluginService::Debug();
    Gaudi::PluginService::SetDebug(value);
    return debug;
  }
  /// Access factory by name
#if GAUDI_PLUGIN_SERVICE_VERSION==2
  std::any MAKE_FUNC(create,GAUDI_PLUGIN_SERVICE_VERSION)(const char* id, const char* /* sig */)   {
    using namespace Gaudi::PluginService::v2;
    const Details::Registry::FactoryInfo& info = Details::Registry::instance().getInfo(id, true);
    return info.factory;
  }
#elif GAUDI_PLUGIN_SERVICE_VERSION==1
  std::any MAKE_FUNC(create,GAUDI_PLUGIN_SERVICE_VERSION)(const char* id, const char* sig)   {
    std::any ret;
    ret = (void*)Gaudi::PluginService::Details::getCreator(id,sig);
    return ret;
  }
#endif
#if GAUDI_PLUGIN_SERVICE_VERSION==2
  /// Add a new factory to the registry
  void MAKE_FUNC(add_factory,GAUDI_PLUGIN_SERVICE_VERSION)(const char* id, std::any&& stub, const char* /* sig */, const char* /* ret */)   {
    using namespace Gaudi::PluginService::v2;
    Details::Registry::Properties props = {};
    std::string lib_name = "";
    Details::Registry::instance().add( id, {lib_name, std::move( stub ), std::move( props )} );
  }
#elif GAUDI_PLUGIN_SERVICE_VERSION==1
  /// Add a new factory to the registry
  void MAKE_FUNC(add_factory,GAUDI_PLUGIN_SERVICE_VERSION)(const char* id, std::any&& stub, const char* sig, const char* ret)   {
    Gaudi::PluginService::Details::Registry::instance().add(id, std::any_cast<void*>(stub), sig, ret, id);
  }
#endif
}
