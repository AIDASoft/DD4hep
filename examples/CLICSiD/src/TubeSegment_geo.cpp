// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#include "DD4hep/DetFactoryHelper.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static Ref_t create_element(LCDD& lcdd, xml_h e, Ref_t)  {
  xml_det_t  x_det  (e);
  xml_comp_t x_tube = x_det.tubs();
  xml_dim_t  pos    = x_det.position();
  xml_dim_t  rot    = x_det.rotation();
  string     name   = x_det.nameStr();
  Tube       tub    (x_tube.rmin(),x_tube.rmax(),x_tube.zhalf());
  Volume     vol    (name,tub,lcdd.material(x_det.materialStr()));

  vol.setVisAttributes(lcdd, x_det.visStr());
    
  DetElement   sdet(name,x_det.id());
  Volume       mother = lcdd.pickMotherVolume(sdet);
  PlacedVolume phv = 
    mother.placeVolume(vol,Transform3D(RotationZYX(rot.z(),rot.y(),rot.x()),Position(-pos.x(),-pos.y(),pos.z())));
  phv.addPhysVolID("id",x_det.id());
  sdet.setPlacement(phv);
  return sdet;
}

DECLARE_DETELEMENT(TubeSegment,create_element);
