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

// Include files
#include "JSON/Helper.h"
#include "DD4hep/Printout.h"
#include "DD4hep/LCDD.h"

#include <iostream>
#include <map>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

namespace  {

  struct MyDetExtension  {
    int idD, Ni, Nj;
    double posDX, posDY, posDZ;
    double dimDX, dimDY, dimDZ;
    double pixelX, pixelY, pixelZ;
    DetElement detector;

    MyDetExtension(DetElement e) : idD(0), Ni(0), Nj(0),
        posDX(0.0), posDY(0.0), posDZ(0.0),
        dimDX(0.0), dimDY(0.0), dimDZ(0.0),
        pixelX(0.0), pixelY(0.0), pixelZ(0.0), detector(e) {}
    MyDetExtension(const MyDetExtension& e, DetElement d) 
      : idD(e.idD), Ni(e.Ni), Nj(e.Nj),
        posDX(e.posDX), posDY(e.posDY), posDZ(e.posDZ),
        dimDX(e.dimDX), dimDY(e.dimDY), dimDZ(e.dimDZ),
        pixelX(e.pixelX), pixelY(e.pixelY), pixelZ(e.pixelZ),
        detector(d)						       
    {
    }
  };
}
typedef MyDetExtension DetectorExtension;

static Ref_t create_detector(LCDD &lcdd, json_h e, SensitiveDetector sens)  {
  json_det_t x_det = e;             	//json-detelemnt of the detector taken as an argument
  string det_name = x_det.nameStr();	//det_name is the name of the json-detelement
  Assembly assembly (det_name);
  int detectors_id = x_det.id();

  DetElement sdet(det_name,x_det.id());        //sdet is the detelement of the detector!!(actually is a Handle,already a pointer to m_element)
  DetectorExtension* ext = new MyDetExtension(sdet);
  sdet.addExtension<MyDetExtension>(ext);
  ext->idD= detectors_id;

  json_comp_t det_po = x_det.child(_U(position));

  double det_y = det_po.y();     // det_y is the y dimension of the json-detelement
  double det_x = det_po.x();     // det_x is the x dimension of  the json-detelement
  double det_z = det_po.z();     // det_z is the z dimension of the json-detelement
  ext->posDY = det_y;
  ext->posDX = det_x;
  ext->posDZ = det_z;

  json_comp_t det_dim = x_det.child(_U(dimensions));
  double dim_x = det_dim.x();    // det_x is the x dimension of  the json-detelement
  double dim_y = det_dim.y();    // det_y is the y dimension of the json-detelement
  double dim_z = det_dim.z();    // det_z is the z dimension of the json-detelement

  Material mat = lcdd.material("Silicon");

  Volume motherVol = lcdd.pickMotherVolume(sdet); //the mothers volume of our detector

  PlacedVolume pv;	//struct of Handle giving the volume id(ayto pou 8a kanw volume kai 8a to steilw me setplacement),dld o detector mou
  json_comp_t dtc_mod = x_det.child(_U(module));	    // considering the module-pixel of the detector
  double pixelX = dtc_mod.x();  // The x dimension of the module
  double pixelY = dtc_mod.y();  // The y dimension of the module
  double pixelZ = dtc_mod.z();  // The z dimension of the module

  int Ni = dim_x/pixelX;         // how many pixels in the x dimension
  int Nj = dim_y/pixelY;

  ext->dimDX = dim_x;
  ext->dimDY = dim_y;
  ext->dimDZ = dim_z;
  ext->pixelX = pixelX;
  ext->pixelY = pixelY;
  ext->pixelZ = pixelZ;
  ext->Ni= Ni;
  ext->Nj= Nj;


  Volume m_volume(det_name, Box(dim_x, dim_y, dim_z), mat);	//as parameters it needs name,solid,material
  m_volume.setVisAttributes(lcdd.visAttributes(x_det.visStr()));	//I DONT MIND ABOUT THIS!
  pv = motherVol.placeVolume(m_volume,Transform3D(Position(det_x,det_y,det_z)));  //det_x,det_y,det_z are the dimensions of the detector in space

  json_comp_t dtctr = x_det;
  if ( dtctr.isSensitive() ) {
    sens.setType("tracker");
    pv.addPhysVolID("system",detectors_id);
    m_volume.setSensitiveDetector(sens);
  }
  sdet.setPlacement(pv);
  // Support additional test if LCDD_InhibitConstants is set to TRUE
  lcdd.constant<double>("world_side");
  return sdet;
}

DECLARE_JSON_DETELEMENT(MiniTelPixel_json,create_detector)
