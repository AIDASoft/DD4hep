// $Id: $
//====================================================================
//  // Simple tube filled with air 
//  // used for tracking purposes ...
//  
//--------------------------------------------------------------------
//
//  Author     : F.Gaede
//
//====================================================================
#include "DD4hep/DetFactoryHelper.h"

#include "DDRec/Surface.h"
#include "DDRec/DetectorData.h"


using namespace dd4hep;
using namespace dd4hep::detail;
using namespace dd4hep::rec ;
using namespace DDSurfaces ;

static Ref_t create_element(Detector& description, xml_h e, SensitiveDetector /* sens */)  {
  
  xml_det_t    x_det = e;
  std::string  name  = x_det.nameStr();
  
  DetElement airTube( name, x_det.id()  ) ;
  
  PlacedVolume pv;
  
  // ----- read xml ----------------------

  xml_dim_t dim = x_det.dimensions();

  double inner_r    =  dim.rmin() ;
  double outer_r    =  dim.rmax() ;
  double z_half     =  dim.zhalf() ;
  double tube_thick =  outer_r - inner_r ;
  
  //--------------------------------------

  Tube   tubeSolid (inner_r, outer_r, z_half ) ;

  Volume tube_vol( name+"_inner_cylinder_air", tubeSolid ,  description.material("Air") ) ;
  
  Vector3D ocyl(  inner_r + 0.5*tube_thick , 0. , 0. ) ;
  
  VolCylinder cylSurf( tube_vol , SurfaceType( SurfaceType::Helper ) , 0.5*tube_thick  , 0.5*tube_thick , ocyl ) ;
  
  volSurfaceList( airTube )->push_back( cylSurf ) ;
  

  //--------------------------------------

  Volume mother =  description.pickMotherVolume( airTube ) ;

  pv = mother.placeVolume( tube_vol ) ;
  
  pv.addPhysVolID( "system", x_det.id() )  ;
  
  airTube.setPlacement( pv );
       
  return airTube;
}

DECLARE_DETELEMENT( AirTube,create_element)
