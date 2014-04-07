#include "DDRec/Surface.h"
#include "DD4hep/Detector.h"

#include <math.h>
#include <exception>

#include "TGeoMatrix.h"

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


    VolSurfaceList* volSurfaceList( DetElement& det ) {

      
      VolSurfaceList* list = 0 ;

      try {

	list = det.extension< VolSurfaceList >() ;

      } catch( std::runtime_error e){ 
	
	list = det.addExtension<VolSurfaceList >(  new VolSurfaceList ) ; 
      }

      return list ;
    }


    //====================

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
	Volume vol = pv.volume();
	
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


    Surface::Surface( Geometry::DetElement det, VolSurface volSurf ) : _det( det) , _volSurf( volSurf ), _wtM(0){

      initialize() ;
    }      
 

    IMaterial Surface::innerMaterial() const {

    }
      

    IMaterial Surface::outerMaterial() const {

    }          

    double Surface::distance(const Vector3D& point ) const {

    }
      
    bool Surface::insideBounds(const Vector3D& point, double epsilon) const {

    }

    void Surface::initialize() {
      
      // first we need to find the right volume for the local surface in the DetElement's volumes
      std::list< PlacedVolume > pVList ;
      PlacedVolume pv = _det.placement() ;
      Volume theVol = _volSurf.volume() ;
      
      if( ! findVolume(  pv, theVol , pVList ) ){
	
	throw std::runtime_error( " ***** ERROR: No Volume found for DetElement with surface " ) ;
      } 

      // //-----------------------------------
      // if (nodes.size() < 2) {
      // 	return new TGeoHMatrix(*gGeoIdentity);
      // }
      // auto_ptr<TGeoHMatrix> mat(new TGeoHMatrix(*gGeoIdentity));
      // for (size_t i = 0, n=nodes.size(); n>0 && i < n-1; ++i)  {
      // 	const PlacedVolume& p = nodes[i];
      // 	TGeoMatrix* m = p->GetMatrix();
      // 	mat->MultiplyLeft(m);
      // }
      // if ( inverse )  {
      // 	auto_ptr<TGeoHMatrix> inv(new TGeoHMatrix(mat->Inverse()));
      // 	mat = inv;
      // }
      // return mat.release();
      //--------------------------------------
      
      std::cout << " **** Surface::initialize() # placements for surface = " << pVList.size() 
		<< " worldTransform : " 
		<< std::endl ; 
      

      //=========== compute and cache world transform for surface ==========

      TGeoMatrix* wm = _det.object<DetElement::Object>().worldTransformation() ;

#if 1 // debug
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

      //---- if the volSurface is not in the DetElement's volume, we left mutliply the path to the volume to the world transform
      for( std::list<PlacedVolume>::iterator it = ++( pVList.begin() ) , n = pVList.end() ; it != n ; ++it ){

      	PlacedVolume pv = *it ;
      	TGeoMatrix* m = pv->GetMatrix();
      	// std::cout << "  +++ matrix for placed volume : " << std::endl ;
      	// m->Print() ;

      	wtI->MultiplyLeft( m );
      }
      std::auto_ptr<TGeoHMatrix> wt( new TGeoHMatrix( wtI->Inverse() ) ) ;

      std::cout << "  +++ new world transform matrix  : " << std::endl ;
      wt->Print() ;

      // cache the world transform for the surface
      _wtM = wt.release()  ;


      //  ============ now fill the global surface vectors ==========================

      // void      TGeoMatrix::MasterToLocal(const Double_t *master, Double_t *local)
      // void      TGeoMatrix::LocalToMaster(const Double_t *local, Double_t *master)
      // void      TGeoMatrix::MasterToLocalVect(const Double_t *master, Double_t *local)
      // void      TGeoMatrix::LocalToMasterVect(const Double_t *local, Double_t *master)

      double ua[3], va[3], na[3], oa[3] ;
      // double um[3], vm[3], nm[3], om[3] ;
      // double ul[3], vl[3], nl[3], ol[3] ;
      
      const Vector3D& u =  _volSurf.u()  ;
      const Vector3D& v =  _volSurf.v()  ;
      const Vector3D& n =  _volSurf.normal()  ;
      const Vector3D& o =  _volSurf.origin()  ;

      _wtM->LocalToMasterVect( u , ua ) ;
      _wtM->LocalToMasterVect( v , va ) ;
      _wtM->LocalToMasterVect( n , na ) ;
      _wtM->LocalToMaster(     o , oa ) ;

      
     
      _u = Vector3D( ua ) ;
      _v = Vector3D( va ) ;
      _n = Vector3D( na ) ;
      _o = Vector3D( oa ) ;

      std::cout << " --- global surface vectors : ------- " << std::endl 
      		<< "    u : " << _u << std::endl 
      		<< "    v : " << _v << std::endl 
      		<< "    n : " << _n << std::endl 
      		<< "    o : " << _o << std::endl ;
      

      
    }


  } // namespace
} // namespace
