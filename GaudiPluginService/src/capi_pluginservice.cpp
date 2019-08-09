#include "capi_pluginservice.h"

#define GAUDI_PLUGIN_SERVICE_V2
#include <Gaudi/PluginService.h>

#include <algorithm>
#include <vector>

#include <iostream>

using namespace Gaudi::PluginService::v2::Details;

cgaudi_pluginsvc_t cgaudi_pluginsvc_instance() {
  static Registry& cxxreg = Registry::instance();
  return {&cxxreg};
}

int cgaudi_pluginsvc_get_factory_size( cgaudi_pluginsvc_t self ) {
  const Registry::FactoryMap& fmap = static_cast<const Registry*>( self.registry )->factories();
  return int( fmap.size() );
}

cgaudi_factory_t cgaudi_pluginsvc_get_factory_at( cgaudi_pluginsvc_t self, int n ) {
  const Registry* reg       = static_cast<const Registry*>( self.registry );
  const auto&     factories = reg->factories();
  if ( n >= static_cast<int>( factories.size() ) ) return {self, nullptr};
  return {self, next( begin( factories ), n )->first.c_str()};
}

const char* cgaudi_factory_get_library( cgaudi_factory_t self ) {
  Registry& reg = Registry::instance();
  return reg.getInfo( self.id ).library.c_str();
}

const char* cgaudi_factory_get_type( cgaudi_factory_t self ) {
  Registry&          reg = Registry::instance();
  static std::string cache;
  cache = demangle( reg.getInfo( self.id ).factory.type() );
  return cache.c_str();
}

const char* cgaudi_factory_get_classname( cgaudi_factory_t self ) {
  Registry&          reg = Registry::instance();
  static std::string cache;
  cache = reg.getInfo( self.id ).getprop( "ClassName" );
  return cache.c_str();
}

int cgaudi_factory_get_property_size( cgaudi_factory_t self ) {
  Registry&                    reg = Registry::instance();
  const Registry::FactoryInfo& fi  = reg.getInfo( self.id );
  return int( fi.properties.size() );
}

cgaudi_property_t cgaudi_factory_get_property_at( cgaudi_factory_t self, int n ) {
  cgaudi_property_t            cprop{self.registry, self.id, nullptr};
  Registry&                    reg = Registry::instance();
  const Registry::FactoryInfo& fi  = reg.getInfo( cprop.id );
  if ( n < static_cast<int>( fi.properties.size() ) ) cprop.key = next( begin( fi.properties ), n )->first.c_str();
  return cprop;
}

const char* cgaudi_property_get_key( cgaudi_property_t self ) { return self.key; }

const char* cgaudi_property_get_value( cgaudi_property_t self ) {
  Registry&                    reg  = Registry::instance();
  const Registry::FactoryInfo& fi   = reg.getInfo( self.id );
  auto                         prop = fi.properties.find( self.key );
  return prop != fi.properties.end() ? prop->second.c_str() : nullptr;
}
