// $Id: PolyhedraEndcapCalorimeter2_geo.cpp 1306 2014-08-22 12:29:38Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "XML/Layering.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static Ref_t create_detector(LCDD& lcdd, xml_h e, SensitiveDetector sens)  {
  xml_det_t   x_det     = e;
  xml_dim_t   dim       = x_det.dimensions();
  int         det_id    = x_det.id();
  string      det_name  = x_det.nameStr();
  bool        reflect   = x_det.reflect(true);
  Material    air       = lcdd.air();
  int         numsides  = dim.numsides();
  double      rmin      = dim.rmin();
  double      rmax      = dim.rmax()*std::cos(M_PI/numsides);
  double      zmin      = dim.zmin();
  Layering    layering(x_det);
  double      totalThickness = layering.totalThickness();
  Volume      envelopeVol("envelope",PolyhedraRegular(numsides,rmin,rmax,totalThickness),air);
    
  int l_num = 1;
  int layerType   = 0;
  double layerZ   = -totalThickness/2;

  envelopeVol.setAttributes(lcdd,x_det.regionStr(),x_det.limitsStr(),x_det.visStr());

  for(xml_coll_t c(x_det,_U(layer)); c; ++c)  {
    xml_comp_t       x_layer  = c;
    double           l_thick  = layering.layer(l_num-1)->thickness();
    string           l_name   = _toString(layerType,"layer%d");
    int              l_repeat = x_layer.repeat();
    Volume           l_vol(l_name,PolyhedraRegular(numsides,rmin,rmax,l_thick),air);
      
    int s_num = 1;
    double sliceZ = -l_thick/2;
    for(xml_coll_t s(x_layer,_U(slice)); s; ++s)  {
      xml_comp_t x_slice = s;
      string     s_name  = _toString(s_num,"slice%d");
      double     s_thick = x_slice.thickness();
      Material   s_mat   = lcdd.material(x_slice.materialStr());
      Volume     s_vol(s_name,PolyhedraRegular(numsides,rmin,rmax,s_thick),s_mat);
        
      if ( x_slice.isSensitive() )  {
	sens.setType("calorimeter");
	s_vol.setSensitiveDetector(sens);
      }
      s_vol.setVisAttributes(lcdd.visAttributes(x_slice.visStr()));
      sliceZ += s_thick/2;
      PlacedVolume s_phv = l_vol.placeVolume(s_vol,Position(0,0,sliceZ));
      s_phv.addPhysVolID("slice",s_num);
      s_num++;
    }
    l_vol.setVisAttributes(lcdd.visAttributes(x_layer.visStr()));
    if ( l_repeat <= 0 ) throw std::runtime_error(det_name+"> Invalid repeat value");
    for(int j=0; j<l_repeat; ++j) {
      string phys_lay = det_name + _toString(l_num,"_layer%d");
      layerZ += l_thick/2;
      PlacedVolume  phys_vol = envelopeVol.placeVolume(l_vol,Position(0,0,layerZ));
      phys_vol.addPhysVolID("layer", l_num);
      layerZ += l_thick/2;
      ++l_num;
    }
    ++layerType;
  }

  DetElement sdet(det_name,det_id);
  Volume motherVol = lcdd.pickMotherVolume(sdet);
  // Reflect it.
  if ( reflect )  {
    Assembly assembly(det_name);
    PlacedVolume pv = motherVol.placeVolume(assembly);
    pv.addPhysVolID("system", det_id);
    sdet.setPlacement(pv);

    DetElement   sdetA(sdet,det_name+"_A",x_det.id());
    pv = assembly.placeVolume(envelopeVol,Transform3D(RotationZYX(M_PI/numsides,0,0),
						      Position(0,0,zmin+totalThickness/2)));
    pv.addPhysVolID("barrel", 1);
    sdetA.setPlacement(pv);

    DetElement   sdetB = sdetA.clone(det_name+"_B",x_det.id());
    sdet.add(sdetB);
    pv = assembly.placeVolume(envelopeVol,Transform3D(RotationZYX(M_PI/numsides,M_PI,0),
						Position(0,0,-(zmin+totalThickness/2))));
    pv.addPhysVolID("barrel", 2);
    sdetB.setPlacement(pv);
  }
  else  {
    PlacedVolume pv = motherVol.placeVolume(envelopeVol,
					    Transform3D(RotationZYX(M_PI/numsides,0,0),
							Position(0,0,zmin+totalThickness/2)));
    pv.addPhysVolID("system", det_id);
    pv.addPhysVolID("barrel", 1);
    sdet.setPlacement(pv);
  }

  return sdet;
}

DECLARE_DETELEMENT(DD4hep_PolyhedraEndcapCalorimeter2,create_detector)
DECLARE_DEPRECATED_DETELEMENT(PolyhedraEndcapCalorimeter2,create_detector)
