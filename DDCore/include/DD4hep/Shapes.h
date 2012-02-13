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
class TGeoTrd2;
class TGeoTrap;
class TGeoTubeSeg;
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
      void _assign(LCDD& lcdd, Implementation* n, const std::string& nam, const std::string& tit, bool cbbox=true);
      

      public:

      /// Default constructor for uninitialized object
      Solid_type() : Handle<Implementation>() {}

      /// Direct assignment using the implementation pointer 
      Solid_type(Implementation* p) : Handle<Implementation>(p) {}

      /// Constructor to be used when reading the already parsed object
      Solid_type(const Handle<Implementation>& e) : Handle<Implementation>(e) {}

      /// Constructor to be used when reading the already parsed object: need to check pointers
      template <typename Q> 
      Solid_type(const Handle<Q>& e) : Handle<T>(e) {}

      /// Access to shape name
      const char* name() const;

      /// Auto conversion to underlying ROOT object
      operator Implementation*() const     { return this->m_element; }

      /// Overloaded operator -> to access underlying object
      Implementation* operator->() const   { return this->m_element; }
    };
    typedef Solid_type<TGeoShape> Solid;


    /**@class Box Shapes.h 
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    struct Box : public Solid_type<TGeoBBox>  {
      protected:
      void make(LCDD& lcdd, const std::string& name, double x, double y, double z);

      public:
      /// Constructor to be used when reading the already parsed box object
      template <typename Q> 
      Box(const Handle<Q>& e) : Solid_type<Implementation>(e) {}

      /// Constructor to be used when creating a new box object
      Box(LCDD& lcdd, const std::string& name) 
      {  make(lcdd, name, 0, 0, 0); }

      /// Constructor to be used when creating a new box object
      Box(LCDD& lcdd, const std::string& name, double x, double y, double z)
      {  make(lcdd, name, x, y, z); }

      /// Constructor to be used when creating a new box object
      template<typename X, typename Y, typename Z>
      Box(LCDD& lcdd, const std::string& name, const X& x, const Y& y, const Z& z)
      {  make(lcdd, name, _toDouble(x),_toDouble(y),_toDouble(z)); }

      /// Set the box dimensions
      Box& setDimensions(double x, double y, double z);
    };

    /**@class Polycone Shapes.h 
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    struct Polycone : public Solid_type<TGeoPcon>  {
      /// Constructor to be used when reading the already parsed polycone object
      template <typename Q> Polycone(const Handle<Q>& e) : Solid_type<Implementation>(e) {}

      /// Constructor to be used when creating a new polycone object
      Polycone(LCDD& lcdd, const std::string& name);

      /// Constructor to be used when creating a new polycone object
      Polycone(LCDD& lcdd, const std::string& name, double start, double delta);

      /// Constructor to be used when creating a new polycone object. Add at the same time all Z planes
      Polycone(LCDD& lcdd, const std::string& name, double start, double delta, const std::vector<double>& rmin, const std::vector<double>& rmax, const std::vector<double>& z);

      /// Add Z-planes to the Polycone
      void addZPlanes(const std::vector<double>& rmin, const std::vector<double>& rmax, const std::vector<double>& z);
    };

    /**@class Tube Shapes.h 
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    struct Tube : public Solid_type<TGeoTubeSeg>  {
      protected:
      void make(LCDD& lcdd, const std::string& name,double rmin,double rmax,double z,double deltaPhi);

      public:
      /// Constructor to assign an object
      template <typename Q> Tube(const Handle<Q>& e) : Solid_type<Implementation>(e) {}

      /// Constructor to be used when creating a new tube object
      Tube(LCDD& lcdd, const std::string& name)
      { make(lcdd,name,0, 0, 0, 0); }

      /// Constructor to be used when creating a new tube object with attribute initialization
      Tube(LCDD& lcdd, const std::string& name, double rmin, double rmax, double z, double deltaPhi=2*M_PI)
      { make(lcdd,name,rmin, rmax, z, deltaPhi); }

      /// Constructor to be used when creating a new tube object with attribute initialization
      template<typename RMIN, typename RMAX, typename Z, typename DELTAPHI>
      Tube(LCDD& lcdd, const std::string& name, const RMIN& rmin, const RMAX& rmax, const Z& z, const DELTAPHI& deltaPhi)  
      {
        make(lcdd,name,_toDouble(rmin),_toDouble(rmax),_toDouble(z),_toDouble(deltaPhi));
      }

      /// Set the box dimensions
      Tube& setDimensions(double rmin, double rmax, double z, double deltaPhi=2*M_PI);
    };

    /**@class Cone Shapes.h 
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    struct Cone : public Solid_type<TGeoCone>  {
      protected:
      void make(LCDD& lcdd, const std::string& name,double z,double rmin1,double rmax1,double rmin2,double rmax2);

      public:

      /// Constructor to be used when reading the already parsed object
      template <typename Q> Cone(const Handle<Q>& e) : Solid_type<Implementation>(e) {}

      /// Constructor to be used when creating a new object
      Cone(LCDD& lcdd, const std::string& name)
      { make(lcdd,name,0, 0, 0, 0, 0); }

      /// Constructor to be used when creating a new object with attribute initialization
      Cone(LCDD& lcdd, const std::string& name,
           double z,
           double rmin1,
           double rmax1,
           double rmin2,
           double rmax2)
      { make(lcdd,name,z,rmin1,rmax1,rmin2,rmax2); }

      template<typename Z, typename RMIN1, typename RMAX1, typename RMIN2, typename RMAX2>
      Cone(LCDD& lcdd, const std::string& name, const Z& z, const RMIN1& rmin1, const RMAX1& rmax1, const RMIN2& rmin2, const RMAX2& rmax2)
      { make(lcdd,name,_toDouble(z),_toDouble(rmin1),_toDouble(rmax1),_toDouble(rmin2),_toDouble(rmax2)); }

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

      /// Constructor to be used when creating a new object with attribute initialization
      Trap( LCDD& lcdd, const std::string& name,
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

      /// Constructor to be used when creating a new object
      Trapezoid(LCDD& lcdd, const std::string& name);
      /// Constructor to be used when creating a new object with attribute initialization
      Trapezoid(LCDD& lcdd, const std::string& name, double x1, double x2, double y1, double y2, double z);
      /// Set the Trapezoid dimensions
      Trapezoid& setDimensions(double x1, double x2, double y1, double y2, double z);
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
      PolyhedraRegular(LCDD& lcdd, const std::string& name, int nsides, double rmin, double rmax, double zlen);
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

      /// Constructor to be used when creating a new object
      BooleanSolid(LCDD& lcdd, const std::string& type, const std::string& name, const std::string& expr);
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
      SubtractionSolid(LCDD& lcdd, const std::string& name, const std::string& expr);
      /// Constructor to be used when creating a new object
      SubtractionSolid(LCDD& lcdd, const std::string& name, const Solid& shape1, const Solid& shape2);
      /// Constructor to be used when creating a new object
      SubtractionSolid(LCDD& lcdd, const std::string& name, const Solid& shape1, const Solid& shape2, const Position& pos, const Rotation& rot);
    };

  }       /* End namespace Geometry           */
}         /* End namespace DD4hep            */
#endif    /* DD4hep_GEOMETRY_SOLIDS_H        */
