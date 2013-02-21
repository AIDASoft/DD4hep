// $Id:$
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

static Ref_t create_detector(LCDD& lcdd, XML::Element x_det, SensitiveDetector sens)  {
  Material   air       = lcdd.air();
  string     det_name  = x_det.attr<string>("name");
  int        det_id    = x_det.attr<int>("id");
  bool       reflect   = x_det.attr<bool>("reflect");
  DetElement sdet(det_name,det_id);
  Volume     motherVol = lcdd.pickMotherVolume(sdet);
  int l_num = 0;
    
  for(xml_coll_t i(x_det,"layer"); i; ++i, ++l_num)  {
    XML::Element  x_layer = i;
    string l_nam = det_name+_toString(l_num,"_layer%d");
    double  zmin = x_layer.attr<double>("inner_z");
    double  rmin = x_layer.attr<double>("inner_r");
    double  rmax = x_layer.attr<double>("outer_r");
    double  z    = zmin, layerWidth = 0.;
    int     s_num = 0;

    for(xml_coll_t j(x_layer,"slice"); j; ++j)  {
      double thickness = j.attr<double>("thickness");
      layerWidth += thickness;
    }
    Tube    l_tub(rmin,rmax,layerWidth,2*M_PI);
    Volume  l_vol(l_nam,l_tub,air);
    string  l_vis = x_layer.attr<string>("vis");
    l_vol.setVisAttributes(lcdd,l_vis);
    for(xml_coll_t j(x_layer,"slice"); j; ++j, ++s_num)  {
      XML::Element x_slice  = j;
      double thick    = x_slice.attr<double>("thickness");
      bool   is_sensitive = x_slice.attr<bool>("sensitive");
      string mat_name = x_slice.attr<string>("material");
      Material mat = lcdd.material(mat_name);
      string s_nam = l_nam+_toString(s_num,"_slice%d");
      Volume s_vol(s_nam, Tube(rmin,rmax,thick), mat);

      if ( x_slice.hasAttr("sensitive") )  {
	if ( x_slice.attr<bool>("sensitive") )  {
	  s_vol.setSensitiveDetector(sens);
	}
      }
      string region, limits, vis;
      if ( x_slice.hasAttr("region") )  {
	region = x_slice.attr<string>("region");
      }
      if ( x_slice.hasAttr("limits") )  {
	limits = x_slice.attr<string>("limits");
      }
      if ( x_slice.hasAttr("vis") )  {
	vis    = x_slice.attr<string>("vis");
      }
      s_vol.setAttributes(lcdd,region,limits,vis);

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
      lpvR.addPhysVolID(_X(system),sdet.id());
      lpvR.addPhysVolID(_X(barrel),2);
      DetElement layerR = layer.clone(l_nam+"_reflect");
      sdet.add(layerR.setPlacement(lpvR));
    }
  }
  sdet.setCombineHits(x_det.attr<bool>("combineHits"),sens);
  return sdet;
}

DECLARE_DETELEMENT(DiskTracker_no_shortcuts,create_detector);
