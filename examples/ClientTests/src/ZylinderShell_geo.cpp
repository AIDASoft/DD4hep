// $Id: PolyconeSupport_geo.cpp 941 2013-12-12 18:47:03Z markus.frank@cern.ch $
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

static Ref_t create_detector(LCDD& lcdd, xml_h e, SensitiveDetector sensitive)  {
  xml_det_t  x_det   = e;
  string     name    = x_det.nameStr();
  DetElement sdet   (name,x_det.id());
  Assembly   assembly(name+"_assembly");
  Material   mat    (lcdd.material(x_det.materialStr()));
  PlacedVolume pv;

  for(xml_coll_t m(e,_U(module)); m; ++m)  {
    xml_comp_t mod = m;
    vector<double> rmin,rmax,z;
    int num = 0;
    for(xml_coll_t c(m,_U(zplane)); c; ++c, ++num)  {
      xml_comp_t dim(c);
      rmin.push_back(dim.rmin());
      rmax.push_back(dim.rmax());
      z.push_back(dim.z()/2);
    }
    if ( num < 2 )  {
      throw runtime_error("ZylinderShell["+name+"]> Not enough Z planes. minimum is 2!");
    }
    Polycone   cone  (0.,2*M_PI,rmin,rmax,z);
    Volume     volume(name, cone, mat);
    volume.setVisAttributes(lcdd, x_det.visStr());
    volume.setSensitiveDetector(sensitive);
    pv = assembly.placeVolume(volume);
    pv.addPhysVolID("barrel",mod.id());
  }

  pv = lcdd.pickMotherVolume(sdet).placeVolume(assembly);
  pv.addPhysVolID("system",x_det.id());
  sdet.setPlacement(pv);
  return sdet;
}

DECLARE_DETELEMENT(ZylinderShell,create_detector)
