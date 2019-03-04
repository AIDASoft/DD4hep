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
#include "DD4hep/detail/OpticalSurfaceManagerInterna.h"
#include "DD4hep/ExtensionEntry.h"
#include "DD4hep/Detector.h"
#include "DD4hep/Printout.h"

// C/C++ includes
#include <sstream>
#include <iomanip>

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
    Object* o = access();
    string  n = de.path() + '#' + nam;
    TGeoSkinSurface* surf = o->detector.manager().GetSkinSurface(n.c_str());
    if ( surf ) return surf;
    auto i = o->skinSurfaces.find(Object::LocalKey(de, nam));
    if ( i != o->skinSurfaces.end() ) return (*i).second;
    return 0;
  }
  except("SkinSurface",
         "++ Cannot access SkinSurface %s without valid detector element!",nam.c_str());
  return SkinSurface();
}

/// Access border surface by its identifier
BorderSurface  OpticalSurfaceManager::getBorderSurface(DetElement de, const string& nam)  const   {
  if ( de.isValid() )  {
    Object* o = access();
    string  n = de.path() + '#' + nam;
    TGeoBorderSurface* surf = o->detector.manager().GetBorderSurface(n.c_str());
    if ( surf ) return surf;
    auto i = o->borderSurfaces.find(Object::LocalKey(de, nam));
    if ( i != o->borderSurfaces.end() ) return (*i).second;
    return 0;
  }
  except("BorderSurface",
         "++ Cannot access BorderSurface %s without valid detector element!",nam.c_str());
  return BorderSurface();
}

/// Access optical surface data by its identifier
OpticalSurface OpticalSurfaceManager::getOpticalSurface(DetElement de, const string& nam)  const   {
  if ( de.isValid() )  {
    Object* o = access();
    string  n = de.path() + '#' + nam;
    TGeoOpticalSurface* surf = o->detector.manager().GetOpticalSurface(n.c_str());
    if ( surf ) return surf;
    auto i = o->opticalSurfaces.find(n);
    if ( i != o->opticalSurfaces.end() ) return (*i).second;
    return 0;
  }
  except("OpticalSurface",
         "++ Cannot access OpticalSurface %s without valid detector element!",nam.c_str());
  return OpticalSurface();
}

/// Add skin surface to manager
void OpticalSurfaceManager::addSkinSurface(DetElement de, SkinSurface surf)  const   {
  if ( access()->skinSurfaces.insert(make_pair(make_pair(de,surf->GetName()), surf)).second )
    return;
  except("OpticalSurfaceManager","++ Skin surface %s already present for DE:%s.",
         surf->GetName(), de.name());
}

/// Add border surface to manager
void OpticalSurfaceManager::addBorderSurface(DetElement de, BorderSurface surf)  const   {
  if ( access()->borderSurfaces.insert(make_pair(make_pair(de,surf->GetName()), surf)).second )
    return;
  except("OpticalSurfaceManager","++ Border surface %s already present for DE:%s.",
         surf->GetName(), de.name());
}

/// Add optical surface data to manager
void OpticalSurfaceManager::addOpticalSurface(OpticalSurface surf)  const   {
  if ( access()->opticalSurfaces.insert(make_pair(surf->GetName(), surf)).second )
    return;
  except("OpticalSurfaceManager","++ Optical surface %s already present.",
         surf->GetName());
}

/// Register the temporary surface objects with the TGeoManager
void OpticalSurfaceManager::registerSurfaces(DetElement subdetector)    {
  Object* o = access();
  unique_ptr<Object> extension(new Object(o->detector));
  for(auto& s : o->opticalSurfaces)  {
    //string n = s.first.first.path() + '#' + s.first.second;
    //s.second->SetName(n.c_str());
    o->detector.manager().AddOpticalSurface(s.second.ptr());
    extension->opticalSurfaces.insert(s);
  }
  o->opticalSurfaces.clear();
  
  for(auto& s : o->skinSurfaces)  {
    string n = s.first.first.path() + '#' + s.first.second;
    s.second->SetName(n.c_str());
    o->detector.manager().AddSkinSurface(s.second.ptr());
    extension->skinSurfaces.insert(s);
  }
  o->skinSurfaces.clear();
  
  for(auto& s : o->borderSurfaces)  {
    string n = s.first.first.path() + '#' + s.first.second;
    s.second->SetName(n.c_str());
    o->detector.manager().AddBorderSurface(s.second.ptr());
    extension->borderSurfaces.insert(s);
  }
  o->borderSurfaces.clear();
  
  if ( extension->opticalSurfaces.empty() &&
       extension->borderSurfaces.empty()  &&
       extension->skinSurfaces.empty() )   {
    return;
  }
  subdetector.addExtension(new detail::DeleteExtension<Object,Object>(extension.release()));
}
#endif
