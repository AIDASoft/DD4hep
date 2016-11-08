//==========================================================================
//  AIDA Detector description implementation for LCD
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

// Framework includes
#include "DD4hep/DetFactoryHelper.h"

using namespace DD4hep;
using namespace DD4hep::Geometry;

static Ref_t create_detector(LCDD& lcdd, xml_h e, SensitiveDetector sens)  {
  // XML detector object: DDCore/XML/XMLDetector.h
  xml_dim_t x_det = e;  
  //Create the DetElement for DD4hep
  DetElement d_det(x_det.nameStr(),x_det.id());

  // XML dimension object: DDCore/XML/XMLDimension.h
  xml_dim_t x_det_dim(x_det.dimensions());
  double inner_r = x_det_dim.rmin();
  double outer_r = x_det_dim.rmax();
  Assembly calo_vol(x_det.nameStr()+"_envelope");
  PlacedVolume pv;

  // Set envelope volume attributes
  calo_vol.setAttributes(lcdd,x_det.regionStr(),x_det.limitsStr(),x_det.visStr());

  // Declare this sensitive detector as a calorimeter
  Tube tub(inner_r,outer_r,x_det_dim.z()/2.0,0.0,2*M_PI);
  Volume tub_vol(x_det.nameStr()+"_tube",tub,lcdd.material("Iron"));
  calo_vol.placeVolume(tub_vol).addPhysVolID("module",1);
  sens.setType("calorimeter");
  tub_vol.setSensitiveDetector(sens);
  d_det.setAttributes(lcdd,tub_vol,x_det.regionStr(),x_det.limitsStr(),x_det.visStr());

  // Place the calo inside the world
  PlacedVolume  calo_plv = lcdd.pickMotherVolume(d_det).placeVolume(calo_vol);
  calo_plv.addPhysVolID("system",x_det.id());
  calo_plv.addPhysVolID("barrel",0);
  d_det.setPlacement(calo_plv);
  return d_det;
}

DECLARE_DETELEMENT(IronCylinder,create_detector)
