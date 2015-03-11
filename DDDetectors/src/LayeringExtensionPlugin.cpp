// $Id: SiTrackerBarrel_geo.cpp 1360 2014-10-27 16:32:06Z Nikiforos.Nikiforou@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#include <map>
#include "DDRec/Extensions/LayeringExtensionImpl.h"
using namespace DD4hep::DDRec;
using namespace DD4hep::Geometry;

namespace  {
  struct  LayeringContext {
  public:
    DetElement det, parent;
    std::map<int,DetElement> layers;
    LayeringContext() {}
    ~LayeringContext()  {
      LayeringExtensionImpl* e = new LayeringExtensionImpl();
      det.addExtension<LayeringExtension>(e);
      for(std::map<int,DetElement>::const_iterator i=layers.begin(); i!=layers.end();++i)   {
	DetElement de = (*i).second;
	e->setLayer(de.id(), de, Position(0,0,1));
	std::cout << " Add layer:" << de.name() 
		  << " ID: " << de.id() 
		  << " Parent:" << de.parent().name() << std::endl;
      }
    }
  };
}
#define SURFACEINSTALLER_DATA LayeringContext
#define DD4HEP_USE_SURFACEINSTALL_HELPER LayeringExtensionPlugin
#include "DD4hep/SurfaceInstaller.h"


/// Install volume information. Default implementation only prints!
template <typename T> void Installer<T>::install(DetElement de, PlacedVolume /* pv */) {
  if ( ::strstr(de.name(),"layer") != 0 )   {
    if ( !data.parent.isValid() )  {
      data.det = m_det;
      data.parent = de.parent();
    }
    if ( data.parent == de.parent() && data.layers.find(de.id()) == data.layers.end() )  {
      data.layers[de.id()] = de;
    }
  }
}

