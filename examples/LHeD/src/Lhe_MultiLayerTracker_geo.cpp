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
//
//  mod.:        P.Kostka LHeD (asymmetrical detector placement in z)
// 
//==========================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "XML/Layering.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

static Ref_t create_detector(Detector& description, xml_h e, SensitiveDetector sens)  {
  xml_det_t  x_det     = e;
  string     det_name  = x_det.nameStr();
  Material   air       = description.air();
  DetElement sdet        (det_name,x_det.id());
  Assembly   assembly    (det_name+"_assembly");
  PlacedVolume pv;
  int n = 0;

  for(xml_coll_t i(x_det,_U(layer)); i; ++i, ++n)  {
    xml_comp_t x_layer = i;
    string  l_name = det_name+_toString(n,"_layer%d");
    double  z    = x_layer.outer_z();
    double  rmin = x_layer.inner_r();
    double  r    = rmin;
    DetElement layer(sdet,_toString(n,"layer%d"),x_layer.id());
    Tube    l_tub (rmin,2*rmin,z);
    Volume  l_vol(l_name,l_tub,air);
    int im = 0;

    for(xml_coll_t j(x_layer,_U(slice)); j; ++j, ++im)  {
      xml_comp_t x_slice = j;
      Material mat = description.material(x_slice.materialStr());
      string s_name= l_name+_toString(im,"_slice%d");
      double thickness = x_slice.thickness();
      Tube   s_tub(r,r+thickness,z,2*M_PI);
      Volume s_vol(s_name, s_tub, mat);

      r += thickness;
      if ( x_slice.isSensitive() ) {
        sens.setType("tracker");
        s_vol.setSensitiveDetector(sens);
      }
      // Set Attributes
      s_vol.setAttributes(description,x_slice.regionStr(),x_slice.limitsStr(),x_slice.visStr());
      pv = l_vol.placeVolume(s_vol);
      // Slices have no extra id. Take the ID of the layer!
      pv.addPhysVolID("slice",im);
    }
    l_tub.setDimensions(rmin,r,z);
    //cout << l_name << " " << rmin << " " << r << " " << z << endl;
    l_vol.setVisAttributes(description,x_layer.visStr());
    
    double z_offset = x_layer.hasAttr(_U(z_offset)) ? x_layer.z_offset() : 0.0;
    pv = assembly.placeVolume(l_vol,Position(0,0,z_offset));
    pv.addPhysVolID("layer",n);
    layer.setPlacement(pv);
  }
  if ( x_det.hasAttr(_U(combineHits)) ) {
    sdet.setCombineHits(x_det.combineHits(),sens);
  }

  pv = description.pickMotherVolume(sdet).placeVolume(assembly);
  pv.addPhysVolID("system",sdet.id()).addPhysVolID("barrel",0);
  sdet.setPlacement(pv);
  return sdet;
}

DECLARE_DETELEMENT(Lhe_MultiLayerTracker,create_detector)
