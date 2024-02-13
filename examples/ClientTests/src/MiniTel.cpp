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

// C/C++ include files
#include <iostream>
#include <map>

using namespace dd4hep;

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
  xml_det_t   x_det    = e;	//xml-detelemnt of the detector taken as an argument
  xml_comp_t  det_dim  = x_det.child(_U(dimensions));
  xml_comp_t  det_mod  = x_det.child(_U(module));	    // considering the module-pixel of the detector
  std::string det_name = x_det.nameStr();	//det_name is the name of the xml-detelement
  Assembly    assembly (det_name);
  DetElement  sdet(det_name,x_det.id());        //sdet is the detelement of the detector!!(actually is a Handle,already a pointer to m_element)
  Volume      motherVol = description.pickMotherVolume(sdet); //the mothers volume of our detector
  Material    mat = description.material("Silicon");

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

  Box    sensor_box(Box(det_dim.x(),det_dim.y(),det_dim.z()));
  Volume sensor_vol(det_name, sensor_box, mat);
  sensor_vol.setVisAttributes(description.visAttributes(det_mod.visStr()));
  sensor_vol.setLimitSet(description, x_det.limitsStr());
  sensor_vol.setRegion(description, x_det.regionStr());
  sensor_vol.setSensitiveDetector(sens);
  if ( x_det.isSensitive() ) {
    sens.setType("tracker");
  }
  PlacedVolume pv;
  DetElement   side_det;
  double       epsilon = 1e-10;
  Position     side_pos(0,0,30*dd4hep::mm);
  Position     env_dim_min(sensor_box.x()+epsilon, sensor_box.y()+epsilon, +100000.0);
  Position     env_dim_max(sensor_box.x()+epsilon, sensor_box.y()+epsilon, -100000.0);

  for( xml_coll_t mod(x_det, _U(module_position)); mod; mod++ )    {
    xml_comp_t x_pos = mod;
    if ( x_pos.z() > env_dim_max.z() ) {
      env_dim_max.SetZ(x_pos.z());
      printout(DEBUG,"MiniTel","Envelope z_max = %f",x_pos.z());
    }
    if ( x_pos.z() < env_dim_min.z() )  {
      env_dim_min.SetZ(x_pos.z());
      printout(DEBUG,"MiniTel","Envelope z_min = %f",x_pos.z());
    }
  }

  Volume side_vol;
  if ( x_det.hasChild(_U(assembly)) )   {
    side_vol = Assembly("side_0");
    printout(DEBUG,"MiniTel","Using assembly envelope");
  }
  else   {
    Box side_box(env_dim_max.x(), env_dim_max.y(), (env_dim_max.z()-env_dim_min.z())/2.0+sensor_box.z() + epsilon);
    side_vol = Volume("side_0", side_box, description.air());
    printout(DEBUG,"MiniTel","Envelope Box = %f",side_box.z());
  }
  side_det = DetElement(sdet,"side_0", x_det.id());
  if ( x_det.hasChild(_U(side_position)) )  {
    xml_comp_t x_pos = x_det.child(_U(side_position));
    side_pos = Position(x_pos.x(0), x_pos.y(0), x_pos.z(3*dd4hep::cm));
    printout(ALWAYS,"","side_pos = %f",side_pos.z());
  }

  Unicode tag("missing_module_placements");
  xml_dim_t miss = x_det.child(tag, false);
  int missing_placement = miss ? miss.number() : 0;
  pv = assembly.placeVolume(side_vol, side_pos);
  pv.addPhysVolID("side",0);
  side_det.setPlacement(pv);
  int count = 0;
  for( xml_coll_t mpos(x_det, _U(module_position)); mpos; mpos++ )    {
    xml_comp_t x_pos = mpos;
    DetElement module(side_det, _toString(count, "module_%d"), count);
    pv = side_vol.placeVolume(sensor_vol,Transform3D(Position(x_pos.x(),x_pos.y(),x_pos.z())));
    pv.addPhysVolID("module", ++count);
    if ( missing_placement > 0 && count%missing_placement == 0 )   {
      printout(WARNING,"","Drop placement of DetElement: %s", module.path().c_str());
      continue;
    }
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
