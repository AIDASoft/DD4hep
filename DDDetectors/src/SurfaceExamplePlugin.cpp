// $Id: run_plugin.h 1663 2015-03-20 13:54:53Z gaede $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation européenne pour la Recherche nucléaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================
//
// Specialized generic detector plugin
// 
//==========================================================================
// Framework include files
namespace { struct UserData { int a,b,c; }; }
#define SURFACEINSTALLER_DATA UserData
#define DD4HEP_USE_SURFACEINSTALL_HELPER DD4hep_SurfaceExamplePlugin
#include "DD4hep/SurfaceInstaller.h"

namespace {
  template <> void Installer<UserData>::handle_arguments(int argc, char** argv)   {
    for(int i=0; i<argc; ++i)  {
      double value = -1;
      char* ptr = ::strchr(argv[i],'=');
      if ( ptr )  {
        value = DD4hep::Geometry::_toDouble(++ptr);
      }
      std::cout << "SurfaceExamplePlugin: argument[" << i << "] = " << argv[i] 
                << " value = " << value << std::endl;
    }
  }

  /// Install measurement surfaces
  template <>
  void Installer<UserData>::install(DetElement /* component */, PlacedVolume /* pv */)   {
    // Do here whatever is necessary ....
  }
}
