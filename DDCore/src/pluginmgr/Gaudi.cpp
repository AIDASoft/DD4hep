#define private public
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
    return Gaudi::PluginService::Details::getCreator(id, sig);
  }
  /// Add a new factory to the registry
  void dd4hep_pluginmgr_add_factory(const char* id, void* stub, const char* sig, const char* ret)   {
    Gaudi::PluginService::Details::Registry::instance().add(id,stub,sig,ret,id);
  }
}

