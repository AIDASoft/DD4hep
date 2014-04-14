#include "DDRec/Surface.h"
#include "DD4hep/Detector.h"

#include <math.h>
#include <memory>
#include <exception>

#include "TGeoMatrix.h"

namespace DD4hep {
  namespace DDRec {
 
    using namespace Geometry ;

    //--------------------------------------------------------

    /** Copy c'tor - copies handle */
    SurfaceMaterial::SurfaceMaterial( Geometry::Material m ) : Geometry::Material( m ) {} 
    
    SurfaceMaterial::SurfaceMaterial( const SurfaceMaterial& sm )  : Geometry::Material( sm ) {
      //      (*this).Geometry::Material::m_element =  sm.Geometry::Material::m_element  ; 
    }
    
    SurfaceMaterial:: ~SurfaceMaterial() {} 

    //--------------------------------------------------------


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


    VolSurfaceList* volSurfaceList( DetElement& det ) {

      
      VolSurfaceList* list = 0 ;

      try {

	list = det.extension< VolSurfaceList >() ;

      } catch( std::runtime_error e){ 
	
	list = det.addExtension<VolSurfaceList >(  new VolSurfaceList ) ; 
      }

      return list ;
    }


    //======================================================================================================================

    bool findVolume( PlacedVolume pv,  Volume theVol, std::list< PlacedVolume >& volList ) {
      

      volList.push_back( pv ) ;
      
      // unsigned count = volList.size() ;
      // for(unsigned i=0 ; i < count ; ++i) {
      // 	std::cout << " **" ;
      // }
      // std::cout << " searching for volume: " << std::hex << theVol.ptr() << "  <-> pv.volume : " <<  pv.volume().ptr() 
      // 		<< " pv.volume().ptr() == theVol.ptr() " <<  (pv.volume().ptr() == theVol.ptr() )
      // 		<< std::endl ;


      if( pv.volume().ptr() == theVol.ptr() ) { 
	
	return true ;
	
      } else {
	
	//--------------------------------

	const TGeoNode* node = pv.ptr();
	
	if ( !node ) {
	  
	  throw std::runtime_error("*** findVolume: Invalid  placement:  - node pointer Null ! " );
	}
	//	Volume vol = pv.volume();
	
	//	std::cout << "              ndau = " << node->GetNdaughters() << std::endl ;

	for (Int_t idau = 0, ndau = node->GetNdaughters(); idau < ndau; ++idau) {
	  
	  TGeoNode* daughter = node->GetDaughter(idau);
	  PlacedVolume placement( daughter );
	  
	  if ( !placement.data() ) {
	    throw std::runtime_error("*** findVolume: Invalid not instrumented placement:"+std::string(daughter->GetName())
				     +" [Internal error -- bad detector constructor]");
	  }
	  
	  PlacedVolume pv_dau = Ref_t(daughter); // why use a Ref_t here  ???
	  
	  if( findVolume(  pv_dau , theVol , volList ) ) {
	    
	    //	    std::cout << "  ----- found in daughter volume !!!  " << std::hex << pv_dau.volume().ptr() << std::endl ;

	    return true ;
	  } 
	}

	//  ------- not found:

	volList.pop_back() ;

	return false ;
	//--------------------------------

      }
    } 


    Surface::Surface( Geometry::DetElement det, VolSurface volSurf ) : _det( det) , _volSurf( volSurf ), _wtM(0) , _id( 0) , _type( _volSurf.type() )  {

      initialize() ;
    }      
 

    const IMaterial& Surface::innerMaterial() const {
      
      SurfaceMaterial& mat = _volSurf->_innerMat ;
      
      if( ! mat.isValid() ) {
	
	// fixme: for now just set the material of the volume holding the surface
	//        neeed averaged material in case of several volumes...
	//	_volSurf.setInnerMaterial( _volSurf.volume().material() ) ;
	
	mat = _volSurf.volume().material() ;

	//	std::cout << "  **** Surface::innerMaterial() - assigning volume material to surface : " << mat.name() << std::endl ;
      }
      return  _volSurf.innerMaterial()  ;
    }
      

    const IMaterial& Surface::outerMaterial() const {

      SurfaceMaterial& mat = _volSurf->_outerMat ;
      
      if( ! mat.isValid() ) {
	
	// fixme: for now just set the material of the volume holding the surface
	//        neeed averaged material in case of several volumes...
	//	_volSurf.setOuterMaterial( _volSurf.volume().material() ) ;
	
	mat  =  _volSurf.volume().material() ;

 	//std::cout << "  **** Surface::outerMaterial() - assigning volume material to surface : " << mat.name() << std::endl ;
      }

      return  _volSurf.outerMaterial()  ;
    }          

    double Surface::distance(const Vector3D& point ) const {

      double pa[3] ;
      _wtM->MasterToLocal( point , pa ) ;
      Vector3D localPoint( pa ) ;
      
      return ( _volSurf.type().isPlane() ?   VolPlane(_volSurf).distance( localPoint )  : VolCylinder(_volSurf).distance( localPoint ) ) ;
    }
      
    bool Surface::insideBounds(const Vector3D& point, double epsilon) const {

      double pa[3] ;
      _wtM->MasterToLocal( point , pa ) ;
      Vector3D localPoint( pa ) ;
      
      return ( _volSurf.type().isPlane() ?   VolPlane(_volSurf).insideBounds( localPoint, epsilon )  : VolCylinder(_volSurf).insideBounds( localPoint , epsilon) ) ;
    }

    void Surface::initialize() {
      
      // first we need to find the right volume for the local surface in the DetElement's volumes
      std::list< PlacedVolume > pVList ;
      PlacedVolume pv = _det.placement() ;
      Volume theVol = _volSurf.volume() ;
      
      if( ! findVolume(  pv, theVol , pVList ) ){
	
	throw std::runtime_error( " ***** ERROR: No Volume found for DetElement with surface " ) ;
      } 

      // std::cout << " **** Surface::initialize() # placements for surface = " << pVList.size() 
      // 		<< " worldTransform : " 
      // 		<< std::endl ; 
      

      //=========== compute and cache world transform for surface ==========

      TGeoMatrix* wm = _det.object<DetElement::Object>().worldTransformation() ;

#if 0 // debug
      wm->Print() ;
      for( std::list<PlacedVolume>::iterator it= pVList.begin(), n = pVList.end() ; it != n ; ++it ){
      PlacedVolume pv = *it ;
      TGeoMatrix* m = pv->GetMatrix();
      std::cout << "  +++ matrix for placed volume : " << std::endl ;
      m->Print() ;
      }
#endif

      // need to get the inverse transformation ( see Detector.cpp )
      std::auto_ptr<TGeoHMatrix> wtI( new TGeoHMatrix( wm->Inverse() ) ) ;

      //---- if the volSurface is not in the DetElement's volume, we need to mutliply the path to the volume to the
      // DetElements world transform
      for( std::list<PlacedVolume>::iterator it = ++( pVList.begin() ) , n = pVList.end() ; it != n ; ++it ){

      	PlacedVolume pv = *it ;
      	TGeoMatrix* m = pv->GetMatrix();
      	// std::cout << "  +++ matrix for placed volume : " << std::endl ;
      	// m->Print() ;
	//      	wtI->MultiplyLeft( m );

      	wtI->Multiply( m );
      }

      //      std::cout << "  +++ new world transform matrix  : " << std::endl ;

#if 0 //fixme: which convention to use here - the correct should be wtI, however it is the inverse of what is stored in DetElement ???
      std::auto_ptr<TGeoHMatrix> wt( new TGeoHMatrix( wtI->Inverse() ) ) ;
      wt->Print() ;
      // cache the world transform for the surface
      _wtM = wt.release()  ;
#else
      //      wtI->Print() ;
      // cache the world transform for the surface
      _wtM = wtI.release()  ;
#endif


      //  ============ now fill the global surface vectors ==========================

      double ua[3], va[3], na[3], oa[3] ;

      _wtM->LocalToMasterVect( _volSurf.u()      , ua ) ;
      _wtM->LocalToMasterVect( _volSurf.v()      , va ) ;
      _wtM->LocalToMasterVect( _volSurf.normal() , na ) ;
      _wtM->LocalToMaster    ( _volSurf.origin() , oa ) ;

      _u.fill( ua ) ;
      _v.fill( va ) ;
      _n.fill( na ) ;
      _o.fill( oa ) ;

      // std::cout << " --- global surface vectors : ------- " << std::endl 
      // 		<< "    u : " << _u << std::endl 
      // 		<< "    v : " << _v << std::endl 
      // 		<< "    n : " << _n << std::endl 
      // 		<< "    o : " << _o << std::endl ;
      

      //  =========== check parallel and orthogonal to Z ===================
      
      _type.checkParallelToZ( *this ) ;

      _type.checkOrthogonalToZ( *this ) ;
    
      
     //======== set the unique surface ID from the DetElement ( and placements below ? )

      // just use the DetElement ID for now ...
      _id = _det.volumeID() ;

      // typedef PlacedVolume::VolIDs IDV ;
      // DetElement d = _det ;
      // while( d.isValid() &&  d.parent().isValid() ){
      // 	PlacedVolume pv = d.placement() ;
      // 	if( pv.isValid() ){
      // 	  const IDV& idV = pv.volIDs() ; 
      // 	  std::cout	<< " VolIDs : " << d.name() << std::endl ;
      // 	  for( unsigned i=0, n=idV.size() ; i<n ; ++i){
      // 	    std::cout  << "  " << idV[i].first << " - " << idV[i].second << std::endl ;
      // 	  }
      // 	}
      // 	d = d.parent() ;
      // }
     
    }
    //===================================================================================================================
      

  } // namespace
} // namespace
