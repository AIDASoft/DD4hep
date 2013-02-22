// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "TGeoTube.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static Ref_t create_element(LCDD& lcdd, xml_h e, SensitiveDetector sens)  {
  struct param_t { double inner, outer, zhalf; };
  xml_det_t   x_det  = e;
  string      name   = x_det.nameStr();
  DetElement  sdet(name,x_det.id());
  Assembly    assembly(name+"_assembly");
  xml_comp_t  x_barrel = x_det.child(Unicode("barrel"));
  xml_comp_t  x_endcap = x_det.child(Unicode("endcap"));
  param_t barrel = { x_barrel.inner_r(), x_barrel.outer_r(), x_barrel.zhalf()};
  param_t endcap = { x_endcap.inner_r(), x_endcap.outer_r(), x_endcap.thickness()/2.0};
  int symmetry   = x_det.child(Unicode("symmetry")).attr<int>(_U(value));
  double tilt    = M_PI/symmetry - M_PI/2.0;

  // Visualisation attributes
  VisAttr vis = lcdd.visAttributes(x_det.visStr());

  lcdd.pickMotherVolume(sdet).placeVolume(assembly);
  return sdet;
}

DECLARE_DETELEMENT(Tesla_template,create_element);
