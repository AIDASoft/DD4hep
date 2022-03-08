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
#ifndef DDEVE_FACTORIES_H
#define DDEVE_FACTORIES_H

#ifndef __CINT__
#include "DD4hep/Plugins.h"
#endif
#include "DDEve/View.h"
#include "DDEve/Display.h"

namespace {
  /// Base factory template
  template <typename P, typename S> class Factory;
  
  /// DDEve view factory
  DD4HEP_PLUGIN_FACTORY_ARGS_2(dd4hep::View*,dd4hep::Display*,const char*) { return new P(a0,a1); }
}

#define DECLARE_VIEW_FACTORY(x) \
DD4HEP_PLUGINSVC_FACTORY(x,DD4hep_DDEve_##x,dd4hep::View*(dd4hep::Display*, const char*),__LINE__)

#endif // DDEVE_FACTORIES_H
