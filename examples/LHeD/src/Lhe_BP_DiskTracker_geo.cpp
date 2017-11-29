//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
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
//             : P.Kostka   (LHeC more refined version circ-ell BP)
// 
//==========================================================================
#include "DD4hep/DetFactoryHelper.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

static Ref_t create_detector(Detector& description, xml_h e, SensitiveDetector sens)  {
  xml_det_t  x_det     = e;
  Material   air       = description.air();
  string     det_name  = x_det.nameStr();
  string     det_type  = x_det.typeStr();
  bool       reflect   = x_det.reflect();
  DetElement sdet        (det_name,x_det.id());
  Assembly   assembly    (det_name+"_assembly");
  PlacedVolume pv;
  int l_num = 0;
    
  for(xml_coll_t i(x_det,_U(layer)); i; ++i, ++l_num)  {
    xml_comp_t x_layer = i;
    string l_nam = det_name+_toString(l_num,"_layer%d");
    double  zmin = x_layer.inner_z();
    double  rmin = x_layer.inner_r();
    double  rmax = x_layer.outer_r();
    double  z    = zmin, layerWidth = 0.;
    int     s_num = 0;
    double  rmax_ell  = x_layer.rmax();

    for(xml_coll_t j(x_layer,_U(slice)); j; ++j)  {
      double thickness = xml_comp_t(j).thickness();
      layerWidth += thickness;
    }


    
    double ra    = 1.;             // ellipse long radius init
           ra    = rmax_ell;       // ellipse long radius
    double rb    = 1.;             // ellipse short radius init
           rb    = rmin;           // ellipse short radius
    double thickl = 0.;    // layer thickness
           thickl = layerWidth;    // layer thickness
    double rdiff = rmax - rmin;    // layer thickness
   
    EllipticalTube bpElTubeOut1(ra+rdiff, rb+rdiff, thickl);
    EllipticalTube bpElTubeInn1(ra, rb, thickl+0.1);
    SubtractionSolid bpElTube1(bpElTubeOut1,bpElTubeInn1);

    Tube bpTube1(rb, rb+rdiff, thickl+0.1, 3*M_PI/2, M_PI/2);
    UnionSolid beamTube1(bpElTube1,bpTube1);

    Tube bpTube2(rb+rdiff, ra+rdiff, thickl+0.1, 3*M_PI/2, M_PI/2);
    SubtractionSolid l_tub(beamTube1,bpTube2);

    Volume  l_vol(l_nam,l_tub,air);
    l_vol.setVisAttributes(description,x_layer.visStr());
    for(xml_coll_t j(x_layer,_U(slice)); j; ++j, ++s_num)  {
      xml_comp_t x_slice = j;
      double thicks = x_slice.thickness();
      Material mat = description.material(x_slice.materialStr());
      string s_nam = l_nam+_toString(s_num,"_slice%d");
    
    EllipticalTube bpElTubeOut2(ra+rdiff, rb+rdiff, thickl);
    EllipticalTube bpElTubeInn2(ra, rb, thickl+0.1);
    SubtractionSolid bpElTube2(bpElTubeOut2,bpElTubeInn2);

    Tube bpTube3(rb, rb+rdiff, thickl+0.1, 3*M_PI/2, M_PI/2);
    UnionSolid beamTube2(bpElTube2,bpTube3);

    Tube bpTube4(rb+rdiff, ra+rdiff, thickl+0.1, 3*M_PI/2, M_PI/2);
    SubtractionSolid s_tub(beamTube2,bpTube4);

    Volume   s_vol(s_nam, s_tub, mat);
    s_vol.setVisAttributes(description,x_layer.visStr());
        
      if ( x_slice.isSensitive() ) {
	    sens.setType("tracker");
	    s_vol.setSensitiveDetector(sens);
       }
    l_vol.setAttributes(description,x_slice.regionStr(),x_slice.limitsStr(),x_slice.visStr());
    pv = s_vol.placeVolume(s_vol,Position(0,0,z-zmin-layerWidth/2+thicks/2));
    pv.addPhysVolID("slice",s_num);
    }

    DetElement layer(sdet,l_nam+"_pos",l_num);
    pv = assembly.placeVolume(l_vol,Position(0,0,zmin+layerWidth/2.));
    pv.addPhysVolID("layer",l_num);
    pv.addPhysVolID("barrel",1);
    layer.setPlacement(pv);
    if ( reflect )  {
      pv = assembly.placeVolume(l_vol,Transform3D(RotationY(2*M_PI),Position(0,0,-zmin-layerWidth/2)));
      pv.addPhysVolID("layer",l_num);
      pv.addPhysVolID("barrel",2);
      DetElement layerR = layer.clone(l_nam+"_neg");
      sdet.add(layerR.setPlacement(pv));
    }
  }
  if ( x_det.hasAttr(_U(combineHits)) ) {
    sdet.setCombineHits(x_det.attr<bool>(_U(combineHits)),sens);
  }
  pv = description.pickMotherVolume(sdet).placeVolume(assembly);
  pv.addPhysVolID("system", x_det.id());      // Set the subdetector system ID.
  sdet.setPlacement(pv);
  return sdet;
}

DECLARE_DETELEMENT(Lhe_BP_DiskTracker,create_detector)
