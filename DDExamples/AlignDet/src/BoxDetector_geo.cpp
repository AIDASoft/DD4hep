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
  xml_det_t   x_det = e;
  string      name  = x_det.nameStr();
  xml_comp_t  x_box  (x_det.child(_X(box)));
  xml_dim_t   x_pos  (x_det.child(_X(position)));
  xml_dim_t   x_rot  (x_det.child(_X(rotation)));
  Material    mat    (lcdd.material(x_det.materialStr()));
  DetElement  det    (lcdd,name,x_det.typeStr(),x_det.id());
  Box         det_box(lcdd,name,x_box.x(),x_box.y(),x_box.z());
  Volume      det_vol(lcdd,name+"_vol",det_box, mat);
  Volume      mother = lcdd.pickMotherVolume(det);

  det_vol.setVisAttributes(lcdd, x_det.visStr());
  PlacedVolume phv = mother.placeVolume(det_vol,Position(x_pos.x(),x_pos.y(),x_pos.z()),
					Rotation(x_rot.x(),x_rot.y(),x_rot.z()));
  phv.addPhysVolID(_A(id),x_det.id());
  det.setPlacement(phv);
  return det;
}

// first argument is the type from the xml file
DECLARE_DETELEMENT(BoxSegment,create_element)
