#include "DD4hep/DDTest.h"

#include "DD4hep/LCDD.h"
#include "DD4hep/DD4hepUnits.h"
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


    //=============== test global to local ===================

    Vector3D point = o + 34.3 * u - 42.7 * v ; 

    ISurface::Vector2D lp = surf.globalToLocal( point ) ;
    //    std::cout << " --- local coordinates of " << point << " : (" << lp[0] << "," << lp[1] << ")" << std::endl ;
    test(  STR( lp[0] ) == STR( 34.3 ) , true , " local u coordinate is 34.4 "  ) ;  
    test(  STR( lp[1] ) == STR( -42.7 ) , true , " local v coordinate is -42.7 "  ) ;  

    Vector3D pointPrime = surf.localToGlobal( lp ) ;
    test(  pointPrime.isEqual( point ) , true , " point after global to local to global is the same " ) ;

    // ----- test with rotated coordinates
    Vector3D ur,vr ; 
    ur.fill( 0. ,  cos( 3.5/180.*M_PI  ) ,  sin( 3.5/180.*M_PI  ) ) ;
    vr.fill( 0. , -sin( 3.5/180.*M_PI  ) ,  cos( 3.5/180.*M_PI  ) ) ;
    VolPlane surfR( vol , SurfaceType( SurfaceType::Sensitive ), thick/2, thick/2 , ur,vr,n,o ) ;

    Vector3D pointR = o + 34.3 * ur - 42.7 * vr ; 

    lp = surfR.globalToLocal( pointR ) ;
    //    std::cout << " --- local coordinates of " << pointR << " : (" << lp[0] << "," << lp[1] << ")" << std::endl ;
    test(  STR( lp[0] ) == STR( 34.3 ) , true , " local u coordinate is 34.4 "  ) ;  
    test(  STR( lp[1] ) == STR( -42.7 ) , true , " local v coordinate is -42.7 "  ) ;  

    Vector3D pointPrimeR = surfR.localToGlobal( lp ) ;
    test(  pointPrimeR.isEqual( pointR ) , true , " point after global to local to global is the same " ) ;

    // ----- test with non-orthogonal rotated coordinates
    Vector3D vr2 ; 
    vr2.fill( 0. , -sin( 35./180.*M_PI  ) ,  cos( 35./180.*M_PI  ) ) ;
    VolPlane surfR2( vol , SurfaceType( SurfaceType::Sensitive ), thick/2, thick/2 , ur,vr2,n,o ) ;

    Vector3D pointR2 = o + 34.3 * ur - 42.7 * vr2 ; 

    lp = surfR2.globalToLocal( pointR2 ) ;
    //    std::cout << " --- local coordinates of " << pointR << " : (" << lp[0] << "," << lp[1] << ")" << std::endl ;
    test(  STR( lp[0] ) == STR( 34.3 ) , true , " local u coordinate is 34.4 "  ) ;  
    test(  STR( lp[1] ) == STR( -42.7 ) , true , " local v coordinate is -42.7 "  ) ;  

    Vector3D pointPrimeR2 = surfR2.localToGlobal( lp ) ;
    test(  pointPrimeR2.isEqual( pointR2 ) , true , " point after global to local to global is the same " ) ;


    //==================================================



    // --- test SurfaceMaterial
    SurfaceMaterial sm( mat ) ;

    // material properies of Si :
    test( STR( sm.A() )  , STR( 28.0855 ) , "   SurfaceMaterial.A() == 28.0855 " ) ; 

    test( STR( sm.Z() )  , STR( 14 ) , "   SurfaceMaterial.Z() == 14 " ) ; 

    test( STR( sm.density() )  , STR( 2.33 ) , "   SurfaceMaterial.density() == 2.33 " ) ; 

    test( STR( sm.radiationLength() / dd4hep::mm )  , STR( 93.4961 ) , "   SurfaceMaterial.radiationLength() == 93.4961 * mm " ) ; 

    test( STR( sm.interactionLength() / dd4hep::mm )  , STR( 457.532 ) , "   SurfaceMaterial.interactionLength() == 457.532 * mm " ) ; 
    


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
    
    VolCylinder surfT( vol , SurfaceType( SurfaceType::Sensitive ), thick/2, thick/2 , o_radius  ) ;

    std::cout << " *** cylinder surface : " << surfT << std::endl ;

    test( surfT.insideBounds(  Vector3D(  radius * sin(0.75) , radius * cos( 0.75 ) , 49.  )) , true 
	  , " insideBounds Vector3D(  radius * sin(0.75) , radius * cos( 0.75 ) , 49. )  " ) ; 

    test( surfT.insideBounds(  Vector3D(  radius * sin(0.75) , radius * cos( 0.75 ) , 50.01  )) , false 
	  , " insideBounds Vector3D(  radius * sin(0.75) , radius * cos( 0.75 ) , 50.01 )  " ) ; 

    test( surfT.insideBounds(  Vector3D(  (radius+0.001) * sin(0.75) , (radius+0.001) * cos( 0.75 ) , 49.  )) , false 
	  , " insideBounds Vector3D(  (radius+0.001) * sin(0.75) , (radius+0.001) * cos( 0.75 ) , 49. )  " ) ; 


    test( surfT.insideBounds(  Vector3D(  (radius+0.00005) * sin(0.75) , (radius+0.00005) * cos( 0.75 ) , 49.  )) , true
	  , " insideBounds Vector3D(  (radius+0.00005) * sin(0.75) , (radius+0.00005) * cos( 0.75 ) , 49. )  " ) ; 


    Vector3D y( 0. , radius , 42 ) ;
    
    Vector3D yn = surfT.normal( y ) ;

    bool dummy =   yn.isEqual( Vector3D( 0. , 1. , 0 ) ) ;
    test( dummy , true , "  normal at (0.,radius,42) is  Vector3D( 0. , 1. , 0 ) " ) ; 
      
    if( ! dummy ) 
      std::cout << " ** yn = " << yn << std::endl ;


    Vector3D yv = surfT.v( y ) ;
    dummy = yv.isEqual( Vector3D( -1. , 0. , 0 ) ) ;
    test( dummy , true , "  v at (0.,radius,42) is  Vector3D( -1. , 0. , 0 ) " ) ; 
    if( ! dummy ) 
      std::cout << " ** yv = " << yv << std::endl ;


    //=============== test global to local ===================
    
    Vector3D pointC( radius , -42.7/radius , 34.3  , Vector3D::cylindrical  )  ;
    
    ISurface::Vector2D lpC = surfT.globalToLocal( pointC ) ;

    // std::cout << " --- local coordinates of " << pointC << " : (" << lpC[0] << "," << lpC[1] << ")" << std::endl ;

    test(  STR( lpC[0] ) == STR( 34.3 ) , true , " local u coordinate is 34.4 "  ) ;  
    test(  STR( lpC[1] ) == STR( -42.7 ) , true , " local v coordinate is -42.7 "  ) ;  

    Vector3D pointPrimeC = surfT.localToGlobal( lpC ) ;

    // std::cout << " --- global coordinates of point after local to global ( " << pointC.rho() << ", " << pointC.phi() << ", " << pointC.z()
    //   	      << " ) : (" << lpC[0] << "," << lpC[1] << ")" << std::endl ;

    test(  pointPrimeC.isEqual( pointC ) , true , " point after global to local to global is the same " ) ;

    //========================================================





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
