// $Id: SiTrackerBarrel_geo.cpp 1360 2014-10-27 16:32:06Z Nikiforos.Nikiforou@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
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
