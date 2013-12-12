// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#include "DD4hep/DetFactoryHelper.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static Ref_t create_detector(LCDD& lcdd, xml_h e, Ref_t)  {
  xml_det_t  x_det   = e;
  string     name    = x_det.nameStr();
  DetElement sdet   (name,x_det.id());
  Material   mat    (lcdd.material(x_det.materialStr()));
  vector<double> rmin,rmax,z;
  int num = 0;

  for(xml_coll_t c(e,_U(zplane)); c; ++c, ++num)  {
    xml_comp_t dim(c);
    rmin.push_back(dim.rmin());
    rmax.push_back(dim.rmax());
    z.push_back(dim.z()/2);
  }
  if ( num < 2 )  {
    throw runtime_error("PolyCone["+name+"]> Not enough Z planes. minimum is 2!");
  }
  Polycone   cone  (0.,2.*M_PI*RAD_2_DEGREE,rmin,rmax,z);
  Volume     volume(name, cone, mat);
  volume.setVisAttributes(lcdd, x_det.visStr());
  sdet.setPlacement(lcdd.pickMotherVolume(sdet).placeVolume(volume));
  return sdet;
}

DECLARE_DETELEMENT(PolyconeSupport,create_detector);
