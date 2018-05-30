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

static Ref_t create_element(Detector& description, xml_h e, Ref_t sens)  {
  xml_det_t  x_det  (e);
  xml_comp_t x_cone = x_det.cone();
  xml_dim_t  pos    = x_det.position();
  xml_dim_t  rot    = x_det.rotation();
  string     name   = x_det.nameStr();
  Cone       cone   (x_cone.dz(),x_cone.rmin1(),x_cone.rmax1(),x_cone.rmin2(),x_cone.rmax2());
  Volume     vol    (name,cone,description.material(x_det.materialStr()));

  vol.setVisAttributes(description, x_det.visStr());
  vol.setLimitSet(description, x_det.limitsStr());
  vol.setRegion(description, x_det.regionStr());
  if ( x_det.isSensitive() )   {
    SensitiveDetector sd = sens;
    xml_dim_t sd_typ = x_det.child(_U(sensitive));
    vol.setSensitiveDetector(sens);
    sd.setType(sd_typ.typeStr());
  }

  DetElement   sdet(name,x_det.id());
  Volume       mother = description.pickMotherVolume(sdet);
  PlacedVolume phv = 
    mother.placeVolume(vol,Transform3D(RotationZYX(rot.z(),rot.y(),rot.x()),Position(-pos.x(),-pos.y(),pos.z())));
  if ( x_det.hasAttr(_U(id)) )  {
    phv.addPhysVolID("system",x_det.id());
  }
  sdet.setPlacement(phv);
  return sdet;
}

DECLARE_DETELEMENT(DD4hep_ConeSegment,create_element)

