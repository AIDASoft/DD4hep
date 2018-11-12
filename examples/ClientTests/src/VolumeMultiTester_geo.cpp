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

// Framework include files
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/Printout.h"

// ROOT include file
#include "TClass.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

static Ref_t create_element(Detector& description, xml_h xml_det, SensitiveDetector /* sens */)  {
  xml_det_t    x_det = xml_det;
  string       det_name = x_det.nameStr();
  Box          box(100, 100, 100);
  Volume       vol(det_name+"_vol", box, description.air());
  DetElement   det(det_name,x_det.typeStr(), x_det.id());
  VolumeMulti  mv;

  mv = box.divide(vol.ptr(), "Div", 1, 3, -100, 10);
  printout(ALWAYS,"VolumeMultiTester","+++ VolumeMulti %p",mv.ptr());
  printout(ALWAYS,"VolumeMultiTester","+++ VolumeMulti %s",mv.name());
  printout(ALWAYS,"VolumeMultiTester","+++ VolumeMulti %s",mv->IsA()->GetName());

  mv = box.divide(vol.ptr(), "Div2", 1, 3, 0, 50);
  printout(ALWAYS,"VolumeMultiTester","+++ VolumeMulti %p",mv.ptr());
  printout(ALWAYS,"VolumeMultiTester","+++ VolumeMulti %s",mv.name());
  printout(ALWAYS,"VolumeMultiTester","+++ VolumeMulti %s",mv->IsA()->GetName());

  PlacedVolume pv = description.pickMotherVolume(det).placeVolume(vol);
  pv.addPhysVolID("system",x_det.id());
  det.setPlacement(pv);
  return det;
}

DECLARE_DETELEMENT(VolumeMultiTester,create_element)
