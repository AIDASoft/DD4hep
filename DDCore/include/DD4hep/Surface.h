/*
 * Surface.h 
 * Created on: Mar, 24 2014
 * Author: F.Gaede, DESY
 */

#ifndef Surface_H
#define Surface_H

//#include "DD4hep/Detector.h"
//#include <vector>

#include "DD4hep/Objects.h"
#include "DD4hep/Volumes.h"
//#include "Math/Vector3D.h"

#include <list>

namespace DD4hep {
  namespace Geometry {

    /** Helper struct to describe surface parameters for Volumes.
     * The surface holds local vectors for u,v,n and the orgigin - all in local Volume coordinates.
     * Additionally inner and outer thicknesses can be specified (too be used for averaging material 
     * properties for the "surface"). 
     * 
     * @author F.Gaede, DESY
     * @date Apr, 1 2014
     * @version $Id:$
     */
    struct Surface : public Ref_t {

      typedef Position Vector3D ;

      enum SurfaceType{
	Cylinder = 1,
	Plane,
	Other
      } ;
      

      struct Object: public TNamed {
      
	Volume _vol ;
	SurfaceType _type ;
	bool _isSensitive ;
	Vector3D _u ;
	Vector3D _v ;
	Vector3D _n ;
	Vector3D _o ;
	double _th_i ;
	double _th_o ;
	Material _innerMat ;
	Material _outerMat ;

        Object();

	Object( Volume vol, SurfaceType type, bool isSensitive , double thickness_inner ,double thickness_outer, 
		Vector3D u ,Vector3D v ,Vector3D n ,Vector3D o )  ;

        /// Default destructor
        virtual ~Object();
        /// Copy the object

        void copy(const Object& c) {
	  _vol = c._vol ;
	  _type = c._type ;
	  _isSensitive = c._isSensitive ;
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
      
      // copied from DetElement - are these needed ?
      // /// Templated destructor function
      // template <typename T> static void _delete(void* ptr) {
      //   delete (T*) (ptr);
      // }
      // /// Templated copy constructor
      // template <typename T> static void* _copy(const void* ptr, Surface surf) {
      //   return new T(*(dynamic_cast<const T*>((T*) ptr)), surf);
      // }
      //------------------


      Surface() :  Ref_t() { }
      

      Surface( Volume vol, SurfaceType type, bool isSensitive , double thickness_inner ,double thickness_outer, 
	       Vector3D u ,Vector3D v ,Vector3D n ,Vector3D o = Vector3D(0.,0.,0.) ) ;      
      
      Volume getVolume() const { return object<Object>()._vol ; }

      /** Type of the surface - see enum SurfaceType */
      SurfaceType getSurfaceType() const { return object<Object>()._type ; }
      
      /** True if this is a measurement surface that has measurement directions stored - (needed ?) */
      bool isSensitive() const { return object<Object>()._isSensitive ; } 
      
      //==== geometry ====
      
      /** First direction of measurement U */
      Vector3D getU() const { return object<Object>()._u ; }
      
      /** Second direction of measurement V */
      Vector3D getV() const { return object<Object>()._v ; }
      
      /** Normal vector of the surface */
      Vector3D getN() const { return object<Object>()._n ; }
      
      /** Get Origin of local coordinate system on surface */
      Vector3D getOrigin() const { return object<Object>()._o ;}
      
      // //==== materials ====
      
      // /** Inner material assigned to the surface */
      Material getInnerMaterial() const { return object<Object>()._innerMat ; }
      
      // /** Outer material assigned to the surface */
      Material getOuterMaterial() const { return object<Object>()._outerMat ; }
      
      /** Thickness of inner material */
      double getInnerThickness() const { return object<Object>()._th_i ; }

      /** Thickness of outer material */
      double getOuterThickness() const { return object<Object>()._th_o ; }

      // //=== navigation ====

      // /** Returns true if the given point is on the surface and inside the bounds. Epsilon
      // 	  is the allowed distance from the surface (in tgeo units, i.e. cm )*/
      virtual bool isInsideBounds(const Vector3D& p , double epsilon=1.0e-4) const ;


      // /** Convert local coordinates on the surface to a global position */
      // virtual Vector3D getPosition( double u, double v ) const  ;

    // protected:
    //   DetElement det;
    };



    /** std::list of Surfaces that takes ownership.
     */
    struct SurfaceList : std::list< Surface > {

      SurfaceList() {}
      // required c'tors for extension mechanism
      SurfaceList(const Geometry::DetElement& d){
	// anything to do here  ?
      }
      SurfaceList(const SurfaceList& c,const Geometry::DetElement& det){
	// anything to do here  ?
      }

      virtual ~SurfaceList(){

	// delete all surfaces attached to this volume
	for( SurfaceList::iterator i=begin(), n=end() ; i !=n ; ++i ) {
	  delete (*i).ptr() ;
	}

      }

    } ;

    SurfaceList* surfaceList( DetElement& det ) ;

    //    void addSurface( DetElement& det , Surface& surf ) ;


  } /* namespace Geometry */
} /* namespace DD4hep */
#endif /* Surface */
