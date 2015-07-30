#include "DDRec/Surface.h"
#include "DD4hep/objects/DetectorInterna.h"
#include "DD4hep/Memory.h"

#include "DDRec/MaterialManager.h"

#include <cmath>
#include <memory>
#include <exception>

#include "TGeoMatrix.h"
#include "TGeoShape.h"
#include "TRotation.h"

namespace DD4hep {
  namespace DDRec {
 
    using namespace Geometry ;


    //======================================================================================================
    
#if !use_materialdata
   
    std::string SurfaceMaterial::name() const { return Geometry::Material::name() ; }
    double SurfaceMaterial::Z() const {  return Geometry::Material::Z() ; } 
    double SurfaceMaterial::A() const { return Geometry::Material::A() ; } 
    double SurfaceMaterial::density() const {  return Geometry::Material::density() ; }
    double SurfaceMaterial::radiationLength() const { return Geometry::Material::radLength() ; } 
    double SurfaceMaterial::interactionLength() const  { return Geometry::Material::intLength() ; }
    
#endif    
    //======================================================================================================
    
    
    
    SurfaceData::SurfaceData() : _type( SurfaceType() ) ,
                                 _u( Vector3D() ) ,
                                 _v( Vector3D()  ) ,
                                 _n( Vector3D() ) ,
                                 _o( Vector3D() ) ,
                                 _th_i( 0. ),
                                 _th_o( 0. ),
                                 _innerMat( MaterialData() ),
                                 _outerMat( MaterialData() ),
                                 _vol()
    {
    }
  
  
    SurfaceData::SurfaceData( SurfaceType typ, double thickness_inner ,double thickness_outer, 
                              Vector3D u_val ,Vector3D v_val ,Vector3D n ,Vector3D o, Volume vol ) :  _type(typ ) ,
                                                                                                      _u( u_val ) ,
                                                                                                      _v( v_val ) ,
                                                                                                      _n( n ) ,
      _o( o ),
      _th_i( thickness_inner ),
      _th_o( thickness_outer ),  
      _innerMat( MaterialData() ),
      _outerMat( MaterialData() ),
      _vol(vol)
    {
    }
  
  
    //======================================================================================================
  

    VolSurface::VolSurface( Volume vol, SurfaceType typ, double thickness_inner ,double thickness_outer, 
                            Vector3D u_val ,Vector3D v_val ,Vector3D n ,Vector3D o ) :  
      
      Geometry::Handle< SurfaceData >( new SurfaceData(typ, thickness_inner ,thickness_outer, u_val,v_val,n,o, vol) )  {
    }      
    

    void VolSurface::setU(const Vector3D& u_val) { object<SurfaceData>()._u = u_val  ; }
    void VolSurface::setV(const Vector3D& v_val) { object<SurfaceData>()._v = v_val ; }
    void VolSurface::setNormal(const Vector3D& n) { object<SurfaceData>()._n = n ; }
    
    long64 VolSurface::id() const  { return 0 ; } 

    const SurfaceType& VolSurface::type() const { return object<SurfaceData>()._type ; }
    Vector3D VolSurface::u( const Vector3D& point ) const {  point.x() ; return object<SurfaceData>()._u ; }
    Vector3D VolSurface::v(const Vector3D& point  ) const { point.x() ;  return object<SurfaceData>()._v ; }
    Vector3D VolSurface::normal(const Vector3D& point ) const {  point.x() ; return object<SurfaceData>()._n ; }
    const Vector3D& VolSurface::origin() const { return object<SurfaceData>()._o ;}

    Vector2D VolSurface::globalToLocal( const Vector3D& point) const {

      Vector3D p = point - origin() ;

      // create new orthogonal unit vectors
      // FIXME: these vectors should be cached really ... 

      double uv = u() * v() ;
      Vector3D uprime = ( u() - uv * v() ).unit() ; 
      Vector3D vprime = ( v() - uv * u() ).unit() ; 
      double uup = u() * uprime ;
      double vvp = v() * vprime ;
      
      return  Vector2D(   p*uprime / uup ,  p*vprime / vvp ) ;
    }
    
    Vector3D VolSurface::localToGlobal( const Vector2D& point) const {

      Vector3D g = origin() + point[0] * u() + point[1] * v() ;

      return g ;
    }

    const IMaterial&  VolSurface::innerMaterial() const{  return  object<SurfaceData>()._innerMat ;  }
    const IMaterial&  VolSurface::outerMaterial() const { return  object<SurfaceData>()._outerMat  ; }
    double VolSurface::innerThickness() const { return object<SurfaceData>()._th_i ; }
    double VolSurface::outerThickness() const { return object<SurfaceData>()._th_o ; }
    

    double VolSurface::length_along_u() const {
      
      const DDSurfaces::Vector3D& o = this->origin() ;
      const DDSurfaces::Vector3D& u_val = this->u( o ) ;      
      DDSurfaces::Vector3D  um = -1. * u_val ;
      
      double dist_p = volume()->GetShape()->DistFromInside( const_cast<double*> ( o.const_array() ) , 
                                                            const_cast<double*> ( u_val.const_array() ) ) ;
      double dist_m = volume()->GetShape()->DistFromInside( const_cast<double*> ( o.const_array() ) , 
                                                            const_cast<double*> ( um.array()      ) ) ;
      return dist_p + dist_m ;
    }
    
    double VolSurface::length_along_v() const {

      const DDSurfaces::Vector3D& o = this->origin() ;
      const DDSurfaces::Vector3D& v_val = this->v( o ) ;      
      DDSurfaces::Vector3D  vm = -1. * v_val ;
      
      double dist_p = volume()->GetShape()->DistFromInside( const_cast<double*> ( o.const_array() ) , 
                                                            const_cast<double*> ( v_val.const_array() ) ) ;
      double dist_m = volume()->GetShape()->DistFromInside( const_cast<double*> ( o.const_array() ) , 
                                                            const_cast<double*> ( vm.array()      ) ) ;
      return dist_p + dist_m ;
    }
    

    double VolSurface::distance(const Vector3D& point ) const  {  point.x() ; return 1.e99 ; }
    
    bool VolSurface::insideBounds(const Vector3D& point, double epsilon) const {  
      point.x() ; (void) epsilon ; return false ; 
    }

    //======================================================================================================


    /** Distance to surface */
    double VolPlane::distance(const Vector3D& point ) const {

      return ( point - origin() ) *  normal()  ;
    }
    
    /// Checks if the given point lies within the surface
    bool VolPlane::insideBounds(const Vector3D& point, double epsilon) const {
      
#if 0
      double dist = std::abs ( distance( point ) ) ;
      
      bool inShape = volume()->GetShape()->Contains( point.const_array() ) ;
      
      std::cout << " ** Surface::insideBound( " << point << " ) - distance = " << dist 
                << " origin = " << origin() << " normal = " << normal() 
                << " p * n = " << point * normal() 
                << " isInShape : " << inShape << std::endl ;
	
      return dist < epsilon && inShape ;
#else
	
      //fixme: older versions of ROOT (~<5.34.10 ) take a non const pointer as argument - therefore use a const cast here for the time being ...
      return ( std::abs ( distance( point ) ) < epsilon )  &&  volume()->GetShape()->Contains( const_cast<double*> (point.const_array() )  ) ; 
#endif
 
    }


    //=============================================================================================================

    VolCylinder::VolCylinder( Geometry::Volume vol, SurfaceType typ, double thickness_inner ,double thickness_outer,  Vector3D o ) :

      VolSurface( vol, typ, thickness_inner, thickness_outer, Vector3D() , Vector3D() , Vector3D() , o ) {
    
      Vector3D v_val( 0., 0., 1. ) ;

      Vector3D o_rphi( o.x() , o.y() , 0. ) ;

      Vector3D n =  o_rphi.unit() ; 
    
      Vector3D u_val = v_val.cross( n ) ;

      setU( u_val ) ;
      setV( v_val ) ;
      setNormal( n ) ;

      object<SurfaceData>()._type.setProperty( SurfaceType::Plane    , false ) ;

      object<SurfaceData>()._type.setProperty( SurfaceType::Cylinder , true ) ;
 
      object<SurfaceData>()._type.checkParallelToZ( *this ) ;

      object<SurfaceData>()._type.checkOrthogonalToZ( *this ) ;
    }      


    Vector3D VolCylinder::v( const Vector3D& point  ) const { 
      // for now we just have v const as (0,0,1)
      point.x() ; return VolSurface::v() ; 
    }
    
    Vector3D VolCylinder::u(const Vector3D& point ) const {  

      Vector3D n( 1. , point.phi() , 0. , Vector3D::cylindrical ) ;

      // std::cout << " u : " << u()
      // 		<< " n : " << n 
      // 		<< " u X n :" << u().cross( n ) ;  
      return v().cross( n ) ; 
    }
    
    Vector3D VolCylinder::normal(const Vector3D& point ) const {  

      // normal is just given by phi of the point 
      return Vector3D( 1. , point.phi() , 0. , Vector3D::cylindrical ) ;
    }




    Vector2D VolCylinder::globalToLocal( const Vector3D& point) const {
      
      // cylinder is parallel to v here so u is Z and v is r *Phi
      double phi = point.phi() - origin().phi() ;
      
      while( phi < -M_PI ) phi += 2.*M_PI ;
      while( phi >  M_PI ) phi -= 2.*M_PI ;
 
      return  Vector2D( origin().rho() * phi, point.z() - origin().z() ) ;
    }
    
    
    Vector3D VolCylinder::localToGlobal( const Vector2D& point) const {

      double z = point.v() + origin().z() ;
      double phi = point.u() / origin().rho() + origin().phi() ;

      while( phi < -M_PI ) phi += 2.*M_PI ;
      while( phi >  M_PI ) phi -= 2.*M_PI ;

      return Vector3D( origin().rho() , phi, z  , Vector3D::cylindrical )    ;
    }


    /** Distance to surface */
    double VolCylinder::distance(const Vector3D& point ) const {

      return point.rho() - origin().rho()  ;
    }
    
    /// Checks if the given point lies within the surface
    bool VolCylinder::insideBounds(const Vector3D& point, double epsilon) const {
      
#if 0
      double distR = std::abs( distance( point ) ) ;
      
      bool inShapeT = volume()->GetShape()->Contains( const_cast<double*> ( point.const_array() ) ) ;
      
      std::cout << " ** Surface::insideBound( " << point << " ) - distance = " << distR 
                << " origin = " << origin() 
                << " isInShape : " << inShapeT << std::endl ;
      
      return distR < epsilon && inShapeT ;
#else
      
      return ( std::abs ( distance( point ) ) < epsilon )  &&  volume()->GetShape()->Contains( const_cast<double*> (point.const_array())  ) ; 

#endif
    }




    //================================================================================================================


    VolSurfaceList::~VolSurfaceList(){
      
      // delete all surfaces attached to this volume
      for( VolSurfaceList::iterator i=begin(), n=end() ; i !=n ; ++i ) {
        i->clear() ;
      }
      
    }
    //=======================================================

    SurfaceList::~SurfaceList(){
      
      if( _isOwner ) {
        // delete all surfaces attached to this volume
        for( SurfaceList::iterator i=begin(), n=end() ; i !=n ; ++i ) {
          delete (*i) ;
        }
      }
    }

    //================================================================================================================

    VolSurfaceList* volSurfaceList( DetElement& det ) {

      
      VolSurfaceList* list = 0 ;

      try {

        list = det.extension< VolSurfaceList >() ;

      } catch(const std::runtime_error& e){ 
	
        list = det.addExtension<VolSurfaceList >(  new VolSurfaceList ) ; 
      }

      return list ;
    }


    //======================================================================================================================

    bool findVolume( PlacedVolume pv,  Volume theVol, std::list< PlacedVolume >& volList ) {
      

      volList.push_back( pv ) ;
      
      //   unsigned count = volList.size() ;
      //   for(unsigned i=0 ; i < count ; ++i) {
      //   	std::cout << " **" ;
      //   }
      //   std::cout << " searching for volume: " << theVol.name() << " " << std::hex << theVol.ptr() << "  <-> pv.volume : "  << pv.name() << " " <<  pv.volume().ptr() 
      //    		<< " pv.volume().ptr() == theVol.ptr() " <<  (pv.volume().ptr() == theVol.ptr() )
      //    		<< std::endl ;
      

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

    //======================================================================================================================

    Surface::Surface( Geometry::DetElement det, VolSurface volSurf ) : _det( det) , _volSurf( volSurf ), 
                                                                       _wtM(0) , _id( 0) , _type( _volSurf.type() )  {

      initialize() ;
    }      
 
    long64 Surface::id() const { return _id ; }

    const SurfaceType& Surface::type() const { return _type ; }

    Vector3D Surface::u( const Vector3D& point ) const { point.x() ; return _u ; }
    Vector3D Surface::v(const Vector3D& point ) const {  point.x() ; return _v ; }
    Vector3D Surface::normal(const Vector3D& point ) const {  point.x() ; return _n ; }
    const Vector3D& Surface::origin() const { return _o ;}
    double Surface::innerThickness() const { return _volSurf.innerThickness() ; }
    double Surface::outerThickness() const { return _volSurf.outerThickness() ; }
    double Surface::length_along_u() const { return _volSurf.length_along_u() ; }
    double Surface::length_along_v() const { return _volSurf.length_along_v() ; }

    /** Thickness of outer material */
    
    const IMaterial& Surface::innerMaterial() const {
      
      SurfaceMaterial& mat = _volSurf->_innerMat ;
      
      //      std::cout << "  **** Surface::innerMaterial() " << mat << std::endl ;

      if( ! mat.isValid() ) {
	
        MaterialManager matMgr ;

        Vector3D p = _o - innerThickness() * _n  ;

        const MaterialVec& materials = matMgr.materialsBetween( _o , p  ) ;
	
        // std::cout << " ####### found materials between points : " << _o << " and " << p << " : " ;
        // for( unsigned i=0,n=materials.size();i<n;++i){
        //   std::cout <<  materials[i].first.name() << "[" <<   materials[i].second << "], " ;
        // }
        // std::cout << std::endl ;
        // const MaterialData& matAvg = matMgr.createAveragedMaterial( materials ) ; 
        // mat = matAvg ;
        // std::cout << "  **** Surface::innerMaterial() - assigning averaged material to surface : " << mat << std::endl ;

        mat = ( materials.size() > 1  ? matMgr.createAveragedMaterial( materials ) : materials[0].first  ) ;

      }
      return  mat ;
    }
      

    const IMaterial& Surface::outerMaterial() const {

      SurfaceMaterial& mat = _volSurf->_outerMat ;
      
      if( ! mat.isValid() ) {
	
        MaterialManager matMgr ;

        Vector3D p = _o + outerThickness() * _n  ;

        const MaterialVec& materials = matMgr.materialsBetween( _o , p  ) ;
	
        mat = ( materials.size() > 1  ? matMgr.createAveragedMaterial( materials ) : materials[0].first  ) ;

      }
      return  mat ;
    }          


    Vector2D Surface::globalToLocal( const Vector3D& point) const {

      Vector3D p = point - origin() ;

      // create new orthogonal unit vectors
      // FIXME: these vectors should be cached really ... 

      double uv = u() * v() ;
      Vector3D uprime = ( u() - uv * v() ).unit() ; 
      Vector3D vprime = ( v() - uv * u() ).unit() ; 
      double uup = u() * uprime ;
      double vvp = v() * vprime ;
      
      return  Vector2D(   p*uprime / uup ,  p*vprime / vvp ) ;
    }
    
    
    Vector3D Surface::localToGlobal( const Vector2D& point) const {

      Vector3D g = origin() + point[0] * u() + point[1] * v() ;
      return g ;
    }


    Vector3D Surface::volumeOrigin() const { 

      double o_array[3] ;

      _wtM->LocalToMaster    ( Vector3D() , o_array ) ;
      
      Vector3D o(o_array) ;
     
      return o ;
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
        theVol = _volSurf.volume() ;
        std::stringstream sst ; sst << " ***** ERROR: Volume " << theVol.name() << " not found for DetElement " << _det.name()  << " with surface "  ;
        throw std::runtime_error( sst.str() ) ;
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
      // std::auto_ptr<TGeoHMatrix> wtI( new TGeoHMatrix( wm.Inverse() ) ) ;
      // has been fixed now, no need to get the inverse anymore:
      dd4hep_ptr<TGeoHMatrix> wtI( new TGeoHMatrix( wm ) ) ;

      //---- if the volSurface is not in the DetElement's volume, we need to mutliply the path to the volume to the
      // DetElements world transform
      for( std::list<PlacedVolume>::iterator it = ++( pVList.begin() ) , n = pVList.end() ; it != n ; ++it ){

      	PlacedVolume pvol = *it ;
      	TGeoMatrix* m = pvol->GetMatrix();
      	// std::cout << "  +++ matrix for placed volume : " << std::endl ;
      	// m->Print() ;
        //wtI->MultiplyLeft( m );

      	wtI->Multiply( m );
      }

      //      std::cout << "  +++ new world transform matrix  : " << std::endl ;

#if 0 //fixme: which convention to use here - the correct should be wtI, however it is the inverse of what is stored in DetElement ???
      dd4hep_ptr<TGeoHMatrix> wt( new TGeoHMatrix( wtI->Inverse() ) ) ;
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
      //      const DDSurfaces::Vector3D& lv = _volSurf.v() ;
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

        TVector3 norm( ln.x() , ln.y() , ln.z() ) ;

	
        DDSurfaces::Vector3D first, previous ;

        for(unsigned i=0 ; i< nMax ; ++i ){ 
	  
          double alpha = double(i) * dAlpha ;
	  
          TVector3 vec( lu.x() , lu.y() , lu.z() ) ;

          TRotation rot ;
          rot.Rotate( alpha , norm );
	
          TVector3 vecR = rot * vec ;
	  
          DDSurfaces::Vector3D luRot ;
          luRot.fill( vecR ) ;
 	  
          double dist = shape->DistFromInside( const_cast<double*> (lo.const_array()) , const_cast<double*> (luRot.const_array())  , 3, 0.1 ) ;
	  
          // local point at volume boundary
          DDSurfaces::Vector3D lp = lo + dist * luRot ;

          DDSurfaces::Vector3D gp ;
	  
          _wtM->LocalToMaster( lp , gp.array() ) ;

          // std::cout << " **** normal:" << ln << " lu:" << lu  << " alpha:" << alpha << " luRot:" << luRot << " lp :" << lp  << " gp:" << gp << " dist : " << dist 
          // 	    << " is point " << gp << " inside : " << shape->Contains( gp.const_array()  )  
          // 	    << " dist from outside for lo,lu " <<  shape->DistFromOutside( lo.const_array()  , lu.const_array()   , 3 )    
          // 	    << " dist from inside for lo,ln " <<  shape->DistFromInside( lo.const_array()  , ln.const_array()   , 3 )    
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

    //================================================================================================================

    Vector3D CylinderSurface::u( const Vector3D& point  ) const { 

      Vector3D lp , u_val ;
      _wtM->MasterToLocal( point , lp.array() ) ;
      const DDSurfaces::Vector3D& lu = VolCylinder(_volSurf).u( lp  ) ;
      _wtM->LocalToMasterVect( lu , u_val.array() ) ;
      return u_val ; 
    }
    
    Vector3D CylinderSurface::v(const Vector3D& point ) const {  
      Vector3D lp , v_val ;
      _wtM->MasterToLocal( point , lp.array() ) ;
      const DDSurfaces::Vector3D& lv =  VolCylinder(_volSurf).v( lp  ) ;
      _wtM->LocalToMasterVect( lv , v_val.array() ) ;
      return v_val ; 
    }
    
    Vector3D CylinderSurface::normal(const Vector3D& point ) const {  
      Vector3D lp , n ;
      _wtM->MasterToLocal( point , lp.array() ) ;
      const DDSurfaces::Vector3D& ln =  VolCylinder(_volSurf).normal( lp  ) ;
      _wtM->LocalToMasterVect( ln , n.array() ) ;
      return n ; 
    }

    Vector2D CylinderSurface::globalToLocal( const Vector3D& point) const {
      
      Vector3D lp;
      _wtM->MasterToLocal( point , lp.array() ) ;

      return   VolCylinder(_volSurf).globalToLocal( lp )  ;
    }
    
    
    Vector3D CylinderSurface::localToGlobal( const Vector2D& point) const {

      Vector3D lp =  VolCylinder(_volSurf).localToGlobal( point ) ;
      Vector3D p ;
      _wtM->LocalToMaster( lp , p.array() ) ;

      return p ;
    }

    double CylinderSurface::radius() const {	return _volSurf.origin().rho() ;  }

    Vector3D CylinderSurface::center() const {	return volumeOrigin() ;  }

    //================================================================================================================




  } // namespace
} // namespace


#include "DD4hep/Handle.inl"
typedef DD4hep::DDRec::SurfaceData SurfaceData;
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(SurfaceData);
