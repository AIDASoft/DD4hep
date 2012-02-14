// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DD4hep/DetFactoryHelper.h"
#include "ILDExTPC.h"
using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

namespace DD4hep { namespace Geometry {
  
  template <> Ref_t DetElementFactory<ILDExTPC>::create(LCDD& lcdd, const xml_h& e, SensitiveDetector&)  {
    xml_det_t   x_det = e;
    xml_comp_t  x_tube (x_det.child(_X(tubs)));
    string      name  = x_det.nameStr();
    Material    mat    (lcdd.material(x_det.materialStr()));
    ILDExTPC    tpc    (lcdd,name,x_det.typeStr(),x_det.id());
    Tube        tpc_tub(lcdd,name+"_envelope",x_tube.rmin(),x_tube.rmax(),x_tube.zhalf());
    Volume      tpc_vol(lcdd,name+"_envelope_volume", tpc_tub, mat);

    for(xml_coll_t c(e,_X(detector)); c; ++c)  {
      xml_comp_t  px_det  (c);
      xml_comp_t  px_tube (px_det.child(_X(tubs)));
      xml_dim_t   px_pos  (px_det.child(_X(position)));
      xml_dim_t   px_rot  (px_det.child(_X(rotation)));
      xml_comp_t  px_mat  (px_det.child(_X(material)));
      string      part_nam(px_det.nameStr());
      Material    part_mat(lcdd.material(px_mat.nameStr()));
      DetElement  part_det(lcdd,part_nam,px_det.typeStr(),px_det.id());
      Tube        part_tub(lcdd,part_nam+"_tube",px_tube.rmin(),px_tube.rmax(),px_tube.zhalf());
      Volume      part_vol(lcdd,part_nam,part_tub,part_mat);
      Position    part_pos(px_pos.x(),px_pos.y(),px_pos.z());
      Rotation    part_rot(px_rot.x(),px_rot.y(),px_rot.z());

      part_vol.setVisAttributes(lcdd,px_det.visStr());

      PlacedVolume part_phv = tpc_vol.placeVolume(part_vol,part_pos,part_rot);
      part_phv.addPhysVolID(_A(id),px_det.id());
      part_det.addPlacement(part_phv);
      switch(part_det.id()) {
      case 0:	tpc.setInnerWall(part_det);  break;
      case 1:	tpc.setOuterWall(part_det);  break;
      case 5:	tpc.setGasVolume(part_det);  break;
      }
      tpc.add(part_det);
    }
    tpc_vol.setVisAttributes(lcdd, x_det.visStr());
    PlacedVolume phv = lcdd.pickMotherVolume(tpc).placeVolume(tpc_vol);
    tpc.addPlacement(phv);
    return tpc;
  }
}}

DECLARE_NAMED_DETELEMENT_FACTORY(DD4hep,ILDExTPC);
