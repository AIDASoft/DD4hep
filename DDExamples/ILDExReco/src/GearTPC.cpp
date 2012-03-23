// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "ILDExTPC.h"
#include "GearTPC.h"
#include "DD4hep/Volumes.h"
#include "DD4hep/Shapes.h"
#include "TGeoTube.h"
#include "TPCData.h"
#include "DDTPCEndPlate.h"

using namespace std;

namespace DD4hep {
  using namespace Geometry;
  
  GearTPC::GearTPC(const Ref_t& e) : Geometry::DetElement(e) {
  }
  
  double GearTPC::getInnerRadius() const {
    DetElement innerWall   = data<TPCData>()->innerWall;
    Tube       tube  = innerWall.volume().solid();
    return tube->GetRmin();
  }
  double GearTPC::getOuterRadius() const {
    DetElement outerWall   = data<TPCData>()->outerWall;
    Tube       tube  = outerWall.volume().solid();
    return tube->GetRmax();
  }

  double GearTPC::getMaxDriftLength() const {
    DetElement gas   = data<TPCData>()->gas;
    Tube       tube  = gas.volume().solid();
    return tube->GetDz();
  }

  double GearTPC::getEndPlateThickness() const {
    DetElement ep   = data<TPCData>()->endplate;
    Tube       tube  = ep.volume().solid();
    return tube->GetDz();
  }


  int GearTPC::getNModules() const {
    DDTPCEndPlate dep   = child("TPC_EndPlate");
    return dep.getNModules();
  }

 
}
