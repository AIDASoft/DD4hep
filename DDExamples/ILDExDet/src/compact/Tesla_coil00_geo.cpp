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

static Ref_t create_element(LCDD& lcdd, const xml_h& e, SensitiveDetector& sens)  {
  xml_det_t   x_det  = e;
  string      name   = x_det.nameStr();
  DetElement  sdet(name,x_det.id());
  Assembly    assembly(name);
  xml_comp_t  x_coil = x_det.child(Unicode("coil"));

  Tube   coilTub(x_coil.inner_r(),x_coil.outer_r(),x_coil.zhalf());
  Volume coilVol(name+"_coil",coilTub,lcdd.material(x_coil.materialStr()));
  coilVol.setVisAttributes(lcdd.visAttributes(x_det.visStr()));
  assembly.placeVolume(coilVol);

  PlacedVolume pv=lcdd.pickMotherVolume(sdet).placeVolume(assembly);
  sdet.setPlacement(pv);
  return sdet;
}

DECLARE_DETELEMENT(Tesla_coil00,create_element);
