// $Id$
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
//
// Specialized generic detector constructor
// 
//==========================================================================
#include "DD4hep/DetFactoryHelper.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static Ref_t create_element(LCDD& lcdd, xml_h e, Ref_t sens)  {
  xml_det_t   x_det = e;
  string      name  = x_det.nameStr();
  xml_comp_t  box    (x_det.child(_U(box)));
  xml_dim_t   pos    (x_det.child(_U(position)));
  xml_dim_t   rot    (x_det.child(_U(rotation)));
  Material    mat    (lcdd.material(x_det.materialStr()));
  DetElement  det    (name,x_det.id());
  Volume      det_vol(name+"_vol",Box(box.x(),box.y(),box.z()), mat);
  Volume      mother = lcdd.pickMotherVolume(det);
  Transform3D transform(Rotation3D(RotationZYX(rot.z(),rot.y(),rot.x())),Position(pos.x(),pos.y(),pos.z()));
  PlacedVolume phv = mother.placeVolume(det_vol,transform);

  det_vol.setVisAttributes(lcdd, x_det.visStr());
  det_vol.setLimitSet(lcdd, x_det.limitsStr());
  det_vol.setRegion(lcdd, x_det.regionStr());
  if ( x_det.isSensitive() )   {
    SensitiveDetector sd = sens;
    xml_dim_t sd_typ = x_det.child(_U(sensitive));
    det_vol.setSensitiveDetector(sens);
    sd.setType(sd_typ.typeStr());
  }
  if ( x_det.hasAttr(_U(id)) )  {
    phv.addPhysVolID("system",x_det.id());
  }
  det.setPlacement(phv);
  return det;
}

// first argument is the type from the xml file
DECLARE_DETELEMENT(DD4hep_BoxSegment,create_element)
DECLARE_DEPRECATED_DETELEMENT(BoxSegment,create_element)
