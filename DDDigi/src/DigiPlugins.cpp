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
#include <DD4hep/Printout.h>
#include <DD4hep/Primitives.h>
#include <DD4hep/Plugins.h>
#include <DDDigi/DigiKernel.h>
#include <DDDigi/DigiAction.h>
#include <DDDigi/DigiPlugins.h>

static inline dd4hep::ComponentCast* component(void* p) { return (dd4hep::ComponentCast*)p; }

void* dd4hep::digi::create_action(const std::string& factory,
				  const dd4hep::digi::DigiKernel& kernel,
				  const std::string& arg, void* (*cast)(void*))   {
  void* object = PluginService::Create<DigiAction*>(factory, &kernel, arg);
  if ( !object ) {
    PluginDebug dbg;
    object = PluginService::Create<DigiAction*>(factory, &kernel, arg);
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
