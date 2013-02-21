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
  xml_det_t    x_det  = e;
  string       name   = x_det.nameStr();
  DetElement   sdet(name,x_det.id());
  Assembly     sitVol(name+"_assembly");
  Volume       motherVol   = lcdd.pickMotherVolume(sdet);
  PlacedVolume pv;

  for(xml_coll_t c(x_det,Unicode("layer")); c; ++c)  {
    xml_comp_t x_layer = c;
    double zhalf = x_layer.zhalf();
    string layer_nam = _toString(x_layer.id(),"_layer%d");
    DetElement layer_det(sdet,layer_nam,x_layer.id());
    Tube   tub(x_layer.inner_r(),x_layer.inner_r()+x_layer.thickness(),zhalf);
    Volume vol(layer_nam,tub,lcdd.material(x_layer.materialStr()));
    vol.setVisAttributes(lcdd.visAttributes(x_layer.visStr()));
    vol.setSensitiveDetector(sens);
    pv = sitVol.placeVolume(vol);
    layer_det.setPlacement(pv);
  }
  pv = motherVol.placeVolume(sitVol);
  sdet.setPlacement(pv);
  return sdet;
}

DECLARE_DETELEMENT(Tesla_sit00,create_element);
