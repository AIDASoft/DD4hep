#ifndef DDRec_Surface_H
#define DDRec_Surface_H

#include "DD4hep/Objects.h"
#include "DD4hep/Volumes.h"
#include "DD4hep/Detector.h"

#include "DDSurfaces/ISurface.h"

#include <list>

class TGeoMatrix ;

namespace DD4hep {
  namespace DDRec {
  
    using namespace DDSurfaces ;
    

    // typedef DDRec::Material SurfaceMaterial ;

    /** Wrapper class to  Geometry::Material that implements the DDSurfaces::IMaterial interface.
     *
     * @author F.Gaede, DESY
     * @date Apr, 6 2014
     * @version $Id$
     */
    struct SurfaceMaterial : public virtual Geometry::Material ,  public IMaterial{

      /** Copy c'tor - copies handle */
      SurfaceMaterial( Geometry::Material m )  : Geometry::Material( m ) {}  

      SurfaceMaterial( const SurfaceMaterial& sm ) : Geometry::Material( sm ) {}  

      virtual ~SurfaceMaterial() {}

      /// material name
      virtual std::string name() const { return Geometry::Material::name() ; }

      /// averaged proton number
      virtual double Z() const {  return Geometry::Material::Z() ; } 
      
      /// averaged atomic number
      virtual double A() const { return Geometry::Material::A() ; } 
      
      /// density - units ?
      virtual double density() const {  return Geometry::Material::density() ; }
      
      /// radiation length - tgeo units 
      virtual double radiationLength() const { return Geometry::Material::radLength() ; } 
      
      /// interaction length - tgeo units 
      virtual double interactionLength() const  { return Geometry::Material::intLength() ; }

    };

    /** Helper class for holding surface data. 
     * @author F.Gaede, DESY
     * @date Apr, 6 2014
     * @version $Id$
     */
    struct SurfaceData{
    
      SurfaceType _type ;
      Vector3D _u ;
      Vector3D _v ;
      Vector3D _n ;
      Vector3D _o ;
      double _th_i ;
      double _th_o ;
      SurfaceMaterial _innerMat ;
      SurfaceMaterial _outerMat ;
    
      SurfaceData();
    
      SurfaceData( SurfaceType type, double thickness_inner ,double thickness_outer, 
		   Vector3D u ,Vector3D v ,Vector3D n ,Vector3D o )  ;
    
      /// Default destructor
      virtual ~SurfaceData() {} 
    
      /// Copy the object
    
      void copy(const SurfaceData& c) {
	_type = c._type ;
	_u = c._u ;
	_v = c._v ;
	_n = c._n ;
	_o = c._o;
	_th_i = c._th_i ;
	_th_o = c._th_o ;
	_innerMat = c._innerMat ;
	_outerMat = c._innerMat ;
      }
    } ;
  

    /** Implementation of ISurface for a local surface attached to a volume. 
     * 
     * @author F.Gaede, DESY
     * @date Apr, 6 2014
     * @version $Id$
     */
    class VolSurface : public Geometry::Handle< SurfaceData > , public ISurface {
    
    protected:
    
      Geometry::Volume _vol ;
    
    public:
    
      virtual ~VolSurface() {} 

      VolSurface() { }
    
      VolSurface( Geometry::Volume vol, SurfaceType type, double thickness_inner ,double thickness_outer, 
		  Vector3D u ,Vector3D v ,Vector3D n , Vector3D o = Vector3D(0.,0.,0.) ) ;      
    
      Geometry::Volume volume() const { return _vol ; }
    

      /// The id of this surface - always 0 for VolSurfaces
      virtual long64 id() const  { return 0 ; } 

     /** properties of the surface encoded in Type.
       * @see SurfaceType
       */
      virtual const SurfaceType& type() const { return object<SurfaceData>()._type ; }
    
      //==== geometry ====
      
      /** First direction of measurement U */
      virtual const Vector3D& u( const Vector3D& point = Vector3D() ) const {  point.x() ; return object<SurfaceData>()._u ; }
    
      /** Second direction of measurement V */
      virtual const Vector3D& v(const Vector3D& point = Vector3D() ) const { point.x() ;  return object<SurfaceData>()._v ; }
    
      /// Access to the normal direction at the given point
      virtual const Vector3D& normal(const Vector3D& point = Vector3D() ) const {  point.x() ; return object<SurfaceData>()._n ; }
    
      /** Get Origin of local coordinate system on surface */
      virtual const Vector3D& origin() const { return object<SurfaceData>()._o ;}

      /// Access to the material in opposite direction of the normal
      virtual const IMaterial& innerMaterial() const{  return  object<SurfaceData>()._innerMat ;  }

      /// Access to the material in direction of the normal
      virtual const IMaterial& outerMaterial() const { return  object<SurfaceData>()._outerMat  ; }
    
      /** Thickness of inner material */
      virtual double innerThickness() const { return object<SurfaceData>()._th_i ; }

      /** Thickness of outer material */
      virtual double outerThickness() const { return object<SurfaceData>()._th_o ; }


      // need default implementations for putting it in list....
      
      /** Distance to surface */
      virtual double distance(const Vector3D& point ) const  {  point.x() ; return 1.e99 ; }
      
      /// Checks if the given point lies within the surface
      virtual bool insideBounds(const Vector3D& point, double epsilon=1e-4 ) const {  point.x() ; (void) epsilon ; return false ; }


      //fixme: protected: + friend declaration ?

      /// set the inner Material
      void setInnerMaterial( Geometry::Material mat ){  object<SurfaceData>()._innerMat = mat ; }
      /// set the outer Materal
      void setOuterMaterial( Geometry::Material mat ){  object<SurfaceData>()._outerMat = mat ; }

    };


    //======================================================================================================


    /** std::list of VolSurfaces that takes ownership.
     * @author F.Gaede, DESY
     * @date Apr, 6 2014
     * @version $Id$
     */
    struct VolSurfaceList : std::list< VolSurface > {
    
      VolSurfaceList() {}

      // required c'tors for extension mechanism
      VolSurfaceList(Geometry::DetElement& det){
	// det.addExtension<VolSurfaceList>( this ) ;
      }

      VolSurfaceList(const VolSurfaceList& vsl, Geometry::DetElement& det ){
	
	//fixme: this causes a seg fault ...

	// VolSurfaceList* nL = new VolSurfaceList ;
	// nL->insert( nL->end() , vsl.begin() , vsl.end() ) ;
	// det.addExtension<VolSurfaceList>( nL ) ;
      }
    
      virtual ~VolSurfaceList(){
      
	// delete all surfaces attached to this volume
	// fixme: causes seg fault if same surfaces attached to more than one list
	//         -> how do we deal with this ?
	// for( VolSurfaceList::iterator i=begin(), n=end() ; i !=n ; ++i ) {
	//   delete (*i).ptr() ;
	// }
      
      }
    } ;
  
    VolSurfaceList* volSurfaceList( Geometry::DetElement& det ) ;

    //======================================================================================================

    /** Implementation of planar surface attached to a volume 
     * @author F.Gaede, DESY
     * @date Apr, 6 2014
     * @version $Id$
     */
    class VolPlane : public VolSurface {
      
    public:
      
      VolPlane() : VolSurface() { }

      VolPlane(const VolSurface& vs ) : VolSurface( vs ) { }
      
      VolPlane( Geometry::Volume vol, SurfaceType type, double thickness_inner ,double thickness_outer, 
		Vector3D u ,Vector3D v ,Vector3D n , Vector3D o = Vector3D(0.,0.,0.) ) :
	
	VolSurface( vol, type, thickness_inner, thickness_outer, u,v,n,o ) {

	object<SurfaceData>()._type.setProperty( SurfaceType::Plane    , true ) ;
	object<SurfaceData>()._type.setProperty( SurfaceType::Cylinder , false ) ;

      }      
      
      /** Distance to surface */
      virtual double distance(const Vector3D& point ) const  ;
      
      /// Checks if the given point lies within the surface
      virtual bool insideBounds(const Vector3D& point, double epsilon=1.e-4) const  ;

      
    } ;

    //======================================================================================================

    /** Implementation of cylindrical surface attached to a volume 
     * @author F.Gaede, DESY
     * @date Apr, 6 2014
     * @version $Id$
     */
    class VolCylinder : public VolSurface {
      
    public:
      
      VolCylinder() : VolSurface() { }

      VolCylinder(const VolSurface& vs ) : VolSurface( vs ) { }
      
      VolCylinder( Geometry::Volume vol, SurfaceType type, double thickness_inner ,double thickness_outer, 
		Vector3D u ,Vector3D v ,Vector3D n , Vector3D o = Vector3D(0.,0.,0.) ) :
	
	VolSurface( vol, type, thickness_inner, thickness_outer, u,v,n,o ) {

	object<SurfaceData>()._type.setProperty( SurfaceType::Plane    , false ) ;
	object<SurfaceData>()._type.setProperty( SurfaceType::Cylinder , true ) ;

      }      
      
      /** Distance to surface */
      virtual double distance(const Vector3D& point ) const  ;
      
      /// Checks if the given point lies within the surface
      virtual bool insideBounds(const Vector3D& point, double epsilon=1.e-4) const  ;

    } ;

    //======================================================================================================

    /** Implementation of Surface class holding a local surface attached to a volume and the DetElement 
     *  holding this surface.
     * 
     * @author F.Gaede, DESY
     * @date Apr, 7 2014
     * @version $Id$
     */
    class Surface:  public ISurface {
    
    protected:
      
      Geometry::DetElement _det ;
      VolSurface _volSurf ;
      TGeoMatrix* _wtM ; // matrix for world transformation of surface

      long64 _id ;

      SurfaceType _type ;
      Vector3D _u ;
      Vector3D _v ;
      Vector3D _n ;
      Vector3D _o ;

      Surface() :_det( Geometry::DetElement() ), _volSurf( VolSurface() ), _wtM( 0 ) , _id( 0)  { }

    public:
    
      virtual ~Surface() {} 

      Surface( Geometry::DetElement det, VolSurface volSurf ) ;      
    
      /// The id of this surface - corresponds to DetElement id ( or'ed with the placement ids )
      virtual long64 id() const { return _id ; }

      /** properties of the surface encoded in Type.
       * @see SurfaceType
       */
      virtual const SurfaceType& type() const { return _type ; }
    
      Geometry::Volume volume() const { return _volSurf.volume()  ; }

      VolSurface volSurface() const { return _volSurf ; }


      //==== geometry ====
      
      /** First direction of measurement U */
      virtual const Vector3D& u( const Vector3D& point = Vector3D() ) const { point.x() ; return _u ; }
    
      /** Second direction of measurement V */
      virtual const Vector3D& v(const Vector3D& point = Vector3D() ) const {  point.x() ; return _v ; }
    
      /// Access to the normal direction at the given point
      virtual const Vector3D& normal(const Vector3D& point = Vector3D() ) const {  point.x() ; return _n ; }
    
      /** Get Origin of local coordinate system on surface */
      virtual const Vector3D& origin() const { return _o ;}

      /** Thickness of inner material */
      virtual double innerThickness() const { return _volSurf.innerThickness() ; }

      /** Thickness of outer material */
      virtual double outerThickness() const { return _volSurf.outerThickness() ; }

      /// Access to the material in opposite direction of the normal
      virtual const IMaterial& innerMaterial() const ;
     
      /// Access to the material in direction of the normal
      virtual const IMaterial& outerMaterial() const ;
          
      /** Distance to surface */
      virtual double distance(const Vector3D& point ) const  ;
      
      /// Checks if the given point lies within the surface
      virtual bool insideBounds(const Vector3D& point, double epsilon=1.e-4) const ;

      //---------------------------------------------------
      /** Get vertices constraining the surface for drawing ( might not be exact boundaries) -
       *  at most nMax points are returned.
       */
      //      std::vector< Vector3D > getVertices( unsigned nMax=360 ) ;

      /** Get lines constraining the surface for drawing ( might not be exact boundaries) -
       *  at most nMax lines are returned.
       */
      std::vector< std::pair< Vector3D, Vector3D> > getLines(unsigned nMax=100) ;

    protected:
      void initialize() ;

    };

    //======================================================================================================

    class CylinderSurface:  public Surface, public ICylinder {

    public:

      CylinderSurface( Geometry::DetElement det, VolSurface volSurf ) : Surface( det, volSurf ) { }      
      
      virtual double radius() const {
	
	return _volSurf.origin().rho() ;
      }
    } ;

    //======================================================================================================
    /** std::list of Surfaces that optionally takes ownership.
     * @author F.Gaede, DESY
     * @date Apr, 10 2014
     * @version $Id$
     */
    class SurfaceList : public std::list< Surface* > {
    
    protected:
      bool _isOwner ;

    public:
      SurfaceList(bool isOwner=false ) : _isOwner( isOwner )  {}

      SurfaceList(const SurfaceList& other ) : std::list< Surface* >( other ), _isOwner( false ){}

      // required c'tors for extension mechanism
      SurfaceList(const Geometry::DetElement& ){
	// anything to do here  ?
      }
      SurfaceList(const SurfaceList& ,const Geometry::DetElement& ){
	// anything to do here  ?
      }
    
      virtual ~SurfaceList(){
      
	if( _isOwner ) {
	  // delete all surfaces attached to this volume
	  for( SurfaceList::iterator i=begin(), n=end() ; i !=n ; ++i ) {
	    delete (*i) ;
	  }
	}
      }

    } ;
  
    //    SurfaceList* surfaceList( Geometry::DetElement& det ) ;

    //======================================================================================================

  
  } /* namespace */
} /* namespace */

#endif /* Surface */
