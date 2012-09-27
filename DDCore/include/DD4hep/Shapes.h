// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#ifndef DD4hep_GEOMETRY_SOLIDS_H
#define DD4hep_GEOMETRY_SOLIDS_H

// Framework include files
#include "DD4hep/Handle.h"

// C/C++ include files
#include <vector>

// Forward declarations
class TGeoShape;
class TGeoBBox;
class TGeoPcon;
class TGeoPgon;
class TGeoCone;
class TGeoConeSeg;
class TGeoSphere;
class TGeoTrd2;
class TGeoTorus;
class TGeoTrap;
class TGeoTubeSeg;
class TGeoParaboloid;
class TGeoCompositeShape;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Geometry namespace declaration
   */
  namespace Geometry  {

    // Forward declarations
    struct Position;
    struct Rotation;

    /**@class Solid_type Shapes.h 
     *
     *   Base class for Solid objects
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    template <typename T> struct Solid_type : public Handle<T>  {
      protected:
      typedef T Implementation;
      void _setDimensions(double* param);
      /// Assign pointrs and register solid to geometry
      void _assign(Implementation* n, const std::string& nam, const std::string& tit, bool cbbox=true);

      public:

      /// Default constructor for uninitialized object
      Solid_type() : Handle<Implementation>() {}

      /// Direct assignment using the implementation pointer 
      Solid_type(Implementation* p) : Handle<Implementation>(p) {}

      /// Constructor to be used when reading the already parsed object
      Solid_type(const Handle<Implementation>& e) : Handle<Implementation>(e) {}

      /// Constructor to be used when reading the already parsed object: need to check pointers
      template <typename Q> Solid_type(const Handle<Q>& e) : Handle<T>(e) {}

      /// Access to shape name
      const char* name() const;

      /// Auto conversion to underlying ROOT object
      operator Implementation*() const       { return this->m_element; }

      /// Overloaded operator -> to access underlying object
      Implementation* operator->() const     { return this->m_element; }
    };
    typedef Solid_type<TGeoShape> Solid;


    /**@class Box Shapes.h 
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    struct Box : public Solid_type<TGeoBBox>  {
      protected:
      void make(const std::string& name, double x, double y, double z);

      public:
      /// Constructor to be used when reading the already parsed box object
      template <typename Q> 
      Box(const Handle<Q>& e) : Solid_type<Implementation>(e) {}

      /// Constructor to be used when creating an identifiable new box object
      Box(const std::string& name) 
      {  make(name, 0, 0, 0); }

      /// Constructor to be used when creating an anonymous new box object (retrieves name from volume)
      Box() 
      {  make("", 0, 0, 0); }

      /// Constructor to be used when creating an identifiable new box object
      Box(const std::string& name, double x, double y, double z)
      {  make(name, x, y, z); }

      /// Constructor to be used when creating an anonymous new box object (retrieves name from volume)
      Box(double x, double y, double z)
      {  make("", x, y, z); }

      /// Constructor to be used when creating an identifiable new box object
      template<typename X, typename Y, typename Z>
      Box(const std::string& name, const X& x, const Y& y, const Z& z)
      {  make(name, _toDouble(x),_toDouble(y),_toDouble(z)); }

      /// Constructor to be used when creating an anonymous new box object (retrieves name from volume)
      template<typename X, typename Y, typename Z>
      Box(const X& x, const Y& y, const Z& z)
      {  make("", _toDouble(x),_toDouble(y),_toDouble(z)); }

      /// Set the box dimensions
      Box& setDimensions(double x, double y, double z);

      /// Access half "length" of the box
      double x() const;

      /// Access half "width" of the box
      double y() const;

      /// Access half "depth" of the box
      double z() const;
    };

    /**@class Polycone Shapes.h 
     *
     *   Polycone. It has at least 9 parameters :
     *      - the lower phi limit;
     *      - the range in phi;
     *      - the number of z planes (at least two) where the inner/outer
     *        radii are changing;
     *      - z coordinate, inner and outer radius for each z plane
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    struct Polycone : public Solid_type<TGeoPcon>  {
      /// Constructor to be used when reading the already parsed polycone object
      template <typename Q> Polycone(const Handle<Q>& e) : Solid_type<Implementation>(e) {}

      /// Constructor to be used when creating a new polycone object
      Polycone(const std::string& name="");

      /// Constructor to be used when creating a new polycone object
      Polycone(double start, double delta);

      /// Constructor to be used when creating a new polycone object
      Polycone(const std::string& name, double start, double delta);

      /// Constructor to be used when creating a new polycone object. Add at the same time all Z planes
      Polycone(double start, double delta, const std::vector<double>& rmin, const std::vector<double>& rmax, const std::vector<double>& z);

      /// Constructor to be used when creating a new polycone object. Add at the same time all Z planes
      Polycone(const std::string& name, double start, double delta, const std::vector<double>& rmin, const std::vector<double>& rmax, const std::vector<double>& z);

      /// Add Z-planes to the Polycone
      void addZPlanes(const std::vector<double>& rmin, const std::vector<double>& rmax, const std::vector<double>& z);
    };

    /**@class ConeSegment Shapes.h DDCore/Shapes.h
     *
     *   A ConeSegment is, in the general case, a Phi segment of a cone, with
     *   half-length dz, inner and outer radii specified at -dz and +dz.
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    struct ConeSegment : public Solid_type<TGeoConeSeg>  {
      /// Constructor to be used when reading the already parsed ConeSegment object
      template <typename Q> ConeSegment(const Handle<Q>& e) : Solid_type<Implementation>(e) {}

      /// Constructor to be used when creating a new ConeSegment object
      ConeSegment(const std::string& name="");

      /// Constructor to be used when initializing a new object
      ConeSegment(double dz, double rmin1, double rmax1, double rmin2, double rmax2, double phi1=0.0, double phi2=2.0*M_PI);

      /// Constructor to be used when creating a new ConeSegment object
      ConeSegment(const std::string& name,double dz, double rmin1, double rmax1, double rmin2, double rmax2, double phi1=0.0, double phi2=2.0*M_PI);

      /// Set the cone segment dimensions
      ConeSegment& setDimensions(double dz, double rmin1, double rmax1, double rmin2, double rmax2, double phi1=0.0, double phi2=2.0*M_PI);
    };

    /**@class Tube Shapes.h 
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    struct Tube : public Solid_type<TGeoTubeSeg>  {
      protected:
      void make(const std::string& name,double rmin,double rmax,double z,double deltaPhi);

      public:
      /// Constructor to assign an object
      template <typename Q> Tube(const Handle<Q>& e) : Solid_type<Implementation>(e) {}

      /// Constructor to be used when creating a new anonymous tube object
      Tube()
      { make("",0, 0, 0, 0); }

      /// Constructor to be used when creating a new identifiable tube object
      Tube(const std::string& name)
      { make(name,0, 0, 0, 0); }

      /// Constructor to be used when creating a new anonymous tube object with attribute initialization
      Tube(double rmin, double rmax, double z, double deltaPhi=2*M_PI)
      { make("",rmin, rmax, z, deltaPhi); }

      /// Constructor to be used when creating a new identifiable tube object with attribute initialization
      Tube(const std::string& name, double rmin, double rmax, double z, double deltaPhi=2*M_PI)
      { make(name,rmin, rmax, z, deltaPhi); }

      /// Constructor to be used when creating a new anonymous tube object with attribute initialization
      template<typename RMIN, typename RMAX, typename Z, typename DELTAPHI>
      Tube(const RMIN& rmin, const RMAX& rmax, const Z& z, const DELTAPHI& deltaPhi)  
      {
        make("",_toDouble(rmin),_toDouble(rmax),_toDouble(z),_toDouble(deltaPhi));
      }

      /// Constructor to be used when creating a new identifiable tube object with attribute initialization
      template<typename RMIN, typename RMAX, typename Z, typename DELTAPHI>
      Tube(const std::string& name, const RMIN& rmin, const RMAX& rmax, const Z& z, const DELTAPHI& deltaPhi)  
      {
        make(name,_toDouble(rmin),_toDouble(rmax),_toDouble(z),_toDouble(deltaPhi));
      }

      /// Set the tube dimensions
      Tube& setDimensions(double rmin, double rmax, double z, double deltaPhi=2*M_PI);
    };

    /**@class Cone Shapes.h 
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    struct Cone : public Solid_type<TGeoCone>  {
      protected:
      void make(const std::string& name,double z,double rmin1,double rmax1,double rmin2,double rmax2);

      public:

      /// Constructor to be used when reading the already parsed object
      template <typename Q> Cone(const Handle<Q>& e) : Solid_type<Implementation>(e) {}

      /// Constructor to be used when creating a new anonymous object
      Cone()      { make("",0, 0, 0, 0, 0); }
      /// Constructor to be used when creating a new object
      Cone(const std::string& name)      { make(name,0, 0, 0, 0, 0); }
      /// Constructor to be used when creating a new anonymous object with attribute initialization
      Cone(double z,
           double rmin1,
           double rmax1,
           double rmin2,
           double rmax2)
      { make("",z,rmin1,rmax1,rmin2,rmax2); }
      /// Constructor to be used when creating a new object with attribute initialization
      Cone(const std::string& name,
           double z,
           double rmin1,
           double rmax1,
           double rmin2,
           double rmax2)
      { make(name,z,rmin1,rmax1,rmin2,rmax2); }

      template<typename Z, typename RMIN1, typename RMAX1, typename RMIN2, typename RMAX2>
      Cone(const Z& z, const RMIN1& rmin1, const RMAX1& rmax1, const RMIN2& rmin2, const RMAX2& rmax2)
      { make("",_toDouble(z),_toDouble(rmin1),_toDouble(rmax1),_toDouble(rmin2),_toDouble(rmax2)); }

      template<typename Z, typename RMIN1, typename RMAX1, typename RMIN2, typename RMAX2>
      Cone(const std::string& name, const Z& z, const RMIN1& rmin1, const RMAX1& rmax1, const RMIN2& rmin2, const RMAX2& rmax2)
      { make(name,_toDouble(z),_toDouble(rmin1),_toDouble(rmax1),_toDouble(rmin2),_toDouble(rmax2)); }

      /// Set the box dimensions
      Cone& setDimensions(double z,double rmin1,double rmax1,double rmin2,double rmax2);
    };

    /**@class Trap Shapes.h 
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    struct Trap : public Solid_type<TGeoTrap>  {
      /// Constructor to be used when reading the already parsed object
      template <typename Q> Trap( const Handle<Q>& e) : Solid_type<Implementation>(e) {}

      /// Constructor to be used when creating a new identified object with attribute initialization
      Trap( const std::string& name,
            double z,
            double theta,
            double phi,
            double y1,
            double x1,
            double x2,
            double alpha1,
            double y2,
            double x3,
            double x4,
            double alpha2);
      /// Constructor to be used when creating a new anonymous object with attribute initialization
      Trap( double z,
            double theta,
            double phi,
            double y1,
            double x1,
            double x2,
            double alpha1,
            double y2,
            double x3,
            double x4,
            double alpha2);

      /// Set the trap dimensions
      Trap& setDimensions(double z,double theta,double phi,
                          double y1,double x1,double x2,double alpha1,
                          double y2,double x3,double x4,double alpha2);
    };

    /**@class Trapezoid Shapes.h 
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    struct Trapezoid : public Solid_type<TGeoTrd2> {
      /// Constructor to be used when reading the already parsed object
      template <typename Q> Trapezoid(const Handle<Q>& e) : Solid_type<Implementation>(e) {}

      /// Constructor to be used when creating a new anonymous object
      Trapezoid();
      /// Constructor to be used when creating a new identified object
      Trapezoid(const std::string& name);
      /// Constructor to be used when creating a new anonymous object with attribute initialization
      Trapezoid(double x1, double x2, double y1, double y2, double z);
      /// Constructor to be used when creating a new identified object with attribute initialization
      Trapezoid(const std::string& name, double x1, double x2, double y1, double y2, double z);
      /// Set the Trapezoid dimensions
      Trapezoid& setDimensions(double x1, double x2, double y1, double y2, double z);
    };

    /**@class Torus Shapes.h 
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    struct Torus : public Solid_type<TGeoTorus> {
      /// Constructor to be used when reading the already parsed object
      template <typename Q> Torus(const Handle<Q>& e) : Solid_type<Implementation>(e) {}

      /// Constructor to be used when creating a new anonymous object
      Torus();
      /// Constructor to be used when creating a identified new object
      Torus(const std::string& name);
      /// Constructor to be used when creating a new anonymous object with attribute initialization
      Torus(double r, double rmin, double rmax, double phi=M_PI, double delta_phi=2.*M_PI);
      /// Constructor to be used when creating a new identified object with attribute initialization
      Torus(const std::string& name, double r, double rmin, double rmax, double phi=M_PI, double delta_phi=2.*M_PI);
      /// Set the Torus dimensions
      Torus& setDimensions(double r, double rmin, double rmax, double phi, double delta_phi);
    };

    /**@class Sphere Shapes.h 
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    struct Sphere : public Solid_type<TGeoSphere> {
      /// Constructor to be used when reading the already parsed object
      template <typename Q> Sphere(const Handle<Q>& e) : Solid_type<Implementation>(e) {}

      /// Constructor to be used when creating a new anonymous object
      Sphere();
      /// Constructor to be used when creating a identified new object
      Sphere(const std::string& name);
      /// Constructor to be used when creating a new anonymous object with attribute initialization
      Sphere(double rmin, double rmax, double theta=0., double delta_theta=M_PI, double phi=0.0, double delta_phi=2.*M_PI);
      /// Constructor to be used when creating a new identified object with attribute initialization
      Sphere(const std::string& name, double rmin, double rmax, double theta=0., double delta_theta=M_PI, double phi=0., double delta_phi=2.*M_PI);
      /// Set the Sphere dimensions
      Sphere& setDimensions(double rmin, double rmax, double theta, double delta_theta, double phi, double delta_phi);
    };

    /**@class Paraboloid Shapes.h 
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    struct Paraboloid : public Solid_type<TGeoParaboloid> {
      /// Constructor to be used when reading the already parsed object
      template <typename Q> Paraboloid(const Handle<Q>& e) : Solid_type<Implementation>(e) {}

      /// Constructor to be used when creating a new anonymous object
      Paraboloid();
      /// Constructor to be used when creating a new identified object
      Paraboloid(const std::string& name);
      /// Constructor to be used when creating a new anonymous object with attribute initialization
      Paraboloid(double r_low, double r_high, double delta_z);
      /// Constructor to be used when creating a new identified object with attribute initialization
      Paraboloid(const std::string& name, double r_low, double r_high, double delta_z);
      /// Set the Paraboloid dimensions
      Paraboloid& setDimensions(double r_low, double r_high, double delta_z);
    };

    /**@class PolyhedraRegular Shapes.h 
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    struct PolyhedraRegular : public Solid_type<TGeoPgon>  {
      /// Constructor to be used when reading the already parsed object
      template <typename Q> 
      PolyhedraRegular(const Handle<Q>& e) : Solid_type<Implementation>(e) {}

      /// Constructor to be used when creating a new object
      PolyhedraRegular(int nsides, double rmin, double rmax, double zlen);
      /// Constructor to be used when creating a new object
      PolyhedraRegular(const std::string& name, int nsides, double rmin, double rmax, double zlen);
    };

    /**@class BooleanSolid Shapes.h 
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    struct BooleanSolid : public Solid_type<TGeoCompositeShape>  {
      protected:
      /// Constructor to be used when reading the already parsed object
      BooleanSolid() : Solid_type<Implementation>() {}

      public:
      /// Constructor to be used when reading the already parsed object
      template <typename Q> 
      BooleanSolid(const Handle<Q>& e) : Solid_type<Implementation>(e) {}
    };

    /**@class SubtractionSolid Shapes.h 
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    struct SubtractionSolid : public BooleanSolid  {
      /// Constructor to be used when reading the already parsed object
      template<typename Q> SubtractionSolid(const Handle<Q>& e) : BooleanSolid(e) {}
      /// Constructor to be used when creating a new object
      SubtractionSolid(const Solid& shape1, const Solid& shape2, const Position& pos, const Rotation& rot);
       /// Constructor to be used when creating a new object
      SubtractionSolid(const std::string& name, const Solid& shape1, const Solid& shape2, const Position& pos, const Rotation& rot);
   };

    /**@class UnionSolid Shapes.h 
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    struct UnionSolid : public BooleanSolid  {
      /// Constructor to be used when reading the already parsed object
      template<typename Q> UnionSolid(const Handle<Q>& e) : BooleanSolid(e) {}
      /// Constructor to be used when creating a new object
      UnionSolid(const Solid& shape1, const Solid& shape2, const Position& pos, const Rotation& rot);
      /// Constructor to be used when creating a new object
      UnionSolid(const std::string& name, const Solid& shape1, const Solid& shape2, const Position& pos, const Rotation& rot);
    };

    /**@class IntersectionSolid Shapes.h 
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    struct IntersectionSolid : public BooleanSolid  {
      /// Constructor to be used when reading the already parsed object
      template<typename Q> IntersectionSolid(const Handle<Q>& e) : BooleanSolid(e) {}
      /// Constructor to be used when creating a new object
      IntersectionSolid(const Solid& shape1, const Solid& shape2, const Position& pos, const Rotation& rot);
      /// Constructor to be used when creating a new object
      IntersectionSolid(const std::string& name, const Solid& shape1, const Solid& shape2, const Position& pos, const Rotation& rot);
    };

  }       /* End namespace Geometry           */
}         /* End namespace DD4hep            */
#endif    /* DD4hep_GEOMETRY_SOLIDS_H        */
