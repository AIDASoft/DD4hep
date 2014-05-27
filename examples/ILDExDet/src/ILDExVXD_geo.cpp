// $Id: ILDExVXD_geo.cpp 673 2013-08-05 10:01:33Z gaede $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "VXDData.h"

#ifdef DD4HEP_WITH_GEAR
#include "DDRec/DDGear.h"
#include "gearimpl/ZPlanarParametersImpl.h"
#endif

#include "DDRec/Surface.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;
using namespace DD4hep::DDRec ;
using namespace DDSurfaces ;

#define no_split_ladders 0

static Ref_t create_element(LCDD& lcdd, xml_h e, SensitiveDetector sens)  {
  DetElement   vxd;
  xml_det_t    x_det = e;
  string       name  = x_det.nameStr();

  Assembly     assembly(name+"_assembly");

  // replace assembly with cylinder of air:
  xml_comp_t  x_tube (x_det.child(_U(tubs)));
  Tube        envelope_cylinder(x_tube.rmin(),x_tube.rmax(),x_tube.zhalf());
  //  Volume      assembly ("vxd_envelope_cyl", envelope_cylinder ,lcdd.air());

  PlacedVolume pv;

  VXDData* vxd_data = new VXDData();
  vxd.assign(vxd_data,name,x_det.typeStr());
  vxd_data->id = x_det.id();


#ifdef DD4HEP_WITH_GEAR
  //--------------- gear: create gear::ZPlanarParameters and add them as Extension
  gear::ZPlanarParametersImpl* gearZPlanar = new gear::ZPlanarParametersImpl( gear::ZPlanarParameters::CCD ,  0.0,  0.0,  0.0,  0.0,  0.0 ) ;
  // ZPlanarParametersImpl( int type, double shellInnerRadius, double shellOuterRadius, double shellHalfLength, double shellGap, double shellRadLength ) ;
  // -> this VXD has no outer shell ...
  vxd.addExtension<GearHandle>( new GearHandle( gearZPlanar, "VXDParameters" )  ) ;
  //--------------------------------------------------------------------
#endif

  for(xml_coll_t c(e,_U(layer)); c; ++c)  {

    xml_comp_t  x_layer  (c);

    xml_comp_t  x_support (x_layer.child(_U(support)));
    xml_comp_t  x_ladder  (x_layer.child(_U(ladder)));

    int         layer_id   = x_layer.id();
    int         nLadders   = x_ladder.number();
    string      layername  = name+_toString(layer_id,"_layer%d");
    double      dphi       = 2.*M_PI/double(nLadders);





    /// ======== test layer assembly -> results in assembly in assembly and
#define use_layer_assembly 1        // crashes ild_exsimu
#if use_layer_assembly 

    // --- create an assembly and DetElement for the layer 

    Assembly     layer_assembly( "layer_assembly" +_toString(layer_id,"_%d") );
    
    DetElement   layerDE( vxd , _toString(layer_id,"layer_%d"), x_det.id() );
    
    pv = assembly.placeVolume(  layer_assembly );
    pv.addPhysVolID("layer", layer_id );  

    layerDE.setPlacement( pv ) ;

    //--------------------------------
#else
    //=========
    Assembly&  layer_assembly = assembly ;
    //=========

#endif 









#if no_split_ladders
    double      zhalf      = x_ladder.zhalf();
#else
    double      zhalf      = x_ladder.zhalf() / 2 ;
#endif

    double      offset     = x_ladder.offset();
    double      sens_radius= x_ladder.radius();
    double      sens_thick = x_ladder.thickness();
    double      supp_thick = x_support.thickness();

    // double      radius     = sens_radius + ((sens_thick+supp_thick)/2. - sens_thick/2.);
    //fg: this is the radius(distance) of the support ladder which should be under(inside) the sensitive ladder
    double      radius     = sens_radius  - supp_thick ;

    double      phi0       =  x_layer.phi0() ;

    //    double      width      = 2.*tan(dphi/2.)*(sens_radius-sens_thick/2.);
    double      width      =  x_ladder.width();

    Box         ladderbox ( (sens_thick+supp_thick)/2., width/2., zhalf );

    Volume      laddervol (layername+"_ladder",ladderbox,lcdd.air());

    Material    sensmat    = lcdd.material(x_ladder.materialStr());
    Box         sensbox   (sens_thick/2.,  width/2.,   zhalf);
    Volume      sensvol   (layername+"_sens",sensbox,sensmat);


    // create a measurement plane for the tracking surface attched to the sensitive volume 
    Vector3D u( 0. , 1. , 0. ) ;
    Vector3D v( 0. , 0. , 1. ) ;
    Vector3D n( 1. , 0. , 0. ) ;
    //    Vector3D o( 0. , 0. , 0. ) ;
    VolPlane surf( sensvol , SurfaceType(SurfaceType::Sensitive) , sens_thick/2 + supp_thick , sens_thick/2 , u,v,n ) ; //,o ) ;

    Material    suppmat  = lcdd.material(x_support.materialStr());
    Box         suppbox   (supp_thick/2.,width/2.,zhalf);
    Volume      suppvol   (layername+"_supp",suppbox,suppmat);

    // --- position the sensitive on top of the support !
    Position    senspos   ( (sens_thick+supp_thick)/2. - sens_thick/2., 0, 0 );
    Position    supppos   ( (sens_thick+supp_thick)/2. - sens_thick - supp_thick/2., 0, 0 );
      
    sens.setType("tracker");
    sensvol.setSensitiveDetector(sens);
    sensvol.setAttributes(lcdd,x_det.regionStr(),x_det.limitsStr(),x_layer.visStr());

    laddervol.placeVolume(sensvol,senspos);
    laddervol.placeVolume(suppvol,supppos);

    VXDData::Layer layer;
    // Add ladder layer
    layer.NLadders     = nLadders ;
    layer.internalPhi0 = phi0 ; //M_PI/2. - dphi;   // phi0 given wrt x-axis  - internalPhi0 is wrt yaxis , negative orientation
    layer.Distance     = 0.0; // What is this really (?) : ladderDistance;
    layer.Offset       = offset;              // offset given in direction of positive rotation (increasing phi)
    layer.Thickness    = supp_thick;
    layer.Length       = suppbox.x();
    layer.Width        = suppbox.y();
    layer.RadLength    = suppmat->GetMaterial()->GetRadLen();
    vxd_data->_lVec.push_back(layer);

    // Now add sensitive ladder layer
    layer.NLadders     = nLadders ;
    layer.internalPhi0 = phi0 ;  //M_PI/2. - dphi;  // phi0 given wrt x-axis  - internalPhi0 is wrt yaxis , negative orientation
    layer.Distance     = 0.0;    // What is this really (?) : sensitiveDistance ;
    layer.Offset       = offset;  // offset given in direction of positive rotation (increasing phi)
    layer.Thickness    = sens_thick;
    layer.Length       = sensbox.x();
    layer.Width        = sensbox.y();
    layer.RadLength    = sensmat->GetMaterial()->GetRadLen();
    vxd_data->_sVec.push_back(layer);


    for(int j=0; j<nLadders; ++j) {

      double dj = double(j);      
      double phi = phi0 + dj*dphi  ;

      string laddername = layername + _toString(j,"_ladder%d");

      double lthick = sens_thick + supp_thick ;
      
      RotationZYX rot( phi , 0, 0  ) ;


#if no_split_ladders

      pv = layer_assembly.placeVolume( laddervol,Transform3D( rot, Position( (radius + lthick/2.)*cos(phi)  - offset * sin( phi ) ,
									  (radius + lthick/2.)*sin(phi)  + offset * cos( phi ) ,
									  0. ) ));

      pv.addPhysVolID("layer", layer_id ).addPhysVolID( "module" , j ).addPhysVolID("sensor", 0 )   ;


      DetElement   ladderDE( layerDE ,  laddername , x_det.id() );
      ladderDE.setPlacement( pv ) ;


#else

      // put one wafer at plus z and one at minus z
      
      pv = layer_assembly.placeVolume( laddervol, Transform3D( rot ,  Position( (radius + lthick/2.)*cos(phi)  - offset * sin( phi ) ,
									  (radius + lthick/2.)*sin(phi)  + offset * cos( phi ) ,
									  zhalf ) ) );

      pv.addPhysVolID("layer", layer_id ).addPhysVolID( "module" , j ).addPhysVolID("sensor", 0 ).addPhysVolID("side", 1 )   ;



#if use_layer_assembly 
      DetElement   ladderDEposZ( layerDE ,  laddername+"_posZ" , x_det.id() );
#else
      DetElement   ladderDEposZ( vxd ,  laddername+"_posZ" , x_det.id() );
#endif



      ladderDEposZ.setPlacement( pv ) ;

      volSurfaceList( ladderDEposZ )->push_back( surf ) ;


      pv = layer_assembly.placeVolume( laddervol, Transform3D( rot ,  Position( (radius + lthick/2.)*cos(phi)  - offset * sin( phi ) ,
									  (radius + lthick/2.)*sin(phi)  + offset * cos( phi ) ,
									  -zhalf ) ) );

      pv.addPhysVolID("layer", layer_id ).addPhysVolID( "module" , j ).addPhysVolID("sensor", 0 ).addPhysVolID("side", -1 )   ;  ;

#if use_layer_assembly 
      DetElement   ladderDEnegZ( layerDE ,  laddername+"_negZ" , x_det.id() );
#else
      DetElement   ladderDEnegZ( vxd ,  laddername+"_negZ" , x_det.id() );
 #endif

     ladderDEnegZ.setPlacement( pv ) ;


      volSurfaceList( ladderDEnegZ)->push_back( surf ) ;


#endif

      //=========== debug test ===============================
      //      Surface* gSurf = new Surface( ladderDEnegZ,  surf ) ;
      //======================================================


      //pv = layer_assembly.placeVolume( sensvol, pos, rot ) ;

    }
    vxd.setVisAttributes(lcdd, x_det.visStr(),laddervol);

#ifdef DD4HEP_WITH_GEAR
    //----------------- gear ---------------------------------------------
    double ladderRadLength = suppmat->GetMaterial()->GetRadLen() /tgeo::mm ; 
    double sensitiveRadLength = sensmat->GetMaterial()->GetRadLen() /tgeo::mm ; 
    
    gearZPlanar->addLayer ( nLadders, phi0 ,  radius/tgeo::mm, offset/tgeo::mm ,  supp_thick/tgeo::mm  , 2*zhalf/tgeo::mm, width/tgeo::mm, ladderRadLength,  
			    (radius+supp_thick)/tgeo::mm,  offset/tgeo::mm ,  sens_thick/tgeo::mm ,  2*zhalf/tgeo::mm ,  width/tgeo::mm, sensitiveRadLength) ;
    
    // addLayer (int nLadders, double phi0, double ladderDistance, double ladderOffset, double ladderThickness, double ladderLength, double ladderWidth, double ladderRadLength, 
    //           double sensitiveDistance, double sensitiveOffset, double sensitiveThickness, double sensitiveLength, double sensitiveWidth, double sensitiveRadLength)
    //----------------- gear ---------------------------------------------
#endif

    layer_assembly->GetShape()->ComputeBBox() ;
 

  }
  Volume mother =  lcdd.pickMotherVolume(vxd) ;

  pv = mother.placeVolume(assembly);

  pv.addPhysVolID( "system", x_det.id() ).addPhysVolID("side",0 )  ;
  
  vxd.setPlacement(pv);
	
  assembly->GetShape()->ComputeBBox() ;
  //  TGeoBBox* bbox =  assembly->GetShape()->GetBBox() ;



  return vxd;
}

DECLARE_DETELEMENT(ILDExVXD,create_element);
