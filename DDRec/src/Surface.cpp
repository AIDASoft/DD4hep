#include "DDRec/Surface.h"
#include "DD4hep/objects/DetectorInterna.h"

#include <math.h>
#include <memory>
#include <exception>
#include <memory> 

#include "TGeoMatrix.h"
#include "TGeoShape.h"
#include "TRotation.h"

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
      
      Geometry::Handle< SurfaceData >( new SurfaceData( type, thickness_inner ,thickness_outer, u,v,n,o) ) ,

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
      //  	std::cout << " **" ;
      // }
      // std::cout << " searching for volume: " << theVol.name() << " " << std::hex << theVol.ptr() << "  <-> pv.volume : "  << pv.name() << " " <<  pv.volume().ptr() 
      //  		<< " pv.volume().ptr() == theVol.ptr() " <<  (pv.volume().ptr() == theVol.ptr() )
      //  		<< std::endl ;
      

      if( pv.volume().ptr() == theVol.ptr() ) { 
	
	return true ;
	
      } else {
	
	//--------------------------------

	const TGeoNode* node = pv.ptr();
	
	if ( !node ) {
	  
	  //	  std::cout <<  " *** findVolume: Invalid  placement:  - node pointer Null for volume:  " << pv.name() << std::endl ;

	  throw std::runtime_error("*** findVolume: Invalid  placement:  - node pointer Null ! " + std::string( pv.name()  ) );
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

      const TGeoHMatrix& wm = _det.worldTransformation() ;

#if 0 // debug
      wm.Print() ;
      for( std::list<PlacedVolume>::iterator it= pVList.begin(), n = pVList.end() ; it != n ; ++it ){
      PlacedVolume pv = *it ;
      TGeoMatrix* m = pv->GetMatrix();
      std::cout << "  +++ matrix for placed volume : " << std::endl ;
      m->Print() ;
      }
#endif

      // need to get the inverse transformation ( see Detector.cpp )
      std::auto_ptr<TGeoHMatrix> wtI( new TGeoHMatrix( wm.Inverse() ) ) ;

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

       // std::cout << " --- local and global surface vectors : ------- " << std::endl 
       // 			<< "    u : " << _volSurf.u()       << "  -  " << _u << std::endl 
       // 			<< "    v : " << _volSurf.v()       << "  -  " << _v << std::endl 
       // 			<< "    n : " << _volSurf.normal()  << "  -  " << _n << std::endl 
       // 			<< "    o : " << _volSurf.origin()  << "  -  " << _o << std::endl ;
      

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
      
    std::vector< std::pair<Vector3D, Vector3D> > Surface::getLines(unsigned nMax) {


      const static double epsilon = 1e-6 ; 

      std::vector< std::pair<Vector3D, Vector3D> > lines ;

	
      // get local and global surface vectors
      const DDSurfaces::Vector3D& lu = _volSurf.u() ;
      const DDSurfaces::Vector3D& lv = _volSurf.v() ;
      const DDSurfaces::Vector3D& ln = _volSurf.normal() ;
      const DDSurfaces::Vector3D& lo = _volSurf.origin() ;
      
      Volume vol = volume() ; 
      const TGeoShape* shape = vol->GetShape() ;
      
      
      if( type().isPlane() ) {
	
	if( shape->IsA() == TGeoBBox::Class() ) {
	  
	  TGeoBBox* box = ( TGeoBBox* ) shape  ;
	  
	  DDSurfaces::Vector3D boxDim(  box->GetDX() , box->GetDY() , box->GetDZ() ) ;  
	  
	  
	  bool isYZ = std::fabs(  ln.x() - 1.0 ) < epsilon  ; // normal parallel to x
	  bool isXZ = std::fabs(  ln.y() - 1.0 ) < epsilon  ; // normal parallel to y
	  bool isXY = std::fabs(  ln.z() - 1.0 ) < epsilon  ; // normal parallel to z
	  
	  
	  if( isYZ || isXZ || isXY ) {  // plane is parallel to one of the box' sides -> need 4 vertices from box dimensions
	    
	    // if isYZ :
	    unsigned uidx = 1 ;
	    unsigned vidx = 2 ;
	    
	    DDSurfaces::Vector3D ubl(  0., 1., 0. ) ; 
	    DDSurfaces::Vector3D vbl(  0., 0., 1. ) ;
	    
	    if( isXZ ) {
	      
	      ubl.fill( 1., 0., 0. ) ;
	      vbl.fill( 0., 0., 1. ) ;
	      uidx = 0 ;
	      vidx = 2 ;
	      
	    } else if( isXY ) {
	      
	      ubl.fill( 1., 0., 0. ) ;
	      vbl.fill( 0., 1., 0. ) ;
	      uidx = 0 ;
	      vidx = 1 ;
	    }
	    
	    DDSurfaces::Vector3D ub ;
	    DDSurfaces::Vector3D vb ;
	    _wtM->LocalToMasterVect( ubl , ub.array() ) ;
	    _wtM->LocalToMasterVect( vbl , vb.array() ) ;
	    
	    lines.reserve(4) ;
	    
	    lines.push_back( std::make_pair( _o + boxDim[ uidx ] * ub  + boxDim[ vidx ] * vb ,  _o - boxDim[ uidx ] * ub  + boxDim[ vidx ] * vb ) ) ;
	    lines.push_back( std::make_pair( _o - boxDim[ uidx ] * ub  + boxDim[ vidx ] * vb ,  _o - boxDim[ uidx ] * ub  - boxDim[ vidx ] * vb ) ) ;
	    lines.push_back( std::make_pair( _o - boxDim[ uidx ] * ub  - boxDim[ vidx ] * vb ,  _o + boxDim[ uidx ] * ub  - boxDim[ vidx ] * vb ) ) ;
	    lines.push_back( std::make_pair( _o + boxDim[ uidx ] * ub  - boxDim[ vidx ] * vb ,  _o + boxDim[ uidx ] * ub  + boxDim[ vidx ] * vb ) ) ;
	    
	    return lines ;
	  }	    

	} else if( shape->IsA() == TGeoConeSeg::Class() ) {

	  TGeoCone* cone = ( TGeoCone* ) shape  ;

	  // can only deal with special case of z-disk and origin in center of cone
	  if( type().isZDisk() && lo.rho() < epsilon ) {
	    
	    double zhalf = cone->GetDZ() ;
	    double rmax1 = cone->GetRmax1() ;
	    double rmax2 = cone->GetRmax2() ;
	    double rmin1 = cone->GetRmin1() ;
	    double rmin2 = cone->GetRmin2() ;
	    
	    // two circles around origin 
	    // get radii at position of plane 
	    double r0 =  rmin1 +  ( rmin2 - rmin1 ) / ( 2. * zhalf )   *  ( zhalf + lo.z()  ) ;  
	    double r1 =  rmax1 +  ( rmax2 - rmax1 ) / ( 2. * zhalf )   *  ( zhalf + lo.z()  ) ;  

	    
	    unsigned n = nMax / 4 ;
	    double dPhi = 2.* ROOT::Math::Pi() / double( n ) ; 
	    
	    for( unsigned i = 0 ; i < n ; ++i ) {
	      
	      Vector3D rv00(  r0*sin(  i   *dPhi ) , r0*cos(  i   *dPhi )  , 0. ) ;
	      Vector3D rv01(  r0*sin( (i+1)*dPhi ) , r0*cos( (i+1)*dPhi )  , 0. ) ;

	      Vector3D rv10(  r1*sin(  i   *dPhi ) , r1*cos(  i   *dPhi )  , 0. ) ;
	      Vector3D rv11(  r1*sin( (i+1)*dPhi ) , r1*cos( (i+1)*dPhi )  , 0. ) ;
	      
	     
	      Vector3D pl0 =  lo + rv00 ;
	      Vector3D pl1 =  lo + rv01 ;

	      Vector3D pl2 =  lo + rv10 ;
	      Vector3D pl3 =  lo + rv11 ;
	      

	      Vector3D pg0,pg1,pg2,pg3 ;
	      
	      _wtM->LocalToMaster( pl0, pg0.array() ) ;
	      _wtM->LocalToMaster( pl1, pg1.array() ) ;
	      _wtM->LocalToMaster( pl2, pg2.array() ) ;
	      _wtM->LocalToMaster( pl3, pg3.array() ) ;
	      
	      lines.push_back( std::make_pair( pg0, pg1 ) ) ;
	      lines.push_back( std::make_pair( pg2, pg3 ) ) ;
	    }

	    //add some vertical and horizontal lines so that the disc is seen in the rho-z projection

	    n = 4 ; dPhi = 2.* ROOT::Math::Pi() / double( n ) ;

	    for( unsigned i = 0 ; i < n ; ++i ) {
	      
	      Vector3D rv0(  r0*sin( i * dPhi ) , r0*cos(  i * dPhi )  , 0. ) ;
	      Vector3D rv1(  r1*sin( i * dPhi ) , r1*cos(  i * dPhi )  , 0. ) ;
	      
	      Vector3D pl0 =  lo + rv0 ;
	      Vector3D pl1 =  lo + rv1 ;

	      Vector3D pg0,pg1 ;
	      
	      _wtM->LocalToMaster( pl0, pg0.array() ) ;
	      _wtM->LocalToMaster( pl1, pg1.array() ) ;
	      
	      lines.push_back( std::make_pair( pg0, pg1 ) ) ;
	    }

	  }
	  
	  return lines ;
	}
	// ===== default for arbitrary planes in arbitrary shapes ================= 
	
	// We create nMax vertices by rotating the local u vector around the normal
	// and checking the distance to the volume boundary in that direction.
	// This is brute force and not very smart, as many points are created on straight 
	// lines and the edges are still rounded. 
	// The alterative would be to compute the true intersections a plane and the most
	// common shapes - at least for boxes that should be not too hard. To be done...
	
	lines.reserve( nMax ) ;
	
	double dAlpha =  2.* ROOT::Math::Pi() / double( nMax ) ; 

	TVector3 normal( ln.x() , ln.y() , ln.z() ) ;

	
	DDSurfaces::Vector3D first, previous ;

	for(unsigned i=0 ; i< nMax ; ++i ){ 
	  
	  double alpha = double(i) * dAlpha ;
	  
	  TVector3 vec( lu.x() , lu.y() , lu.z() ) ;

	  TRotation rot ;
	  rot.Rotate( alpha , normal );
	
	  TVector3 vecR = rot * vec ;
	  
	  DDSurfaces::Vector3D luRot ;
	  luRot.fill( vecR ) ;
 	  
	  double dist = shape->DistFromInside( lo, luRot , 3, 0.1 ) ;
	  
	  // local point at volume boundary
	  DDSurfaces::Vector3D lp = lo + dist * luRot ;

	  DDSurfaces::Vector3D gp ;
	  
	  _wtM->LocalToMaster( lp , gp.array() ) ;

	  // std::cout << " **** normal:" << ln << " lu:" << lu  << " alpha:" << alpha << " luRot:" << luRot << " lp :" << lp  << " gp:" << gp << " dist : " << dist 
	  // 	    << " is point " << gp << " inside : " << shape->Contains( gp )  
	  // 	    << " dist from outside for lo,lu " <<  shape->DistFromOutside( lo , lu  , 3 )    
	  // 	    << " dist from inside for lo,ln " <<  shape->DistFromInside( lo , ln  , 3 )    
	  // 	    << std::endl;
	  //	  shape->Dump() ;
	  

	  if( i >  0 ) 
	    lines.push_back( std::make_pair( previous, gp )  ) ;
	  else
	    first = gp ;

	  previous = gp ;
	}
	lines.push_back( std::make_pair( previous, first )  ) ;


      } else if( type().isCylinder() ) {  

	//	if( shape->IsA() == TGeoTube::Class() ) {
	if( shape->IsA() == TGeoConeSeg::Class() ) {

	  lines.reserve( nMax ) ;

	  TGeoTube* tube = ( TGeoTube* ) shape  ;
	  
	  double zHalf = tube->GetDZ() ;

	  Vector3D zv( 0. , 0. , zHalf ) ;
	  
	  double r = lo.rho() ;



	  unsigned n = nMax / 4 ;
	  double dPhi = 2.* ROOT::Math::Pi() / double( n ) ; 

	  for( unsigned i = 0 ; i < n ; ++i ) {

	    Vector3D rv0(  r*sin(  i   *dPhi ) , r*cos(  i   *dPhi )  , 0. ) ;
	    Vector3D rv1(  r*sin( (i+1)*dPhi ) , r*cos( (i+1)*dPhi )  , 0. ) ;

	    // 4 points on local cylinder

	    Vector3D pl0 =  zv + rv0 ;
	    Vector3D pl1 =  zv + rv1 ;
	    Vector3D pl2 = -zv + rv1  ;
	    Vector3D pl3 = -zv + rv0 ;

	    Vector3D pg0,pg1,pg2,pg3 ;

	    _wtM->LocalToMaster( pl0, pg0.array() ) ;
	    _wtM->LocalToMaster( pl1, pg1.array() ) ;
	    _wtM->LocalToMaster( pl2, pg2.array() ) ;
	    _wtM->LocalToMaster( pl3, pg3.array() ) ;

	    lines.push_back( std::make_pair( pg0, pg1 ) ) ;
	    lines.push_back( std::make_pair( pg1, pg2 ) ) ;
	    lines.push_back( std::make_pair( pg2, pg3 ) ) ;
	    lines.push_back( std::make_pair( pg3, pg0 ) ) ;
	  }
	}
      }
      return lines ;

    }

    // //===================================================================================================================
      
    // std::vector< Vector3D > Surface::getVertices(unsigned nMax) {


    //   const static double epsilon = 1e-6 ; 

    //   std::vector< Vector3D > _vert ;

	
    //   // get local and global surface vectors
    //   const DDSurfaces::Vector3D& lu = _volSurf.u() ;
    //   const DDSurfaces::Vector3D& lv = _volSurf.v() ;
    //   const DDSurfaces::Vector3D& ln = _volSurf.normal() ;
    //   const DDSurfaces::Vector3D& lo = _volSurf.origin() ;
      
    //   Volume vol = volume() ; 
    //   const TGeoShape* shape = vol->GetShape() ;
      
      
    //   if( type().isPlane() ) {
	
    // 	if( shape->IsA() == TGeoBBox::Class() ) {
	  
    // 	  TGeoBBox* box = ( TGeoBBox* ) shape  ;
	  
    // 	  DDSurfaces::Vector3D boxDim(  box->GetDX() , box->GetDY() , box->GetDZ() ) ;  
	  
	  
    // 	  bool isYZ = std::fabs(  ln.x() - 1.0 ) < epsilon  ; // normal parallel to x
    // 	  bool isXZ = std::fabs(  ln.y() - 1.0 ) < epsilon  ; // normal parallel to y
    // 	  bool isXY = std::fabs(  ln.z() - 1.0 ) < epsilon  ; // normal parallel to z
	  
	  
    // 	  if( isYZ || isXZ || isXY ) {  // plane is parallel to one of the box' sides -> need 4 vertices from box dimensions
	    
    // 	    // if isYZ :
    // 	    unsigned uidx = 1 ;
    // 	    unsigned vidx = 2 ;
	    
    // 	    DDSurfaces::Vector3D ubl(  0., 1., 0. ) ; 
    // 	    DDSurfaces::Vector3D vbl(  0., 0., 1. ) ;
	    
    // 	    if( isXZ ) {
	      
    // 	      ubl.fill( 1., 0., 0. ) ;
    // 	      vbl.fill( 0., 0., 1. ) ;
    // 	      uidx = 0 ;
    // 	      vidx = 2 ;
	      
    // 	    } else if( isXY ) {
	      
    // 	      ubl.fill( 1., 0., 0. ) ;
    // 	      vbl.fill( 0., 1., 0. ) ;
    // 	      uidx = 0 ;
    // 	      vidx = 1 ;
    // 	    }
	    
    // 	    DDSurfaces::Vector3D ub ;
    // 	    DDSurfaces::Vector3D vb ;
    // 	    _wtM->LocalToMasterVect( ubl , ub.array() ) ;
    // 	    _wtM->LocalToMasterVect( vbl , vb.array() ) ;
	    
    // 	    _vert.resize(4) ;
	    
    // 	    _vert[0] = _o + boxDim[ uidx ] * ub  + boxDim[ vidx ] * vb ;
    // 	    _vert[1] = _o - boxDim[ uidx ] * ub  + boxDim[ vidx ] * vb ;
    // 	    _vert[2] = _o - boxDim[ uidx ] * ub  - boxDim[ vidx ] * vb ;
    // 	    _vert[3] = _o + boxDim[ uidx ] * ub  - boxDim[ vidx ] * vb ;
	    
    // 	    return _vert ;
    // 	  }	    
    // 	}
	
    // 	// ===== default for arbitrary planes in arbitrary shapes ================= 
	
    // 	// We create nMax vertices by rotating the local u vector around the normal
    // 	// and checking the distance to the volume boundary in that direction.
    // 	// This is brute force and not very smart, as many points are created on straight 
    // 	// lines and the edges are still rounded. 
    // 	// The alterative would be to compute the true intersections a plane and the most
    // 	// common shapes - at least for boxes that should be not too hard. To be done...
	
    // 	_vert.resize( nMax ) ;
	
    // 	double dAlpha =  2.* ROOT::Math::Pi() / double( nMax ) ; 

    // 	TVector3 normal( ln.x() , ln.y() , ln.z() ) ;

    // 	for(unsigned i=0 ; i< nMax ; ++i ){ 
	  
    // 	  double alpha = double(i) * dAlpha ;
	  
    // 	  TVector3 vec( lu.x() , lu.y() , lu.z() ) ;

    // 	  TRotation rot ;
    // 	  rot.Rotate( alpha , normal );
	
    // 	  TVector3 vecR = rot * vec ;
	  
    // 	  DDSurfaces::Vector3D luRot ;
    // 	  luRot.fill( vecR ) ;
 	  
    // 	  double dist = shape->DistFromInside( lo, luRot , 3, 0.1 ) ;
	  
    // 	  // local point at volume boundary
    // 	  DDSurfaces::Vector3D lp = lo + dist * luRot ;

    // 	  DDSurfaces::Vector3D gp ;
	  
    // 	  _wtM->LocalToMaster( lp , gp.array() ) ;

    // 	  //	  std::cout << " **** normal:" << ln << " lu:" << lu  << " alpha:" << alpha << " luRot:" << luRot << " lp :" << lp  << " gp:" << gp << std::endl;

    // 	  _vert[i] = gp ;
    // 	}


    //   } else if( type().isCylinder() ) {  

    // 	//	if( shape->IsA() == TGeoTube::Class() ) {
    // 	if( shape->IsA() == TGeoConeSeg::Class() ) {

    // 	  _vert.resize( nMax ) ;

    // 	  TGeoTube* tube = ( TGeoTube* ) shape  ;
	  
    // 	  double zHalf = tube->GetDZ() ;

    // 	  Vector3D zv( 0. , 0. , zHalf ) ;
	  
    // 	  double r = lo.rho() ;

    // 	  unsigned n = nMax / 4 ;
    // 	  n = 8 ;

    // 	  double dPhi = 2.* ROOT::Math::Pi() / double( n ) ; 

    // 	  for( unsigned i = 0 ; i < n ; ++i ) {

    // 	    Vector3D rv0(  r*sin(  i   *dPhi ) , r*cos(  i   *dPhi )  , 0. ) ;
    // 	    Vector3D rv1(  r*sin( (i+1)*dPhi ) , r*cos( (i+1)*dPhi )  , 0. ) ;

    // 	    // 4 points on local cylinder

    // 	    Vector3D pl0 =  zv + rv0 ;
    // 	    Vector3D pl1 =  zv + rv1 ;
    // 	    Vector3D pl2 = -zv + rv1  ;
    // 	    Vector3D pl3 = -zv + rv0 ;

    // 	    Vector3D pg0,pg1,pg2,pg3 ;

    // 	    _wtM->LocalToMaster( pl0, pg0.array() ) ;
    // 	    _wtM->LocalToMaster( pl1, pg1.array() ) ;
    // 	    _wtM->LocalToMaster( pl2, pg2.array() ) ;
    // 	    _wtM->LocalToMaster( pl3, pg3.array() ) ;

    // 	    _vert[ i*5     ] = pg0 ;
    // 	    _vert[ i*5 + 1 ] = pg1 ;
    // 	    _vert[ i*5 + 2 ] = pg2 ;
    // 	    _vert[ i*5 + 3 ] = pg3 ;
    // 	    _vert[ i*5 + 3 ] = pg0 ;
	    
    // 	  }
    // 	}
    //   }
    //   return _vert ;

    // }

    //===================================================================================================================

    // double CylinderSurfaceSurface::radius() const {

    //   return _volSurf.origin().rho() ;
    // }

    // //===================================================================================================================

  } // namespace
} // namespace
