// $Id: run_plugin.h 1663 2015-03-20 13:54:53Z gaede $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation européenne pour la Recherche nucléaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================
//
// Specialized generic detector constructor
// 
//==========================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "XML/Layering.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static Ref_t create_detector(LCDD& lcdd, xml_h e, SensitiveDetector sens)  {
  xml_det_t   x_det     = e;
  xml_dim_t   dim       = x_det.dimensions();
  int         det_id    = x_det.id();
  bool        reflect   = x_det.reflect(true);
  string      det_name  = x_det.nameStr();
  Material    air       = lcdd.air();
  int         numsides  = dim.numsides();
  double      rmin      = dim.rmin();
  double      rmax      = dim.rmax()*std::cos(M_PI/numsides);
  double      zmin      = dim.zmin();
  Layering    layering(x_det);
  double      totalThickness = layering.totalThickness();
  Volume      endcapVol("endcap",PolyhedraRegular(numsides,rmin,rmax,totalThickness),air);
  DetElement  endcap("endcap",det_id);

  int l_num = 1;
  int layerType   = 0;
  double layerZ   = -totalThickness/2;

  endcapVol.setAttributes(lcdd,x_det.regionStr(),x_det.limitsStr(),x_det.visStr());

  for(xml_coll_t c(x_det,_U(layer)); c; ++c)  {
    xml_comp_t       x_layer  = c;
    double           l_thick  = layering.layer(l_num-1)->thickness();
    string           l_name   = _toString(layerType,"layer%d");
    int              l_repeat = x_layer.repeat();
    Volume           l_vol(l_name,PolyhedraRegular(numsides,rmin,rmax,l_thick),air);
    vector<PlacedVolume> sensitives;

    int s_num = 1;
    double sliceZ = -l_thick/2;
    for(xml_coll_t s(x_layer,_U(slice)); s; ++s)  {
      xml_comp_t x_slice = s;
      string     s_name  = _toString(s_num,"slice%d");
      double     s_thick = x_slice.thickness();
      Material   s_mat   = lcdd.material(x_slice.materialStr());
      Volume     s_vol(s_name,PolyhedraRegular(numsides,rmin,rmax,s_thick),s_mat);
        
      s_vol.setVisAttributes(lcdd.visAttributes(x_slice.visStr()));
      sliceZ += s_thick/2;
      PlacedVolume s_phv = l_vol.placeVolume(s_vol,Position(0,0,sliceZ));
      s_phv.addPhysVolID("slice",s_num);
      if ( x_slice.isSensitive() )  {
	sens.setType("calorimeter");
	s_vol.setSensitiveDetector(sens);
	sensitives.push_back(s_phv);
      }
      sliceZ += s_thick/2;
      s_num++;
    }
    l_vol.setVisAttributes(lcdd.visAttributes(x_layer.visStr()));
    if ( l_repeat <= 0 ) throw std::runtime_error(x_det.nameStr()+"> Invalid repeat value");
    for(int j=0; j<l_repeat; ++j) {
      string phys_lay = _toString(l_num,"layer%d");
      layerZ += l_thick/2;
      DetElement    layer_elt(endcap, phys_lay, l_num);
      PlacedVolume  pv = endcapVol.placeVolume(l_vol,Position(0,0,layerZ));
      pv.addPhysVolID("layer", l_num);
      layer_elt.setPlacement(pv);
      for(size_t ic=0; ic<sensitives.size(); ++ic)  {
	PlacedVolume sens_pv = sensitives[ic];
	DetElement comp_elt(layer_elt,sens_pv.volume().name(),l_num);
	comp_elt.setPlacement(sens_pv);
      }
      layerZ += l_thick/2;
      ++l_num;
    }
    ++layerType;
  }

  double z_pos = zmin+totalThickness/2;
  PlacedVolume pv;
  // Reflect it.
  if ( reflect )  {
    Assembly    assembly(det_name);
    DetElement  both_endcaps(det_name,det_id);
    Volume      motherVol = lcdd.pickMotherVolume(both_endcaps);
    DetElement  sdetA = endcap;
    Ref_t(sdetA)->SetName((det_name+"_A").c_str());
    DetElement  sdetB = endcap.clone(det_name+"_B",x_det.id());

    pv = assembly.placeVolume(endcapVol,Transform3D(RotationZYX(M_PI/numsides,0,0),
						    Position(0,0,z_pos)));
    pv.addPhysVolID("barrel", 1);
    sdetA.setPlacement(pv);

    pv = assembly.placeVolume(endcapVol,Transform3D(RotationZYX(M_PI/numsides,M_PI,0),
						    Position(0,0,-z_pos)));
    pv.addPhysVolID("barrel", 2);
    sdetB.setPlacement(pv);

    pv = motherVol.placeVolume(assembly);
    pv.addPhysVolID("system", det_id);
    both_endcaps.setPlacement(pv);
    both_endcaps.add(sdetA);
    both_endcaps.add(sdetB);
    return both_endcaps;
  }
  Volume motherVol = lcdd.pickMotherVolume(endcap);
  pv = motherVol.placeVolume(endcapVol,Transform3D(RotationZYX(M_PI/numsides,0,0),
						 Position(0,0,z_pos)));
  pv.addPhysVolID("system", det_id);
  pv.addPhysVolID("barrel", 1);
  endcap.setPlacement(pv);
  Ref_t(endcap)->SetName(det_name.c_str());
  return endcap;
}

DECLARE_DETELEMENT(DD4hep_PolyhedraEndcapCalorimeter2,create_detector)
DECLARE_DEPRECATED_DETELEMENT(PolyhedraEndcapCalorimeter2,create_detector)
