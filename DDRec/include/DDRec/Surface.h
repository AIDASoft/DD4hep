#ifndef DD4Surfaces_Surface_H
#define DD4Surfaces_Surface_H

#include "DD4hep/Objects.h"
#include "DD4hep/Volumes.h"
#include "DD4hep/Detector.h"

#include "DDSurfaces/ISurface.h"

#include <list>

class TGeoMatrix ;

namespace DD4hep {
  namespace DDRec {
  
    using namespace DDSurfaces ;
    

    /** Wrapper class to  Geometry::Material that implements the DDSurfaces::IMaterial interface.
     *
     * @author F.Gaede, DESY
     * @date Apr, 6 2014
     * @version $Id:$
     */
    struct SurfaceMaterial : public virtual Geometry::Material ,  public IMaterial{

      /** Copy c'tor - copies handle */
      SurfaceMaterial( Geometry::Material m ) ; // : Geometry::Material( m ) {} 

      SurfaceMaterial( const SurfaceMaterial& sm ) ;
      // {
      // 	(*this).Geometry::Material::m_element =  sm.Geometry::Material::m_element  ; 
      // }

      virtual ~SurfaceMaterial() ; //{} 

      /// material name
      virtual std::string name() const { return m_element->GetMaterial()->GetName()  ; }

      /// averaged proton number
      virtual double Z() const { return m_element->GetMaterial()->GetZ()  ; }
      
      /// averaged atomic number
      virtual double A() const { return m_element->GetMaterial()->GetA() ; }
      
      /// density - units ?
      virtual double density() const { return m_element->GetMaterial()->GetDensity() ; } 
      
      /// radiation length - tgeo units 
      virtual double radiationLength() const { return Geometry::Material::radLength() ; } 
      
      /// interaction length - tgeo units 
      virtual double interactionLength() const  { return Geometry::Material::intLength() ; }

    };

    /** Helper class for holding surface data. 
     * @author F.Gaede, DESY
     * @date Apr, 6 2014
     * @version $Id:$
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
     * @version $Id:$
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
    

      /** properties of the surface encoded in Type.
       * @see SurfaceType
       */
      virtual const SurfaceType& type() const { return object<SurfaceData>()._type ; }
    
      //==== geometry ====
      
      /** First direction of measurement U */
      virtual const Vector3D& u( const Vector3D& point = Vector3D() ) const { return object<SurfaceData>()._u ; }
    
      /** Second direction of measurement V */
      virtual const Vector3D& v(const Vector3D& point = Vector3D() ) const { return object<SurfaceData>()._v ; }
    
      /// Access to the normal direction at the given point
      virtual const Vector3D& normal(const Vector3D& point = Vector3D() ) const { return object<SurfaceData>()._n ; }
    
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
      virtual double distance(const Vector3D& point ) const  { return 0. ; }
      
      /// Checks if the given point lies within the surface
      virtual bool insideBounds(const Vector3D& point, double epsilon=1e-4 ) const { return false ; }


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
     * @version $Id:$
     */
    struct VolSurfaceList : std::list< VolSurface > {
    
      VolSurfaceList() {}
      // required c'tors for extension mechanism
      VolSurfaceList(const Geometry::DetElement& d){
	// anything to do here  ?
      }
      VolSurfaceList(const VolSurfaceList& c,const Geometry::DetElement& det){
	// anything to do here  ?
      }
    
      virtual ~VolSurfaceList(){
      
	// delete all surfaces attached to this volume
	for( VolSurfaceList::iterator i=begin(), n=end() ; i !=n ; ++i ) {
	  delete (*i).ptr() ;
	}
      
      }
    } ;
  
    VolSurfaceList* volSurfaceList( Geometry::DetElement& det ) ;

    //======================================================================================================

    /** Implementation of planar surface attached to a volume 
     * @author F.Gaede, DESY
     * @date Apr, 6 2014
     * @version $Id:$
     */
    class VolPlane : public VolSurface {
      
    public:
      
      VolPlane() : VolSurface() { }
      
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
     * @version $Id:$
     */
    class VolCylinder : public VolSurface {
      
    public:
      
      VolCylinder() : VolSurface() { }
      
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
     * @version $Id:$
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
      virtual long64 id() { return _id ; }

      /** properties of the surface encoded in Type.
       * @see SurfaceType
       */
      virtual const SurfaceType& type() const { return _type ; }
    
      //==== geometry ====
      
      /** First direction of measurement U */
      virtual const Vector3D& u( const Vector3D& point = Vector3D() ) const { return _u ; }
    
      /** Second direction of measurement V */
      virtual const Vector3D& v(const Vector3D& point = Vector3D() ) const { return _v ; }
    
      /// Access to the normal direction at the given point
      virtual const Vector3D& normal(const Vector3D& point = Vector3D() ) const { return _n ; }
    
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

    protected:
      void initialize() ;

    };


    //======================================================================================================

  
  } /* namespace */
} /* namespace */

#endif /* Surface */
