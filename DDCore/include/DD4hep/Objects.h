// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#ifndef DD4hep_GEOMETRY_OBJECTS_H
#define DD4hep_GEOMETRY_OBJECTS_H

// Framework include files
#include "DD4hep/Handle.h"
class TMap;
class TGeoElement;
class TGeoMaterial;
class TGeoMatrix;
class TGeoRotation;
class TGeoTranslation;
class TGeoPhysicalNode;
#include "TGeoPhysicalNode.h"

// C/C++ include files
#define _USE_MATH_DEFINES
#include <cmath>
#ifndef M_PI
  #define M_PI 3.14159265358979323846
#endif
/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  // Forward declarations
  class IDDescriptor;

  /*
   *   Geoemtry namespace declaration
   */
  namespace Geometry  {

    // Forward declarations
    struct LCDD;

    /** @class Author Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Author : public Ref_t  {
      /// Default constructor
      Author() : Ref_t() {}
      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q> 
      Author(const Handle<Q>& e) : Ref_t(e)  {}
      /// Constructor to be used when creating a new DOM tree
      Author(LCDD& doc);
      //void setAuthorName(const char* nam)    {  setAttr("name",nam); }
      //void setAuthorEmail(const char* addr)  {  setAttr("email",addr); }
    };

    /** @class Header Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Header : public Ref_t  {
      /// Default constructor
      Header() : Ref_t() {}
      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q> 
      Header(const Handle<Q>& e) : Ref_t(e)  {}
      /// Constructor to be used when creating a new DOM tree
      Header(LCDD& doc);
    };

    /** @class Constant Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Constant : public Ref_t  {
      /// Default constructor
      Constant() : Ref_t() {}
      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q> 
      Constant(const Handle<Q>& e) : Ref_t(e)  {}
      /// Constructor to be used when creating a new DOM tree
      Constant(LCDD& doc, const std::string& name);
      /// Constructor to be used when creating a new DOM tree
      Constant(LCDD& doc, const std::string& name, const std::string& val);
      /// String representation of this object
      std::string toString()  const;
    };

    // We want the 3 coordinates ass-to-ass, so that they can be interpreted as "double*"
#ifdef _WIN32
#pragma pack(push,DD4Hep_Objects_Position,1)
#define DD4HEP_BYTE_ALIGN(x) x
#else
#define DD4HEP_BYTE_ALIGN(x) x __attribute__((__packed__))
#endif
    /** @class Position Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    DD4HEP_BYTE_ALIGN(struct) Position  {
      /// 3-dimensional cartesian coordinates
      double x, y, z;
      /// Default constructor
      Position() : x(0), y(0), z(0) {}
      /// Initializing constructor
      Position(double xval, double yval, double zval) : x(xval), y(yval), z(zval) {     }
      /// Is it a identity rotation ?
      bool isNull() const                             { return x==0 && y==0 && z==0;    }
      /// Access to array like coordinates
      const double* coordinates() const               { return &x;                      }
      /// Initializer for all member variables
      Position& set(double xv, double yv, double zv)  { x=xv; y=yv; z=zv; return *this; }
    };
#ifdef _WIN32
#pragma pack(pop,DD4Hep_Objects_Position)
#pragma pack(push,DD4Hep_Objects_Rotation,1)
#endif
    /** @class Rotation Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    DD4HEP_BYTE_ALIGN(struct) Rotation  {
      double theta, phi, psi;
      /// Default constructor
      Rotation() : theta(0), phi(0), psi(0) {}
      /// Initializing constructor
      Rotation(double thetaval, double phival, double psival) : theta(thetaval), phi(phival), psi(psival) {}
      /// Is it a identity rotation ?
      bool isNull() const                              { return theta==0 && phi==0 && psi==0;   }
      /// Access to array like coordinates
      const double* angles() const                     { return &theta;                         }
      /// Initializer for all member variables
      Rotation& set(double th, double ph, double ps)   { theta=th; phi=ph; psi=ps;return *this; }
    };

#ifdef _WIN32
#pragma pack(pop,DD4Hep_Objects_Rotation,1)
#endif
#undef DD4HEP_BYTE_ALIGN

    /** @class IdentityPos Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct IdentityPos {
      /// Default constructor
      IdentityPos() {}
    };

    /** @class IdentityRot Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct IdentityRot {
      /// Default constructor
      IdentityRot() {}
    };

    /** @class ReflectRot Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct ReflectRot : public Rotation {
      /// Default constructor
      ReflectRot() : Rotation(M_PI,0.,0.) {}
    };
#if 0
    /** @class Transformation Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Transform : public Handle<TGeoMatrix> {
      /// Default constructor
      Transform() : Handle<TGeoMatrix>() {}
      /// Default constructor
      Transform(TGeoMatrix* m) : Handle<TGeoMatrix>(m) {}
      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q> 
      Transform(const Handle<Q>& e) : Handle<TGeoMatrix>(e) {}
      /// Constructor to be used when creating a new DOM tree. Automatically sets attributes
      Transform(LCDD& doc, const std::string& name);
    };

#endif
    /** @class Atom Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Atom : public Handle<TGeoElement>  {
      /// Default constructor
      Atom() : Handle<TGeoElement>() {}
      /// Constructor to be used when creating a new DOM tree
      template <typename Q> 
      Atom(const Handle<Q>& e) : Handle<TGeoElement>(e) {}
      /// Constructor to be used when reading the already parsed DOM tree
      Atom(LCDD& doc, const std::string& name, const std::string& formula, int Z, int N, double density);
    };

    /** @class Material Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Material : public Handle<TGeoMaterial>  {
      /// Default constructor
      Material() : Handle<TGeoMaterial>() {}
      /// Constructor to be used when creating a new DOM tree
      template <typename Q> 
      Material(const Handle<Q>& e) : Handle<TGeoMaterial>(e) {}
      /// Constructor to be used when reading the already parsed DOM tree
      Material(LCDD& doc, const std::string& name);
      /// String representation of this object
      std::string toString()  const;
    };

    /** @class VisAttr Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct VisAttr : public Ref_t  {
      struct Object  {
	unsigned long magic;
	void*         col;
        int           color;
	float         alpha;
        unsigned char drawingStyle, lineStyle, showDaughters, visible;
        Object() : magic(magic_word()), col(0), color(0), alpha(0), drawingStyle(true), showDaughters(true), visible(true)  {}
      };
      enum DrawingStyle { 
        WIREFRAME=0x2,
        LAST_DRAWING_STYLE
      };
      enum LineStyle  {
        SOLID=0x1,
        DASHED=0x2,
        LAST_LINE_STYLE
      };
      /// Default constructor
      VisAttr() : Ref_t() {}
      /// Constructor to be used for assignment from object handle
      template <typename Q> 
      VisAttr(const Handle<Q>& e) : Ref_t(e) {}
      /// Copy constructor for handle
      VisAttr(const VisAttr& e) : Ref_t(e) {}
      /// Constructor to be used when creating a new registered visualization object
      VisAttr(LCDD& doc, const std::string& name);
      /// Additional data accessor
      Object& _data()   const {  return *data<Object>();  }

      /// Get Flag to show/hide daughter elements
      bool showDaughters() const;
      /// Set Flag to show/hide daughter elements
      void setShowDaughters(bool value);

      /// Get visibility flag
      bool visible() const;
      /// Set visibility flag
      void setVisible(bool value);

      /// Get line style
      int lineStyle() const;
      /// Set line style
      void setLineStyle(LineStyle style);

      /// Get drawing style
      int drawingStyle()  const;
      /// Set drawing style
      void setDrawingStyle(DrawingStyle style);

      /// Get alpha value
      float alpha() const;
      /// Set alpha value
      void setAlpha(float value);

      /// Get object color
      int color()   const;
      /// Set object color
      void setColor(float red, float green, float blue);

      /// Get RGB values of the color (if valid)
      bool rgb(float& red, float& green, float& blue) const;

      /// String representation of this object
      std::string toString()  const;
    };

    /**@class AligmentEntry
     * 
     * Class representing an alignment entry 
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct AlignmentEntry : public Handle<TGeoPhysicalNode>  {
      typedef Handle<TGeoPhysicalNode> Base;
      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q> 
      AlignmentEntry(const Handle<Q>& h) : Base(h) {}
      /// Constructor to be used when creating a new aligment entry
      AlignmentEntry(LCDD& doc, const std::string& path);
      /// Align the PhysicalNode (translation only)
      int align(const Position& pos, bool check=false, double overlap=0.001);
      /// Align the PhysicalNode (rotation only)
      int align(const Rotation& rot, bool check=false, double overlap=0.001);
      /// Align the PhysicalNode (translation + rotation)
      int align(const Position& pos, const Rotation& rot, bool check=false, double overlap=0.001);
    };


    /** @class Limit Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Limit : public Ref_t  {
      typedef std::pair<std::string,double> Object;

      /// Constructor to be used when creating a new limit object
      Limit(LCDD& doc, const std::string& name);
      /// Additional data accessor
      Object& _data()   const {  return *data<Object>();  }
      void setParticles(const std::string& particleNames);
      void setValue(double value);
      void setUnit(const std::string& unit);
    };

    /** @class LimitSet Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct LimitSet : public Ref_t  {
      typedef TMap Object;
      /// Constructor to be used when reading the already parsed DOM tree
      LimitSet() : Ref_t() {}
      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q> 
      LimitSet(const Handle<Q>& e) : Ref_t(e) {}
      /// Constructor to be used when creating a new DOM tree
      LimitSet(LCDD& doc, const std::string& name);
      void addLimit(const Ref_t& limit);
    };

    /** @class Region Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Region : public Ref_t  {
      struct Object  {
	unsigned long magic;
        double        threshold;
        double        cut;
        bool          store_secondaries;
        std::string   lunit, eunit;
      };
      /// Default constructor
      Region() : Ref_t() {}
      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q> 
      Region(const Handle<Q>& e) : Ref_t(e) {}
      /// Constructor to be used when creating a new DOM tree
      Region(LCDD& doc, const std::string& name);

      /// Additional data accessor
      Object& _data()   const {  return *data<Object>();  }
      Region& setStoreSecondaries(bool value);
      Region& setThreshold(double value);
      Region& setCut(double value);
      Region& setLengthUnit(const std::string& unit);
      Region& setEnergyUnit(const std::string& unit);
    };

    /** @class IDSpec Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct IDSpec : public Ref_t   {
      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q> 
      IDSpec(const Handle<Q>& e) : Ref_t(e) {}
      /// Constructor to be used when creating a new DOM tree
      IDSpec(LCDD& doc, const std::string& name, const IDDescriptor& dsc);
      void addField(const std::string& name, const std::pair<int,int>& field);
    };

  }       /* End namespace Geometry           */
}         /* End namespace DD4hep            */
#endif    /* DD4hep_GEOMETRY_OBJECTS_H       */
