// $Id: Readout.cpp 590 2013-06-03 17:02:43Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_PLUGINS_ROOT5_INL
#define DD4HEP_PLUGINS_ROOT5_INL

#include <set>
#include <map>
#include <list>
#include <vector>
#include "DD4hep/Printout.h"
#include "Reflex/PluginService.h"
#include "Reflex/Reflex.h"
#include "Reflex/Builder/ReflexBuilder.h"

namespace DD4hep  {

  template <typename R> R PluginService::Create(const std::string& name)  
  { return ROOT::Reflex::PluginService::Create<R>(name); }

  template <typename R, typename A0> R PluginService::Create(const std::string& name, A0 a0)  
  { return ROOT::Reflex::PluginService::Create<R>(name,a0); }

  template <typename R, typename A0, typename A1>
  R PluginService::Create(const std::string& name, A0 a0, A1 a1)  
  { return ROOT::Reflex::PluginService::Create<R>(name, a0, a1); }

  template <typename R, typename A0, typename A1, typename A2>
  R PluginService::Create(const std::string& name, A0 a0, A1 a1, A2 a2)  
  { return ROOT::Reflex::PluginService::Create<R>(name, a0, a1, a2); }

  template <typename R, typename A0, typename A1, typename A2, typename A3>
  R PluginService::Create(const std::string& name, A0 a0, A1 a1, A2 a2, A3 a3)  
  { return ROOT::Reflex::PluginService::Create<R>(name, a0, a1, a2, a3); }

  template <typename R, typename A0, typename A1, typename A2, typename A3, typename A4>
  R PluginService::Create(const std::string& name, A0 a0, A1 a1, A2 a2, A3 a3, A4 a4)  
  { return ROOT::Reflex::PluginService::Create<R>(name, a0, a1, a2, a3, a4); }

  template <typename R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
  R PluginService::Create(const std::string& name, A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5)  
  { return ROOT::Reflex::PluginService::Create<R>(name, a0, a1, a2, a3, a4, a5); }

  namespace plugin_signatures_namespace {
    namespace {
      template <typename T> union FuncPtr {
        FuncPtr(T t) { fcn = t; }
        void* ptr;
        T fcn;
      };
      template <typename T> FuncPtr<T> __func(T t) { return FuncPtr<T>(t); }
    }

    template <typename R> void* instantiate_creator ()  
    { return __func(PluginService::Create<R>).ptr; }
    
    template <typename R, typename A0> void* instantiate_creator(A0)  
    { return __func(PluginService::Create<R,A0>).ptr; }

    template <typename R, typename A0, typename A1> void* instantiate_creator(A0,A1)  
    { return __func(PluginService::Create<R,A0,A1>).ptr; }

    template <typename R, typename A0, typename A1, typename A2>
      void* instantiate_creator(A0,A1,A2)  
    { return __func(PluginService::Create<R,A0,A1,A2>).ptr; }

    template <typename R, typename A0, typename A1, typename A2, typename A3>
      void* instantiate_creator(A0,A1,A2,A3)  
    { return __func(PluginService::Create<R,A0,A1,A2,A3>).ptr; }

    template <typename R, typename A0, typename A1, typename A2, typename A3, typename A4>
      void* instantiate_creator(A0,A1,A2,A3,A4)  
    { return __func(PluginService::Create<R,A0,A1,A2,A3,A4>).ptr; }

    template <typename R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
      void* instantiate_creator(A0,A1,A2,A3,A4,A5)  
    { return __func(PluginService::Create<R,A0,A1,A2,A3,A4,A5>).ptr; }

    namespace  {
      template <typename SIGNATURE> static void reflex_plugin(const std::string& name, DD4hep::PluginService::stub_t stub)  {
        ROOT::Reflex::Type typ = ROOT::Reflex::TypeBuilder(name.c_str(),ROOT::Reflex::PUBLIC);
        ROOT::Reflex::Type sig = ROOT::Reflex::FunctionDistiller < SIGNATURE > ::Get();
        std::string fname = (std::string(PLUGINSVC_FACTORY_NS "::") + ROOT::Reflex::PluginService::FactoryName(name));
        ROOT::Reflex::FunctionBuilder func(sig, fname.c_str(), stub, 0, "", ROOT::Reflex::PUBLIC);
        func.AddProperty("name", name).AddProperty("id", name);
	if ( PluginService::debug() )  {
          printout(INFO,"PluginService","+++ Declared factory for id %s with signature %s.",fname.c_str(),sig.Name().c_str());
        }
      }
    }

  }
}

#define DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(R, ARGS) namespace DD4hep {		\
  template <> void PluginRegistry< R ARGS >::add(const char* n, DD4hep::PluginService::stub_t f) \
  {   plugin_signatures_namespace::reflex_plugin< R ARGS >(n,f);   } \
  namespace plugin_signatures_namespace { template void* instantiate_creator<R> ARGS ; }}

#endif  // DD4HEP_PLUGINS_ROOT5_INL

