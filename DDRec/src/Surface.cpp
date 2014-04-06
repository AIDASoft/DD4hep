#include "DDRec/Surface.h"
#include "DD4hep/Detector.h"


namespace DD4hep {
  namespace DDRec {
 
    using namespace Geometry ;

    SurfaceData::SurfaceData() : _type( SurfaceType() ) ,
		    _u( Vector3D() ) ,
		    _v( Vector3D()  ) ,
		    _n( Vector3D() ) ,
		    _o( Vector3D() ) ,
		    _th_i( 0. ),
		    _th_o( 0. ),
		    _innerMat( Material() ),
		    _outerMat( Material() ) {
    }
  
  
    SurfaceData::SurfaceData( SurfaceType type , double thickness_inner ,double thickness_outer, 
		 Vector3D u ,Vector3D v ,Vector3D n ,Vector3D o ) :  _type(type ) ,
								     _u( u ) ,
								     _v( v ) ,
								     _n( n ) ,
								     _o( o ),
								     _th_i( thickness_inner ),
								     _th_o( thickness_outer ),  
								     _innerMat( Material() ),
								     _outerMat( Material() ) {
    }
  
  
  
    VolSurface::VolSurface( Volume vol, SurfaceType type, double thickness_inner ,double thickness_outer, 
			    Vector3D u ,Vector3D v ,Vector3D n ,Vector3D o ) :  
      
      Handle( new SurfaceData( type, thickness_inner ,thickness_outer, u,v,n,o) ) ,

      _vol( vol ) {
    }      
    

    /** Distance to surface */
    double VolPlane::distance(const Vector3D& point ) const {

      return ( point - origin() ) *  normal()  ;
    }
    
    /// Checks if the given point lies within the surface
    bool VolPlane::insideBounds(const Vector3D& point, double epsilon) const {


#if 0
      double dist = std::abs ( distance( point ) ) ;
      
      bool inShape = volume()->GetShape()->Contains( point ) ;
      
      std::cout << " ** Surface::insideBound( " << point << " ) - distance = " << dist 
 		<< " origin = " << origin() << " normal = " << normal() 
 		<< " p * n = " << point * normal() 
 		<< " isInShape : " << inShape << std::endl ;
	
      return dist < epsilon && inShape ;
 #else
	
      return ( std::abs ( distance( point ) ) < epsilon )  &&  volume()->GetShape()->Contains( point ) ; 
 #endif
 
    }

    /** Distance to surface */
    double VolCylinder::distance(const Vector3D& point ) const {

      return point.rho() - origin().rho()  ;
    }
    
    /// Checks if the given point lies within the surface
    bool VolCylinder::insideBounds(const Vector3D& point, double epsilon) const {
      
#if 0
      double distR = std::abs( distance( point ) ) ;
      
      bool inShapeT = volume()->GetShape()->Contains( point ) ;
      
      std::cout << " ** Surface::insideBound( " << point << " ) - distance = " << distR 
 		<< " origin = " << origin() 
 		<< " isInShape : " << inShapeT << std::endl ;
      
      return distR < epsilon && inShapeT ;
#else
      
      return ( std::abs ( distance( point ) ) < epsilon )  &&  volume()->GetShape()->Contains( point ) ; 

#endif
    }



    //====================


    VolSurfaceList* surfaceList( DetElement& det ) {

      
      VolSurfaceList* list = 0 ;

      try {

	list = det.extension< VolSurfaceList >() ;

      } catch( std::runtime_error e){ 
	
	list = det.addExtension<VolSurfaceList >(  new VolSurfaceList ) ; 
      }

      return list ;
    }


  } // namespace
} // namespace
