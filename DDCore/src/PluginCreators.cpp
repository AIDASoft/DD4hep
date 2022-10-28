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
#include <DD4hep/PluginCreators.h>
#include <DD4hep/Primitives.h>
#include <DD4hep/Printout.h>
#include <DD4hep/Plugins.h>

// C/C++ include files
#include <cstring>
//#include <dlfcn.h>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  static inline ComponentCast* component(void* p) { return (ComponentCast*)p; }

  void* createProcessor(Detector& description, int argc, char** argv, void* (*cast)(void*))  {
    void* processor = 0;
    if ( argc < 2 )   {
      except("createProcessor","++ dd4hep-plugins: No processor creator name given!");
    }
    for(int i=0; i<argc; ++i)  {
      if ( 0 == ::strncmp(argv[i],"-processor",4) )  {
        std::vector<char*> args;
        std::string fac = argv[++i];
        for(int j=++i; j<argc && argv[j] &&
              0 != ::strncmp(argv[j],"-processor",4) &&
              0 != ::strncmp(argv[j],"-end-processor",8); ++j)
          args.emplace_back(argv[j]);
        int num_arg = int(args.size());
        args.emplace_back(nullptr);
        processor = PluginService::Create<void*>(fac,&description,num_arg,&args[0]);
        if ( !processor ) {
          PluginDebug dbg;
          processor = PluginService::Create<void*>(fac,&description,num_arg,&args[0]);
          if ( !processor )  {
            except("createProcessor","dd4hep-plugins: Failed to locate plugin %s. \n%s %s",
                   fac.c_str(), dbg.missingFactory(fac).c_str(),
                   /* ::dlerror() ? ::dlerror() : */ "");
          }
        }
        if ( cast )   {
          void* obj = cast(processor);
          if ( obj ) return obj;
          ComponentCast* c = component(processor);
          invalidHandleAssignmentError(typeid(cast),typeid(*c));
        }
      }
    }
    if ( !processor )  {
      except("createProcessor",
             "dd4hep-plugins: Found arguments in plugin call, but could not make any sense of them: %s",
             arguments(argc,argv).c_str());
    }
    return processor;
  }

  void* createPlugin(const std::string& factory, Detector& description, int argc, char** argv, void* (*cast)(void*))  {
    void* object = PluginService::Create<void*>(factory, &description, argc, argv);
    if ( !object ) {
      PluginDebug dbg;
      object = PluginService::Create<void*>(factory, &description, argc, argv);
      if ( !object )  {
        except("createPlugin","dd4hep-plugins: Failed to locate plugin %s. \n%s.",
               factory.c_str(), dbg.missingFactory(factory).c_str());
      }
    }
    if ( cast )   {
      void* obj = cast(object);
      if ( obj ) return obj;
      ComponentCast* c = component(object);
      invalidHandleAssignmentError(typeid(cast),typeid(*c));
    }
    return object;
  }

  /// Handler for factories of type: ConstructionFactory
  void* createPlugin(const std::string& factory, Detector& description, void* (*cast)(void*))  {
    char* argv[] = {0};
    int   argc = 0;
    return createPlugin(factory, description, argc, argv, cast);
  }
  /// Handler for factories of type: ConstructionFactory
  void* createPlugin(const std::string& factory, 
                     Detector& description, 
                     const std::string& arg,
                     void* (*cast)(void*))   {
    char* argv[] = { (char*)arg.c_str(), 0 };
    int   argc = 1;
    return createPlugin(factory, description, argc, argv, cast);
  }

}
