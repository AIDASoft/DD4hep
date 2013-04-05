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

static Ref_t create_detector(LCDD& lcdd, xml_h e, SensitiveDetector sens)  {
  xml_det_t  x_det     = e;
  Material   air       = lcdd.air();
  string     det_name  = x_det.nameStr();
  bool       reflect   = x_det.reflect();
  DetElement sdet(det_name,x_det.id());
  Volume     motherVol = lcdd.pickMotherVolume(sdet);
  int l_num = 0;
    
  for(xml_coll_t i(x_det,_U(layer)); i; ++i, ++l_num)  {
    xml_comp_t x_layer = i;
    string l_nam = det_name+_toString(l_num,"_layer%d");
    double  zmin = x_layer.inner_z();
    double  rmin = x_layer.inner_r();
    double  rmax = x_layer.outer_r();
    double  z    = zmin, layerWidth = 0.;
    int     s_num = 0;
      
    for(xml_coll_t j(x_layer,_U(slice)); j; ++j)  {
      double thickness = xml_comp_t(j).thickness();
      layerWidth += thickness;
    }
    Tube    l_tub(rmin,rmax,layerWidth,2*M_PI);
    Volume  l_vol(l_nam,l_tub,air);
    l_vol.setVisAttributes(lcdd,x_layer.visStr());
    for(xml_coll_t j(x_layer,_U(slice)); j; ++j, ++s_num)  {
      xml_comp_t x_slice = j;
      double thick = x_slice.thickness();
      Material mat = lcdd.material(x_slice.materialStr());
      string s_nam = l_nam+_toString(s_num,"_slice%d");
      Volume s_vol(s_nam, Tube(rmin,rmax,thick), mat);
        
      if ( x_slice.isSensitive() ) {
	sens.setType("tracker");
	s_vol.setSensitiveDetector(sens);
      }
      s_vol.setAttributes(lcdd,x_slice.regionStr(),x_slice.limitsStr(),x_slice.visStr());

      PlacedVolume spv = l_vol.placeVolume(s_vol,Position(0,0,z-zmin-layerWidth/2+thick/2));
      spv.addPhysVolID("layer",l_num);
      spv.addPhysVolID("slice",s_num);
    }

    PlacedVolume lpv = motherVol.placeVolume(l_vol,Position(0,0,zmin+layerWidth/2.));
    lpv.addPhysVolID("system",sdet.id());
    lpv.addPhysVolID("barrel",1);
    DetElement layer(sdet,l_nam,l_num);
    layer.setPlacement(lpv);
    if ( reflect )  {
      PlacedVolume lpvR = motherVol.placeVolume(l_vol,Position(0,0,-zmin-layerWidth/2),ReflectRot());
      lpvR.addPhysVolID("system",sdet.id());
      lpvR.addPhysVolID("barrel",2);
      DetElement layerR = layer.clone(l_nam+"_reflect");
      sdet.add(layerR.setPlacement(lpvR));
    }
  }
  if ( x_det.hasAttr(_U(combineHits)) ) {
    sdet.setCombineHits(x_det.attr<bool>(_U(combineHits)),sens);
  }
  return sdet;
}

DECLARE_DETELEMENT(DiskTracker,create_detector);
