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
#include "DD4hep/DetectorHelper.h"

// ROOT include files
#include "TGeoManager.h"

using namespace std;
using namespace dd4hep;

/// Access the sensitive detector of a given subdetector (if the sub-detector is sensitive!)
SensitiveDetector DetectorHelper::sensitiveDetector(const std::string& detector) const    {
  const string& det_name = detector;
  SensitiveDetector sensitive = ptr()->sensitiveDetector(det_name);
  return sensitive;
}

/// Given a detector element, access it's sensitive detector (if the sub-detector is sensitive!)
SensitiveDetector DetectorHelper::sensitiveDetector(DetElement detector) const    {
  for(DetElement par = detector; par.isValid(); par = par.parent())  {
    if ( par.ptr() != ptr()->world().ptr() )  {
      PlacedVolume pv = par.placement();
      if ( pv.isValid() )   {
        const auto& ids = pv.volIDs();
        for(const auto& i : ids )  {
          if ( i.first == "system" )   {
            return sensitiveDetector(par.name());
          }
        }
      }
    }
  }
  return SensitiveDetector();
}

/// Find a detector element by it's system ID
DetElement DetectorHelper::detectorByID(int id)  const    {
  const Detector::HandleMap& detectors = ptr()->detectors();
  for(const auto& det : detectors )  {
    DetElement de(det.second);
    if ( de.id() == id ) return de;
  }
  return DetElement();
}

/// Access an element from the element table by name
Atom DetectorHelper::element(const std::string& nam)  const   {
  TGeoManager&      mgr = access()->manager();
  TGeoElementTable* tab = mgr.GetElementTable();
  TGeoElement*      elt = tab->FindElement(nam.c_str());
  if ( !elt )    {
    string n = nam;
    transform(n.begin(), n.end(), n.begin(), ::toupper);
    elt = tab->FindElement(n.c_str());     // Check for IRON
    if ( !elt )    {
      transform(n.begin(), n.end(), n.begin(), ::tolower);
      elt = tab->FindElement(n.c_str());   // Check for iron
      if ( !elt )    {
        n[0] = ::toupper(n[0]);
        elt = tab->FindElement(n.c_str()); // Check for Iron
      }
    }
  }
  return elt;
}

/// Access a material from the material table by name
Material DetectorHelper::material(const std::string& nam)  const   {
  TGeoManager& mgr = access()->manager();
  TGeoMedium*  med = mgr.GetMedium(nam.c_str());
  if ( !med )    {
    string n = nam;
    transform(n.begin(), n.end(), n.begin(), ::toupper);
    med = mgr.GetMedium(n.c_str());     // Check for IRON
    if ( !med )    {
      transform(n.begin(), n.end(), n.begin(), ::tolower);
      med = mgr.GetMedium(n.c_str());   // Check for iron
      if ( !med )    {
        n[0] = ::toupper(n[0]);
        med = mgr.GetMedium(n.c_str()); // Check for Iron
      }
    }
  }
  return med;
}
