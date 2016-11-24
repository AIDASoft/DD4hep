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
#include "DD4hep/ConditionsData.h"
#include "DDDB/DDDBReaderContext.h"

/// C/C++ include files
#include <string>
#include <vector>
#include <map>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the AIDA detector description toolkit supporting XML utilities
  namespace DDDB {

    /// Forward declarations
    class  Shape;
    class  LogVol;
    class  Catalog;
    class  TabProperty;
    class  Document;

    using Geometry::DetElement;
    using Geometry::Rotation3D;
    using Geometry::RotationZYX;
    using Geometry::Transform3D;
    using Geometry::Translation3D;
    using Geometry::Position;
    using Geometry::LCDD;

    /// Basic named object
    /**   \ingroup DD4HEP_DDDB
     */
    class Named : public Conditions::ClientData  {
    public:
      typedef std::map<std::string, std::string> StringMap;
      typedef std::map<std::string, std::pair<std::string,std::string > > StringPairMap;
      std::string  name, id;
      Document*    document;
      int          refCount;

      /// Default constructor
      Named();
      /// Initializing constructor
      Named(const std::string& c);
      /// Copy constructor
      Named(const Named& c);
      /// Default destructor
      virtual ~Named();
      /// Assignment operator
      Named& operator=(const Named& c);
      /// Assign document
      void setDocument(Document* doc);

      const char* c_name() const { return name.c_str(); }
      const char* c_id() const   { return id.c_str();   }
      void release()             { if ( --refCount <= 0 ) delete this;      }
    };

    /// Structure supporting basic XML document information
    /**   \ingroup DD4HEP_DDDB
     */
    class Document : public Named  {
    public:
      DDDBReaderContext context;
      /// Default constructor
      Document();
      /// Default destructor
      virtual ~Document();
      /// Reference count mechanism
      Document* addRef()  {  ++refCount; return this;  }
    };

    /// Intermediate structure representing author's data
    /**   \ingroup DD4HEP_DDDB
     */
    class Author : public Named {
    public:
      Author() : Named() {}
    };

    /// Intermediate structure representing versioning data
    /**   \ingroup DD4HEP_DDDB
     */
    class Version : Named {
    public:
      Version() : Named() {}
    };

    /// Intermediate structure representing data of one atom
    /**   \ingroup DD4HEP_DDDB
     */
    class Atom  {
    public:
      double A, Zeff;
      Atom() : A(0), Zeff(0) {}
    };

    /// Intermediate structure representing data of a Isotope
    /**   \ingroup DD4HEP_DDDB
     */
    class Isotope : public Named  {
    public:
      double A,Z,density;
      /// Default constructor
      Isotope();
      /// Default destructor
      virtual ~Isotope();
      /// Reference count mechanism
      Isotope* addRef()  {  ++refCount; return this;  }
    };

    /// Intermediate structure representing data of a Element
    /**   \ingroup DD4HEP_DDDB
     */
    class Element : public Named {
    public:
      enum {SOLID,LIQUID,GAS,UNKNOWN};
      std::vector<std::pair<std::string,double> > isotopes;
      std::string path, symbol;
      class Atom atom;
      double density, ionization;
      int state;

      /// Default constructor
      Element();
      /// Copy constructor
      Element(const Element& e);
      /// Default destructor
      virtual ~Element();
      /// Reference count mechanism
      Element* addRef()  {  ++refCount; return this;  }
    };

    /// Intermediate structure representing data of a material component
    /**   \ingroup DD4HEP_DDDB
     */
    class MaterialComponent  {
    public:
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
    class Material : public Named {
    public:
      typedef std::vector<MaterialComponent> Components;
      typedef std::vector<std::string>       Properties;
      std::string path;
      Components components;
      Properties properties;
      double density, pressure, temperature, radlen, lambda;
      /// Default constructor
      Material();
      /// Default destructor
      virtual ~Material();
      /// Reference count mechanism
      Material* addRef()  {  ++refCount; return this;  }
    };

    /// Structure supporting conversion of the Box shape
    /**   \ingroup DD4HEP_DDDB
     */
    class Box    {
    public:
      static int type() { return 100; }
      double x,y,z; 
      double dot2() const  { return x*x + y*y + z*z; }
    };

    /// Structure supporting conversion of the Cons shape
    /**   \ingroup DD4HEP_DDDB
     */
    class Cons                     {
    public:
      static int type() { return 102; } 
      double innerRadiusMZ, innerRadiusPZ, outerRadiusMZ, outerRadiusPZ, sizeZ;
    };

    /// Structure supporting conversion of the ConsSegment shape
    /**   \ingroup DD4HEP_DDDB
     */
    class ConeSegment                     {
    public:
      static int type() { return 103; } 
      double start, delta;      
      double innerRadiusMZ, innerRadiusPZ, outerRadiusMZ, outerRadiusPZ, sizeZ;
    };

    /// Structure supporting conversion of the Tubs shape
    /**   \ingroup DD4HEP_DDDB
     */
    class Tubs                     {
    public:
      static int type() { return 104; }
      double innerRadius, outerRadius, sizeZ, start, delta;
    };

    /// Structure supporting conversion of the elliptical tube shape
    /**   \ingroup DD4HEP_DDDB
     */
    class EllipticalTube   {
    public:
      static int type() { return 105; }
      double a, b, dz;
    };

    /// Structure supporting conversion of a z-plane
    /**   \ingroup DD4HEP_DDDB
     */
    class ZPlane                   {
    public:
      static int type() { return 106; }
      double innerRadius, outerRadius, z;
    };

    /// Structure supporting conversion of the Polycone shape
    /**   \ingroup DD4HEP_DDDB
     */
    class Polycone                 {
    public:
      static int type() { return 107; }
      double start, delta;
    };

    /// Structure supporting conversion of the Polycone shape
    /**   \ingroup DD4HEP_DDDB
     */
    class Polygon                 {
    public:
      static int type() { return 108; }
      double nsides, start, innerRadius, outerRadius, z;      
    };

    /// Structure supporting conversion of the torus
    /**   \ingroup DD4HEP_DDDB
     */
    class Torus   {
    public:
      static int type() { return 109; }
      double rmin, rmax, r, phi, dphi;
    };

    /// Structure supporting conversion of the Sphere
    /**   \ingroup DD4HEP_DDDB
     */
    class Sphere   {
    public:
      static int type() { return 110; }
      double rmin, rmax, theta, delta_theta, phi, delta_phi;
    };

    /// Structure supporting conversion of the Ellipsoid
    /**   \ingroup DD4HEP_DDDB
     */
    class Ellipsoid   {
    public:
      static int type() { return 111; }
      double rlow, rhigh, dz;
    };

    /// Structure supporting conversion of the Trap
    /**   \ingroup DD4HEP_DDDB
     */
    class Trap   {
    public:
      static int type() { return 112; }
      double dz, phi, theta, h1, bl1, tl1, alpha1, h2, bl2, tl2, alpha2;
    };

    /// Structure supporting conversion of the Paraboloid
    /**   \ingroup DD4HEP_DDDB
     */
    class Paraboloid   {
    public:
      static int type() { return 113; }
      double rlow, rhigh, dz;
    };

    /// Structure supporting conversion of the Paraboloid
    /**   \ingroup DD4HEP_DDDB
     */
    class Hyperboloid   {
    public:
      static int type() { return 114; }
      double rmin, rmax, stIn, stOut, dz;
    };

    /// Structure supporting conversion of the TRD
    /**   \ingroup DD4HEP_DDDB
     */
    class TRD   {
    public:
      static int type() { return 115; }
      double x1, x2, y1, y2, z;
    };

    /// Structure supporting conversion of the Assembly
    /**   \ingroup DD4HEP_DDDB
     */
    class Assembly  {
    public:
      static int type() { return 199; }
    };

    /// Structure supporting boolean shapes
    /**   \ingroup DD4HEP_DDDB
     */
    class BooleanShape            {
    public:
      class Shape* first;
    };

    /// Structure supporting conversion of boolean unions
    /**   \ingroup DD4HEP_DDDB
     */
    class BooleanUnion        : public BooleanShape  {
    public:
      static int type() { return 206; } 
    };

    /// Structure supporting conversion of boolean subtractions
    /**   \ingroup DD4HEP_DDDB
     */
    class BooleanSubtraction  : public BooleanShape  {
    public:
      static int type() { return 207; } 
    };

    /// Structure supporting conversion of boolean intersections
    /**   \ingroup DD4HEP_DDDB
     */
    class BooleanIntersection : public BooleanShape  {
    public:
      static int type() { return 208; } 
    };

    /// Structure supporting conversion of boolean operations
    /**   \ingroup DD4HEP_DDDB
     */
    class BooleanOperation {
    public:
      class Shape* shape;
      Transform3D trafo;
      BooleanOperation() : shape(0) {}
    };


    /// Structure supporting conversion of a physical volume
    /**   \ingroup DD4HEP_DDDB
     */
    class PhysVol : public Named  {
    public:
      enum { PHYSVOL_REGULAR=1,
             PHYSVOL_PARAM1D=2,
             PHYSVOL_PARAM2D=3,
             PHYSVOL_PARAM3D=4
      };
      int type;
      std::string    logvol, path;
      Transform3D trafo;
      /// Default constructor
      PhysVol();
      /// Copy constructor
      PhysVol(const PhysVol& c);
      /// Default destructor
      virtual ~PhysVol();
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
      /// Reference count mechanism
      PhysVol* addRef()  {  ++refCount; return this;  }
    };

    /// Structure supporting conversion of parametrized physical volumes
    /**   \ingroup DD4HEP_DDDB
     */
    class ParamPhysVol : public PhysVol {
    public:
      int number1;
      Transform3D trafo1;
      /// Default constructor
      ParamPhysVol() : PhysVol(), number1(0) { type = PHYSVOL_PARAM1D; }
      /// Copy constructor
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
    class ParamPhysVol2D : public ParamPhysVol  {
    public:
      int number2;
      Transform3D trafo2;
      /// Default constructor
      ParamPhysVol2D() : ParamPhysVol(), number2(0), trafo2()  { type = PHYSVOL_PARAM2D; }
    };

    /// Structure supporting conversion of 3D parametrized physical volumes
    /**   \ingroup DD4HEP_DDDB
     */
    class ParamPhysVol3D : public ParamPhysVol2D  {
    public:
      int number3;
      Transform3D trafo3;
      /// Default constructor
      ParamPhysVol3D() : ParamPhysVol2D(), number3(0), trafo3()  { type = PHYSVOL_PARAM3D; }
    };

    /// Structure supporting conversion of a logical volume
    /**   \ingroup DD4HEP_DDDB
     */
    class LogVol : public Named  {
    public:
      std::string   material, shape, path;
      std::vector<PhysVol*> physvols;
      /// Default constructor
      LogVol();
      /// Default destructor
      virtual ~LogVol();
      /// Reference count mechanism
      LogVol* addRef()  {  ++refCount; return this;  }
    };

    /// Structure supporting conversion of a detector element or a catalog
    /**   \ingroup DD4HEP_DDDB
     */
    class Catalog : public Named {
    public:
      typedef std::map<std::string, Catalog*>      CatRefs;
      typedef std::map<std::string, LogVol*>       LvRefs;
      typedef std::map<std::string, TabProperty*>  PropRefs;
      LvRefs        logvolrefs;
      LvRefs        logvols;
      CatRefs       catalogrefs;
      CatRefs       catalogs;
      PropRefs      tabpropertyrefs;
      StringPairMap params;
      StringMap     conditioninfo;
      std::string   type, path, author, version, logvol, condition, support, npath;
      int           level, typeID;
      /// Default constructor
      Catalog();
      Catalog(const Catalog&, const DetElement&) {}
      /// Default destructor
      virtual ~Catalog();
      /// Reference count mechanism
      Catalog* addRef()  {  ++refCount; return this;  }
      std::pair<const Catalog*,std::string> parent(const std::string& nam)  const;
    };

    /// Structure supporting conversion of any arbitrary shape
    /**   \ingroup DD4HEP_DDDB
     */
    class Shape : public Named {
    public:
      typedef std::vector<ZPlane>           ZPlanes;
      typedef std::vector<BooleanOperation> Operations;
      typedef std::vector<ParamPhysVol>     ParamVolumes;
      int type;
      union  {
        class Assembly             assembly;
        class Box                  box;
        class Cons                 cons;
        class Tubs                 tubs;
        class Polycone             polycone;
        class ConeSegment          coneSegment;
        class EllipticalTube       ellipticalTube;
        class Polygon              polygon;
        class Torus                torus;
        class Sphere               sphere;
        class Ellipsoid            ellipsoid;
        class Paraboloid           paraboloid;
        class Hyperboloid          hyperboloid;
        class TRD                  trd;
        class Trap                 trap;
        class BooleanShape         boolean;
        class BooleanUnion         boolean_union;
        class BooleanSubtraction   BooleanSubtraction;
        class BooleanIntersection  boolean_intersection;
      } s;
      ZPlanes zplanes;
      Operations boolean_ops;
      ParamVolumes paramVols;
      std::string path;
      /// Default constructor
      Shape();
      /// Default destructor
      ~Shape();
      /// Reference count mechanism
      Shape* addRef()  {  ++refCount; return this;  }
    };

    /// Tabulated property
    /**
     *   \author  M.Frank
     *   \version 1.0
     *   \date    31/03/2016
     *   \ingroup DD4HEP_DDDB
     */
    class TabProperty : public Named  {
    public:
      typedef std::pair<double, double> Entry;
      std::string path, type, xunit, yunit, xaxis, yaxis;
      std::vector<Entry> data;
      /// Default constructor
      TabProperty();
      /// Default destructor
      ~TabProperty();
      /// Reference count mechanism
      TabProperty* addRef()  {  ++refCount; return this;  }
    };

    /// LHCb geometry description interface to the conditions database
    /**
     *   \author  M.Frank
     *   \version 1.0
     *   \date    31/03/2016
     *   \ingroup DD4HEP_DDDB
     */
    class dddb   {
    public:
      typedef std::pair<long long int, long long int> iov_t;
      typedef std::map<std::string,std::string>   Refs;
      typedef std::map<std::string,Document*>     Documents;
      typedef std::map<std::string,LogVol*>       Volumes;
      typedef std::map<std::string,PhysVol*>      Placements;
      typedef std::map<std::string,Catalog*>      Catalogs;
      typedef std::map<std::string,Isotope*>      Isotopes;
      typedef std::map<std::string,Element*>      Elements;
      typedef std::map<std::string,Material*>     Materials;
      typedef std::map<std::string,Shape*>        Shapes;
      typedef std::map<std::string,TabProperty*>  TabProperties;
      typedef std::map<std::string,Conditions::Condition::Object*>  Conditions;

      /// Default constructor
      dddb();
      /// Default destructor
      virtual ~dddb();

      /// World dimensions
      Box        world;
      /// Inventory of input documents
      Documents  documents;
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
      /// Inventory of tabulated properties
      TabProperties tabproperties, tabpropertyPaths;
      /// Inventory of conditions
      Conditions conditions, conditionPaths;
      /// Inventory of catalogs
      Catalogs   catalogs, catalogPaths;
      /// Detector element hierarchy
      Catalog    *top, *structure, *geometry;
    };
    class dddb_conditions {};

    template <typename T> class Increment {
    public:
      static int& counter() { static int cnt=0; return cnt; }
      Increment()   { ++counter(); }
      ~Increment()  { --counter(); }
    };

  } /* End namespace DDDB    */

  /// Specialized printout method. Not all above object types are supported.
  template <typename T> void dddb_print(const T* object);

} /* End namespace DD4hep    */

#endif /* DD4HEP_DDDB_DDDBCONVERSION_H  */
