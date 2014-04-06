#include "DD4hep/DDTest.h"

#include "DD4hep/LCDD.h"
#include "DD4hep/TGeoUnits.h"
#include "DD4hep/Volumes.h"
#include "DD4hep/Detector.h"

#include "DDRec/Surface.h"
//#include "DDSurfaces/ISurface.h"

#include <exception>
#include <iostream>
#include <sstream>
#include <assert.h>
#include <cmath>

using namespace std ;
using namespace DD4hep ;
using namespace Geometry;
using namespace DDRec ;
using namespace DDSurfaces ;

//using namespace tgeo ;

// this should be the first line in your test
DDTest test = DDTest( "surface" ) ; 

//=============================================================================
/** Helper class for floating point comparisons using string representations */
class STR {
  STR() {} 
  float _val ;
  std::string _str ;
public:
  STR ( float val ) : _val( val ){
    std::stringstream s1 ; 
    s1 << _val ;
    _str = s1.str() ;
  }
  std::string str() const { return _str ; }
  
  bool operator==( const STR& s2) const {
    return this->str() == s2.str() ;
  }
};

std::ostream& operator<<(std::ostream& os , const STR& s) {
  os << s.str() ;
  return os ;
} 
//=============================================================================

int main(int argc, char** argv ){
    
  test.log( "test units" );
  
  if( argc < 2 ) {
    std::cout << " usage:  test_surface units.xml " << std::endl ;
    exit(1) ;
  }

  try{
    
    // ----- write your tests in here -------------------------------------

    LCDD& lcdd = LCDD::getInstance();

    lcdd.fromCompact( argv[1] );

    // --- test a planar surface
    double thick  = 0.005 ;
    double width  = 1.0  ;
    double length = 10.0 ;

    Material    mat    = lcdd.material( "Silicon" );
    Box         box   ( thick/2.,  width/2.,  length/2. );
    Volume      vol   ( "test_box", box, mat);
    
    Vector3D u( 0. , 1. , 0. ) ;
    Vector3D v( 0. , 0. , 1. ) ;
    Vector3D n( 1. , 0. , 0. ) ;
    Vector3D o( 0. , 0. , 0. ) ;


    VolPlane surf( vol , SurfaceType( SurfaceType::Sensitive ), thick/2, thick/2 , u,v,n,o ) ;

    // test inside bounds for some points:

    test( surf.insideBounds(  Vector3D(  0. , 23. , 42.  )  ) , false , " insideBounds Vector3D(   0. , 23. , 42. )   " ) ; 

    test( surf.insideBounds(  Vector3D(  0,  .23 ,  .42  )  ) , true , " insideBounds Vector3D(    0,  .23 ,  .42  )   " ) ; 

    test( surf.insideBounds(  Vector3D(  0.00003 ,  .23 ,  .42  )  ) , true , " insideBounds Vector3D(   0.00003 ,  .23 ,  .42  )   " ) ; 


    // --- test SurfaceMaterial
    SurfaceMaterial sm( mat ) ;

    // FIXME: these cause a seg fault ....

    // test( STR( sm.A() )  , STR( 93.4961 ) , "   SurfaceMaterial.A() == 93.4961 " ) ; 
    
    // test( STR( sm.Z() )  , STR( 93.4961 ) , "   SurfaceMaterial.Z() == 93.4961 " ) ; 

    test( STR( sm.radiationLength() / tgeo::mm )  , STR( 93.4961 ) , "   SurfaceMaterial.radiationLength() == 93.4961 * mm " ) ; 

    test( STR( sm.interactionLength() / tgeo::mm )  , STR( 457.532 ) , "   SurfaceMaterial.interactionLength() == 457.532 * mm " ) ; 
    


    // test surface type:

    test( surf.type().isSensitive() , true , " surface is sensitive " )  ;

    test( surf.type().isPlane() , true , " surface is Plane " )  ;

    surf.type().checkParallelToZ(  surf ) ;

    test( surf.type().isZPlane() , true , " surface is ZPlane " )  ;

    test( surf.type().isCylinder() , false , " surface is no Cylinder " )  ;


    // --------------------------------------------------------------------

    // test a cylindrical surface

    thick  = 1.0 ;
    length = 100.0 ;
    double radius = 42.0  ;

    mat    = lcdd.material( "Air" );
    Tube    tube  ( radius - thick/2., radius + thick/2. , length/2. );
    vol = Volume( "test_tube", tube , mat);

    Vector3D o_radius = Vector3D( radius , 0. , 0.  ) ;
    
    VolCylinder surfT( vol , SurfaceType( SurfaceType::Sensitive ), thick/2, thick/2 , u,v,n, o_radius  ) ;

    test( surfT.insideBounds(  Vector3D(  radius * sin(0.75) , radius * cos( 0.75 ) , 49.  )) , true 
	  , " insideBounds Vector3D(  radius * sin(0.75) , radius * cos( 0.75 ) , 49. )  " ) ; 

    test( surfT.insideBounds(  Vector3D(  radius * sin(0.75) , radius * cos( 0.75 ) , 50.01  )) , false 
	  , " insideBounds Vector3D(  radius * sin(0.75) , radius * cos( 0.75 ) , 50.01 )  " ) ; 

    test( surfT.insideBounds(  Vector3D(  (radius+0.001) * sin(0.75) , (radius+0.001) * cos( 0.75 ) , 49.  )) , false 
	  , " insideBounds Vector3D(  (radius+0.001) * sin(0.75) , (radius+0.001) * cos( 0.75 ) , 49. )  " ) ; 


    test( surfT.insideBounds(  Vector3D(  (radius+0.00005) * sin(0.75) , (radius+0.00005) * cos( 0.75 ) , 49.  )) , true
	  , " insideBounds Vector3D(  (radius+0.00005) * sin(0.75) , (radius+0.00005) * cos( 0.75 ) , 49. )  " ) ; 



    // test surface type:

    test( surfT.type().isSensitive() , true , " surface is sensitive " )  ;

    test( surfT.type().isPlane() , false , " surface is no Plane " )  ;


    surfT.type().checkParallelToZ(  surfT ) ;

    test( surfT.type().isZCylinder() , true , " surface is ZCylinder " )  ;


   // --------------------------------------------------------------------


  } catch( exception &e ){
    //} catch( ... ){

    test.log( e.what() );
    test.error( "exception occurred" );
  }

  return 0;
}

//=============================================================================
