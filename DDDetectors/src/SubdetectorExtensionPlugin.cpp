// $Id: $
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
//
// Specialized generic detector plugin
// 
//==========================================================================
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
