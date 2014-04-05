#include "DD4hep/Surface.h"
#include "DD4hep/Detector.h"

namespace DD4hep {
  namespace Geometry {
    

    Surface::Object::Object() : _vol( Volume() ), _type( Surface::Other ) ,
			_isSensitive( false ) ,
			_u( Vector3D() ) ,
			_v( Vector3D()  ) ,
			_n( Vector3D() ) ,
			_o( Vector3D() ) ,
			_th_i( 0. ),
			_th_o( 0. ),
			_innerMat( Material() ),
			_outerMat( Material() ) {
    }
    

    Surface::Object::Object( Volume vol, SurfaceType type, bool isSensitive , double thickness_inner ,double thickness_outer, 
		     Vector3D u ,Vector3D v ,Vector3D n ,Vector3D o ) :  _vol( vol ) ,
									 _type(type ) ,
									 _isSensitive(isSensitive ) ,
									 _u( u ) ,
									 _v( v ) ,
									 _n( n ) ,
									 _o( o ),
									 _th_i( thickness_inner ),
									 _th_o( thickness_outer ),  
									 _innerMat( Material() ),
									 _outerMat( Material() ) {
    }

    Surface::Object::~Object() {
      
      // anything to clean up here ?
    }


    Surface::Surface( Volume vol, SurfaceType type, bool isSensitive , double thickness_inner ,double thickness_outer, 
		      Vector3D u ,Vector3D v ,Vector3D n ,Vector3D o ) :  
      Ref_t( new Object( vol, type, isSensitive,thickness_inner ,thickness_outer, u,v,n,o)    ) {
    }      
    



    bool Surface::isInsideBounds(const Vector3D& p , double epsilon) const {


      double pa[3] = { p.X() , p.Y() , p.Z()   } ;

      switch( getSurfaceType()  ) {
	
      case Surface::Cylinder : { 
	
#if 0
	double distR = std::abs (  p.Rho() - getOrigin().X() ) ;
	
	bool inShapeT = getVolume()->GetShape()->Contains( pa ) ;
	
	std::cout << " ** Surface::isInsideBound( " << p << " ) - distance = " << distR 
		  << " origin = " << getOrigin() 
		  << " isInShape : " << inShapeT << std::endl ;
	
	return distR < epsilon && inShapeT ;
#else
	
	return ( std::abs ( p.Rho() - getOrigin().X() ) < epsilon )  &&  getVolume()->GetShape()->Contains( pa ) ; 
#endif
	
      }
	break ;
	
      default: { // plane
	
	
#if 0
	double dist = std::abs ( ( p - getOrigin() ).Dot( getN() )  ) ;
	
	bool inShape = getVolume()->GetShape()->Contains( pa ) ;
	
	std::cout << " ** Surface::isInsideBound( " << p << " ) - distance = " << dist 
		  << " origin = " << getOrigin() << " normal = " << getN() 
		  << " p * n = " << p.Dot( getN() ) 
		  << " isInShape : " << inShape << std::endl ;
	
	return dist < epsilon && inShape ;
#else
	
	return ( std::abs ( ( p - getOrigin() ).Dot(  getN() ) ) < epsilon )  &&  getVolume()->GetShape()->Contains( pa ) ; 
#endif
      }	
      }
      
    }




    SurfaceList* surfaceList( DetElement& det ) {

      
      SurfaceList* list = 0 ;

      try {

	list = det.extension< SurfaceList >() ;

      } catch( std::runtime_error e){ 
	
	list = det.addExtension<SurfaceList >(  new SurfaceList ) ; 
      }

      return list ;
    }


  }
}
