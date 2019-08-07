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

// Include files
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/OpticalSurfaces.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Detector.h"

#include <iostream>
#include <map>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

/// Example of a water box with a box of air inside, the "bubble"
static Ref_t create_detector(Detector &description, xml_h e, SensitiveDetector /* sens */)  {
  xml_det_t x_det    = e;
  string det_name    = x_det.nameStr();
  DetElement sdet(det_name,x_det.id());
  xml_det_t x_tank   = x_det.child(_Unicode(tank));
  xml_det_t x_bubble = x_det.child(_Unicode(bubble));

  Assembly hall_vol("Hall");
  Box    encl_box(_toDouble("world_x-2*cm"),_toDouble("world_y-2*cm"),_toDouble("world_z-2*cm"));
  Volume encl_vol("Enclosure",encl_box,description.air());
  Box    tank_box(x_tank.x(), x_tank.y(), x_tank.z());
  Volume tank_vol("Tank",tank_box,description.material(x_tank.attr<string>(_U(material))));
  Box    bubble_box(x_bubble.x(), x_bubble.y(), x_bubble.z());
  Volume bubble_vol("Bubble",bubble_box,description.material(x_bubble.attr<string>(_U(material))));

  encl_vol.setVisAttributes(description.invisible());
  tank_vol.setVisAttributes(description, x_tank.visStr());
  bubble_vol.setVisAttributes(description, x_bubble.visStr());
  
  PlacedVolume bubblePlace = tank_vol.placeVolume(bubble_vol);
  bubblePlace.addPhysVolID("bubble",1);
  PlacedVolume tankPlace   = encl_vol.placeVolume(tank_vol);
  tankPlace.addPhysVolID("tank",1);
  PlacedVolume enclPlace   = hall_vol.placeVolume(encl_vol);
  PlacedVolume hallPlace   = description.pickMotherVolume(sdet).placeVolume(hall_vol);
  hallPlace.addPhysVolID("system",x_det.id());
  sdet.setPlacement(hallPlace);

#if ROOT_VERSION_CODE >= ROOT_VERSION(6,17,0)
  // Now attach the surface
  OpticalSurfaceManager surfMgr = description.surfaceManager();
  OpticalSurface waterSurf  = surfMgr.opticalSurface("/world/"+det_name+"#WaterSurface");
  OpticalSurface airSurf    = surfMgr.opticalSurface("/world/"+det_name+"#AirSurface");
  BorderSurface  tankSurf   = BorderSurface(description, sdet, "HallTank",   waterSurf, tankPlace,   enclPlace);
  BorderSurface  bubbleSurf = BorderSurface(description, sdet, "TankBubble", airSurf,   bubblePlace, tankPlace);
  bubbleSurf.isValid();
  tankSurf.isValid();
#else
  bubblePlace.isValid();
#endif
  return sdet;
}
DECLARE_DETELEMENT(DD4hep_OpNovice,create_detector)
