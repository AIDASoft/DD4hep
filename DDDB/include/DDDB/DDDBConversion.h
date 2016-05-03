// $Id$
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================
//
// DDDB is a detector description convention developed by the LHCb experiment.
// For further information concerning the DTD, please see:
// http://lhcb-comp.web.cern.ch/lhcb-comp/Frameworks/DetDesc/Documents/lhcbDtd.pdf
//
//==========================================================================
#ifndef DD4HEP_DDDB_DDDBCONVERSION_H
#define DD4HEP_DDDB_DDDBCONVERSION_H

/// Framework include files
#include "DD4hep/Objects.h"

/// C/C++ include files
#include <string>
#include <vector>
#include <map>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the AIDA detector description toolkit supporting XML utilities
  namespace DDDB {

    /// Forward declarations
    struct Shape;
    struct LogVol;
    struct Catalog;

    struct LogVolRef {};
    struct ElementRef  {};
    struct MaterialRef  {};
    struct CatalogRef  {};
    struct Condition  {};
    struct ConditionRef  {};
    struct ConditionInfo  {};
    struct DetElem  {};
    struct DetElemRef  {};
    struct Param  {};
    struct Parameter  {};
    struct ParamVector  {};
    struct GeometryInfo  {};

    using Geometry::VisAttr;
    using Geometry::Position;
    using Geometry::RotationZYX;
    using Geometry::Transform3D;
    using ROOT::Math::Rotation3D;

    /// Basic named object
    /**   \ingroup DD4HEP_DDDB
     */
    struct Named  {
      typedef std::map<std::string, std::string> StringMap;
      std::string name, id;
      int refCount;
      Named() : refCount(1) {}
      Named(const std::string& c) : name(c), id(), refCount(1)  {}
      Named(const Named& c) : name(c.name), id(c.id), refCount(1)  {}
      Named& operator=(const Named& c) {
        if ( this != &c )  {
          name=c.name;
          id=c.id;
        }
        return *this;
      }
      const char* c_name() const { return name.c_str(); }
      const char* c_id() const   { return id.c_str(); }      
    };

    /// Intermediate structure representing author's data
    /**   \ingroup DD4HEP_DDDB
     */
    struct Author : public Named {
      Author() : Named() {}
    };

    /// Intermediate structure representing versioning data
    /**   \ingroup DD4HEP_DDDB
     */
    struct Version : Named {
      Version() : Named() {}
    };

    /// Intermediate structure representing data of one atom
    /**   \ingroup DD4HEP_DDDB
     */
    struct Atom  {
      double A, Zeff;
      Atom() : A(0), Zeff(0) {}
    };

    /// Intermediate structure representing data of a Isotope
    /**   \ingroup DD4HEP_DDDB
     */
    struct Isotope : public Named  {
      double A,Z,density;
      Isotope() : Named() {}
    };

    /// Intermediate structure representing data of a Element
    /**   \ingroup DD4HEP_DDDB
     */
    struct Element : public Named {
      enum {SOLID,LIQUID,GAS,UNKNOWN};
      std::vector<std::pair<std::string,double> > isotopes;
      std::string path, symbol;
      struct Atom atom;
      double density, ionization;
      int state;

      Element() : density(0), ionization(0), state(UNKNOWN) {}
      Element(const Element& e) 
        : Named(e), isotopes(e.isotopes), path(e.path), symbol(e.symbol), 
          atom(e.atom), density(e.density), 
          ionization(e.ionization), state(e.state)
      {}
      Element* addRef()  {  ++refCount; return this;  }
    };

    /// Intermediate structure representing data of a material component
    /**   \ingroup DD4HEP_DDDB
     */
    struct MaterialComponent  {
      std::string name;
      double fractionmass;
      int natoms;
      MaterialComponent() : name(), fractionmass(-1), natoms(-1) {}
      MaterialComponent(const MaterialComponent& c) : name(c.name), fractionmass(c.fractionmass), natoms(c.natoms) {}
      MaterialComponent& operator=(const MaterialComponent& c)   {
        fractionmass = c.fractionmass;
        natoms = c.natoms;
        name   = c.name;
        return *this;
      }
      const char* c_name() const { return name.c_str(); }
    };

    /// Intermediate structure representing data of a material definition
    /**   \ingroup DD4HEP_DDDB
     */
    struct Material : public Named {
      typedef std::vector<MaterialComponent> Components;
      typedef std::vector<std::string>       Properties;
      std::string path;
      Components components;
      Properties properties;
      double density, pressure, temperature, radlen, lambda;
      Material() : density(0), pressure(-1), temperature(-1), radlen(0), lambda(0) {}
      Material* addRef()  {  ++refCount; return this;  }
    };

    /// Structure supporting conversion of the Box shape
    /**   \ingroup DD4HEP_DDDB
     */
    struct Box    {
      static int type() { return 100; }
      double x,y,z; 
      double dot2() const  { return x*x + y*y + z*z; }
    };

    /// Structure supporting conversion of the Cons shape
    /**   \ingroup DD4HEP_DDDB
     */
    struct Cons                     {
      static int type() { return 102; } 
      double innerRadiusMZ, innerRadiusPZ, outerRadiusMZ, outerRadiusPZ, sizeZ;
    };

    /// Structure supporting conversion of the ConsSegment shape
    /**   \ingroup DD4HEP_DDDB
     */
    struct ConeSegment                     {
      static int type() { return 103; } 
      double start, delta;      
      double innerRadiusMZ, innerRadiusPZ, outerRadiusMZ, outerRadiusPZ, sizeZ;
    };

    /// Structure supporting conversion of the Tubs shape
    /**   \ingroup DD4HEP_DDDB
     */
    struct Tubs                     {
      static int type() { return 104; }
      double innerRadius, outerRadius, sizeZ, start, delta;
    };

    /// Structure supporting conversion of the elliptical tube shape
    /**   \ingroup DD4HEP_DDDB
     */
    struct EllipticalTube   {
      static int type() { return 105; }
      double a, b, dz;
    };

    /// Structure supporting conversion of a z-plane
    /**   \ingroup DD4HEP_DDDB
     */
    struct ZPlane                   {
      static int type() { return 106; }
      double innerRadius, outerRadius, z;
    };

    /// Structure supporting conversion of the Polycone shape
    /**   \ingroup DD4HEP_DDDB
     */
    struct Polycone                 {
      static int type() { return 107; }
      double start, delta;
    };

    /// Structure supporting conversion of the Polycone shape
    /**   \ingroup DD4HEP_DDDB
     */
    struct Polygon                 {
      static int type() { return 108; }
      double nsides, start, innerRadius, outerRadius, z;      
    };

    /// Structure supporting conversion of the torus
    /**   \ingroup DD4HEP_DDDB
     */
    struct Torus   {
      static int type() { return 109; }
      double rmin, rmax, r, phi, dphi;
    };

    /// Structure supporting conversion of the Sphere
    /**   \ingroup DD4HEP_DDDB
     */
    struct Sphere   {
      static int type() { return 110; }
      double rmin, rmax, theta, delta_theta, phi, delta_phi;
    };

    /// Structure supporting conversion of the Ellipsoid
    /**   \ingroup DD4HEP_DDDB
     */
    struct Ellipsoid   {
      static int type() { return 111; }
      double rlow, rhigh, dz;
    };

    /// Structure supporting conversion of the Trap
    /**   \ingroup DD4HEP_DDDB
     */
    struct Trap   {
      static int type() { return 112; }
      double dz, phi, theta, h1, bl1, tl1, alpha1, h2, bl2, tl2, alpha2;
    };

    /// Structure supporting conversion of the Paraboloid
    /**   \ingroup DD4HEP_DDDB
     */
    struct Paraboloid   {
      static int type() { return 113; }
      double rlow, rhigh, dz;
    };

    /// Structure supporting conversion of the Paraboloid
    /**   \ingroup DD4HEP_DDDB
     */
    struct Hyperboloid   {
      static int type() { return 114; }
      double rmin, rmax, stIn, stOut, dz;
    };

    /// Structure supporting conversion of the TRD
    /**   \ingroup DD4HEP_DDDB
     */
    struct TRD   {
      static int type() { return 115; }
      double x1, x2, y1, y2, z;
    };

    /// Structure supporting conversion of the Assembly
    /**   \ingroup DD4HEP_DDDB
     */
    struct Assembly  {
      static int type() { return 199; }
    };

    /// Structure supporting boolean shapes
    /**   \ingroup DD4HEP_DDDB
     */
    struct BooleanShape            {
      struct Shape* first;
    };

    /// Structure supporting conversion of boolean unions
    /**   \ingroup DD4HEP_DDDB
     */
    struct BooleanUnion        : public BooleanShape  {
      static int type() { return 206; } 
    };

    /// Structure supporting conversion of boolean subtractions
    /**   \ingroup DD4HEP_DDDB
     */
    struct BooleanSubtraction  : public BooleanShape  {
      static int type() { return 207; } 
    };

    /// Structure supporting conversion of boolean intersections
    /**   \ingroup DD4HEP_DDDB
     */
    struct BooleanIntersection : public BooleanShape  {
      static int type() { return 208; } 
    };

    /// Structure supporting conversion of boolean operations
    /**   \ingroup DD4HEP_DDDB
     */
    struct BooleanOperation {
      struct Shape* shape;
      Transform3D trafo;
      BooleanOperation() : shape(0) {}
    };


    /// Structure supporting conversion of a physical volume
    /**   \ingroup DD4HEP_DDDB
     */
    struct PhysVol : public Named  {
      enum { PHYSVOL_REGULAR=1,
             PHYSVOL_PARAM1D=2,
             PHYSVOL_PARAM2D=3,
             PHYSVOL_PARAM3D=4
      };
      int type;
      std::string    logvol, path;
      Transform3D trafo;
      PhysVol() : type(PHYSVOL_REGULAR), logvol(), path(), trafo() {}
      PhysVol(const PhysVol& c) : Named(c), type(c.type), logvol(c.logvol), path(c.path), trafo(c.trafo) {}
      PhysVol& operator=(const PhysVol& c) {
        if ( this != &c )  {
          this->Named::operator=(c);
          type   = c.type;
          logvol = c.logvol;
          path   = c.path;
          trafo  = c.trafo;
        }
        return *this;
      }
    };

    /// Structure supporting conversion of parametrized physical volumes
    /**   \ingroup DD4HEP_DDDB
     */
    struct ParamPhysVol : public PhysVol {
      int number1;
      Transform3D trafo1;
      ParamPhysVol() : PhysVol(), number1(0) { type = PHYSVOL_PARAM1D; }
      ParamPhysVol(const ParamPhysVol& c) : PhysVol(c), number1(c.number1), trafo1(c.trafo1) {}
      ParamPhysVol& operator=(const ParamPhysVol& c) {
        if ( this != &c )  {
          this->PhysVol::operator=(c);
          trafo1 = c.trafo1;
          number1 = c.number1;
        }
        return *this;
      }
    };

    /// Structure supporting conversion of 2D parametrized physical volumes
    /**   \ingroup DD4HEP_DDDB
     */
    struct ParamPhysVol2D : public ParamPhysVol  {
      int number2;
      Transform3D trafo2;
      ParamPhysVol2D() : ParamPhysVol(), number2(0), trafo2()  { type = PHYSVOL_PARAM2D; }
    };

    /// Structure supporting conversion of 3D parametrized physical volumes
    /**   \ingroup DD4HEP_DDDB
     */
    struct ParamPhysVol3D : public ParamPhysVol2D  {
      int number3;
      Transform3D trafo3;
      ParamPhysVol3D() : ParamPhysVol2D(), number3(0), trafo3()  { type = PHYSVOL_PARAM3D; }
    };

    /// Structure supporting conversion of a logical volume
    /**   \ingroup DD4HEP_DDDB
     */
    struct LogVol : public Named  {
      std::string   material, shape, path;
      std::vector<PhysVol*> physvols;
      LogVol() : Named(), material(), shape(), physvols() { }
      LogVol* addRef()  {  ++refCount; return this;  }
    };

    /// Structure supporting conversion of a detector element or a catalog
    /**   \ingroup DD4HEP_DDDB
     */
    struct Catalog : public Named {
      typedef std::map<std::string, Catalog*> CatRefs;
      typedef std::map<std::string, LogVol*>  LvRefs;
      LvRefs     logvolrefs;
      LvRefs     logvols;
      CatRefs    catalogrefs;
      CatRefs    catalogs;
      StringMap  params, conditioninfo;
      std::string type, path, author, version, logvol, condition, support, npath;
      int         level;
      Catalog() : Named(), level(0) { }
      std::pair<const Catalog*,std::string> parent(const std::string& nam)  const;
      Catalog* addRef()  {  ++refCount; return this;  }
    };

    /// Structure supporting conversion of any arbitrary shape
    /**   \ingroup DD4HEP_DDDB
     */
    struct Shape : public Named {
      typedef std::vector<ZPlane>           ZPlanes;
      typedef std::vector<BooleanOperation> Operations;
      typedef std::vector<ParamPhysVol>     ParamVolumes;
      int type;
      union  {
        struct Assembly             assembly;
        struct Box                  box;
        struct Cons                 cons;
        struct Tubs                 tubs;
        struct Polycone             polycone;
        struct ConeSegment          coneSegment;
        struct EllipticalTube       ellipticalTube;
        struct Polygon              polygon;
        struct Torus                torus;
        struct Sphere               sphere;
        struct Ellipsoid            ellipsoid;
        struct Paraboloid           paraboloid;
        struct Hyperboloid          hyperboloid;
        struct TRD                  trd;
        struct Trap                 trap;
        struct BooleanShape         boolean;
        struct BooleanUnion         boolean_union;
        struct BooleanSubtraction   BooleanSubtraction;
        struct BooleanIntersection  boolean_intersection;
      } s;
      ZPlanes zplanes;
      Operations boolean_ops;
      ParamVolumes paramVols;
      std::string path;
      /// Default constructor
      Shape();
    };

    /// LHCb geometry description interface to the conditions database
    /**
     *   \author  M.Frank
     *   \version 1.0
     *   \date    31/03/2016
     *   \ingroup DD4HEP_DDDB
     */
    struct dddb   {
      typedef std::map<std::string,std::string> Refs;
      typedef std::map<std::string,LogVol*>     Volumes;
      typedef std::map<std::string,PhysVol*>    Placements;
      typedef std::map<std::string,Catalog*>    Catalogs;
      typedef std::map<std::string,Isotope*>    Isotopes;
      typedef std::map<std::string,Element*>    Elements;
      typedef std::map<std::string,Material*>   Materials;
      typedef std::map<std::string,Shape*>      Shapes;
      /// Default constructor
      dddb();
      /// Default destructor
      virtual ~dddb();

      /// World dimensions
      Box        world;
      /// Inventory of catalogs
      Catalogs   catalogs, catalogPaths;
      /// Inventory of isotopes
      Isotopes   isotopes;
      /// Inventory of elements
      Elements   elements, elementPaths;
      /// Inventory of materials
      Materials  materials, materialPaths;
      /// Inventory of shapes used by the geometry
      Shapes     shapes;
      /// Inventory of logical volumes
      Volumes    volumes, volumePaths;
      /// Inventory of volume placements
      Placements placements, placementPaths;
      Catalog    *top, *structure, *geometry;
    };

    template <typename T> struct Increment {
      static int& counter() { static int cnt=0; return cnt; }
      Increment()   { ++counter(); }
      ~Increment()  { --counter(); }
    };
  } /* End namespace DDDB    */

  /// Specialized printout method. Not all above object types are supported.
  template <typename T> void dddb_print(const T* object);

} /* End namespace DD4hep    */

#endif /* DD4HEP_DDDB_DDDBCONVERSION_H  */
