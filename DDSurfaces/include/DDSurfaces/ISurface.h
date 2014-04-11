#ifndef DDSurfaces_ISurface_H
#define DDSurfaces_ISurface_H

#include "DDSurfaces/IMaterial.h"
#include "DDSurfaces/Vector3D.h"

#include <bitset>
#include <math.h>

namespace DDSurfaces {
  
  struct SurfaceType ;

  typedef long long int long64 ;

 /** Interface for tracking surfaces. 
   * The surface provides access to vectors for u,v,n and the orgigin and
   * the inner and outer materials with corresponding thicknesses.
   *  
   * @author C.Grefe, CERN, F. Gaede, DESY
   * @version $Id: $
   * @date Mar 7 2014
   */
  class ISurface {
    
  public:
    /// Destructor
    virtual ~ISurface() {}
    
    /// properties of the surface encoded in Type.
    virtual const SurfaceType& type() const =0 ;
    
    /// The id of this surface - corresponds to DetElement id ( or'ed with the placement ids )
    virtual long64 id() const =0 ;

    /// Checks if the given point lies within the surface
    virtual bool insideBounds(const Vector3D& point, double epsilon=1.e-4) const =0 ;
    
    /** First direction of measurement U */
    virtual const Vector3D& u( const Vector3D& point = Vector3D() ) const =0 ;
    
    /** Second direction of measurement V */
    virtual const Vector3D& v(const Vector3D& point = Vector3D() ) const =0 ;
    
    /// Access to the normal direction at the given point
    virtual const Vector3D& normal(const Vector3D& point = Vector3D() ) const =0 ;
    
    /** Get Origin of local coordinate system on surface */
    virtual const Vector3D& origin() const =0 ;
    
    /// Access to the material in opposite direction of the normal
    virtual const IMaterial& innerMaterial() const =0 ;
    
    /// Access to the material in direction of the normal
    virtual const IMaterial& outerMaterial() const =0 ;
    
    /** Thickness of inner material */
    virtual double innerThickness() const =0 ;
    
    /** Thickness of outer material */
    virtual double outerThickness() const =0 ;
    
    /** Distance to surface */
    virtual double distance(const Vector3D& point ) const =0 ;
    
  } ;
  

  /** Helper class for describing surface properties.
   *  Usage: SurfaceType type(  SurfaceType::Plane, SurfaceType::Sensitive ) ; 
   *
   * @author F. Gaede, DESY
   * @version $Id: $
   * @date Apr 6 2014
   */
  class SurfaceType{
  
  public:
    /// enum for defining the bits used to decode the properties
    enum{
      Cylinder = 1,
      Plane,
      Sensitive,
      Helper,
      ParallelToZ,
      OrthogonalToZ
    } ;
    
    ///default c'tor
    SurfaceType() : _bits(0) {}

    // c'tor that sets one property
    SurfaceType( unsigned prop0 ) : _bits(0) { 
      _bits.set( prop0 ) ;
    } 
    
    // c'tor that sets two properties
    SurfaceType( unsigned prop0 , unsigned prop1 ) : _bits(0) { 
      _bits.set( prop0 ) ;
      _bits.set( prop1 ) ;
    } 
    
    // c'tor that sets three properties
    SurfaceType( unsigned prop0 , unsigned prop1 , unsigned prop2 ) : _bits(0) { 
      _bits.set( prop0 ) ;
      _bits.set( prop1 ) ;
      _bits.set( prop2 ) ;
    } 
    
    // c'tor that sets four properties
    SurfaceType( unsigned prop0 , unsigned prop1 , unsigned prop2, unsigned prop3 ) : _bits(0) { 
      _bits.set( prop0 ) ;
      _bits.set( prop1 ) ;
      _bits.set( prop2 ) ;
      _bits.set( prop3 ) ;
    } 
    
    /// set the given peorperty
    void setProperty( unsigned prop , bool val = true ) { _bits.set( prop , val ) ;  } 
    
    /// true if surface is sensitive
    bool isSensitive() const { return _bits[ SurfaceType::Sensitive ] ; }
    
    /// true if surface is helper surface for navigation
    bool isHelper() const  { return _bits[ SurfaceType::Helper ] ; }
    
    /// true if this a planar surface
    bool isPlane() const { return _bits[ SurfaceType::Plane ] ; } 
    
    /// true if this a cylindrical surface
    bool isCylinder() const { return _bits[ SurfaceType::Cylinder ] ; } 

    /// true if surface is parallel to Z
    bool isParallelToZ() const { return _bits[ SurfaceType::ParallelToZ ] ; } 

    /// true if surface is orthogonal to Z
    bool isOrthogonalToZ() const { return _bits[ SurfaceType::OrthogonalToZ ] ; } 
   
   /// true if this is a cylinder parallel to Z
    bool isZCylinder() const  { return ( _bits[ SurfaceType::Cylinder ] &&  _bits[ SurfaceType::ParallelToZ ] ) ; } 

   /// true if this is a plane parallel to Z
    bool isZPlane() const  { return ( _bits[ SurfaceType::Plane ] &&  _bits[ SurfaceType::ParallelToZ ] ) ; 
    } 
    
    /// true if this is a plane orthogonal to Z
    bool isZDisk() const  { return ( _bits[ SurfaceType::Plane ] &&  _bits[ SurfaceType::OrthogonalToZ ] ) ; } 


    /** True if surface is parallel to Z with accuracy epsilon - result is cached in bit SurfaceType::ParallelToZ */
    bool checkParallelToZ( const ISurface& surf , double epsilon=1.e-6 ) const { 
      
      double proj = std::fabs( surf.normal() * Vector3D(0.,0.,1.) )  ;
      
      _bits.set(  SurfaceType::ParallelToZ , ( proj < epsilon )  ) ;
      
      // std::cout << " ** checkParallelToZ() - normal : " <<  surf.normal() << " pojection : " << proj 
      // 		<< " _bits[ SurfaceType::ParallelToZ ] = " <<  bool( _bits[ SurfaceType::ParallelToZ ] )
      // 		<< "  ( std::fabs( proj - 1. ) < epsilon )  ) = " <<   ( proj < epsilon ) << std::endl ;

      return  _bits[ SurfaceType::ParallelToZ ] ;
    }

    /** True if surface is orthogonal to Z with accuracy epsilon - result is cached in bit SurfaceType::OrthogonalToZ */
    bool checkOrthogonalToZ( const ISurface& surf , double epsilon=1.e-6 ) const { 
      
      double proj = std::fabs( surf.normal() * Vector3D(0.,0.,1.) )  ;
      
      _bits.set(  SurfaceType::OrthogonalToZ , ( std::fabs( proj - 1. ) < epsilon )  ) ;
      
      // std::cout << " ** checkOrthogonalToZ() - normal : " <<  surf.normal() << " pojection : " << proj 
      // 		<< " _bits[ SurfaceType::OrthogonalToZ ] = " << bool( _bits[ SurfaceType::OrthogonalToZ ] ) 
      // 		<< "  ( std::fabs( proj - 1. ) < epsilon )  ) = " <<  ( std::fabs( proj - 1. ) < epsilon ) << std::endl ;


      return  _bits[ SurfaceType::OrthogonalToZ ] ;
    }


  protected:

    mutable std::bitset<32> _bits ;
  } ;

  /// dump SurfaceType operator 
  inline std::ostream& operator<<( std::ostream& os , const SurfaceType& t ) {

    os << "sensitive[" << t.isSensitive() << "] helper[" << t.isHelper() << "] plane[" << t.isPlane()  << "] cylinder[" << t.isCylinder()  
       << "] parallelToZ[" << t.isParallelToZ()  << "] orthogonalToZ[" << t. isOrthogonalToZ()  << "] zCylinder[" << t.isZCylinder() 
       <<  "] zPlane[" << t.isZPlane() << "]"  ; 

    return os ;
  }



  /// dump ISurface operator 
  inline std::ostream& operator<<( std::ostream& os , const ISurface& s ) {
    
    os <<  "   id: " << s.id() << " type : " << s.type() << std::endl  
       <<  "   u : " << s.u() << " v : " << s.v() << " normal : " << s.normal() << " origin : " << s.origin() << std::endl   ;
    os <<  "   inner material : " << s.innerMaterial() << std::endl  
       <<  "   outerMaterial :  " << s.outerMaterial() << std::endl   ;

      return os ;
  }


} /* namespace DDSurfaces */

#endif /* DDSurfaces_ISurface_H */
