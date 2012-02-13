// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DetFactoryHelper.h"
#include "CompactDetectors.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

namespace DD4hep { namespace Geometry {
  
  template <> Ref_t DetElementFactory<PolyconeSupport>::create(LCDD& lcdd, const xml_h& e, SensitiveDetector&)  {
    xml_det_t  x_det   = e;
    string     name    = x_det.nameStr();
    DetElement sdet   (lcdd,name,x_det.typeStr(),x_det.id());
    Polycone   cone   (lcdd,name+"_envelope_polycone");
    Material   mat    (lcdd.material(x_det.materialStr()));
    Volume     volume (lcdd,name+"_envelope_volume", cone, mat);
    vector<double> rmin,rmax,z;
    int num = 0;

    for(xml_coll_t c(e,_X(zplane)); c; ++c, ++num)  {
      xml_comp_t dim(c);
      rmin.push_back(dim.rmin());
      rmax.push_back(dim.rmax());
      z.push_back(dim.z()/2);
    }
    if ( num < 2 )  {
      throw runtime_error("PolyCone["+name+"]> Not enough Z planes. minimum is 2!");
    }
    cone.addZPlanes(rmin,rmax,z);
    volume.setVisAttributes(lcdd, x_det.visStr());
    sdet.addPlacement(lcdd.pickMotherVolume(sdet).placeVolume(volume));
    return sdet;
  }
}}

DECLARE_NAMED_DETELEMENT_FACTORY(DD4hep,PolyconeSupport);
