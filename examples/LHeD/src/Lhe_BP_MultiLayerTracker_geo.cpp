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
//                             ( circ-ellipt. in x,y)
//==========================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/Printout.h"
#include "XML/Layering.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

static Ref_t create_detector(Detector& description, xml_h e, SensitiveDetector sens)  {
  xml_det_t  x_det     = e;
  string     det_name  = x_det.nameStr();
  string     det_type  = x_det.typeStr();
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
    double  rmax = x_layer.outer_r();
    double  r    = rmin;
    double  rmax_ell  = x_layer.rmax();

    double ra    = 1.;             // ellipse long radius init
           ra    = rmax_ell;       // ellipse long radius
    double rb    = 1.;             // ellipse short radius init
           rb    = rmin;           // ellipse short radius
    double thick = rmax - rmin;    // layer thickness
    
    EllipticalTube bpElTubeOut(ra+thick, rb+thick, z);
    EllipticalTube bpElTubeInn(ra, rb, z+0.1);
    SubtractionSolid bpElTube(bpElTubeOut,bpElTubeInn);

    Tube bpTube1(rb, rb+thick, z+0.1, 3*M_PI/2, M_PI/2);
    UnionSolid beamTube1(bpElTube,bpTube1);

    Tube bpTube2(rb+thick, ra+thick, z+0.1, 3*M_PI/2, M_PI/2);
    SubtractionSolid l_tub(beamTube1,bpTube2);

    DetElement layer(sdet,_toString(n,"layer%d"),x_layer.id());  
    Volume   l_vol(l_name, l_tub, air);
    
    cout << l_name << " " << rmin << " " << r << " " << z << endl;
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

DECLARE_DETELEMENT(Lhe_BP_MultiLayerTracker,create_detector)
