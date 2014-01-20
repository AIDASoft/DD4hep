// $Id$
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
#include "Math/Vector3D.h"
#include "Math/Transform3D.h"
#include "Math/Translation3D.h"
#include "Math/RotationX.h"
#include "Math/RotationY.h"
#include "Math/RotationZ.h"
#include "Math/Rotation3D.h"
#include "Math/RotationZYX.h"
#include "Math/EulerAngles.h"
#include "Math/VectorUtil.h"

// C/C++ include files
#include <set>
#include <limits>
#include <vector>

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
  namespace Geometry {

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
    struct Author: public Ref_t {
      /// Definition of the implementation type
      typedef TNamed Object;
      /// Default constructor
      Author()
          : Ref_t() {
      }
      /// Constructorto be used for assignment from a handle
      Author(const Author& e)
          : Ref_t(e) {
      }
      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q>
      Author(const Handle<Q>& e)
          : Ref_t(e) {
      }
      /// Constructor to be used when creating a new DOM tree
      Author(LCDD& doc);
      /// Assignment operator
      Author& operator=(const Author& e)  {
	m_element = e.m_element;
	return *this;
      }
      /// Access the auhor's name
      std::string authorName() const;
      /// Set the author's name
      void setAuthorName(const std::string& nam);
      /// Access the auhor's email address
      std::string authorEmail() const;
      /// Set the author's email address
      void setAuthorEmail(const std::string& addr);
    };

    /** @class Header Objects.h
     *
     *  Description of the geometry header. Containes useful auxiliary information.
     *
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Header: public Ref_t {
      struct Object: public TNamed {
      public:
        std::string url;
        std::string author;
        std::string status;
        std::string version;
        std::string comment;
        /// Standard constructor
        Object();
        /// Default destructor
        virtual ~Object();
      private:
	/// Private copy constructor
        Object(const Object&) : TNamed() {}
	/// Private assignment operator
	Object& operator=(const Object&) { return *this; }
      };
      /// Default constructor
      Header()
          : Ref_t() {
      }
      /// Constructorto be used for assignment from a handle
      Header(const Header& e)
          : Ref_t(e) {
      }
      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q> Header(const Handle<Q>& e)
          : Ref_t(e) {
      }
      /// Constructor to be used when creating a new DOM tree
      Header(const std::string& author, const std::string& url);
      /// Accessor to object name
      const std::string name() const;
      /// Accessor: set object name
      void setName(const std::string& new_name);
      /// Accessor to object title
      const std::string title() const;
      /// Accessor: set object title
      void setTitle(const std::string& new_title);
      /// Accessor to object author
      const std::string& author() const;
      /// Accessor: set object author
      void setAuthor(const std::string& new_author);
      /// Accessor to object url
      const std::string& url() const;
      /// Accessor: set object url
      void setUrl(const std::string& new_url);
      /// Accessor to object status
      const std::string& status() const;
      /// Accessor: set object status
      void setStatus(const std::string& new_status);
      /// Accessor to object version
      const std::string& version() const;
      /// Accessor: set object version
      void setVersion(const std::string& new_version);
      /// Accessor to object comment
      const std::string& comment() const;
      /// Accessor: set object comment
      void setComment(const std::string& new_comment);
    };

    /** @class Constant Objects.h
     *
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Constant: public Ref_t {
      /// Definition of the implementation type
      typedef TNamed Object;
      /// Default constructor
      Constant()
          : Ref_t() {
      }
      /// Constructorto be used for assignment from a handle
      Constant(const Constant& e)
          : Ref_t(e) {
      }
      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q>
      Constant(const Handle<Q>& e)
          : Ref_t(e) {
      }
      /// Constructor to be used when creating a new DOM tree
      Constant(const std::string& name);
      /// Constructor to be used when creating a new DOM tree
      Constant(const std::string& name, const std::string& val);
      /// String representation of this object
      std::string toString() const;
    };

    typedef ROOT::Math::XYZVector Position;
    template <class V> V RotateX(const V& v, double a) {
      return ROOT::Math::VectorUtil::RotateX(v, a);
    }
    template <class V> V RotateY(const V& v, double a) {
      return ROOT::Math::VectorUtil::RotateY(v, a);
    }
    template <class V> V RotateZ(const V& v, double a) {
      return ROOT::Math::VectorUtil::RotateZ(v, a);
    }

    /** Rotation class with the (3D) rotation represented by
     *  angles describing first a rotation of
     *  an angle phi (yaw) about the  Z axis,
     *  followed by a rotation of an angle theta (pitch) about the new Y' axis,
     *  followed by a third rotation of an angle psi (roll) about the final X'' axis.
     *  This is  sometimes referred to as the Euler 321 sequence.
     *  It has not to be confused with the typical Goldstein definition of the Euler Angles
     *  (Z-X-Z or 313 sequence) which is used by the ROOT::Math::EulerAngles class.
     *  @see http://root.cern.ch/root/html/ROOT__Math__RotationZYX.html
     */
    typedef ROOT::Math::RotationZYX RotationZYX;
    /**  @see http://root.cern.ch/root/html/ROOT__Math__RotationZ.html */
    typedef ROOT::Math::RotationZ RotationZ;
    /**  @see http://root.cern.ch/root/html/ROOT__Math__RotationY.html */
    typedef ROOT::Math::RotationY RotationY;
    /**  @see http://root.cern.ch/root/html/ROOT__Math__RotationX.html */
    typedef ROOT::Math::RotationX RotationX;
    /**  @see http://root.cern.ch/root/html/ROOT__Math__Rotation3D.html */
    typedef ROOT::Math::Rotation3D Rotation3D;
    /**  @see http://root.cern.ch/root/html/ROOT__Math__EulerAngels.html */
    typedef ROOT::Math::EulerAngles EulerAngles;
    /**  @see http://root.cern.ch/root/html/ROOT__Math__Transform3D.html */
    typedef ROOT::Math::Transform3D Transform3D;
    /**  @see http://root.cern.ch/root/html/ROOT__Math__Translations3D.html */
    typedef ROOT::Math::Translation3D Translation3D;

    /** @class Atom Objects.h
     *
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Atom: public Handle<TGeoElement> {
      /// Definition of the implementation type
      typedef TGeoElement Object;
      /// Default constructor
      Atom()
          : Handle<TGeoElement>() {
      }
      /// Constructorto be used for assignment from a handle
      Atom(const Handle<TGeoElement>& e)
          : Handle<TGeoElement>(e) {
      }
      /// Constructor to be used when creating from a object handle
      template <typename Q>
      Atom(const Handle<Q>& e)
          : Handle<TGeoElement>(e) {
      }
      /// Constructor to be used when reading the already parsed DOM tree
      Atom(const std::string& name, const std::string& formula, int Z, int N, double density);
    };

    /** @class Material Objects.h
     *
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Material: public Handle<TGeoMedium> {
      /// Definition of the implementation type
      typedef TGeoMedium Object;
      /// Default constructor
      Material()
          : Handle<TGeoMedium>() {
      }
      /// Constructorto be used for assignment from material handle
      Material(const Handle<TGeoMedium>& e)
          : Handle<TGeoMedium>(e) {
      }
      /// Constructorto be used for assignment from object handle
      template <typename Q>
      Material(const Handle<Q>& e)
          : Handle<TGeoMedium>(e) {
      }
      /// String representation of this object
      std::string toString() const;
      /// Access the radiation length of the underlying material
      double radLength() const;
      /// Access the interaction length of the underlying material
      double intLength() const;
    };

    /** @class VisAttr Objects.h
     *
     *  @author  M.Frank
     *  @version 1.0
     */
    struct VisAttr: public Ref_t {
      enum Style {
        SOLID = 0x1, WIREFRAME = 0x2, DASHED = 0x2, LAST_STYLE
      };
      struct Object: public TNamed {
        unsigned long magic;
        void* col;
        int color;
        float alpha;
        unsigned char drawingStyle, lineStyle, showDaughters, visible;
        /// Standard constructor
        Object();
        /// Default destructor
        virtual ~Object();
      };
      /// Default constructor
      VisAttr()
          : Ref_t() {
      }
      /// Constructor to be used for assignment from object handle
      template <typename Q>
      VisAttr(const Handle<Q>& e)
          : Ref_t(e) {
      }
      /// Copy constructor for handle
      VisAttr(const VisAttr& e)
          : Ref_t(e) {
      }
      /// Constructor to be used when creating a new registered visualization object
      VisAttr(const std::string& name);
      /// Assignment operator
      VisAttr& operator=(const VisAttr& attr) {
        m_element = attr.m_element;
        return *this;
      }

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
      int drawingStyle() const;
      /// Set drawing style
      void setDrawingStyle(int style);

      /// Get alpha value
      float alpha() const;
      /// Set alpha value
      void setAlpha(float value);

      /// Get object color
      int color() const;
      /// Set object color
      void setColor(float red, float green, float blue);

      /// Get RGB values of the color (if valid)
      bool rgb(float& red, float& green, float& blue) const;

      /// String representation of this object
      std::string toString() const;
    };

    /**@class AligmentEntry
     *
     * Class representing an alignment entry
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct AlignmentEntry: public Handle<TGeoPhysicalNode> {
      typedef Handle<TGeoPhysicalNode> Base;
      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q>
      AlignmentEntry(const Handle<Q>& h)
          : Base(h) {
      }
      /// Constructor to be used when creating a new aligment entry
      AlignmentEntry(const std::string& path);
      /// Align the PhysicalNode (translation only)
      int align(const Position& pos, bool check = false, double overlap = 0.001);
      /// Align the PhysicalNode (rotation only)
      int align(const RotationZYX& rot, bool check = false, double overlap = 0.001);
      /// Align the PhysicalNode (translation + rotation)
      int align(const Position& pos, const RotationZYX& rot, bool check = false, double overlap = 0.001);
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
      double value;
      /// Default constructor
      Limit()
          : particles(), name(), unit(), content(), value(0.0) {
      }
      /// Copy constructor
      Limit(const Limit& c)
          : particles(c.particles), name(c.name), unit(c.unit), content(c.content), value(c.value) {
      }
      /// Assignment operator
      Limit& operator=(const Limit& c);
      /// Equality operator
      bool operator==(const Limit& c) const;
      /// operator less
      bool operator<(const Limit& c) const;
      /// Conversion to a string representation
      std::string toString() const;
    };

    /** @class LimitSet Objects.h
     *
     *  @author  M.Frank
     *  @version 1.0
     */
    struct LimitSet: public Ref_t {
      struct Object: public TNamed, public std::set<Limit> {
        /// Standard constructor
        Object();
        /// Default destructor
        virtual ~Object();
      };
      /// Constructor to be used when reading the already parsed DOM tree
      LimitSet()
          : Ref_t() {
      }
      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q>
      LimitSet(const Handle<Q>& e)
          : Ref_t(e) {
      }
      /// Constructor to be used when creating a new object
      LimitSet(const std::string& name);
      /// Add new limit. Returns true if the new limit was added, false if it already existed.
      bool addLimit(const Limit& limit);
      /// Accessor to limits container
      const std::set<Limit>& limits() const;
    };

    /** @class Region Objects.h
     *
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Region: public Ref_t {
      struct Object: public TNamed {
        unsigned long magic;
        double threshold;
        double cut;
        bool store_secondaries;
        std::string lunit, eunit;
        std::vector<std::string> user_limits;
        /// Standard constructor
        Object();
        /// Default destructor
        virtual ~Object();
      };
      /// Default constructor
      Region()
          : Ref_t() {
      }
      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q>
      Region(const Handle<Q>& e)
          : Ref_t(e) {
      }
      /// Constructor to be used when creating a new object
      Region(const std::string& name);

      Region& setStoreSecondaries(bool value);
      Region& setThreshold(double value);
      Region& setCut(double value);
      Region& setLengthUnit(const std::string& unit);
      Region& setEnergyUnit(const std::string& unit);
      /// Access references to user limits
      std::vector<std::string>& limits() const;

      /// Access cut value
      double cut() const;
      /// Access production threshold
      double threshold() const;
      /// Access secondaries flag
      bool storeSecondaries() const;
      /// Access the length unit
      const std::string& lengthUnit() const;
      /// Access the energy unit
      const std::string& energyUnit() const;
    };

  } /* End namespace Geometry           */
} /* End namespace DD4hep             */

namespace ROOT {
  namespace Math {
    typedef DD4hep::Geometry::Position Position;
    /// Dot product of 3-vectors.
    inline double operator *(const Position& l, const Position& r) {
      return sqrt(l.X() * r.X() + l.Y() * r.Y() + l.Z() * r.Z());
    }
    /// Calculate the mean length of two vectors
    inline double mean_length(const Position& p1, const Position& p2) {
      return 0.5 * (p1.R() + p2.R()) / 2.0;
    }
    /// Calculate the mean direction of two vectors
    inline Position mean_direction(const Position& p1, const Position& p2) {
      return 0.5 * (p1 + p2);
    }

  }
}

#endif    /* DD4HEP_GEOMETRY_OBJECTS_H        */
