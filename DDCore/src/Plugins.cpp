// $Id: $
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
#include "DD4hep/LCDD.h"
#include "DD4hep/Handle.h"
#include "DD4hep/Plugins.inl"
#include "DD4hep/GeoHandler.h"
#include "XML/XMLElements.h"

using namespace std;
using namespace DD4hep;

namespace {
  inline int* s_debug_value()   {
    static int s_debug_value = ::getenv("DD4HEP_TRACE") == 0 ? 0 : 1;
    return &s_debug_value;
  }
}

bool PluginService::debug()  {
  return *s_debug_value() ? true : false;
}

bool PluginService::setDebug(bool new_value)   {
  int *ptr = s_debug_value();
  bool old_value = *ptr;
  *ptr = new_value ? 1 : 0;
  return old_value;
}

#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)

/// Default constructor
PluginDebug::PluginDebug(int dbg)
  : m_debug(0) {
  m_debug = ROOT::Reflex::PluginService::Debug();
  ROOT::Reflex::PluginService::SetDebug(dbg);
}

/// Default destructor
PluginDebug::~PluginDebug() {
  ROOT::Reflex::PluginService::SetDebug (m_debug);
}

/// Helper to check factory existence
string PluginDebug::missingFactory(const string& name) const {
  ROOT::Reflex::Scope factories = ROOT::Reflex::Scope::ByName(PLUGINSVC_FACTORY_NS);
  string factoryname = ROOT::Reflex::PluginService::FactoryName(name);
  string msg = "\n\t\tNo factory with name " + factoryname + " for type " + name + " found.\n\t\tPlease check library load path.";
  return msg;
}

/// Dummy functions of the plugin service
void* PluginService::getCreator(const std::string&, const std::type_info&)  {  return 0;   }
void  PluginService::addFactory(const std::string&, void*, const std::type_info&, const std::type_info&)  {}

#else   // ROOT 6
#include "DD4hep/Printout.h"
#include "TSystem.h"

namespace   {
  struct PluginInterface  {
    int (*getDebug)();
    int (*setDebug)(int new_value);
    void* (*create)(const char* identifier, const char* signature);
    void  (*add)(const char* identifier, 
                 void* creator_stub, 
                 const char* signature, 
                 const char* return_type);
    PluginInterface();
    static PluginInterface& instance()    {
      static PluginInterface s_instance;
      return s_instance;
    }
  };

  template <typename T> 
  static inline T get_func(const char* plugin, const char* entry)  {
    PluginService::FuncPointer<Func_t> fun(gSystem->DynFindSymbol(plugin,entry));
    PluginService::FuncPointer<T> fp(fun.fptr.ptr);
    if ( 0 == fp.fptr.ptr )      {
      string err = "DD4hep:PluginService: Failed to access symbol "
        "\""+string(entry)+"\" in plugin library "+string(plugin);
      throw runtime_error(err);
    }
    return fp.fptr.fcn;
  }

  PluginInterface::PluginInterface() : getDebug(0), setDebug(0), create(0), add(0)  {    
    const char* plugin_name = ::getenv("DD4HEP_PLUGINMGR");
    if ( 0 == plugin_name )   {
      plugin_name = "libDD4hepGaudiPluginMgr";
    }
    gSystem->Load(plugin_name);
    getDebug = get_func< int (*) ()>(plugin_name,"dd4hep_pluginmgr_getdebug");
    setDebug = get_func< int (*) (int)>(plugin_name,"dd4hep_pluginmgr_getdebug");
    create   = get_func< void* (*) (const char*,
                                    const char*)>(plugin_name,"dd4hep_pluginmgr_create");
    add      = get_func< void (*) (const char* identifier, 
                                   void* creator_stub, 
                                   const char* signature, 
                                   const char* return_type)>(plugin_name,"dd4hep_pluginmgr_add_factory");
  }
}

/// Default constructor
PluginDebug::PluginDebug(int dbg) : m_debug(0) {
  m_debug = PluginInterface::instance().setDebug(dbg);
}

/// Default destructor
PluginDebug::~PluginDebug() {
  PluginInterface::instance().setDebug(m_debug);
}

/// Helper to check factory existence
string PluginDebug::missingFactory(const string& name) const {
  string factoryname = "??? Create("+name+")";
  string msg = "\n\t\tNo factory with name " + factoryname + " for type " + name + " found.\n\t\tPlease check library load path.";
  return msg;
}

void* PluginService::getCreator(const std::string& id, const std::type_info& info)  {
  return PluginInterface::instance().create(id.c_str(), info.name());
}

void PluginService::addFactory(const std::string& id, stub_t stub,
                               const std::type_info&  signature_type,
                               const std::type_info&  return_type)
{
  if ( PluginService::debug() )  {
    printout(INFO,"PluginService","+++ Declared factory[%s] with signature %s type:%s.",
             id.c_str(),signature_type.name(),return_type.name());
  }
  PluginInterface::instance().add(id.c_str(),stub,signature_type.name(),return_type.name());
}
#endif

DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(NamedObject*, (Geometry::LCDD*,XML::Handle_t*,Geometry::Ref_t*))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(NamedObject*, (Geometry::LCDD*,XML::Handle_t*))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(NamedObject*, (Geometry::LCDD*))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(long, (Geometry::LCDD*,XML::Handle_t*))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(long, (Geometry::LCDD*,XML::Handle_t const*))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(long, (Geometry::LCDD*, int, char**))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(long, (Geometry::LCDD*))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(long, (Geometry::LCDD*, const Geometry::GeoHandler*, const std::map<std::string,std::string>*))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(long, ())
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(void*, (const char*))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(void*, (Geometry::LCDD*,int,char**))
