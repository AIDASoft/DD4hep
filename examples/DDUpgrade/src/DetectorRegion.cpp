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
#include "XML/Utilities.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

static Ref_t create_element(Detector& description, xml_h e, Ref_t /* sens_det */)  {
  xml_det_t   x_det = e;
  int         id    = x_det.hasAttr(_U(id)) ? x_det.id() : 0;
  string      name  = x_det.nameStr();
  xml_comp_t  x_vol  (x_det.child(_U(volume)));
  xml_dim_t   x_pos  (x_det.child(_U(position),false));
  xml_dim_t   x_rot  (x_det.child(_U(rotation),false));
  Material    mat    (description.material(x_vol.materialStr()));
  DetElement  det    (name,id);
  Volume      mother = description.pickMotherVolume(det);
  Solid       solid = xml::createShape(description, x_vol.typeStr(), x_vol);
  Volume      volume(name+"_vol",solid, mat);
  PlacedVolume pv;

  if ( x_pos && x_rot )   {
    Transform3D transform(Rotation3D(RotationZYX(x_rot.z(),x_rot.y(),x_rot.x())),
                          Position(x_pos.x(),x_pos.y(),x_pos.z()));
    pv = mother.placeVolume(volume,transform);
  }
  else if ( x_rot )  {
    Transform3D transform(Rotation3D(RotationZYX(x_rot.z(),x_rot.y(),x_rot.x())),
                          Position(0,0,0));
    pv = mother.placeVolume(volume,transform);
  }
  else if ( x_pos )   {
    pv = mother.placeVolume(volume,Position(x_pos.x(),x_pos.y(),x_pos.z()));
  }
  else  {
    pv = mother.placeVolume(volume);
  }
  volume.setVisAttributes(description, x_det.visStr());
  volume.setLimitSet(description, x_det.limitsStr());
  volume.setRegion(description, x_det.regionStr());
  if ( id != 0 )  {
    pv.addPhysVolID("system", id);
  }
  det.setPlacement(pv);
  return det;
}

// first argument is the type from the xml file
DECLARE_DETELEMENT(DD4hep_DetectorRegion,create_element)
