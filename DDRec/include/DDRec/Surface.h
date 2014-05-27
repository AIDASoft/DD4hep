#ifndef DDRec_Surface_H
#define DDRec_Surface_H

#include "DD4hep/Objects.h"
#include "DD4hep/Volumes.h"
#include "DD4hep/Detector.h"

#include "DDSurfaces/ISurface.h"
#include "DDRec/Material.h"

#include <list>

class TGeoMatrix ;

namespace DD4hep {
  namespace DDRec {
  
    using namespace DDSurfaces ;
    
#if 1
    typedef MaterialData SurfaceMaterial ;

#else
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
#endif

    
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
    
      /// default c'tor.
      SurfaceData();
    
      /// Standard c'tor for initialization.
      SurfaceData( SurfaceType type, double thickness_inner ,double thickness_outer, 
		   Vector3D u ,Vector3D v ,Vector3D n ,Vector3D o )  ;
    
      /// Default destructor
      virtual ~SurfaceData() {} 
    
      /// Copy the from object
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
    
      /// setter for daughter classes
      virtual void setU(const Vector3D& u) { object<SurfaceData>()._u = u  ; }
      
      /// setter for daughter classes
      virtual void setV(const Vector3D& v) { object<SurfaceData>()._v = v ; }

      /// setter for daughter classes
      virtual void setNormal(const Vector3D& n) { object<SurfaceData>()._n = n ; }


    public:
    
      virtual ~VolSurface() {} 

      ///default c'tor
      VolSurface() { }
     
      /// Standrad c'tor for initialization.
      VolSurface( Geometry::Volume vol, SurfaceType type, double thickness_inner ,double thickness_outer, 
		  Vector3D u ,Vector3D v ,Vector3D n , Vector3D o = Vector3D(0.,0.,0.) ) ;      
    
      /// the volume to which this surface is attached.
      Geometry::Volume volume() const { return _vol ; }

      /// The id of this surface - always 0 for VolSurfaces
      virtual long64 id() const  { return 0 ; } 

     /** properties of the surface encoded in Type.
       * @see SurfaceType
       */
      virtual const SurfaceType& type() const { return object<SurfaceData>()._type ; }
    
      //==== geometry ====
      
      /** First direction of measurement U */
      virtual Vector3D u( const Vector3D& point = Vector3D() ) const {  point.x() ; return object<SurfaceData>()._u ; }
    
      /** Second direction of measurement V */
      virtual Vector3D v(const Vector3D& point = Vector3D() ) const { point.x() ;  return object<SurfaceData>()._v ; }
    
      /// Access to the normal direction at the given point
      virtual Vector3D normal(const Vector3D& point = Vector3D() ) const {  point.x() ; return object<SurfaceData>()._n ; }
    
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

      // required c'tor for extension mechanism
      VolSurfaceList(Geometry::DetElement& det){
	// det.addExtension<VolSurfaceList>( this ) ;
      }


      // required c'tor for extension mechanism
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
  
    /** Helper function for accessing the list assigned to a DetElement - attaches
     * empty list if needed.
     */
    VolSurfaceList* volSurfaceList( Geometry::DetElement& det ) ;

    //======================================================================================================

    /** Implementation of planar surface attached to a volume 
     * @author F.Gaede, DESY
     * @date Apr, 6 2014
     * @version $Id$
     */
    class VolPlane : public VolSurface {
      
    public:
      
      ///default c'tor
      VolPlane() : VolSurface() { }

      /// copy c'tor
      VolPlane(const VolSurface& vs ) : VolSurface( vs ) { }
      
      /// standard c'tor with all necessary arguments - origin is (0,0,0) if not given.
      VolPlane( Geometry::Volume vol, SurfaceType type, double thickness_inner ,double thickness_outer, 
		Vector3D u ,Vector3D v ,Vector3D n , Vector3D o = Vector3D(0.,0.,0.) ) :
	
	VolSurface( vol, type, thickness_inner, thickness_outer, u,v,n,o ) {

	object<SurfaceData>()._type.setProperty( SurfaceType::Plane    , true ) ;
	object<SurfaceData>()._type.setProperty( SurfaceType::Cylinder , false ) ;
	object<SurfaceData>()._type.checkParallelToZ( *this ) ;
	object<SurfaceData>()._type.checkOrthogonalToZ( *this ) ;

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
      
      /// default c'tor
      VolCylinder() : VolSurface() { }

      /// copy c'tor
      VolCylinder(const VolSurface& vs ) : VolSurface( vs ) { }
      
      /** The standard constructor. The origin vector points to the origin of the coordinate system on the cylinder,
       *  its rho defining the radius of the cylinder. The measurement direction u is set to be (0,0,1), the normal is
       *  chosen to be parallel to the origin vector and v = n X u. 
       */
      VolCylinder( Geometry::Volume vol, SurfaceType type, double thickness_inner ,double thickness_outer,  Vector3D origin ) ;


      //fg: for now we don't allow to set u and v for cylinders 
      // VolCylinder( Geometry::Volume vol, SurfaceType type, double thickness_inner ,double thickness_outer, 
      // 		Vector3D u ,Vector3D v ,Vector3D n , Vector3D o = Vector3D(0.,0.,0.) ) :	
      // 	VolSurface( vol, type, thickness_inner, thickness_outer, u,v,n,o ) {
      // 	object<SurfaceData>()._type.setProperty( SurfaceType::Plane    , false ) ;
      // 	object<SurfaceData>()._type.setProperty( SurfaceType::Cylinder , true ) ;
      // }      
      

      /** First direction of measurement U - rotated to point projected onto the cylinder.
       *  No check is done whether the point actually is on the cylinder surface
       */
      virtual Vector3D u( const Vector3D& point = Vector3D() ) const ;
    
      /** Second direction  of measurement V - rotated to point projected onto the cylinder.
       *  No check is done whether the point actually is on the cylinder surface
       */
      virtual Vector3D v(const Vector3D& point = Vector3D() ) const ;
    
      /** The normal direction at the given point, projected  onto the cylinder.
       *  No check is done whether the point actually is on the cylinder surface
       */
      virtual Vector3D normal(const Vector3D& point = Vector3D() ) const ;

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

      /// default c'tor
      Surface() :_det( Geometry::DetElement() ), _volSurf( VolSurface() ), _wtM( 0 ) , _id( 0)  { }

    public:
    
      virtual ~Surface() {} 

      /** Standard c'tor initializes the surface from the parameters of the VolSurface and the 
       *  transform (placement) of the corresponding volume, if found in DetElement 
       */
      Surface( Geometry::DetElement det, VolSurface volSurf ) ;      
    
      /// The id of this surface - corresponds to DetElement id.
      virtual long64 id() const { return _id ; }

      /** properties of the surface encoded in Type.
       * @see SurfaceType
       */
      virtual const SurfaceType& type() const { return _type ; }
    
      /// The volume that has the surface attached.
      Geometry::Volume volume() const { return _volSurf.volume()  ; }

      /// The VolSurface attched to the volume.
      VolSurface volSurface() const { return _volSurf ; }


      //==== geometry ====
      
      /** First direction of measurement U */
      virtual Vector3D u( const Vector3D& point = Vector3D() ) const { point.x() ; return _u ; }
    
      /** Second direction of measurement V */
      virtual Vector3D v(const Vector3D& point = Vector3D() ) const {  point.x() ; return _v ; }
    
      /// Access to the normal direction at the given point
      virtual  Vector3D normal(const Vector3D& point = Vector3D() ) const {  point.x() ; return _n ; }
    
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

    /** Specialization of Surface for cylinders. Provides acces to the cylinder radius and
     *  implements the access to the rotated surface vectors for points on the cylinder.
     *  @author F.Gaede, DESY
     *  @date May, 10 2014
     */
    class CylinderSurface:  public Surface, public ICylinder {

    public:

      ///Standard c'tor.
      CylinderSurface( Geometry::DetElement det, VolSurface volSurf ) : Surface( det, volSurf ) { }      
      
      /** First direction of measurement U - rotated to point projected onto the cylinder.
       *  No check is done whether the point actually is on the cylinder surface
       */
      virtual Vector3D u( const Vector3D& point = Vector3D() ) const ;
    
      /** Second direction of measurement V - rotated to point projected onto the cylinder.
       *  No check is done whether the point actually is on the cylinder surface
       */
      virtual Vector3D v(const Vector3D& point = Vector3D() ) const ;
    
      /** The normal direction at the given point - rotated to point projected onto the cylinder.
       *  No check is done whether the point actually is on the cylinder surface
       */
      virtual Vector3D normal(const Vector3D& point = Vector3D() ) const ;

      /// the radius of the cylinder (rho of the origin vector)
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
      /// defaul c'tor - allow to set ownership for surfaces
      SurfaceList(bool isOwner=false ) : _isOwner( isOwner )  {}

      /// copy c'tor
      SurfaceList(const SurfaceList& other ) : std::list< Surface* >( other ), _isOwner( false ){}

      /// required c'tor for extension mechanism
      SurfaceList(const Geometry::DetElement& ){
	// anything to do here  ?
      }
      /// required c'tor for extension mechanism
      SurfaceList(const SurfaceList& ,const Geometry::DetElement& ){
	// anything to do here  ?
      }
    
      /// d'tor deletes all owned surfaces
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
