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

  
  //--------------- gear: create gear::ZPlanarParameters and add them as Extension
  gear::ZPlanarParametersImpl* gearZPlanar = new gear::ZPlanarParametersImpl( gear::ZPlanarParameters::HYBRID ,  0.0,  0.0,  0.0,  0.0,  0.0 ) ;
  // ZPlanarParametersImpl( int type, double shellInnerRadius, double shellOuterRadius, double shellHalfLength, double shellGap, double shellRadLength ) ;
  // -> the SIT has no outer shell - set type to HYPRID to keep gear happy (needs fix in gear)
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
    
    Material suppmat =  lcdd.material( x_support.materialStr() ) ;
    Material sensmat =  lcdd.material( x_ladder.materialStr() );

    Volume      laddervol (layername+"_ladder",ladderbox , suppmat );
    Box         sensbox   (sens_thick/2.,width/2.,zhalf);
    Volume      sensvol   (layername+"_sens",sensbox, sensmat );
    Box         suppbox   (supp_thick/2.,width/2.,zhalf);
    Volume      suppvol   (layername+"_supp",suppbox,lcdd.material(x_support.materialStr()));

    // Position    senspos   (-(sens_thick+supp_thick)/2.+sens_thick/2.,0,0);
    // Position    supppos   (-(sens_thick+supp_thick)/2.+sens_thick+supp_thick/2.,0,0);
    // --- position the sensitive on top of the support !
    Position    senspos   ( (sens_thick+supp_thick)/2. - sens_thick/2., 0, 0 );
    Position    supppos   ( (sens_thick+supp_thick)/2. - sens_thick - supp_thick/2., 0, 0 );
      
    sens.setType("tracker");
    sensvol.setSensitiveDetector(sens);
    sensvol.setAttributes(lcdd,x_det.regionStr(),x_det.limitsStr(),x_layer.visStr());

    pv = laddervol.placeVolume(sensvol,senspos) ;
    laddervol.placeVolume(suppvol,supppos);
    sit.setVisAttributes(lcdd, x_det.visStr(),laddervol);

    double radius = sens_radius + ((sens_thick+supp_thick)/2. - sens_thick/2.);

    for(int j=0; j<nLadders; ++j) {
      
      string laddername = layername + _toString(j,"_ladder%d");
      Position pos(radius*cos(j*dphi),radius*sin(j*dphi),0.);

      pv = assembly.placeVolume(laddervol,Transform3D(RotationZ(j*dphi),pos));

      // this will result int the correct cellID to be set...
      pv.addPhysVolID("layer",layer_id).addPhysVolID("module",j).addPhysVolID("sensor",0 ) ;


   }
    //----------------- gear ---------------------------------------------
    double ladderRadLength = suppmat->GetMaterial()->GetRadLen() /tgeo::mm ; 
    double sensitiveRadLength = sensmat->GetMaterial()->GetRadLen() /tgeo::mm ; 

    gearZPlanar->addLayer ( nLadders, 0. , radius/tgeo::mm, 0. ,  supp_thick/tgeo::mm  , 2*zhalf/tgeo::mm, width/tgeo::mm, ladderRadLength,  
			    (radius+supp_thick)/tgeo::mm,  0 ,  sens_thick/tgeo::mm ,  2*zhalf/tgeo::mm ,  width/tgeo::mm, sensitiveRadLength) ;

    // addLayer (int nLadders, double phi0, double ladderDistance, double ladderOffset, double ladderThickness, double ladderLength, double ladderWidth, double ladderRadLength, 
    //           double sensitiveDistance, double sensitiveOffset, double sensitiveThickness, double sensitiveLength, double sensitiveWidth, double sensitiveRadLength)
    //----------------- gear ---------------------------------------------

  }
  


  pv = lcdd.pickMotherVolume(sit).placeVolume(assembly)  ;

  pv.addPhysVolID("system", x_det.id()).addPhysVolID("side",0 ) ;

  sit.setPlacement( pv );
  return sit;
}

DECLARE_DETELEMENT(ILDExSIT,create_element);
