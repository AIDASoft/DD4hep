// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DD4hep/DetFactoryHelper.h"
#include "ILDExVXD.h"
using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

namespace DD4hep { namespace Geometry {
  
  template <> Ref_t DetElementFactory<ILDExVXD>::create(LCDD& lcdd, const xml_h& e, SensitiveDetector&)  {
    xml_det_t   x_det = e;
    string      name  = x_det.nameStr();
    ILDExVXD    vxd    (name,x_det.typeStr(),x_det.id());
    Volume      mother= lcdd.pickMotherVolume(vxd);
    
    for(xml_coll_t c(e,_X(layer)); c; ++c)  {
      xml_comp_t  x_layer  (c);
      xml_comp_t  x_support (x_layer.child(_X(support)));
      xml_comp_t  x_ladder  (x_layer.child(_X(ladder)));
      int         layer_id   = x_layer.id();
      int         nLadders   = x_ladder.number();
      string      layername  = name+_toString(layer_id,"_layer%d");
      double      dphi       = 2.*M_PI/double(nLadders);
      double      zhalf      = x_ladder.zhalf();
      double      offset     = x_ladder.offset();
      double      sens_radius= x_ladder.radius();
      double      sens_thick = x_ladder.thickness();
      double      supp_thick = x_support.thickness();
      double      width      = 2.*tan(dphi/2.)*(sens_radius-sens_thick/2.);
      Material    sens_mat   = lcdd.material(x_ladder.materialStr());
      Material    supp_mat   = lcdd.material(x_support.materialStr());
      Box         ladderbox ((sens_thick+supp_thick)/2.,width/2.,zhalf);
      Volume      laddervol (layername+"_ladder_volume",  ladderbox,sens_mat);
      Box         sensbox   (sens_thick/2.,width/2.,zhalf);
      Volume      sensvol   (layername+"_sens_volume",    sensbox,sens_mat);
      Box         suppbox   (supp_thick/2.,width/2.,zhalf);
      Volume      suppvol   (layername+"_supp_volume",    suppbox,supp_mat);
      Position    senspos   (-(sens_thick+supp_thick)/2.+sens_thick/2.,0,0);
      Position    supppos   (-(sens_thick+supp_thick)/2.+sens_thick/2.+supp_thick/2.,0,0);
      
      sensvol.setVisAttributes(lcdd.visAttributes(x_layer.visStr()));
      laddervol.placeVolume(sensvol,senspos);
      laddervol.placeVolume(suppvol,supppos);
      
      for(int j=0; j<nLadders; ++j) {
        string laddername = layername + _toString(j,"_ladder%d");
        double radius = sens_radius + ((sens_thick+supp_thick)/2. - sens_thick/2.);
        Position pos(radius*cos(j*dphi) - offset*sin(j*dphi),
                     radius*sin(j*dphi) - offset*cos(j*dphi),0.);
        mother.placeVolume(laddervol,pos,Rotation(0,0,j*dphi));
      }
      vxd.setVisAttributes(lcdd, x_det.visStr(),laddervol);
    }
    return vxd;
  }
}}

DECLARE_NAMED_DETELEMENT_FACTORY(DD4hep,ILDExVXD);
