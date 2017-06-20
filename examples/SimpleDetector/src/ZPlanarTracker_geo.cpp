// $Id: $
//====================================================================
//  Simple tracking detector made from planar sensors that are parallel
//  to the z-axis. There are two materials per ladder: one sensitive
//  and one support.
//--------------------------------------------------------------------
//
//  Author     : F.Gaede
//
//====================================================================
#include "DD4hep/DetFactoryHelper.h"

#include "DDRec/Surface.h"
#include "DDRec/DetectorData.h"
#include <exception>

using namespace dd4hep;
using namespace dd4hep::detail;
using namespace dd4hep::rec ;
using namespace DDSurfaces ;

static Ref_t create_element(Detector& description, xml_h e, SensitiveDetector sens)  {
  
  xml_det_t    x_det = e;
  std::string  name  = x_det.nameStr();
  
  // put the whole detector into an assembly
  //  - should be replaced by an envelope volume ...
  
  Assembly assembly( name+"_assembly" );
  
  DetElement tracker( name, x_det.id()  ) ;

  PlacedVolume pv;
  
  rec::ZPlanarData*  zPlanarData = new rec::ZPlanarData ;


  double minRadius = 1e99 ;
  double minZhalf = 1e99 ;


  bool isStripDetector = false ;
  try {
    isStripDetector = x_det.attr<bool>( _Unicode(isStripDetector) ) ;

  } catch(std::runtime_error ){}

  //=========  loop over layer elements in xml  ======================================

  for(xml_coll_t c(e, _U(layer) ); c; ++c)  {
    
    xml_comp_t x_layer( c );
    
    // child elements: ladder and sensitive
    xml_comp_t x_sensitive( x_layer.child( _U(sensitive) ));
    xml_comp_t x_ladder(  x_layer.child( _U(ladder)  ));
    
    int layer_id = x_layer.id();
    int nLadders = x_layer.attr<double>(  _Unicode(nLadders) ) ;

    double dphi = 2.*M_PI / double(nLadders);

    std::string layername = name+_toString(layer_id,"_layer%d");
    

    // --- create an assembly and DetElement for the layer

    Assembly layer_assembly( "layer_assembly" +_toString(layer_id,"_%d") );
   
    DetElement layerDE( tracker , _toString(layer_id,"layer_%d"), x_det.id() );
   
    pv = assembly.placeVolume(  layer_assembly );

    pv.addPhysVolID("layer", layer_id );  

    layerDE.setPlacement( pv ) ;

    //--------------------------------

    double supp_zhalf     = x_ladder.length();
    double supp_offset    = x_ladder.offset();
    double supp_distance  = x_ladder.distance();
    double supp_thickness = x_ladder.thickness();
    double supp_width     = x_ladder.width();

    std::string supp_vis  = x_ladder.visStr() ;
    std::string supp_matS = x_ladder.materialStr() ;
    
    double sens_zhalf     = x_sensitive.length();
    double sens_offset    = x_sensitive.offset();
    double sens_distance  = x_sensitive.distance();
    double sens_thickness = x_sensitive.thickness();
    double sens_width     = x_sensitive.width();

    std::string sens_vis  = x_sensitive.visStr() ;
    std::string sens_matS = x_sensitive.materialStr() ;
    
    double phi0           = x_layer.phi0() ;


    if( sens_distance < minRadius ) minRadius = sens_distance ;
    if( supp_distance < minRadius ) minRadius = supp_distance ;
    if( sens_zhalf < minZhalf ) minZhalf = sens_zhalf ;
    if( supp_zhalf < minZhalf ) minZhalf = supp_zhalf ;

    //-----------------------------------
    //  store the data in an extension to be used for reconstruction 
    rec::ZPlanarData::LayerLayout thisLayer ;

    thisLayer.sensorsPerLadder =  1 ; // for now only one planar sensor
    thisLayer.lengthSensor     =  2. * sens_zhalf ;
    
    thisLayer.distanceSupport  = supp_distance ;
    thisLayer.offsetSupport    = supp_offset ;
    thisLayer.thicknessSupport = supp_thickness ;
    thisLayer.zHalfSupport     = supp_zhalf ;
    thisLayer.widthSupport     = supp_width ;      
    
    thisLayer.distanceSensitive  = sens_distance ;  
    thisLayer.offsetSensitive    = sens_offset ;	  
    thisLayer.thicknessSensitive = sens_thickness ; 
    thisLayer.zHalfSensitive     = sens_zhalf ;	  
    thisLayer.widthSensitive     = sens_width ;     
    
    thisLayer.ladderNumber =  nLadders ;
    thisLayer.phi0         =  phi0 ;
    
    zPlanarData->layers.push_back( thisLayer ) ;
    //-----------------------------------


    Material supp_mat     = description.material( supp_matS ) ;
    Material sens_mat     = description.material( sens_matS ) ;


    //-------
    Box sens_box( sens_thickness/2., sens_width/2., sens_zhalf );
    Box supp_box( supp_thickness/2., supp_width/2., supp_zhalf );

    Volume supp_vol( layername+"_supp", supp_box, supp_mat  );
    Volume sens_vol( layername+"_sens", sens_box, sens_mat );


    // -------- create a measurement plane for the tracking surface attched to the sensitive volume -----
    Vector3D u( 0. , 1. , 0. ) ;
    Vector3D v( 0. , 0. , 1. ) ;
    Vector3D n( 1. , 0. , 0. ) ;
    //    Vector3D o( 0. , 0. , 0. ) ;

    // compute the inner and outer thicknesses that need to be assigned to the tracking surface
    // depending on wether the support is above or below the sensor
    double inner_thickness = ( sens_distance > supp_distance ?  ( sens_distance - supp_distance ) + sens_thickness/2  : sens_thickness/2 ) ;
    double outer_thickness = ( sens_distance > supp_distance ?    sens_thickness/2  :  ( supp_distance - sens_distance ) + supp_thickness - sens_thickness/2   ) ;
    
    SurfaceType type( SurfaceType::Sensitive ) ;

    if( isStripDetector ) 
      type.setProperty( SurfaceType::Measurement1D , true ) ;

    VolPlane surf( sens_vol , type , inner_thickness , outer_thickness , u,v,n ) ; //,o ) ;

    //--------------------------------------------
     
    sens.setType("tracker");
    sens_vol.setSensitiveDetector(sens);
    
    sens_vol.setAttributes( description, x_det.regionStr(), x_det.limitsStr(), sens_vis );
    supp_vol.setAttributes( description, x_det.regionStr(), x_det.limitsStr(), supp_vis );


    //--------- loop over ladders ---------------------------

    for(int j=0; j<nLadders; ++j) {

      double phi = phi0 + j * dphi  ;

      std::string laddername = layername + _toString(j,"_ladder%d");

      RotationZYX rot( phi , 0, 0  ) ;


      // --- place support -----
      double lthick = supp_thickness ;
      double radius = supp_distance ;
      double offset = supp_offset ;
      
      /* pv = */layer_assembly.placeVolume(supp_vol,Transform3D(rot, Position((radius+lthick/2.)*cos(phi) - offset*sin(phi),
                                                                              (radius+lthick/2.)*sin(phi) + offset*cos(phi),
                                                                              0. ) ));

      // --- place sensitive -----
      lthick = sens_thickness ;
      radius = sens_distance ;
      offset = sens_offset ;
      
      pv = layer_assembly.placeVolume( sens_vol,Transform3D(rot, Position((radius + lthick/2.0)*cos(phi) - offset*sin(phi),
                                                                          (radius + lthick/2.0)*sin(phi) + offset*cos(phi),
                                                                          0. ) ));

      //      pv.addPhysVolID("layer", layer_id ).addPhysVolID( "module" , j ).addPhysVolID("sensor", 0 )   ;
      pv.addPhysVolID( "module" , j ).addPhysVolID("sensor", 0 )   ;


      DetElement   ladderDE( layerDE ,  laddername , x_det.id() );
      ladderDE.setPlacement( pv ) ;

      volSurfaceList( ladderDE )->push_back( surf ) ;

    }
    

    //    tracker.setVisAttributes(description, x_det.visStr(),laddervol);
    

    // is this needed ??
    layer_assembly->GetShape()->ComputeBBox() ;

  }

#if 0  //-------- add an inscribing cylinder of air for tracking purposes -----------------
       //  this screw up the geometry and the material scan does not work anymore !!!!!?????

  double tube_thick =  1.0 * dd4hep::mm ;
  double inner_r    =  minRadius - 1.1 * tube_thick ;
  double outer_r    =  inner_r + tube_thick ;
  double z_half     =  minZhalf ; 
  
  Tube   tubeSolid (inner_r, outer_r, z_half ) ;
  Volume tube_vol( name+"_inner_cylinder_air", tubeSolid ,  description.material("Air") ) ;
  
  assembly.placeVolume( tube_vol , Transform3D() ) ;
  
  Vector3D ocyl(  inner_r + 0.5*tube_thick , 0. , 0. ) ;
  
  VolCylinder cylSurf( tube_vol , SurfaceType( SurfaceType::Helper ) , 0.5*tube_thick  , 0.5*tube_thick , ocyl ) ;
  
  volSurfaceList( tracker )->push_back( cylSurf ) ;
  
#endif //----------------------------------------------------------------------------------



  tracker.addExtension< rec::ZPlanarData >( zPlanarData ) ;
  
  Volume mother =  description.pickMotherVolume( tracker ) ;

  pv = mother.placeVolume(assembly);
  
  pv.addPhysVolID( "system", x_det.id() ).addPhysVolID("side",0 )  ;
  
  tracker.setPlacement(pv);
       
  assembly->GetShape()->ComputeBBox() ;

  return tracker;
}

DECLARE_DETELEMENT(ZPlanarTracker,create_element)
