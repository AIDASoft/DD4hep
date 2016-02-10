// $Id$
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
#include "DD4hep/PluginCreators.h"
#include "DD4hep/Primitives.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Plugins.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  static inline ComponentCast* component(void* p) { return (ComponentCast*)p; }

  void* createPlugin(const std::string& factory, Geometry::LCDD& lcdd, int argc, char** argv, void* (*cast)(void*))
  {
    void* object = PluginService::Create<void*>(factory, &lcdd, argc, argv);
    if ( !object ) {
      PluginDebug dbg;
      object = PluginService::Create<void*>(factory, &lcdd, argc, argv);
      if ( !object )  {
        except("ConditionsManager","DD4hep: plugin: Failed to locate plugin %s. [%s].",
               factory.c_str(), dbg.missingFactory(factory).c_str());
      }
    }
    if ( cast )   {
      void* obj = cast(object);
      if ( obj ) return obj;
      invalidHandleAssignmentError(typeid(cast),typeid(*component(object)));
    }
    return object;
  }

  /// Handler for factories of type: ConstructionFactory
  void* createPlugin(const std::string& factory, Geometry::LCDD& lcdd, void* (*cast)(void*))
  {
    char* argv[] = {0};
    int   argc = 0;
    return createPlugin(factory, lcdd, argc, argv, cast);
  }
  /// Handler for factories of type: ConstructionFactory
  void* createPlugin(const std::string& factory, 
                     Geometry::LCDD& lcdd, 
                     const std::string& arg,
                     void* (*cast)(void*))   {
    char* argv[] = { (char*)arg.c_str(), 0 };
    int   argc = 1;
    return createPlugin(factory, lcdd, argc, argv, cast);
  }

}
