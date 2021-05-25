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
  TGeoVolumeMulti* mvol;
  VolumeMulti  mv;

  mv = box.divide(vol.ptr(), "Div1", 1, 3, -100, 10);
  printout(ALWAYS, "VolumeMultiTester", "+++ VolumeMulti %p type: %s [%s] solid: %p",
	   mv.ptr(), mv.name(), mv->IsA()->GetName(), mv.solid().ptr());
  // printout(ALWAYS, "VolumeMultiTester", "+++ VolumeMulti %s", mv.solid().name());
  // printout(ALWAYS, "VolumeMultiTester", "+++ VolumeMulti %s", mv.solid().type());
  mvol = (TGeoVolumeMulti*)mv.ptr();
  for( int i=0, n=mvol->GetNvolumes(); i<n; ++i )   {
    Volume vv = mvol->GetVolume(i);
    printout(ALWAYS, "VolumeMultiTester",
	     "+++ Volume[%03d] %p name: %s [%s] solid: %s tag: '%s'",
	     i, vv.ptr(), vv.name(), vv->IsA()->GetName(),
	     vv.solid().type(), vv.solid()->GetTitle());
  }

  mv = box.divide(vol.ptr(), "Div2", 1, 3,    0, 50);
  printout(ALWAYS, "VolumeMultiTester", "+++ VolumeMulti %p type: %s [%s] solid: %p",
	   mv.ptr(), mv.name(), mv->IsA()->GetName(), mv.solid().ptr());
  // printout(ALWAYS, "VolumeMultiTester", "+++ VolumeMulti %s", mv.solid().name());
  // printout(ALWAYS, "VolumeMultiTester", "+++ VolumeMulti %s", mv.solid().type());
  mvol = (TGeoVolumeMulti*)mv.ptr();
  for( int i=0, n=mvol->GetNvolumes(); i<n; ++i )   {
    Volume vv = mvol->GetVolume(i);
    printout(ALWAYS, "VolumeMultiTester",
	     "+++ Volume[%03d] %p name: %s [%s] solid: %s tag: '%s'",
	     i, vv.ptr(), vv.name(), vv->IsA()->GetName(),
	     vv.solid().type(), vv.solid()->GetTitle());
  }
  PlacedVolume pv = description.pickMotherVolume(det).placeVolume(vol);
  pv.addPhysVolID("system",x_det.id());
  det.setPlacement(pv);
  return det;
}

DECLARE_DETELEMENT(VolumeDivisionTest,create_element)
