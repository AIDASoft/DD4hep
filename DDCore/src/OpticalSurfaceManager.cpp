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
#include "DD4hep/OpticalSurfaceManager.h"
#include "DD4hep/Detector.h"
#include "DD4hep/Printout.h"
#include "DD4hep/detail/Handle.inl"

// C/C++ includes
#include <sstream>
#include <iomanip>

DD4HEP_INSTANTIATE_HANDLE(TGeoManager);

using namespace std;
using namespace dd4hep;

/// static accessor calling DD4hepOpticalSurfaceManagerPlugin if necessary
OpticalSurfaceManager OpticalSurfaceManager::getOpticalSurfaceManager(Detector& description)  {
  return description.surfaceManager();
}

#if ROOT_VERSION_CODE > ROOT_VERSION(6,16,0)

/// Access skin surface by its identifier
SkinSurface  OpticalSurfaceManager::getSkinSurface(DetElement de, const string& nam)  const   {
  if ( de.isValid() )  {
    string n = de.path() + '#' + nam;
    return access()->GetSkinSurface(n.c_str());
  }
  except("SkinSurface",
         "++ Cannot access SkinSurface %s without valid detector element!",nam.c_str());
  return SkinSurface();
}

/// Access border surface by its identifier
BorderSurface  OpticalSurfaceManager::getBorderSurface(DetElement de, const string& nam)  const   {
  if ( de.isValid() )  {
    string n = de.path() + '#' + nam;
    return access()->GetBorderSurface(n.c_str());
  }
  except("BorderSurface",
         "++ Cannot access BorderSurface %s without valid detector element!",nam.c_str());
  return BorderSurface();
}

/// Access optical surface data by its identifier
OpticalSurface OpticalSurfaceManager::getSurface(DetElement de, const string& nam)  const   {
  if ( de.isValid() )  {
    string n = de.path() + '#' + nam;
    return access()->GetOpticalSurface(n.c_str());
  }
  except("OpticalSurface",
         "++ Cannot access OpticalSurface %s without valid detector element!",nam.c_str());
  return OpticalSurface();
}

/// Add skin surface to manager
void OpticalSurfaceManager::addSkinSurface(SkinSurface surf)  const   {
  access()->AddSkinSurface(surf.ptr());
}

/// Add border surface to manager
void OpticalSurfaceManager::addBorderSurface(BorderSurface surf)  const   {
  access()->AddBorderSurface(surf.ptr());
}

/// Add optical surface data to manager
void OpticalSurfaceManager::addSurface(OpticalSurface surf)  const   {
  access()->AddOpticalSurface(surf.ptr());
}
#endif
