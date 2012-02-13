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

using namespace std;

namespace DD4hep {
  using namespace Geometry;
  
  GearTPC::GearTPC(const Ref_t& e) : Geometry::DetElement(e) {
  }
  
  double GearTPC::innerRadius() const {
    DetElement gas   = data<Object>()->gas;
    Tube       tube  = gas.volume().solid();
    return tube->GetRmin();
  }
  double GearTPC::outerRadius() const {
    DetElement gas   = data<Object>()->gas;
    Tube       tube  = gas.volume().solid();
    return tube->GetRmax();
  }
  double GearTPC::pressure() const {
    return data<Object>()->pressure;
  }
}
