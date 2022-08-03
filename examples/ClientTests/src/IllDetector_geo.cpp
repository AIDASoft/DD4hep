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
//==========================================================================
#include "DD4hep/DetFactoryHelper.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

static Ref_t create_element(Detector& description, xml_h e, SensitiveDetector sens)  {
  xml_det_t   x_det = e;
  string      name  = x_det.nameStr();
  Volume      vol   = Assembly(name+"_envelope");
  DetElement  det (name,x_det.id());

  xml_det_t    x_station = x_det.child(_U(station));
  xml_comp_t   x_station_box (x_station.child(_U(box)));
  xml_dim_t    x_station_pos (x_station.child(_U(position)));
  Box          dbox  (x_station_box.x(), x_station_box.y(), x_station_box.z());
  Volume       dvol  (name+"_vol_1", dbox, description.air());
  DetElement   det1 = DetElement(det, name+"_1", x_station.id());
  DetElement   det2 = DetElement(det, name+"_2", x_station.id());
  Position     dpos1 (x_station_pos.x(), x_station_pos.y(), x_station_pos.z());
  PlacedVolume phv  = vol.placeVolume(dvol, dpos1);

  Position     dpos2 (-x_station_pos.x(), -x_station_pos.y(), -x_station_pos.z());
  phv  = vol.placeVolume(dvol, dpos2);
  phv.addPhysVolID("station", 1);

  dvol.setAttributes(description, x_station.regionStr(), x_station.limitsStr(), x_station.visStr());
  det1.setPlacement(phv);
  det2.setPlacement(phv);

  int lnum = 0;
  Material si = description.material("Silicon");
  for(xml_coll_t i(x_station,_U(module)); i; ++i, ++lnum)  {
    xml_comp_t   x_module = i;
    xml_dim_t    x_module_pos(x_module.child(_U(position)));
    string       lnam = _toString(lnum,"layer%d");
    Position     lpos(x_module_pos.x(), x_module_pos.y(), x_module_pos.z());
    Volume       lvol(lnam, Box(x_module.x(), x_module.y(), x_module.z()), si);
    PlacedVolume lpl = dvol.placeVolume(lvol, lpos);
    DetElement   lde(lnam, lnum);

    lvol.setSensitiveDetector(sens);
    lvol.setAttributes(description, x_module.regionStr(), x_module.limitsStr(), x_module.visStr());

    lde.setPlacement(lpl);
    det1.add(lde);
    det2.add(lde);
    lpl.addPhysVolID("module", lnum);
  }

  vol.setAttributes(description, x_det.regionStr(), x_det.limitsStr(), x_det.visStr());
  phv = description.pickMotherVolume(det).placeVolume(vol);
  phv.addPhysVolID("system",x_det.id());
  det.setPlacement(phv);
  
  if ( x_det.isSensitive() )   {
    sens.setType("tracker");
  }
  return det;
}

DECLARE_DETELEMENT(DD4hep_IllDetector,create_element)

