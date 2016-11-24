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

// Framework include files
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/DD4hepUnits.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static Ref_t create_element(LCDD& lcdd, xml_h e, SensitiveDetector sd)  {
  Box         box;
  Volume      vol;
  PlacedVolume phv;
  xml_det_t   x_det  = e;
  double      small  = 1e-3*dd4hep::mm;
  string      name   = x_det.nameStr();
  Material    air    = lcdd.material("Air");
  DetElement  det    (name,x_det.id());
  Assembly    envVol (name+"_envelope");

  for(xml_coll_t im(x_det,_U(module)); im; ++im)  {
    xml_dim_t mod    = im;
    xml_dim_t sens   = mod.child(_U(sensor));
    xml_dim_t chip   = mod.child(_Unicode(chip));
    xml_dim_t pcb    = mod.child(_Unicode(PCB));
    int    noPixX    = sens.attr<int>(_Unicode(NoOfPixX));
    int    noPixY    = sens.attr<int>(_Unicode(NoOfPixY));
    double pitch     = sens.attr<double>(_Unicode(pitch));
    double mod_thick = sens.thickness()+chip.thickness()+pcb.thickness();
    DetElement mod_det(det,_toString(mod.id(),"module_%d"),x_det.id());
    
    // Make envelope box for each module a bit bigger to ensure all children are within bounds...
    box = Box(pitch*noPixX/2e0+small, pitch*noPixY/2e0+small, mod_thick/2e0+small);
    Volume modvol(_toString(mod.id(),"module_%d"), box, air);
    modvol.setVisAttributes(lcdd.visAttributes(mod.visStr()));

    DetElement sens_det(mod_det,"sensor",x_det.id());
    box = Box(pitch*noPixX/2e0, pitch*noPixY/2e0, sens.thickness()/2e0);
    vol = Volume(_toString(mod.id(),"sensor_%d"), box, air);
    vol.setSensitiveDetector(sd);
    vol.setVisAttributes(lcdd.visAttributes(sens.visStr()));
    phv = modvol.placeVolume(vol, Position(0, 0, -mod_thick/2e0+sens.thickness()/2e0));
    phv.addPhysVolID("sensor",1);
    sens_det.setPlacement(phv);

    box = Box(pitch*noPixX/2e0, pitch*noPixY/2e0, chip.thickness()/2e0);
    vol = Volume(_toString(mod.id(),"chip_%d"), box, air);
    vol.setVisAttributes(lcdd.visAttributes(chip.visStr()));
    phv = modvol.placeVolume(vol, Position(0, 0, -mod_thick/2e0+sens.thickness()+chip.thickness()/2e0));
    phv.addPhysVolID("sensor",2);

    box = Box(pitch*noPixX/2e0, pitch*noPixY/2e0, pcb.thickness()/2e0);
    vol = Volume(_toString(mod.id(),"PCB_%d"), box, air);
    vol.setVisAttributes(lcdd.visAttributes(pcb.visStr()));
    phv = modvol.placeVolume(vol, Position(0, 0, -mod_thick/2e0+sens.thickness()+chip.thickness()+pcb.thickness()/2e0));
    phv.addPhysVolID("sensor",3);

    phv = envVol.placeVolume(modvol,Position(0e0, 0e0, mod.z()));
    phv.addPhysVolID("module",mod.id());
    mod_det.setPlacement(phv);
  }
  envVol.setVisAttributes(lcdd.visAttributes(x_det.visStr()));
  phv = lcdd.pickMotherVolume(det).placeVolume(envVol,Position(0,0,0));
  phv.addPhysVolID("system",x_det.id());
  det.setPlacement(phv);
  return det;
}

// first argument is the type from the xml file
DECLARE_DETELEMENT(DD4hep_Example_Telescope,create_element)
