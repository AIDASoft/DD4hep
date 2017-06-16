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

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

static Ref_t create_element(Detector& description, xml_h e, Ref_t)  {
  xml_det_t   x_det = e;
  string      name  = x_det.nameStr();
  xml_comp_t  box    (x_det.child(_U(box)));
  xml_dim_t   pos    (x_det.child(_U(position)));
  xml_dim_t   rot    (x_det.child(_U(rotation)));
  Material    mat    (description.material(x_det.materialStr()));
  DetElement  det    (name,x_det.id());
  Volume      det_vol(name+"_vol",Box(box.x(),box.y(),box.z()), mat);
  Volume      mother = description.pickMotherVolume(det);
  Transform3D transform(Rotation3D(RotationZYX(rot.z(),rot.y(),rot.x())),Position(pos.x(),pos.y(),pos.z()));
  det_vol.setVisAttributes(description, x_det.visStr());
  PlacedVolume phv = mother.placeVolume(det_vol,transform);
  phv.addPhysVolID("id",x_det.id());
  det.setPlacement(phv);
  return det;
}

// first argument is the type from the xml file
DECLARE_DETELEMENT(AlignmentBoxSegment,create_element)
