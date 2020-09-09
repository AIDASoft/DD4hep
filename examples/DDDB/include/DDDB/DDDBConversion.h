//==========================================================================
//  AIDA Detector description implementation 
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
#ifndef DDDB_DDDBCONVERSION_H
#define DDDB_DDDBCONVERSION_H

/// Framework include files
#include <DD4hep/Objects.h>
#include <DD4hep/ConditionsData.h>
#include <DDDB/DDDBReaderContext.h>

/// C/C++ include files
#include <string>
#include <vector>
#include <map>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the AIDA detector description toolkit supporting XML utilities
  namespace DDDB {

    /// Forward declarations
    class  DDDBShape;
    class  DDDBLogVol;
    class  DDDBCatalog;
    class  DDDBTabProperty;
    class  DDDBDocument;

    /// Basic named object
    /**   \ingroup DD4HEP_DDDB
     */
    class DDDBNamed : public cond::ClientData  {
    public:
      typedef std::map<std::string, std::string> StringMap;
      typedef std::map<std::string, std::pair<std::string,std::string > > StringPairMap;
      std::string  name, id;
      DDDBDocument* document = 0;
      int           refCount = 0;

      /// Default constructor
      DDDBNamed() = default;
      /// Initializing constructor
      DDDBNamed(const std::string& c);
      /// Copy constructor
      DDDBNamed(const DDDBNamed& c);
      /// Default destructor
      virtual ~DDDBNamed();
      /// Assignment operator
      DDDBNamed& operator=(const DDDBNamed& c);
      /// Assign document
      void setDocument(DDDBDocument* doc);

      const char* c_name() const { return name.c_str(); }
      const char* c_id() const   { return id.c_str();   }
      void release()             { if ( --refCount <= 0 ) delete this;      }
    };

    /// Structure supporting basic XML document information
    /**   \ingroup DD4HEP_DDDB
     */
    class DDDBDocument : public DDDBNamed  {
    public:
      DDDBReaderContext context;
      /// Default constructor
      DDDBDocument();
      /// Default destructor
      virtual ~DDDBDocument();
      /// Reference count mechanism
      DDDBDocument* addRef()  {  ++refCount; return this;  }
    };

    /// Intermediate structure representing author's data
    /**   \ingroup DD4HEP_DDDB
     */
    class DDDBAuthor : public DDDBNamed {
    public:
      DDDBAuthor() : DDDBNamed() {}
    };

    /// Intermediate structure representing versioning data
    /**   \ingroup DD4HEP_DDDB
     */
    class DDDBVersion : DDDBNamed {
    public:
      DDDBVersion() : DDDBNamed() {}
    };

    /// Intermediate structure representing data of one atom
    /**   \ingroup DD4HEP_DDDB
     */
    class DDDBAtom  {
    public:
      double A = 0E0, Zeff = 0E0;
      DDDBAtom() = default;
    };

    /// Intermediate structure representing data of a Isotope
    /**   \ingroup DD4HEP_DDDB
     */
    class DDDBIsotope : public DDDBNamed  {
    public:
      double A = 0E0,Z = 0E0,density = 0E0;
      /// Default constructor
      DDDBIsotope();
      /// Default destructor
      virtual ~DDDBIsotope();
      /// Reference count mechanism
      DDDBIsotope* addRef()  {  ++refCount; return this;  }
    };

    /// Intermediate structure representing data of a Element
    /**   \ingroup DD4HEP_DDDB
     */
    class DDDBElement : public DDDBNamed {
    public:
      enum {SOLID,LIQUID,GAS,UNKNOWN};
      std::vector<std::pair<std::string,double> > isotopes;
      std::string path, symbol;
      DDDBAtom atom;
      double density, ionization;
      int state;

      /// Default constructor
      DDDBElement();
      /// Copy constructor
      DDDBElement(const DDDBElement& e);
      /// Default destructor
      virtual ~DDDBElement();
      /// Reference count mechanism
      DDDBElement* addRef()  {  ++refCount; return this;  }
    };

    /// Intermediate structure representing data of a material component
    /**   \ingroup DD4HEP_DDDB
     */
    class DDDBMaterialComponent  {
    public:
      std::string name;
      double fractionmass;
      int natoms;
      DDDBMaterialComponent() : name(), fractionmass(-1), natoms(-1) {}
      DDDBMaterialComponent(const DDDBMaterialComponent& c) : name(c.name), fractionmass(c.fractionmass), natoms(c.natoms) {}
      DDDBMaterialComponent& operator=(const DDDBMaterialComponent& c)   {
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
    class DDDBMaterial : public DDDBNamed {
    public:
      typedef std::vector<DDDBMaterialComponent> Components;
      typedef std::vector<std::string>       Properties;
      std::string path;
      Components components;
      Properties properties;
      double density, pressure, temperature, radlen, lambda;
      /// Default constructor
      DDDBMaterial();
      /// Default destructor
      virtual ~DDDBMaterial();
      /// Reference count mechanism
      DDDBMaterial* addRef()  {  ++refCount; return this;  }
    };

    /// Structure supporting conversion of the Box shape
    /**   \ingroup DD4HEP_DDDB
     */
    class DDDBBox    {
    public:
      static int type() { return 100; }
      double x,y,z; 
      double dot2() const  { return x*x + y*y + z*z; }
    };

    /// Structure supporting conversion of the Cons shape
    /**   \ingroup DD4HEP_DDDB
     */
    class DDDBCons                     {
    public:
      static int type() { return 102; } 
      double innerRadiusMZ, innerRadiusPZ, outerRadiusMZ, outerRadiusPZ, sizeZ;
    };

    /// Structure supporting conversion of the ConsSegment shape
    /**   \ingroup DD4HEP_DDDB
     */
    class DDDBConeSegment                     {
    public:
      static int type() { return 103; } 
      double start, delta;      
      double innerRadiusMZ, innerRadiusPZ, outerRadiusMZ, outerRadiusPZ, sizeZ;
    };

    /// Structure supporting conversion of the Tubs shape
    /**   \ingroup DD4HEP_DDDB
     */
    class DDDBTubs                     {
    public:
      static int type() { return 104; }
      double innerRadius, outerRadius, sizeZ, start, delta;
    };

    /// Structure supporting conversion of the elliptical tube shape
    /**   \ingroup DD4HEP_DDDB
     */
    class DDDBEllipticalTube   {
    public:
      static int type() { return 105; }
      double a, b, dz;
    };

    /// Structure supporting conversion of a z-plane
    /**   \ingroup DD4HEP_DDDB
     */
    class DDDBZPlane                   {
    public:
      static int type() { return 106; }
      double innerRadius, outerRadius, z;
    };

    /// Structure supporting conversion of the Polycone shape
    /**   \ingroup DD4HEP_DDDB
     */
    class DDDBPolycone                 {
    public:
      static int type() { return 107; }
      double start, delta;
    };

    /// Structure supporting conversion of the Polycone shape
    /**   \ingroup DD4HEP_DDDB
     */
    class DDDBPolygon                 {
    public:
      static int type() { return 108; }
      double nsides, start, innerRadius, outerRadius, z;      
    };

    /// Structure supporting conversion of the torus
    /**   \ingroup DD4HEP_DDDB
     */
    class DDDBTorus   {
    public:
      static int type() { return 109; }
      double rmin, rmax, r, phi, dphi;
    };

    /// Structure supporting conversion of the Sphere
    /**   \ingroup DD4HEP_DDDB
     */
    class DDDBSphere   {
    public:
      static int type() { return 110; }
      double rmin, rmax, theta, delta_theta, phi, delta_phi;
    };

    /// Structure supporting conversion of the Ellipsoid
    /**   \ingroup DD4HEP_DDDB
     */
    class DDDBEllipsoid   {
    public:
      static int type() { return 111; }
      double rlow, rhigh, dz;
    };

    /// Structure supporting conversion of the Trap
    /**   \ingroup DD4HEP_DDDB
     */
    class DDDBTrap   {
    public:
      static int type() { return 112; }
      double dz, phi, theta, h1, bl1, tl1, alpha1, h2, bl2, tl2, alpha2;
    };

    /// Structure supporting conversion of the Paraboloid
    /**   \ingroup DD4HEP_DDDB
     */
    class DDDBParaboloid   {
    public:
      static int type() { return 113; }
      double rlow, rhigh, dz;
    };

    /// Structure supporting conversion of the Paraboloid
    /**   \ingroup DD4HEP_DDDB
     */
    class DDDBHyperboloid   {
    public:
      static int type() { return 114; }
      double rmin, rmax, stIn, stOut, dz;
    };

    /// Structure supporting conversion of the TRD
    /**   \ingroup DD4HEP_DDDB
     */
    class DDDBTRD   {
    public:
      static int type() { return 115; }
      double x1, x2, y1, y2, z;
    };

    /// Structure supporting conversion of the Assembly
    /**   \ingroup DD4HEP_DDDB
     */
    class DDDBAssembly  {
    public:
      static int type() { return 199; }
    };

    /// Structure supporting boolean shapes
    /**   \ingroup DD4HEP_DDDB
     */
    class DDDBBooleanShape            {
    public:
      class DDDBShape* first;
    };

    /// Structure supporting conversion of boolean unions
    /**   \ingroup DD4HEP_DDDB
     */
    class DDDBBooleanUnion        : public DDDBBooleanShape  {
    public:
      static int type() { return 206; } 
    };

    /// Structure supporting conversion of boolean subtractions
    /**   \ingroup DD4HEP_DDDB
     */
    class DDDBBooleanSubtraction  : public DDDBBooleanShape  {
    public:
      static int type() { return 207; } 
    };

    /// Structure supporting conversion of boolean intersections
    /**   \ingroup DD4HEP_DDDB
     */
    class DDDBBooleanIntersection : public DDDBBooleanShape  {
    public:
      static int type() { return 208; } 
    };

    /// Structure supporting conversion of boolean operations
    /**   \ingroup DD4HEP_DDDB
     */
    class DDDBBooleanOperation {
    public:
      class DDDBShape* shape;
      Transform3D trafo;
      DDDBBooleanOperation() : shape(0) {}
    };


    /// Structure supporting conversion of a physical volume
    /**   \ingroup DD4HEP_DDDB
     */
    class DDDBPhysVol : public DDDBNamed  {
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
      DDDBPhysVol();
      /// Copy constructor
      DDDBPhysVol(const DDDBPhysVol& c);
      /// Default destructor
      virtual ~DDDBPhysVol();
      DDDBPhysVol& operator=(const DDDBPhysVol& c) {
        if ( this != &c )  {
          this->DDDBNamed::operator=(c);
          type   = c.type;
          logvol = c.logvol;
          path   = c.path;
          trafo  = c.trafo;
        }
        return *this;
      }
      /// Reference count mechanism
      DDDBPhysVol* addRef()  {  ++refCount; return this;  }
    };

    /// Structure supporting conversion of parametrized physical volumes
    /**   \ingroup DD4HEP_DDDB
     */
    class DDDBParamPhysVol : public DDDBPhysVol {
    public:
      int number1;
      Transform3D trafo1;
      /// Default constructor
      DDDBParamPhysVol() : DDDBPhysVol(), number1(0) { type = PHYSVOL_PARAM1D; }
      /// Copy constructor
      DDDBParamPhysVol(const DDDBParamPhysVol& c) : DDDBPhysVol(c), number1(c.number1), trafo1(c.trafo1) {}
      DDDBParamPhysVol& operator=(const DDDBParamPhysVol& c) {
        if ( this != &c )  {
          this->DDDBPhysVol::operator=(c);
          trafo1 = c.trafo1;
          number1 = c.number1;
        }
        return *this;
      }
    };

    /// Structure supporting conversion of 2D parametrized physical volumes
    /**   \ingroup DD4HEP_DDDB
     */
    class DDDBParamPhysVol2D : public DDDBParamPhysVol  {
    public:
      int number2;
      Transform3D trafo2;
      /// Default constructor
      DDDBParamPhysVol2D() : DDDBParamPhysVol(), number2(0), trafo2()  { type = PHYSVOL_PARAM2D; }
    };

    /// Structure supporting conversion of 3D parametrized physical volumes
    /**   \ingroup DD4HEP_DDDB
     */
    class DDDBParamPhysVol3D : public DDDBParamPhysVol2D  {
    public:
      int number3;
      Transform3D trafo3;
      /// Default constructor
      DDDBParamPhysVol3D() : DDDBParamPhysVol2D(), number3(0), trafo3()  { type = PHYSVOL_PARAM3D; }
    };

    /// Structure supporting conversion of a logical volume
    /**   \ingroup DD4HEP_DDDB
     */
    class DDDBLogVol : public DDDBNamed  {
    public:
      std::string   material, shape, path;
      std::vector<DDDBPhysVol*> physvols;
      /// Default constructor
      DDDBLogVol();
      /// Default destructor
      virtual ~DDDBLogVol();
      /// Reference count mechanism
      DDDBLogVol* addRef()  {  ++refCount; return this;  }
    };

    /// Structure supporting conversion of a detector element or a catalog
    /**   \ingroup DD4HEP_DDDB
     */
    class DDDBCatalog : public DDDBNamed {
    public:
      typedef std::map<std::string, DDDBCatalog*>      CatRefs;
      typedef std::map<std::string, DDDBLogVol*>       LvRefs;
      typedef std::map<std::string, DDDBTabProperty*>  PropRefs;
      LvRefs        logvolrefs;
      LvRefs        logvols;
      CatRefs       catalogrefs;
      CatRefs       catalogs;
      PropRefs      tabpropertyrefs;
      StringPairMap params;
      StringMap     conditioninfo;
      std::string   type, path, author, version, logvol, condition, support, npath;
      int           level = 0, typeID = 0, classID = 0;
      /// Default constructor
      DDDBCatalog();
      /// Extension constructor (not used)
      DDDBCatalog(const DDDBCatalog& copy, const DetElement& de);
      /// Default destructor
      virtual ~DDDBCatalog();
      /// Assignemtn operator
      DDDBCatalog& operator=(const DDDBCatalog& copy) = default;
      /// Reference count mechanism
      DDDBCatalog* addRef()  {  ++refCount; return this;  }
      /// Access to parent
      std::pair<const DDDBCatalog*,std::string> parent(const std::string& nam)  const;
    };

    /// Structure supporting conversion of any arbitrary shape
    /**   \ingroup DD4HEP_DDDB
     */
    class DDDBShape : public DDDBNamed {
    public:
      typedef std::vector<DDDBZPlane>           ZPlanes;
      typedef std::vector<DDDBBooleanOperation> Operations;
      typedef std::vector<DDDBParamPhysVol>     ParamVolumes;
      int type;
      union  {
        class DDDBAssembly             assembly;
        class DDDBBox                  box;
        class DDDBCons                 cons;
        class DDDBTubs                 tubs;
        class DDDBPolycone             polycone;
        class DDDBConeSegment          coneSegment;
        class DDDBEllipticalTube       ellipticalTube;
        class DDDBPolygon              polygon;
        class DDDBTorus                torus;
        class DDDBSphere               sphere;
        class DDDBEllipsoid            ellipsoid;
        class DDDBParaboloid           paraboloid;
        class DDDBHyperboloid          hyperboloid;
        class DDDBTRD                  trd;
        class DDDBTrap                 trap;
        class DDDBBooleanShape         boolean;
        class DDDBBooleanUnion         boolean_union;
        class DDDBBooleanSubtraction   boolean_subtraction;
        class DDDBBooleanIntersection  boolean_intersection;
      } s;
      ZPlanes zplanes;
      Operations boolean_ops;
      ParamVolumes paramVols;
      std::string path;
      /// Default constructor
      DDDBShape();
      /// Default destructor
      ~DDDBShape();
      /// Reference count mechanism
      DDDBShape* addRef()  {  ++refCount; return this;  }
    };

    /// Tabulated property
    /**
     *   \author  M.Frank
     *   \version 1.0
     *   \date    31/03/2016
     *   \ingroup DD4HEP_DDDB
     */
    class DDDBTabProperty : public DDDBNamed  {
    public:
      typedef std::pair<double, double> Entry;
      std::string path, type, xunit, yunit, xaxis, yaxis;
      std::vector<Entry> data;
      /// Default constructor
      DDDBTabProperty();
      /// Default destructor
      ~DDDBTabProperty();
      /// Reference count mechanism
      DDDBTabProperty* addRef()  {  ++refCount; return this;  }
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
      typedef std::map<std::string,std::string>          Refs;
      typedef std::map<std::string,DDDBDocument*>        Documents;
      typedef std::map<std::string,DDDBLogVol*>          Volumes;
      typedef std::map<std::string,DDDBPhysVol*>         Placements;
      typedef std::map<std::string,DDDBCatalog*>         Catalogs;
      typedef std::map<std::string,DDDBIsotope*>         Isotopes;
      typedef std::map<std::string,DDDBElement*>         Elements;
      typedef std::map<std::string,DDDBMaterial*>        Materials;
      typedef std::map<std::string,DDDBShape*>           Shapes;
      typedef std::map<std::string,DDDBTabProperty*>     TabProperties;
      typedef std::map<std::string,Condition::Object*>   Conditions;

      /// Default constructor
      dddb();
      /// Default destructor
      virtual ~dddb();

      /// World dimensions
      DDDBBox        world;
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
      DDDBCatalog *top, *structure, *geometry;
    };

    /// Declaration of a tag-class to handle conditions
    /**   \ingroup DD4HEP_DDDB
     */
    class dddb_conditions  {};

    /// Helper to implement reference counting depending on types.
    /**   \ingroup DD4HEP_DDDB
     */
    template <typename T> class Increment {
    public:
      static int& counter() { static int cnt=0; return cnt; }
      Increment()   { ++counter(); }
      ~Increment()  { --counter(); }
    };

  } /* End namespace DDDB    */

  /// Specialized printout method. Not all above object types are supported.
  template <typename T> void dddb_print(const T* object);

} /* End namespace dd4hep    */

#endif // DDDB_DDDBCONVERSION_H
