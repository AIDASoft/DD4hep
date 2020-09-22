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

#ifndef DD4HEP_PLUGINS_INL
#define DD4HEP_PLUGINS_INL

#include "DD4hep/Plugins.h"

#if !defined(DD4HEP_PARSERS_NO_ROOT) && ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
#include <set>
#include <map>
#include <list>
#include <vector>

#ifdef __APPLE__
#define Reflex_CollectionProxy 1
#endif

#include "DD4hep/Printout.h"
#include "Reflex/PluginService.h"
#include "Reflex/Reflex.h"
#include "Reflex/Builder/ReflexBuilder.h"

/// The dd4hep namespace declaration
namespace dd4hep  {

  /** Declaration and implementation of all templated Create methods.
    * Concrete instances must be created using the instantiators below.
    *
    * \author M.Frank
    * \date   10/03/2015
    */
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


  /** Internal namespace -- should under no circumstances be used directly.
    * The named namespace is necessary to trick the linker. Entries in an anonymous 
    * namespace would be oiptimized away.....
    *
    * \author M.Frank
    * \date   10/03/2015
    */
  namespace plugin_signatures_namespace {

    namespace {

    /// Helper to convert function pointer to void pointer.
    /**
      * \author M.Frank
      * \date   10/03/2015
      */
      template <typename T> union FuncPtr {
        FuncPtr(T t) { fcn = t; }
        void* ptr;
        T fcn;
      };
      /// Helper to convert function pointer to helper union
      template <typename T> FuncPtr<T> __func(T t) { return FuncPtr<T>(t); }
    }

    /// Defined required creator functions to instantiate the "Create" signatures.
    /** The Create signatures are instantiated from a macro only containing 
      * the arguments. Otherwise these functions are 
      * pretty useless.
      *
      * \author M.Frank
      * \date   10/03/2015
      */
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
      template <typename DD4HEP_SIGNATURE> static void reflex_plugin(const std::string& name, typename dd4hep::PluginRegistry<DD4HEP_SIGNATURE>::stub_t stub)  {
        ROOT::Reflex::Type typ = ROOT::Reflex::TypeBuilder(name.c_str(),ROOT::Reflex::PUBLIC);
        ROOT::Reflex::Type sig = ROOT::Reflex::FunctionDistiller < DD4HEP_SIGNATURE > ::Get();
        std::string fname = (std::string(PLUGINSVC_FACTORY_NS "::") + ROOT::Reflex::PluginService::FactoryName(name));
        ROOT::Reflex::FunctionBuilder func(sig, fname.c_str(), stub, 0, "", ROOT::Reflex::PUBLIC);
        func.AddProperty("name", name).AddProperty("id", name);
	if ( PluginService::debug() )  {
	  std::string sig_name = sig.Name();
          printout(INFO,"PluginService","+++ Declared factory for id %s with signature %s.",fname.c_str(),sig_name.c_str());
        }
      }
    }

  }
}

#define DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(R, ARGS) namespace dd4hep {		\
  template <> void PluginRegistry< R ARGS >::add(const char* n, stub_t f) \
  {   plugin_signatures_namespace::reflex_plugin< R ARGS >(n,f);   } \
  namespace plugin_signatures_namespace { template void* instantiate_creator<R> ARGS ; }}

#endif

#endif  // DD4HEP_PLUGINS_INL
