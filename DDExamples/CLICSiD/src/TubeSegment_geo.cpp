// $Id:$
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

static Ref_t create_element(LCDD& lcdd, const xml_h& e, SensitiveDetector&)  {
  xml_det_t  x_det  (e);
  xml_comp_t x_tube (x_det.child(_X(tubs)));
  xml_dim_t  x_pos  (x_det.child(_X(position)));
  xml_dim_t  x_rot  (x_det.child(_X(rotation)));
  string     name   = x_det.nameStr();
  Tube       tub    (lcdd,name,x_tube.rmin(),x_tube.rmax(),x_tube.zhalf());
  Volume     vol    (lcdd,name,tub,lcdd.material(x_det.materialStr()));
    
  vol.setVisAttributes(lcdd, x_det.visStr());
    
  DetElement   sdet(name,x_det.typeStr(),x_det.id());
  Volume       mother = lcdd.pickMotherVolume(sdet);
  PlacedVolume phv = 
    mother.placeVolume(vol,Position(x_pos.x(),x_pos.y(),x_pos.z()),
                       Rotation(x_rot.x(),x_rot.y(),x_rot.z()));
  phv.addPhysVolID(_A(id),x_det.id());
  sdet.addPlacement(phv);
  return sdet;
}

DECLARE_DETELEMENT(TubeSegment,create_element);
