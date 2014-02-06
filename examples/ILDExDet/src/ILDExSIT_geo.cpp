// $Id: ILDExSIT_geo.cpp 687 2013-08-09 12:56:00Z gaede $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DD4hep/DetFactoryHelper.h"

#include "DDGear.h"
#include "gearimpl/ZPlanarParametersImpl.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;
  
static Ref_t create_element(LCDD& lcdd, xml_h e, SensitiveDetector sens)  {
  xml_det_t    x_det = e;
  string       name  = x_det.nameStr();
  DetElement   sit(name,x_det.id());
  Assembly assembly( name + "assembly"  ) ;
  PlacedVolume pv;

  
  //--------------- create gear::ZPlanarParameters and add them as Extension
  gear::ZPlanarParametersImpl* gearZPlanar = new gear::ZPlanarParametersImpl( 0 ,  0.0,  0.0,  0.0,  0.0,  0.0 ) ;

  // ZPlanarParametersImpl( int type, double shellInnerRadius, double shellOuterRadius, double shellHalfLength, double shellGap, double shellRadLength ) ;


  sit.addExtension<GearHandle>( new GearHandle( gearZPlanar, "SITParameters" )  ) ;
  //--------------------------------------------------------------------

  for(xml_coll_t c(e,_U(layer)); c; ++c)  {


    xml_comp_t  x_layer   (c);
    xml_comp_t  x_support (x_layer.child(_U(support)));
    xml_comp_t  x_ladder  (x_layer.child(_U(ladder)));
    int         layer_id   = x_layer.id();
    int         nLadders   = x_ladder.number();
    string      layername  = name+_toString(layer_id,"_layer%d");
    double      dphi       = 2.*M_PI/double(nLadders);
    double      zhalf      = x_ladder.zhalf();
    double      sens_radius= x_ladder.radius();
    double      sens_thick = x_ladder.thickness();
    double      supp_thick = x_support.thickness();
    double      width      = 2.*tan(dphi/2.)*(sens_radius-sens_thick/2.);
    Box         ladderbox ((sens_thick+supp_thick)/2.,width/2.,zhalf);
    Volume      laddervol (layername+"_ladder",ladderbox,lcdd.material(x_ladder.materialStr()));
    Box         sensbox   (sens_thick/2.,width/2.,zhalf);
    Volume      sensvol   (layername+"_sens",sensbox,lcdd.material(x_ladder.materialStr()));
    Box         suppbox   (supp_thick/2.,width/2.,zhalf);
    Volume      suppvol   (layername+"_supp",suppbox,lcdd.material(x_support.materialStr()));
    Position    senspos   (-(sens_thick+supp_thick)/2.+sens_thick/2.,0,0);
    Position    supppos   (-(sens_thick+supp_thick)/2.+sens_thick+supp_thick/2.,0,0);
      
    sens.setType("tracker");
    sensvol.setSensitiveDetector(sens);
    sensvol.setAttributes(lcdd,x_det.regionStr(),x_det.limitsStr(),x_layer.visStr());

    pv = laddervol.placeVolume(sensvol,senspos) ;
    laddervol.placeVolume(suppvol,supppos);
    sit.setVisAttributes(lcdd, x_det.visStr(),laddervol);

    for(int j=0; j<nLadders; ++j) {
      
      string laddername = layername + _toString(j,"_ladder%d");
      double radius = sens_radius + ((sens_thick+supp_thick)/2. - sens_thick/2.);
      Position pos(radius*cos(j*dphi),radius*sin(j*dphi),0.);

      pv = assembly.placeVolume(laddervol,Transform3D(RotationZ(j*dphi),pos));

      // this will result int the correct cellID to be set...
      pv.addPhysVolID("layer",layer_id).addPhysVolID("module",j).addPhysVolID("sensor",0 ) ;

   }
  }
  


  pv = lcdd.pickMotherVolume(sit).placeVolume(assembly)  ;

  pv.addPhysVolID("system", x_det.id()).addPhysVolID("side",0 ) ;

  sit.setPlacement( pv );
  return sit;
}

DECLARE_DETELEMENT(ILDExSIT,create_element);
