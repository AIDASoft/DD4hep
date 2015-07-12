#include <vector>
#include "capi_pluginservice.h"
#include <Gaudi/PluginService.h>

#include <iostream>
using namespace Gaudi::PluginService::Details;

cgaudi_pluginsvc_t
cgaudi_pluginsvc_instance()
{
  static Registry& cxxreg = Registry::instance();
  cgaudi_pluginsvc_t reg;
  reg.registry = (void*)(&cxxreg);
  return reg;
}

int
cgaudi_pluginsvc_get_factory_size(cgaudi_pluginsvc_t self)
{
  const Registry::FactoryMap &fmap = ((const Registry*)self.registry)->factories();
  return int(fmap.size());
}


cgaudi_factory_t
cgaudi_pluginsvc_get_factory_at(cgaudi_pluginsvc_t self, int n)
{
  const Registry *reg = ((const Registry*)self.registry);
  std::vector<Registry::KeyType> keys;
  keys.reserve(reg->factories().size());
  for (Registry::FactoryMap::const_iterator
         itr = reg->factories().begin(),
         iend= reg->factories().end();
       itr != iend;
       itr++) {
    keys.push_back(itr->first);
  }
  const char *key = keys[n].c_str();
  cgaudi_factory_t fac;
  fac.registry = self;
  fac.id = key;
  return fac;
}

const char*
cgaudi_factory_get_library(cgaudi_factory_t self)
{
  Registry &reg = Registry::instance();
  std::string id = self.id;
  const Registry::FactoryInfo& fi = reg.getInfo(id);
  return fi.library.c_str();
}

const char*
cgaudi_factory_get_type(cgaudi_factory_t self)
{
  Registry &reg = Registry::instance();
  std::string id = self.id;
  const Registry::FactoryInfo& fi = reg.getInfo(id);
  return fi.type.c_str();
}

const char*
cgaudi_factory_get_rtype(cgaudi_factory_t self)
{
  Registry &reg = Registry::instance();
  std::string id = self.id;
  const Registry::FactoryInfo& fi = reg.getInfo(id);
  return fi.rtype.c_str();
}

const char*
cgaudi_factory_get_classname(cgaudi_factory_t self)
{
  Registry &reg = Registry::instance();
  std::string id = self.id;
  const Registry::FactoryInfo& fi = reg.getInfo(id);
  return fi.className.c_str();
}

int
cgaudi_factory_get_property_size(cgaudi_factory_t self)
{
  Registry &reg = Registry::instance();
  std::string id = self.id;
  const Registry::FactoryInfo& fi = reg.getInfo(id);
  return int(fi.properties.size());
}


cgaudi_property_t
cgaudi_factory_get_property_at(cgaudi_factory_t self, int n)
{
  cgaudi_property_t cprop;
  cprop.registry = self.registry;
  cprop.id = self.id;
  cprop.key = NULL;
  Registry &reg = Registry::instance();
  std::string id = cprop.id;
  const Registry::FactoryInfo& fi = reg.getInfo(id);
  int i = 0;
  for (Registry::Properties::const_iterator
         itr = fi.properties.begin(),
         iend = fi.properties.end();
       itr != iend;
       itr++, i++) {
    if (i == n) {
      cprop.key = itr->first.c_str();
      return cprop;
    }
  }
  return cprop;
}

const char*
cgaudi_property_get_key(cgaudi_property_t self)
{
  return self.key;
}

const char*
cgaudi_property_get_value(cgaudi_property_t self)
{
  Registry &reg = Registry::instance();
  std::string id = self.id;
  const Registry::FactoryInfo& fi = reg.getInfo(id);
  Registry::KeyType key = self.key;
  Registry::Properties::const_iterator prop = fi.properties.find(key);
  if (prop == fi.properties.end()) {
    return NULL;
  }
  return prop->second.c_str();
}
