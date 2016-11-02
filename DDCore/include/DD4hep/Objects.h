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
#ifndef DD4HEP_GEOMETRY_OBJECTS_H
#define DD4HEP_GEOMETRY_OBJECTS_H

// Framework include files
#include "DD4hep/Handle.h"
#include "DD4hep/NamedObject.h"

// Forward declarations
class TMap;
class TGeoElement;
class TGeoMaterial;
class TGeoMedium;
class TGeoMatrix;
class TGeoRotation;
class TGeoTranslation;
class TGeoPhysicalNode;
class TGeoIdentity;

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated" // Code that causes warning goes here
#endif
// ROOT include files
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
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

// C/C++ include files
#include <set>
#include <cmath>
#include <limits>
#include <vector>

#define _USE_MATH_DEFINES
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {

    // Forward declarations
    class LCDD;
    class IDDescriptor;
    class VisAttrObject;
    class HeaderObject;
    class ConstantObject;
    class RegionObject;
    class LimitSetObject;

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
     *  \see http://root.cern.ch/root/html/ROOT__Math__RotationZYX.html
     */
    typedef ROOT::Math::RotationZYX   RotationZYX;
    /**  \see http://root.cern.ch/root/html/ROOT__Math__RotationZ.html */
    typedef ROOT::Math::RotationZ     RotationZ;
    /**  \see http://root.cern.ch/root/html/ROOT__Math__RotationY.html */
    typedef ROOT::Math::RotationY     RotationY;
    /**  \see http://root.cern.ch/root/html/ROOT__Math__RotationX.html */
    typedef ROOT::Math::RotationX     RotationX;
    /**  \see http://root.cern.ch/root/html/ROOT__Math__Rotation3D.html */
    typedef ROOT::Math::Rotation3D    Rotation3D;
    /**  \see http://root.cern.ch/root/html/ROOT__Math__EulerAngels.html */
    typedef ROOT::Math::EulerAngles   EulerAngles;
    /**  \see http://root.cern.ch/root/html/ROOT__Math__Transform3D.html */
    typedef ROOT::Math::Transform3D   Transform3D;
    /**  \see http://root.cern.ch/root/html/ROOT__Math__Translations3D.html */
    typedef ROOT::Math::Translation3D Translation3D;

    /** Access to identity transformation  */
    TGeoIdentity* identityTransform();

    
    /// Handle class describing an author entity
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
     */
    class Author: public Ref_t {
    public:
      /// Definition of the implementation type
      typedef NamedObject Object;
      /// Default constructor
      Author()
        : Ref_t() {
      }
      /// Constructorto be used for assignment from a handle
      Author(const Author& e)
        : Ref_t(e) {
      }
      /// Constructor to be used when assigning already valid handle
      template <typename Q>
      Author(const Handle<Q>& e)
        : Ref_t(e) {
      }
      /// Constructor to be used when creating a new object
      Author(LCDD& doc);
      /// Assignment operator
      Author& operator=(const Author& e)  {
        if ( this != & e )  {
          m_element = e.m_element;
        }
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

    /// Handle class describing the basic information about geometry objects as it is defined in LCDD
    /**
     *  Description of the geometry header. Containes useful auxiliary information.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
     */
    class Header: public Handle<HeaderObject> {
    public:
      typedef HeaderObject Object;
      /// Default constructor
      Header()
        : Handle<HeaderObject>() {
      }
      /// Constructorto be used for assignment from a handle
      Header(const Header& e)
        : Handle<HeaderObject>(e) {
      }
      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q> Header(const Handle<Q>& e)
        : Handle<HeaderObject>(e) {
      }
      /// Constructor to be used when creating a new DOM tree
      Header(const std::string& author, const std::string& url);
      /// Assignment operator
      Header& operator=(const Header& e)  {
        if ( this != & e )  {
          m_element = e.m_element;
        }
        return *this;
      }
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

    /// Handle class describing a constant (define) object in lcdd
    /**
     *  Constant objects are parsed by the expression evaluator and
     *  are subsequently available for expression evaluation.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
     */
    class Constant: public Handle<ConstantObject> {
    public:
      /// Definition of the implementation type
      typedef ConstantObject Object;
      /// Default constructor
      Constant()
        : Handle<ConstantObject> () {
      }
      /// Constructorto be used for assignment from a handle
      Constant(const Constant& e)
        : Handle<ConstantObject> (e) {
      }
      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q>
      Constant(const Handle<Q>& e)
        : Handle<ConstantObject> (e) {
      }
      /// Constructor to be used when creating a new DOM tree
      Constant(const std::string& name);
      /// Constructor to be used when creating a new DOM tree
      Constant(const std::string& name, const std::string& val, const std::string& typ="number");
      /// Access the constant
      std::string dataType() const;
      /// String representation of this object
      std::string toString() const;
    };

    /// Handle class describing an element in the periodic table
    /**
     *  For details please see the ROOT TGeo information:
     *  \see http://root.cern.ch/root/html/TGeoElement.html
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
     */
    class Atom: public Handle<TGeoElement> {
    public:
      /// Definition of the implementation type
      typedef TGeoElement Object;
      /// Default constructor
      Atom()
        : Handle<Object>() {
      }
#ifndef __CINT__
      /// Constructorto be used for assignment from a handle
      Atom(const Handle<Object>& e)
        : Handle<Object>(e) {
      }
#endif
      /// Constructor to be used when creating from a object handle
      template <typename Q>
      Atom(const Handle<Q>& e)
        : Handle<Object>(e) {
      }
      /// Constructor to be used when reading the already parsed DOM tree
      Atom(const std::string& name, const std::string& formula, int Z, int N, double density);
    };

    /// Handle class describing a material
    /** @class Material Objects.h
     *
     *  For details please see the ROOT TGeo information:
     *  \see http://root.cern.ch/root/html/TGeoMedium.html
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
     */
    class Material: public Handle<TGeoMedium> {
    public:
      /// Definition of the implementation type
      typedef TGeoMedium Object;

      /// Default constructor
      Material()
        : Handle<Object>() {
      }
#ifndef __CINT__
      /// Constructorto be used for assignment from material handle
      Material(const Handle<Object>& e)
        : Handle<Object>(e) {
      }
#endif
      /// Constructorto be used for assignment from object handle
      template <typename Q>
      Material(const Handle<Q>& e)
        : Handle<Object>(e) {
      }
      /// proton number of the underlying material
      double Z() const ;
      /// atomic number of the underlying material
      double A() const ;
      /// density of the underlying material
      double density() const ;
      /// String representation of this object
      std::string toString() const;
      /// Access the radiation length of the underlying material
      double radLength() const;
      /// Access the interaction length of the underlying material
      double intLength() const;
    };

    /// Handle class describing visualization attributes
    /** @class VisAttr Objects.h
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
     */
    class VisAttr: public Handle<VisAttrObject> {
    public:
      enum Style {
        SOLID = 0x1, WIREFRAME = 0x2, DASHED = 0x2, LAST_STYLE
      };
      typedef VisAttrObject Object;
      /// Default constructor
      VisAttr()
        : Handle<Object>() {
      }
      /// Copy constructor for handle
      VisAttr(const VisAttr& e)
        : Handle<Object>(e) {
      }
#ifndef __CINT__
      /// Copy constructor for handle
      VisAttr(const Handle<Object>& e)
        : Handle<Object>(e) {
      }
#endif
      /// Constructor to be used for assignment from object handle
      template <typename Q>
      VisAttr(const Handle<Q>& e)
        : Handle<Object>(e) {
      }
      /// Constructor to be used when creating a new registered visualization object
      VisAttr(const std::string& name);
      /// Constructor to be used when creating a new registered visualization object
      VisAttr(const char* name);
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

    /// Deprecated: Handle class describing an alignment entry
    /**
     * \deprecated{Class representing an alignment entry}
     *
     * @author  M.Frank
     * @version 1.0
     */
    class AlignmentEntry: public Handle<TGeoPhysicalNode> {
    public:
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

    /// Small object describing a limit structure acting on a particle type
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
     */
    class Limit {
    public:
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

    /// Handle class describing a set of limits as they are used for simulation
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
     */
    class LimitSet: public Handle<LimitSetObject> {
    public:
      typedef LimitSetObject Object;
      /// Constructor to be used when reading the already parsed DOM tree
      LimitSet()
        : Handle<LimitSetObject>() {
      }
      /// Copy constructor for handle
      LimitSet(const LimitSet& e)
        : Handle<LimitSetObject>(e) {
      }
#ifndef __CINT__
      /// Copy constructor for handle
      LimitSet(const Handle<LimitSetObject>& e)
        : Handle<LimitSetObject>(e) {
      }
#endif
      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q>
      LimitSet(const Handle<Q>& e)
        : Handle<LimitSetObject>(e) {
      }
      /// Assignment operator
      LimitSet& operator=(const LimitSet& c)  {
        m_element = c.m_element;
        return *this;
      }
      /// Constructor to be used when creating a new object
      LimitSet(const std::string& name);
      /// Add new limit. Returns true if the new limit was added, false if it already existed.
      bool addLimit(const DD4hep::Geometry::Limit& limit);
      /// Accessor to limits container
      const std::set<DD4hep::Geometry::Limit>& limits() const;
    };

    /// Handle class describing a region as used in simulation
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
     */
    class Region: public Handle<RegionObject> {
    public:
      /// Implemeting class
      typedef RegionObject Object;
      /// Default constructor
      Region()
        : Handle<Object>() {
      }
      /// Copy Constructor
      Region(const Region& e)
        : Handle<Object>(e) {
      }
#ifndef __CINT__
      /// Copy Constructor
      Region(const Handle<RegionObject>& e)
        : Handle<Object>(e) {
      }
#endif
      /// Constructor to be used when assigning handle
      template <typename Q>
      Region(const Handle<Q>& e)
        : Handle<Object>(e) {
      }
      /// Constructor to be used when creating a new object
      Region(const std::string& name);
      /// Assignment operator
      Region& operator=(const Region& c)  {
        m_element = c.m_element;
        return *this;
      }

      Region& setStoreSecondaries(bool value);
      Region& setThreshold(double value);
      Region& setCut(double value);
      /// Access references to user limits
      std::vector<std::string>& limits() const;

      /// Access cut value
      double cut() const;
      /// Access production threshold
      double threshold() const;
      /// Access secondaries flag
      bool storeSecondaries() const;
    };

  } /* End namespace Geometry           */
}   /* End namespace DD4hep             */

#include "Math/Vector4D.h"
#include "Math/Point3D.h"

namespace ROOT {
  namespace Math {
    typedef DD4hep::Geometry::Position Position;
    /// Dot product of 3-vectors.
    inline double operator *(const Position& l, const Position& r) {
      return std::sqrt(l.X() * r.X() + l.Y() * r.Y() + l.Z() * r.Z());
    }
    /// Calculate the mean length of two vectors
    inline double mean_length(const Position& p1, const Position& p2) {
      return 0.5 * (p1.R() + p2.R()) / 2.0;
    }
    /// Calculate the mean direction of two vectors
    inline Position mean_direction(const Position& p1, const Position& p2) {
      return 0.5 * (p1 + p2);
    }

    // These operators are used for component properties.
    // The implementation is in the parsers, but since the parsers
    // do not have public include files, they are defined here.

    /// Allow point insertion of a point in maps
    bool operator<(const XYZPoint& a, const XYZPoint& b);
    /// Allow 3-vector insertion of a  in maps
    bool operator<(const XYZVector& a, const XYZVector& b);
    /// Allow 4-vector insertion of a  in maps
    bool operator<(const PxPyPzEVector& a, const PxPyPzEVector& b);
  }
}

#endif    /* DD4HEP_GEOMETRY_OBJECTS_H        */
