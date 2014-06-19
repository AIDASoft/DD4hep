// $Id: ILDExSIT_geo.cpp 687 2013-08-09 12:56:00Z gaede $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DD4hep/DetFactoryHelper.h"

#ifdef DD4HEP_WITH_GEAR
#include "DDRec/DDGear.h"
#include "gearimpl/ZPlanarParametersImpl.h"
#endif

#include "DDRec/Surface.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;
using namespace DDRec ;
using namespace DDSurfaces ;
  
static Ref_t create_element(LCDD& lcdd, xml_h e, SensitiveDetector sens)  {
  xml_det_t    x_det = e;
  string       name  = x_det.nameStr();
  DetElement   sit(name,x_det.id());
  Assembly assembly( name + "_assembly"  ) ;
  PlacedVolume pv;

  
#ifdef DD4HEP_WITH_GEAR
  //--------------- gear: create gear::ZPlanarParameters and add them as Extension
  gear::ZPlanarParametersImpl* gearZPlanar = new gear::ZPlanarParametersImpl( gear::ZPlanarParameters::HYBRID ,  0.0,  0.0,  0.0,  0.0,  0.0 ) ;
  // ZPlanarParametersImpl( int type, double shellInnerRadius, double shellOuterRadius, double shellHalfLength, double shellGap, double shellRadLength ) ;
  // -> the SIT has no outer shell - set type to HYPRID to keep gear happy (needs fix in gear)
  sit.addExtension<GearHandle>( new GearHandle( gearZPlanar, "SITParameters" )  ) ;
  //--------------------------------------------------------------------
#endif


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

#if 0 /// debug: replacing Box with Trap ...
    Trap sensbox( zhalf,
		  0.0,
		  0.0,
		  width/2.,
		  sens_thick/2.,
		  sens_thick/2.,
		  0.0,
		  width/2.,
		  sens_thick/2.,
		  sens_thick/2.,
		  0.0 ) ;
#else
    Box         sensbox   (sens_thick/2.,width/2.,zhalf);
#endif


    Volume      sensvol   (layername+"_sens",sensbox, sensmat );
    Box         suppbox   (supp_thick/2.,width/2.,zhalf);
    Volume      suppvol   (layername+"_supp",suppbox,lcdd.material(x_support.materialStr()));


    // --- create an assembly and DetElement for the layer 

    Assembly     layer_assembly( layername );
    
    DetElement   layerDE( sit , _toString(layer_id,"layer_%d"), x_det.id() );
    
    pv = assembly.placeVolume(  layer_assembly );
    pv.addPhysVolID("layer", layer_id );  

    layerDE.setPlacement( pv ) ;

    //--------------------------------

    // create a measurement plane for the tracking surface attched to the sensitive volume 
    Vector3D u( 0. , 1. , 0. ) ;
    Vector3D v( 0. , 0. , 1. ) ;

    // implement 7 deg stereo angle 
    if( layer_id % 2 ){
      
      u.fill( 0. ,  cos( 3.5/180.*M_PI  ) ,  sin( 3.5/180.*M_PI  ) ) ;
      v.fill( 0. , -sin( 3.5/180.*M_PI  ) ,  cos( 3.5/180.*M_PI  ) ) ;

    } else {

      u.fill( 0. ,  cos( 3.5/180.*M_PI  ) , -sin( 3.5/180.*M_PI  ) ) ;
      v.fill( 0. ,  sin( 3.5/180.*M_PI  ) ,  cos( 3.5/180.*M_PI  ) ) ;
    }

    Vector3D n( 1. , 0. , 0. ) ;
    VolPlane surf( sensvol , SurfaceType(SurfaceType::Sensitive) , sens_thick/2 + supp_thick/2 , sens_thick/2 , u,v,n ) ;


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

      pv = layer_assembly.placeVolume(laddervol,Transform3D(RotationZ(j*dphi),pos));

      // this will result int the correct cellID to be set...
      pv.addPhysVolID("layer",layer_id).addPhysVolID("module",j).addPhysVolID("sensor",0 ) ;

      // create a DetElement for the ladder and assign the meassurment surface to it
      DetElement   ladderDE( sit ,  laddername , x_det.id() );
      ladderDE.setPlacement( pv ) ;
      volSurfaceList( ladderDE )->push_back( surf ) ;

   }

#ifdef DD4HEP_WITH_GEAR
    //----------------- gear ---------------------------------------------
    double ladderRadLength = suppmat->GetMaterial()->GetRadLen() /dd4hep::mm ; 
    double sensitiveRadLength = sensmat->GetMaterial()->GetRadLen() /dd4hep::mm ; 

    gearZPlanar->addLayer ( nLadders, 0. , radius/dd4hep::mm, 0. ,  supp_thick/dd4hep::mm  , 2*zhalf/dd4hep::mm, width/dd4hep::mm, ladderRadLength,  
			    (radius+supp_thick)/dd4hep::mm,  0 ,  sens_thick/dd4hep::mm ,  2*zhalf/dd4hep::mm ,  width/dd4hep::mm, sensitiveRadLength) ;

    // addLayer (int nLadders, double phi0, double ladderDistance, double ladderOffset, double ladderThickness, double ladderLength, double ladderWidth, double ladderRadLength, 
    //           double sensitiveDistance, double sensitiveOffset, double sensitiveThickness, double sensitiveLength, double sensitiveWidth, double sensitiveRadLength)
    //----------------- gear ---------------------------------------------
#endif

  }
  


  pv = lcdd.pickMotherVolume(sit).placeVolume(assembly)  ;

  pv.addPhysVolID("system", x_det.id()).addPhysVolID("side",0 ) ;

  sit.setPlacement( pv );
  return sit;
}

DECLARE_DETELEMENT(ILDExSIT,create_element);
