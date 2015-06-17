// $Id: Handle.h 1679 2015-03-27 15:24:39Z /DC=ch/DC=cern/OU=Organic Units/OU=Users/CN=sailer/CN=683529/CN=Andre Sailer $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DD4hep/LCDDHelper.h"

using namespace std;
using namespace DD4hep::Geometry;

/// Access the sensitive detector of a given subdetector (if the sub-detector is sensitive!)
SensitiveDetector LCDDHelper::sensitiveDetector(const std::string& detector) const    {
  const string& det_name = detector;
  SensitiveDetector sensitive = ptr()->sensitiveDetector(det_name);
  return sensitive;
}

/// Given a detector element, access it's sensitive detector (if the sub-detector is sensitive!)
SensitiveDetector LCDDHelper::sensitiveDetector(DetElement detector) const    {
  for(DetElement par = detector; par.isValid(); par = par.parent())  {
    if ( par.ptr() != ptr()->world().ptr() )  {
      PlacedVolume pv = par.placement();
      if ( pv.isValid() )   {
	const PlacedVolume::VolIDs& ids = pv.volIDs();
	for(PlacedVolume::VolIDs::const_iterator i=ids.begin(); i!=ids.end();++i)  {
	  if ( (*i).first == "system" )   {
	    return sensitiveDetector(par.name());
	  }
	}
      }
    }
  }
  return SensitiveDetector(0);
}

/// Find a detector element by it's system ID
DetElement LCDDHelper::detectorByID(int id)  const    {
  const LCDD::HandleMap& dets = ptr()->detectors();
  for(LCDD::HandleMap::const_iterator i=dets.begin(); i!=dets.end(); ++i)   {
    DetElement de = (*i).second;
    if ( de.id() == id ) return de;
  }
  return DetElement(0);
}

