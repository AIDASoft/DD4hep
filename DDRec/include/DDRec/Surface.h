#ifndef rec_Surface_H
#define rec_Surface_H

#include "DD4hep/Objects.h"
#include "DD4hep/Volumes.h"
#include "DD4hep/DetElement.h"

#include "DDRec/ISurface.h"
#include "DDRec/Material.h"

#include <list>
#include <memory>

class TGeoMatrix ;

namespace dd4hep {
  namespace rec {
  
    //-------------------------------------------------------------------------------------------

    class VolSurface  ;

    /** Implementation of ISurface for a local surface attached to a volume. 
     *  Provides default implementations for all methods but distance().
     *  Subclasses for specific surfaces overwrite methods as needed.
     * 
     *  @author F.Gaede, DESY
     *  @date Sep 11, 2015
     *  @version $Id$
     */
    class VolSurfaceBase : public ISurface {
      
      friend class VolSurface ;

    protected:
      SurfaceType _type ;
      Vector3D _u ;
      Vector3D _v ;
      Vector3D _n ;
      Vector3D _o ;
      double _th_i ;
      double _th_o ;
      MaterialData _innerMat ;
      MaterialData _outerMat ;    
      Volume _vol ;
      long64 _id ;
      unsigned _refCount ;

      /// setter for daughter classes
      virtual void setU(const Vector3D& u) ;
      /// setter for daughter classes
      virtual void setV(const Vector3D& v) ;
      /// setter for daughter classes
      virtual void setNormal(const Vector3D& n) ;
      /// setter for daughter classes
      virtual void setOrigin(const Vector3D& o) ;

    public:
    
      virtual ~VolSurfaceBase() {} 

      ///default c'tor

      VolSurfaceBase() : 
	_type( SurfaceType() ) ,
	_u( Vector3D() ) ,
	_v( Vector3D()  ) ,
	_n( Vector3D() ) ,
	_o( Vector3D() ) ,
	_th_i( 0. ),
	_th_o( 0. ),
	_innerMat( MaterialData() ),
	_outerMat( MaterialData() ),
	_vol(),
	_id(0),_refCount(0)  { 
      }
      
      
      VolSurfaceBase( SurfaceType typ, 
		      double thickness_inner ,double thickness_outer, 
		      Vector3D u_val ,Vector3D v_val ,
		      Vector3D n ,Vector3D o, Volume vol,int identifier ) : 
	_type(typ ) ,
	_u( u_val ) ,
	_v( v_val ) ,
	_n( n ) ,
	_o( o ),
	_th_i( thickness_inner ),
	_th_o( thickness_outer ),  
	_innerMat( MaterialData() ),
	_outerMat( MaterialData() ),
	_vol(vol) ,
	_id( identifier ), _refCount(0) {
      }
      
      
      /// Copy the from object
      VolSurfaceBase(const VolSurfaceBase& c) {
        _type = c._type ;
        _u = c._u ;
        _v = c._v ;
        _n = c._n ;
        _o = c._o;
        _th_i = c._th_i ;
        _th_o = c._th_o ;
        _innerMat = c._innerMat ;
        _outerMat = c._innerMat ;
        _vol = c._vol;
	_id = c._id ;
	_refCount = 0 ; // new instance
      }


      /// the volume to which this surface is attached.
      Volume volume() const { return _vol ; }

      /// The id of this surface 
      virtual long64 id() const  ;

      /** properties of the surface encoded in Type.
       * @see SurfaceType
       */
      virtual const SurfaceType& type() const ;
    
      //==== geometry ====
      
      /** First direction of measurement U */
      virtual Vector3D u( const Vector3D& point = Vector3D() ) const ;
    
      /** Second direction of measurement V */
      virtual Vector3D v(const Vector3D& point = Vector3D() ) const ;
    
      /// Access to the normal direction at the given point
      virtual Vector3D normal(const Vector3D& point = Vector3D() ) const ;
    
      /** Get Origin of local coordinate system on surface */
      virtual const Vector3D& origin() const ;
      
      /** Convert the global position to the local position (u,v) on the surface */
      virtual Vector2D globalToLocal( const Vector3D& point) const ;
      
      /** Convert the local position (u,v) on the surface to the global position */
      virtual Vector3D localToGlobal( const Vector2D& point) const ;
      
      /// Access to the material in opposite direction of the normal
      virtual const IMaterial& innerMaterial() const ;

      /// Access to the material in direction of the normal
      virtual const IMaterial& outerMaterial() const ;
    
      /** Thickness of inner material */
      virtual double innerThickness() const ;

      /** Thickness of outer material */
      virtual double outerThickness() const ;


      /** The length of the surface along direction u at the origin. For 'regular' boundaries, like rectangles, 
       *  this can be used to speed up the computation of inSideBounds.
       */
      virtual double length_along_u() const ;

      /** The length of the surface along direction v at the origin. For 'regular' boundaries, like rectangles, 
       *  this can be used to speed up the computation of inSideBounds.
       */
      virtual double length_along_v() const ;


      /** Distance to surface */
      virtual double distance(const Vector3D& point ) const ;
      
      /// Checks if the given point lies within the surface
      virtual bool insideBounds(const Vector3D& point, double epsilon=1e-4 ) const ;


      virtual std::vector< std::pair<Vector3D, Vector3D> > getLines(unsigned nMax=100) ;
 
      /// set the inner Material
      void setInnerMaterial( const IMaterial& mat ){ _innerMat = mat ; }

      /// set the outer Materal
      void setOuterMaterial( const IMaterial& mat ){ _outerMat = mat ; }

    };

    //---------------------------------------------------------------------------------------------
    /** Reference counted handle class for a local surface attached to a volume (VolSurfaceBase). 
     *
     * @author F.Gaede, DESY
     * @date Sep, 14 2015
     * @version $Id$
     */
    class VolSurface : public ISurface {
    
    protected:

      VolSurfaceBase* _surf ;

    public:
    
      virtual ~VolSurface(){
	if( _surf ) {
	  -- _surf->_refCount ;
	  if(  _surf->_refCount == 0 ) delete _surf ;
	}
      } 
      ///default c'tor
      VolSurface() : _surf(0) { }

      /// Constructor to be used with an existing object
      VolSurface(VolSurfaceBase* p) : _surf( p ) { ++ _surf->_refCount ; }

      /// Constructor to be used with an existing object
      VolSurface(const VolSurface& vsurf) : _surf( vsurf._surf ) {
	++ _surf->_refCount ;
      }
      
      VolSurface& operator=(const VolSurface& vsurf) {
	_surf = vsurf._surf ;
	++ _surf->_refCount ;
	return *this ;
      }
      

      /// the volume to which this surface is attached.
      Volume volume() const { return _surf->volume() ; }

      /// pointer to underlying object 
      VolSurfaceBase* ptr() const { return _surf ; }	
      
      /// The id of this surface - always 0 for VolSurfaces
      virtual long64 id() const  ;

      /** properties of the surface encoded in Type.
       * @see SurfaceType
       */
      virtual const SurfaceType& type() const ;
    
      //==== geometry ====
      
      /** First direction of measurement U */
      virtual Vector3D u( const Vector3D& point = Vector3D() ) const ;
    
      /** Second direction of measurement V */
      virtual Vector3D v(const Vector3D& point = Vector3D() ) const ;
    
      /// Access to the normal direction at the given point
      virtual Vector3D normal(const Vector3D& point = Vector3D() ) const ;
    
      /** Get Origin of local coordinate system on surface */
      virtual const Vector3D& origin() const ;
      
      /** Convert the global position to the local position (u,v) on the surface */
      virtual Vector2D globalToLocal( const Vector3D& point) const ;
      
      /** Convert the local position (u,v) on the surface to the global position */
      virtual Vector3D localToGlobal( const Vector2D& point) const ;
      
      /// Access to the material in opposite direction of the normal
      virtual const IMaterial& innerMaterial() const ;

      /// Access to the material in direction of the normal
      virtual const IMaterial& outerMaterial() const ;
    
      /** Thickness of inner material */
      virtual double innerThickness() const ;

      /** Thickness of outer material */
      virtual double outerThickness() const ;


      /** The length of the surface along direction u at the origin. For 'regular' boundaries, like rectangles, 
       *  this can be used to speed up the computation of inSideBounds.
       */
      virtual double length_along_u() const ;

      /** The length of the surface along direction v at the origin. For 'regular' boundaries, like rectangles, 
       *  this can be used to speed up the computation of inSideBounds.
       */
      virtual double length_along_v() const ;

      /** Distance to surface */
      virtual double distance(const Vector3D& point ) const ;
      
      /// Checks if the given point lies within the surface
      virtual bool insideBounds(const Vector3D& point, double epsilon=1e-4 ) const ;

      virtual std::vector< std::pair<Vector3D, Vector3D> > getLines(unsigned nMax=100) ;
 
      /// set the innerMaterial
      void setInnerMaterial( const IMaterial& mat ){ _surf->setInnerMaterial( mat ) ; }

      /// set the outer Materal
      void setOuterMaterial( const IMaterial& mat ){  _surf->setOuterMaterial( mat ) ; }

    };

    //======================================================================================================


    struct VolSurfaceList ;
    /** Helper function for accessing the list assigned to a DetElement - attaches
     * empty list if needed.
     */
    VolSurfaceList* volSurfaceList( DetElement& det) ;

    /** std::list of VolSurfaces that takes ownership.
     * @author F.Gaede, DESY
     * @date Apr, 6 2014
     * @version $Id$
     */
    struct VolSurfaceList : std::list< VolSurface > {
    
      VolSurfaceList() {}

      // required c'tor for extension mechanism
      VolSurfaceList(DetElement& det){

        VolSurfaceList* sL = volSurfaceList( det ) ; 

        std::copy( this->end() , sL->begin() , sL->end() ) ;
      }


      // required c'tor for extension mechanism
      VolSurfaceList(const VolSurfaceList& vsl, DetElement& /*det*/ ){
	
        this->insert( this->end() , vsl.begin() , vsl.end() ) ;
      }
    
      virtual ~VolSurfaceList() ;

    } ;
  

    //======================================================================================================

    /** Implementation of a planar surface attached to a volume 
     * @author F.Gaede, DESY
     * @date Sep, 14 2014
     * @version $Id$
     */
    class VolPlaneImpl : public VolSurfaceBase {
      
    public:
      
      ///default c'tor
      VolPlaneImpl() : VolSurfaceBase() { }

      /// standard c'tor with all necessary arguments - origin is (0,0,0) if not given.
      VolPlaneImpl( SurfaceType typ, double thickness_inner ,double thickness_outer, 
		    Vector3D u_val ,Vector3D v_val ,Vector3D n_val , Vector3D o_val, Volume vol, int id_val  ) :
	
      VolSurfaceBase( typ, thickness_inner, thickness_outer, u_val,v_val, n_val, o_val, vol, id_val ) {
	
        _type.setProperty( SurfaceType::Plane    , true ) ;
        _type.setProperty( SurfaceType::Cylinder , false ) ;
        _type.setProperty( SurfaceType::Cone     , false ) ;
        _type.checkParallelToZ( *this ) ;
        _type.checkOrthogonalToZ( *this ) ;
      }      
      
      /** Distance to surface */
      virtual double distance(const Vector3D& point ) const  ;
    } ;

    //======================================================================================================
    /** Implementation of cylindrical surface attached to a volume 
     * @author F.Gaede, DESY
     * @date Apr, 6 2014
     * @version $Id$
     */
    class VolCylinderImpl : public VolSurfaceBase {
      
    public:
      
      /// default c'tor
      VolCylinderImpl() : VolSurfaceBase() { }

      
      /** The standard constructor. The origin vector points to the origin of the coordinate system on the cylinder,
       *  its rho defining the radius of the cylinder. The measurement direction v is set to be (0,0,1), the normal is
       *  chosen to be parallel to the origin vector and u = n X v. 
       */
      VolCylinderImpl( Volume vol, SurfaceType type, double thickness_inner ,double thickness_outer,  Vector3D origin ) ;

      /** First direction of measurement U - rotated to point projected onto the cylinder.
       *  No check is done whether the point actually is on the cylinder surface
       */
      virtual Vector3D u( const Vector3D& point = Vector3D() ) const ;
      
      /** The normal direction at the given point, projected  onto the cylinder.
       *  No check is done whether the point actually is on the cylinder surface
       */
      virtual Vector3D normal(const Vector3D& point = Vector3D() ) const ;

      /** Distance to surface */
      virtual double distance(const Vector3D& point ) const  ;
      
      /** Convert the global position to the local position (u,v) on the surface - v runs along the axis of the cylinder, u is r*phi */
      virtual Vector2D globalToLocal( const Vector3D& point) const ;
      
      /** Convert the local position (u,v) on the surface to the global position  - v runs along the axis of the cylinder, u is r*phi*/
      virtual Vector3D localToGlobal( const Vector2D& point) const ;
    } ;

    //======================================================================================================
    /** Implementation of conical surface attached to a volume 
     * @author F.Gaede, DESY
     * @date Nov, 6 2015
     * @version $Id$
     */
    class VolConeImpl : public VolSurfaceBase {
      
      //internal helper variables
      double _ztip ;     // z position of the tip in the volume coordinate system
      double _zt0 ;      // z distance of the front face from the tip
      double _zt1 ;      // z distance of the back face from the tip
      double _tanTheta ; // tan of half the openeing angle 

    public:
      
      /// default c'tor
      VolConeImpl() : VolSurfaceBase() { }
      
      
      /** The standard constructor. The origin vector points to the origin of the coordinate system on the cone,
       *  its rho defining the mean radius of the cone (z-component of the origin is ignored !).
       *  The measurement direction v defines the opening angle of the cone,
       *  the normal is chosen to be orthogonal to v. NB: the cone is always parallel to the local z axis.
       */
      VolConeImpl( Volume vol, SurfaceType type, double thickness_inner ,double thickness_outer,
		   Vector3D v, Vector3D origin ) ;
      
      /** First direction of measurement U - rotated to point projected onto the cone.
       *  No check is done whether the point actually is on the cone surface
       */
      virtual Vector3D u( const Vector3D& point = Vector3D() ) const ;

      /** Second direction of measurement V - rotated to point projected onto the cone.
       *  No check is done whether the point actually is on the cone surface
       */
      virtual Vector3D v( const Vector3D& point = Vector3D() ) const ;
      
      /** The normal direction at the given point, projected  onto the cone.
       *  No check is done whether the point actually is on the cone surface
       */
      virtual Vector3D normal(const Vector3D& point = Vector3D() ) const ;

      /** Distance to surface */
      virtual double distance(const Vector3D& point ) const  ;
      
      /** Convert the global position to the local position (u,v) on the surface - v runs along the axis of the cone, u is r*phi */
      virtual Vector2D globalToLocal( const Vector3D& point) const ;
      
      /** Convert the local position (u,v) on the surface to the global position  - v runs along the axis of the cone, u is r*phi*/
      virtual Vector3D localToGlobal( const Vector2D& point) const ;

      virtual std::vector< std::pair<Vector3D, Vector3D> > getLines(unsigned nMax=100) ;
  } ;



    //======================================================================================================
    
    /** Template for VolSurface specializations.
     *  Works for surfaces that take all surface vectors (u,v,n,o) in the c'tor.
     * @author F.Gaede, DESY
     * @date Sep, 14 2015
     * @version $Id$
     */
    template <class T>
    class VolSurfaceHandle : public VolSurface {
      
    public:
      VolSurfaceHandle( Volume vol, SurfaceType typ, double thickness_inner ,double thickness_outer, 
			Vector3D u_val ,Vector3D v_val ,Vector3D n_val , Vector3D o_val = Vector3D(0.,0.,0.) ) :
	
	VolSurface(  new T( typ, thickness_inner, thickness_outer, u_val, v_val, n_val, o_val, vol , 0 )  ){
      }
      
      T* operator->() { return static_cast<T*>( _surf ) ; }
    } ;

    //---------------------------------------------------------------------------------------------
    typedef VolSurfaceHandle< VolPlaneImpl > VolPlane ;
    //---------------------------------------------------------------------------------------------

    class VolCylinder : public VolSurface{
    public:
      VolCylinder( Volume vol, SurfaceType typ_val, double thickness_inner ,double thickness_outer,  Vector3D origin_val ) :
        VolSurface( new VolCylinderImpl( vol,  typ_val,  thickness_inner , thickness_outer, origin_val ) ) {}
    } ;

    class VolCone : public VolSurface{
    public:
      VolCone( Volume vol, SurfaceType typ_val, double thickness_inner ,double thickness_outer, Vector3D v_val, Vector3D origin_val ) :
        VolSurface( new VolConeImpl( vol,  typ_val,  thickness_inner , thickness_outer, v_val,  origin_val ) ) {}
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
      
      DetElement _det ;
      mutable VolSurface _volSurf ;
      std::unique_ptr<TGeoMatrix> _wtM ; // matrix for world transformation of surface
      
      long64 _id ;
      
      SurfaceType _type ;
      Vector3D _u ;
      Vector3D _v ;
      Vector3D _n ;
      Vector3D _o ;

      /// default c'tor etc. removed
      Surface() = delete;
      Surface( Surface const& ) = delete;
      Surface& operator=( Surface const& ) = delete;

    public:
    
      virtual ~Surface() {} 

      /** Standard c'tor initializes the surface from the parameters of the VolSurface and the 
       *  transform (placement) of the corresponding volume, if found in DetElement 
       */
      Surface( DetElement det, VolSurface volSurf ) ;      
    
      /// The id of this surface - corresponds to DetElement id.
      virtual long64 id() const ;

      /** properties of the surface encoded in Type.
       * @see SurfaceType
       */
      virtual const SurfaceType& type() const ;
    
      /// The volume that has the surface attached.
      Volume volume() const { return _volSurf.volume()  ; }

      /// The VolSurface attched to the volume.
      VolSurface volSurface() const { return _volSurf ; }

      /// The DetElement belonging to the surface volume
      DetElement detElement() const { return _det; }


      //==== geometry ====
      
      /** First direction of measurement U */
      virtual Vector3D u( const Vector3D& point = Vector3D() ) const ;
    
      /** Second direction of measurement V */
      virtual Vector3D v(const Vector3D& point = Vector3D() ) const ;
    
      /// Access to the normal direction at the given point
      virtual  Vector3D normal(const Vector3D& point = Vector3D() ) const ;
    
      /** Get Origin of local coordinate system on surface */
      virtual const Vector3D& origin() const ;

      /** Convert the global position to the local position (u,v) on the surface */
      virtual Vector2D globalToLocal( const Vector3D& point) const ;
      
      /** Convert the local position (u,v) on the surface to the global position*/
      virtual Vector3D localToGlobal( const Vector2D& point) const ;

      /** Thickness of inner material */
      virtual double innerThickness() const ;

      /** Thickness of outer material */
      virtual double outerThickness() const ;

      /// Access to the material in opposite direction of the normal
      virtual const IMaterial& innerMaterial() const ;
     
      /// Access to the material in direction of the normal
      virtual const IMaterial& outerMaterial() const ;
          
      /** Distance to surface */
      virtual double distance(const Vector3D& point ) const  ;
      
      /// Checks if the given point lies within the surface
      virtual bool insideBounds(const Vector3D& point, double epsilon=1.e-4) const ;

      /** Get Origin of local coordinate system of the associated volume */
      virtual Vector3D volumeOrigin() const  ; 

      /** The length of the surface along direction u at the origin. For 'regular' boundaries, like rectangles, 
       *  this can be used to speed up the computation of inSideBounds.
       */
      virtual double length_along_u() const ;

      /** The length of the surface along direction v at the origin. For 'regular' boundaries, like rectangles, 
       *  this can be used to speed up the computation of inSideBounds.
       */
      virtual double length_along_v() const ;

      /** Get lines constraining the surface for drawing ( might not be exact boundaries) -
       *  at most nMax lines are returned.
       */
      virtual std::vector< std::pair< Vector3D, Vector3D> > getLines(unsigned nMax=100) ;

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
      CylinderSurface( DetElement det, VolSurface volSurf ) : Surface( det, volSurf ) { }      
      
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
 
      /** Convert the global position to the local position (u,v) on the surface - u runs along the axis of the cylinder, v is r*phi */
      virtual Vector2D globalToLocal( const Vector3D& point) const ;
      
      /** Convert the local position (u,v) on the surface to the global position  - u runs along the axis of the cylinder, v is r*phi*/
      virtual Vector3D localToGlobal( const Vector2D& point) const ;

      /// the radius of the cylinder (rho of the origin vector)
      virtual double radius() const ;

      /// the center of the cylinder 
      virtual Vector3D center() const ;

    } ;
    //======================================================================================================

    /** Specialization of Surface for cones - specializes CyliderSurface (for lazyness ...)
     *  @author F.Gaede, DESY
     *  @date May, 10 2014
     */
    class ConeSurface : public CylinderSurface, public ICone {
    public:      
      ConeSurface( DetElement det, VolSurface volSurf ) : CylinderSurface( det, volSurf ) { }      
      
      /// the start radius of the cone
      virtual double radius0() const ;
      
      /// the end radius of the cone
      virtual double radius1() const ;

      /// the start z of the cone
      virtual double z0() const ;
      
      /// the end z of the cone
      virtual double z1() const ;
      
      /// the center of the cone 
      virtual Vector3D center() const ;
     
    };
    //======================================================================================================
    /** std::list of Surfaces that optionally takes ownership.
     * @author F.Gaede, DESY
     * @date Apr, 10 2014
     * @version $Id$
     */
    class SurfaceList : public std::list< ISurface* > {
    
    protected:
      bool _isOwner ;

    public:
      /// defaul c'tor - allow to set ownership for surfaces
      SurfaceList(bool isOwner=false ) : _isOwner( isOwner )  {}

      /// copy c'tor
      SurfaceList(const SurfaceList& other ) : std::list< ISurface* >( other ), _isOwner( false ){}

      /// required c'tor for extension mechanism
      SurfaceList(const DetElement& ) : _isOwner( false ) {
        // anything to do here  ?
      }
      /// required c'tor for extension mechanism
      SurfaceList(const SurfaceList& ,const DetElement& ) : _isOwner( false ) {
        // anything to do here  ?
      }
    
      /// d'tor deletes all owned surfaces
      virtual ~SurfaceList() ;

    } ;
  
    //    SurfaceList* surfaceList( DetElement& det ) ;

    //======================================================================================================

  
  } /* namespace */
} /* namespace */



#endif /* Surface */
