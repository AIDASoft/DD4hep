// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#ifndef DD4HEP_GEOMETRY_OBJECTS_H
#define DD4HEP_GEOMETRY_OBJECTS_H

// Framework include files
#include "DD4hep/Handle.h"
class TMap;
class TGeoElement;
class TGeoMaterial;
class TGeoMedium;
class TGeoMatrix;
class TGeoRotation;
class TGeoTranslation;
class TGeoPhysicalNode;
class TGeoIdentity;
#include "TGeoPhysicalNode.h"

// C/C++ include files
#include <set>
#include <limits>

#define _USE_MATH_DEFINES
#include <cmath>
#ifndef M_PI
  #define M_PI 3.14159265358979323846
#endif


/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Geoemtry namespace declaration
   */
  namespace Geometry  {

    // Forward declarations
    struct LCDD;
    class IDDescriptor;


    /** Access to identity transformation  */
    TGeoIdentity* identityTransform();

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
      Constant(const std::string& name);
      /// Constructor to be used when creating a new DOM tree
      Constant(const std::string& name, const std::string& val);
      /// String representation of this object
      std::string toString()  const;
    };

    // We want the 3 coordinates ass-to-ass, so that they can be interpreted as "double*"
#ifdef _WIN32
#pragma pack(push,DD4Hep_Objects_Position,1)
#define DD4HEP_BYTE_ALIGN(x) x
#elif defined(__CINT__)
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
      Position(double xval, double yval, double zval) : x(xval), y(yval), z(zval) {         }
      /// Is it a identity rotation ?
      bool isNull() const                             { return x==0 && y==0 && z==0;        }
      /// Ceck 2 positions for equality
      bool operator==(const Position& c)  const       { return x==c.x && y==c.y && z==c.z;  }
      /// Ceck 2 positions for in-equality  
      bool operator!=(const Position& c)  const       { return x!=c.x || y!=c.y || x!=c.z;  }
      /// Negation of the direction
      Position operator - () const                    { return Position(-x,-y,-z);          }
      /// Scalar multiplication
      Position& operator *=(double a)                 { x *= a;y *= a;z *= a;return *this;  }
      /// Addition operator
      Position& operator +=(const Position& p)        { x+=p.x;y+=p.y;z+=p.z;return *this;  }
      /// Subtraction operator
      Position& operator -= (const Position& p)       { x-=p.x;y-=p.y;z-=p.z;return *this;  }

      /// Position length
      double length() const                           { return sqrt(x*x + y*y + z*z);       }
      Position& setLength(double new_length) {
	      double len=length();
	      if ( len>std::numeric_limits<double>::epsilon() ) {
	        len = new_length/len;
          x *= len;
          y *= len;
          z *= len;
        }
        else {
          x = 0;
          y = 0;
          z = 0;
        }
        return *this;
      }
      /// Rho - radius in cylindrical coordinates
      double rho() const                              { return std::sqrt(x*x+y*y);          }
      /// Phi - rotation angle around z in cylindrical coordinates
      double phi() const
      { return x == 0.0 && y == 0.0 ? 0.0 : std::atan2(x,y);                                }
      /// Theta - rotation angle around x in cylindrical coordinates
      double theta() const
      { return x == 0.0 && y == 0.0 && z == 0.0 ? 0.0 : std::atan2(std::sqrt(x*x+y*y),y);   }
      /// cos(Theta angle): optimisation for std::cos(pos.theta())
      double cosTheta() const
      { return x == 0.0 && y == 0.0 && z == 0.0 ? 1.0 : z/std::sqrt(x*x+y*y*z*z);           }
      /// Rotates the position vector around the x-axis.
      Position& rotateX(double angle_in_rad);
      /// Rotates the position vector around the y-axis.
      Position& rotateY(double angle_in_rad);
      /// Rotates the position vector around the z-axis.
      Position& rotateZ(double angle_in_rad);

      /// Access to array like coordinates
      const double* coordinates() const               { return &x;                          }
      /// Initializer for all member variables
      Position& set(double xv, double yv, double zv)  { x=xv; y=yv; z=zv; return *this;     }
    };
#ifdef _WIN32
#pragma pack(pop,DD4Hep_Objects_Position)
#pragma pack(push,DD4Hep_Objects_Rotation,1)
#endif
    /// Addition of 2 positions
    inline Position operator + (const Position& l, const Position& r)
    {  return Position(l.x+r.x,l.y+r.y,l.z+r.z);                                           }
    /// Subtraction of to positions
    inline Position operator - (const Position& l, const Position& r)
    {  return Position(l.x-r.x,l.y-r.y,l.z-r.z);                                           }
    /// Dot product of 3-vectors.
    inline double operator * (const Position& l, const Position& r)
    {  return sqrt(l.x*r.x + l.y*r.y + l.z*r.z);                                           }
    /// Positions scaling from left
    inline Position operator * (double l, const Position& r) 
    {  return Position(r.x*l,r.y*l,r.z*l);                                                 }
    /// Positions scaling from right
    inline Position operator * (const Position& l, double r)
    {  return Position(l.x*r,l.y*r,l.z*r);                                                 }
    /// Positions scaling from right
    inline Position operator / (const Position& l, double r)
    {  return Position(l.x/r,l.y/r,l.z/r);                                                 }
    /// Calculate the mean length of two vectors
    inline double mean_length(const Position& p1, const Position& p2)
    {  return 0.5* (p1.length() + p2.length()) / 2.0;                                      }
    /// Calculate the mean direction of two vectors
    inline Position mean_direction(const Position& p1, const Position& p2)
    { return 0.5 * (p1 + p2);                                                              }

    typedef Position Direction;
    typedef Position Momentum;

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
      bool isNull() const                              { return theta==0 && phi==0 && psi==0;              }
      /// Ceck 2 rotations for equality
      bool operator==(const Rotation& c)  const        { return theta==c.theta && phi==c.phi && psi==c.psi;}
      /// Ceck 2 rotations for in-equality 
      bool operator!=(const Rotation& c)  const        { return theta!=c.theta || phi!=c.phi || psi!=c.psi;}
      /// Access to array like coordinates
      const double* angles() const                     { return &theta;                                    }
      /// Initializer for all member variables
      Rotation& set(double th, double ph, double ps)   { theta=th; phi=ph; psi=ps;return *this;            }
      /// Rotates the rotation vector around the x-axis.
      Rotation& rotateX(double angle_in_rad);
      /// Rotates the rotation vector around the y-axis.
      Rotation& rotateY(double angle_in_rad);
      /// Rotates the rotation vector around the z-axis.
      Rotation& rotateZ(double angle_in_rad);
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
      Atom(const std::string& name, const std::string& formula, int Z, int N, double density);
    };

    /** @class Material Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Material : public Handle<TGeoMedium>  {
      /// Default constructor
      Material() : Handle<TGeoMedium>() {}
      /// Constructor to be used when creating a new DOM tree
      template <typename Q> 
      Material(const Handle<Q>& e) : Handle<TGeoMedium>(e) {}
      /// Constructor to be used when reading the already parsed DOM tree
      Material(const std::string& name);
      /// String representation of this object
      std::string toString()  const;
      /// Access the radiation length of the undrelying material
      double radLength() const;
    };

    /** @class VisAttr Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct VisAttr : public Ref_t  {
      enum Style { 
        SOLID=0x1,
        WIREFRAME=0x2,
        DASHED=0x2,
        LAST_STYLE
      };
      struct Object  {
	unsigned long magic;
	void*         col;
        int           color;
	float         alpha;
        unsigned char drawingStyle, lineStyle, showDaughters, visible;
        Object() : magic(magic_word()), col(0), color(0), alpha(0), drawingStyle(SOLID), lineStyle(SOLID), showDaughters(true), visible(true)  {}
      };
      /// Default constructor
      VisAttr() : Ref_t() {}
      /// Constructor to be used for assignment from object handle
      template <typename Q> 
      VisAttr(const Handle<Q>& e) : Ref_t(e) {}
      /// Copy constructor for handle
      VisAttr(const VisAttr& e) : Ref_t(e) {}
      /// Constructor to be used when creating a new registered visualization object
      VisAttr(const std::string& name);
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
      void setLineStyle(int style);

      /// Get drawing style
      int drawingStyle()  const;
      /// Set drawing style
      void setDrawingStyle(int style);

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
      AlignmentEntry(const std::string& path);
      /// Align the PhysicalNode (translation only)
      int align(const Position& pos, bool check=false, double overlap=0.001);
      /// Align the PhysicalNode (rotation only)
      int align(const Rotation& rot, bool check=false, double overlap=0.001);
      /// Align the PhysicalNode (translation + rotation)
      int align(const Position& pos, const Rotation& rot, bool check=false, double overlap=0.001);
    };


    /** @class Limit Objects.h
     *  Small object describing a limit structure 
     * 
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Limit {
      std::string particles;
      std::string name;
      std::string unit;
      std::string content;
      double      value;
      /// Default constructor
      Limit() : particles(), name(), unit(), content(), value(0.0) {}
      /// Copy constructor
      Limit(const Limit& c) : particles(c.particles), name(c.name), unit(c.unit), content(c.content), value(c.value)  {}
      /// Assignment operator
      Limit& operator=(const Limit& c);
      /// Equality operator
      bool operator==(const Limit& c) const;
      /// operator less
      bool operator< (const Limit& c) const;
      /// Conversion to a string representation
      std::string toString()  const;
    };

    /** @class LimitSet Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct LimitSet : public Ref_t  {
      typedef std::set<Limit> Object;  
      /// Constructor to be used when reading the already parsed DOM tree
      LimitSet() : Ref_t() {}
      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q> 
      LimitSet(const Handle<Q>& e) : Ref_t(e) {}
      /// Constructor to be used when creating a new DOM tree
      LimitSet(const std::string& name);
      /// Add new limit. Returns true if the new limit was added, false if it already existed.
      bool addLimit(const Limit& limit);
      /// Accessor to limits container
      const Object& limits() const;
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
	std::vector<std::string> user_limits;
      };
      /// Default constructor
      Region() : Ref_t() {}
      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q> 
      Region(const Handle<Q>& e) : Ref_t(e) {}
      /// Constructor to be used when creating a new DOM tree
      Region(const std::string& name);

      /// Additional data accessor
      Object& _data()   const {  return *data<Object>();  }
      Region& setStoreSecondaries(bool value);
      Region& setThreshold(double value);
      Region& setCut(double value);
      Region& setLengthUnit(const std::string& unit);
      Region& setEnergyUnit(const std::string& unit);
      /// Access references to user limits
      std::vector<std::string>& limits() const;

      /// Access cut value
      double  cut() const;
      /// Access production threshold
      double threshold() const;
      /// Access secondaries flag
      bool storeSecondaries() const;
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
}         /* End namespace DD4hep             */
#endif    /* DD4HEP_GEOMETRY_OBJECTS_H        */
