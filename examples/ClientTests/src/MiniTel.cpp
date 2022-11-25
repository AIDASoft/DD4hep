//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank/M.Clemencic
//
//==========================================================================

// Include files
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Detector.h"

#include <iostream>
#include <map>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

namespace  {
  struct MyDetExtension  {
    int idD, Ni, Nj;
    double dimDX, dimDY, dimDZ;
    double pixelX, pixelY, pixelZ;
    DetElement detector;

    MyDetExtension(DetElement e) : idD(0), Ni(0), Nj(0),
        dimDX(0.0), dimDY(0.0), dimDZ(0.0),
        pixelX(0.0), pixelY(0.0), pixelZ(0.0), detector(e) {}
    MyDetExtension(const MyDetExtension& e, DetElement d) 
      : idD(e.idD), Ni(e.Ni), Nj(e.Nj),
        dimDX(e.dimDX), dimDY(e.dimDY), dimDZ(e.dimDZ),
        pixelX(e.pixelX), pixelY(e.pixelY), pixelZ(e.pixelZ),
        detector(d)						       
    {
    }
  };
}
typedef MyDetExtension DetectorExtension;

static Ref_t create_detector(Detector &description, xml_h e, SensitiveDetector sens)  {
  xml_det_t  x_det    = e;	//xml-detelemnt of the detector taken as an argument
  xml_comp_t det_dim  = x_det.child(_U(dimensions));
  xml_comp_t det_mod  = x_det.child(_U(module));	    // considering the module-pixel of the detector
  string     det_name = x_det.nameStr();	//det_name is the name of the xml-detelement
  Assembly   assembly (det_name);
  DetElement sdet(det_name,x_det.id());        //sdet is the detelement of the detector!!(actually is a Handle,already a pointer to m_element)
  Volume     motherVol = description.pickMotherVolume(sdet); //the mothers volume of our detector
  Material   mat = description.material("Silicon");

  DetectorExtension* ext = new MyDetExtension(sdet);
  sdet.addExtension<MyDetExtension>(ext);
  ext->idD    = x_det.id();
  ext->dimDX  = det_dim.x();    // det_x is the x dimension of  the xml-detelement
  ext->dimDY  = det_dim.y();    // det_y is the y dimension of the xml-detelement
  ext->dimDZ  = det_dim.z();    // det_z is the z dimension of the xml-detelement
  ext->pixelX = det_mod.x();  // The x dimension of the module
  ext->pixelY = det_mod.y();  // The y dimension of the module
  ext->pixelZ = det_mod.z();  // The z dimension of the module
  ext->Ni     = int(det_dim.x()/det_mod.x());
  ext->Nj     = int(det_dim.y()/det_mod.y());

  assembly.setVisAttributes(description.visAttributes(x_det.visStr()));

  Volume sensor_vol(det_name, Box(det_dim.x(),det_dim.y(),det_dim.z()), mat);
  sensor_vol.setVisAttributes(description.visAttributes(det_mod.visStr()));
  sensor_vol.setLimitSet(description, x_det.limitsStr());
  sensor_vol.setRegion(description, x_det.regionStr());
  sensor_vol.setSensitiveDetector(sens);
  if ( x_det.isSensitive() ) {
    sens.setType("tracker");
  }

  int count = 0;
  PlacedVolume pv;
  DetElement   side_det;
  Assembly     side_vol;
  Position     side_pos(0,0,30*dd4hep::mm);

  side_det = DetElement(sdet,"side_0", x_det.id());
  side_vol = Assembly("side_0");
  if ( x_det.hasChild(_U(side_position)) )  {
    xml_comp_t x_pos = x_det.child(_U(side_position));
    side_pos = Position(x_pos.x(0), x_pos.y(0), x_pos.z(3*dd4hep::cm));
  }
  pv = assembly.placeVolume(side_vol, side_pos);
  pv.addPhysVolID("side",0);
  side_det.setPlacement(pv);
  for( xml_coll_t m(x_det, _U(module_position)); m; m++ )    {
    xml_comp_t x_pos = m;
    DetElement module(side_det, _toString(count, "module_%d"), count);
    pv = side_vol.placeVolume(sensor_vol,Transform3D(Position(x_pos.x(),x_pos.y(),x_pos.z())));
    pv.addPhysVolID("module", ++count);
    module.setPlacement(pv);
  }

  if ( x_det.hasChild(_U(reflect)) )   {
    xml_comp_t x_pos = x_det.child(_U(reflect));
    side_pos = Position(x_pos.x(-side_pos.x()), x_pos.y(-side_pos.y()), x_pos.z(-side_pos.z()));
    auto [det, vol] = side_det.reflect("side_1", x_det.id(), sens);
    pv = assembly.placeVolume(vol, Transform3D(RotationZ(M_PI),side_pos));
    pv.addPhysVolID("side",1);
    det.setPlacement(pv);
    sdet.add(det);
  }

  pv = motherVol.placeVolume(assembly);
  pv.addPhysVolID("system", x_det.id());
  sdet.setPlacement(pv);
  return sdet;
}

DECLARE_DETELEMENT(MiniTelPixel,create_detector)
