// $Id: SiTrackerBarrel_geo.cpp 1360 2014-10-27 16:32:06Z Nikiforos.Nikiforou@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
// Framework include files
#include "DDRec/Extensions/SubdetectorExtensionImpl.h"
#define DD4HEP_USE_SURFACEINSTALL_HELPER SubdetectorExtensionPlugin
#include "DD4hep/SurfaceInstaller.h"

/// Install volume information. Default implementation only prints!
template <typename T> void Installer<T>::install(DetElement /* layer */, PlacedVolume /* pv */)   {
  using namespace DD4hep::DDRec;
  SubdetectorExtensionImpl* e = new SubdetectorExtensionImpl(m_det);
  m_det.addExtension<SubdetectorExtension>(e);
  stopScanning();
  std::cout << " Installed subdetector extension:" << m_det.name() << " id: " << m_det.id() << std::endl;
}
