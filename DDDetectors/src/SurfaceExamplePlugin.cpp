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
//
// Specialized generic detector plugin
// 
//==========================================================================
// Framework include files
namespace { struct UserData { /* int a,b,c; */ }; }
#define SURFACEINSTALLER_DATA UserData
#define DD4HEP_USE_SURFACEINSTALL_HELPER dd4hep_SurfaceExamplePlugin
#include "DD4hep/SurfaceInstaller.h"

namespace {
  template <> void Installer<UserData>::handle_arguments(int argc, char** argv)   {
    for(int i=0; i<argc; ++i)  {
      double value = -1;
      char* ptr = ::strchr(argv[i],'=');
      if ( ptr )  {
        value = dd4hep::_toDouble(++ptr);
      }
      std::cout << "SurfaceExamplePlugin: argument[" << i << "] = " << argv[i] 
                << " value = " << value << std::endl;
    }
  }

  /// Install measurement surfaces
  template <>
  void Installer<UserData>::install(dd4hep::DetElement /* component */, dd4hep::PlacedVolume /* pv */)   {
    // Do here whatever is necessary ....
  }
}
