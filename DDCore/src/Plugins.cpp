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
#include "DD4hep/Plugins.h"
#include <cstdlib>

using namespace std;
using namespace dd4hep;

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

#include "DD4hep/Printout.h"
#if !defined(DD4HEP_PARSERS_NO_ROOT)
#include "TSystem.h"
#else
#include <dlfcn.h>
#endif
#include <cstring>

namespace   {
  struct PluginInterface  {
    int (*getDebug)();
    int (*setDebug)(int new_value);
    PluginService::stub_t (*create)(const char* identifier, const char* signature);
    void  (*add)(const char* identifier, 
                 PluginService::stub_t&& creator_stub, 
                 const char* signature, 
                 const char* return_type);
    PluginInterface() noexcept(false);
    static PluginInterface& instance()  noexcept(false)   {
      static PluginInterface s_instance;
      return s_instance;
    }
  };

  template <typename FUNCTION> struct _FP {
    union { void* ptr; FUNCTION fcn; } fptr;
    _FP(FUNCTION func)         {  fptr.fcn = func;        }
    _FP(void* _p)              {  fptr.ptr = _p;          }
  };

  template <typename T> 
  static inline T get_func(void* handle, const char* plugin, const char* entry)  {
#if !defined(DD4HEP_PARSERS_NO_ROOT)
    _FP<Func_t> fun(gSystem->DynFindSymbol(plugin,entry));
    _FP<T> fp(fun.fptr.ptr);
    if ( handle ) {}
#else
    _FP<T> fp(::dlsym(handle, entry));
    if ( !fp.fptr.ptr ) fp.fptr.ptr = ::dlsym(0, entry);
#endif
    if ( 0 == fp.fptr.ptr )      {
      string err = "dd4hep:PluginService: Failed to access symbol "
        "\""+string(entry)+"\" in plugin library "+string(plugin)+
        " ["+string(::strerror(errno))+"]";
      throw runtime_error(err);
    }
    return fp.fptr.fcn;
  }

  PluginInterface::PluginInterface()  noexcept(false)
    : getDebug(0), setDebug(0), create(nullptr), add(nullptr)
  {
    void* handle = 0;
    const char* plugin_name = ::getenv("DD4HEP_PLUGINMGR");
    if ( 0 == plugin_name )   {
      plugin_name = "libDD4hepGaudiPluginMgr";
    }
#if defined(DD4HEP_PARSERS_NO_ROOT)
    handle = ::dlopen(plugin_name, RTLD_LAZY | RTLD_GLOBAL);
#else
    if ( 0 != gSystem->Load(plugin_name) ) {}
#endif
    getDebug = get_func< int (*) ()>(handle, plugin_name,"dd4hep_pluginmgr_getdebug");
    setDebug = get_func< int (*) (int)>(handle, plugin_name,"dd4hep_pluginmgr_getdebug");
    create   = get_func< PluginService::stub_t (*) (const char*,
                                                const char*)>(handle, plugin_name,"dd4hep_pluginmgr_create");
    add      = get_func< void (*) (const char* identifier, 
                                   PluginService::stub_t&& creator_stub, 
                                   const char* signature, 
                                   const char* return_type)>(handle, plugin_name,"dd4hep_pluginmgr_add_factory");
  }
}

/// Default constructor
PluginDebug::PluginDebug(int dbg)  noexcept(false) : m_debug(0) {
  m_debug = PluginInterface::instance().setDebug(dbg);
}

/// Default destructor
PluginDebug::~PluginDebug()   noexcept(false)   {
  PluginInterface::instance().setDebug(m_debug);
}

/// Helper to check factory existence
string PluginDebug::missingFactory(const string& name) const {
  string factoryname = "Create("+name+")";
  string msg = "\t\tNo factory with name " + factoryname + " for type " + name + " found.\n"
    "\t\tPlease check library load path and/or plugin factory name.";
  return msg;
}

PluginService::stub_t PluginService::getCreator(const std::string& id, const std::type_info& info)  {
  return PluginInterface::instance().create(id.c_str(), info.name());
}

void PluginService::addFactory(const std::string& id,
                               PluginService::stub_t&& stub,
                               const std::type_info&  signature_type,
                               const std::type_info&  return_type)
{
  if ( PluginService::debug() )  {
    printout(INFO,"PluginService","+++ Declared factory[%s] with signature %s type:%s.",
             id.c_str(),signature_type.name(),return_type.name());
  }
  PluginInterface::instance().add(id.c_str(),std::move(stub),signature_type.name(),return_type.name());
}

void PluginService::print_bad_cast(const std::string& id,
                                   const stub_t& stub,
                                   const std::type_info& signature,
                                   const char* msg)   {
  bool dbg = PluginInterface::instance().getDebug();
  if ( dbg )   {
    stringstream str;
    str << "Factory requested: " << id << " (" << typeid(signature).name() << ") :" << msg;
    printout(ERROR,"PluginService","%s", str.str().c_str());
    str.str("");
#ifdef DD4HEP_HAVE_PLUGINSVC_V2
    if ( !stub.has_value() )  {
      str << "Stub is invalid!";
      printout(ERROR,"PluginService","%s", str.str().c_str());
    }
#else
    if ( !stub )  {
      str << "Stub is invalid!";
      printout(ERROR,"PluginService","%s", str.str().c_str());
    }
#endif
  }
}

#if !defined(DD4HEP_PARSERS_NO_ROOT)
#include "DD4hep/Detector.h"
#include "DD4hep/Handle.h"
#include "DD4hep/GeoHandler.h"
#include "XML/XMLElements.h"
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(NamedObject*, (Detector*,xml::Handle_t*,Ref_t*))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(NamedObject*, (Detector*,xml::Handle_t*))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(NamedObject*, (Detector*))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(TObject*,     (Detector*,xml::Handle_t*))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(long, (Detector*,xml::Handle_t*))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(long, (Detector*,xml::Handle_t const*))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(long, (Detector*, int, char**))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(long, (Detector*))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(long, (Detector*, const GeoHandler*, const std::map<std::string,std::string>*))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(long, ())
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(void*, (const char*))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(void*, (Detector*,int,char**))
#endif
