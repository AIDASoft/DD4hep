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
namespace dd4hep {

  // Forward declarations
  class Detector;
  class IDDescriptor;
  class VisAttrObject;
  class HeaderObject;
  class ConstantObject;
  class RegionObject;
  class LimitSetObject;

  typedef ROOT::Math::XYZVector Position;
  typedef ROOT::Math::XYZVector XYZAngles;
  
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


  /// Handle class describing an author entity
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_GEOMETRY
   */
  class Author: public Ref_t {
  public:
    /// Default constructor
    Author() = default;
    /// Constructorto be used for assignment from a handle
    Author(const Author& e) = default;
    /// Constructor to be used when assigning already valid handle
    template <typename Q> Author(const Handle<Q>& e) : Ref_t(e) {   }
    /// Constructor to be used when creating a new object
    Author(Detector& doc);
    /// Assignment operator
    Author& operator=(const Author& e) = default;
    /// Access the auhor's name
    std::string authorName() const;
    /// Set the author's name
    void setAuthorName(const std::string& nam);
    /// Access the auhor's email address
    std::string authorEmail() const;
    /// Set the author's email address
    void setAuthorEmail(const std::string& addr);
  };

  /// Handle class describing the basic information about geometry objects as it is defined in Detector
  /**
   *  Description of the geometry header. Containes useful auxiliary information.
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_GEOMETRY
   */
  class Header: public Handle<HeaderObject> {
  public:
    /// Default constructor
    Header() = default;
    /// Constructorto be used for assignment from a handle
    Header(const Header& e) = default;
    /// Constructor to be used when reading the already parsed DOM tree
    template <typename Q> Header(const Handle<Q>& e) : Handle<HeaderObject>(e) {  }
    /// Constructor to be used when creating a new DOM tree
    Header(const std::string& author, const std::string& url);
    /// Assignment operator
    Header& operator=(const Header& e) = default;
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

  /// Handle class describing a constant (define) object in description
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
    /// Default constructor
    Constant() = default;
    /// Constructorto be used for assignment from a handle
    Constant(const Constant& e) = default;
    /// Constructor to be used when reading the already parsed DOM tree
    template <typename Q> Constant(const Handle<Q>& e) : Handle<ConstantObject> (e) {  }
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
    /// Default constructor
    Atom() = default;
#ifndef __CINT__
    /// Constructorto be used for assignment from a handle
    Atom(const Handle<Object>& e) : Handle<Object>(e) {   }
#endif
    /// Constructor to be used when creating from a object handle
    template <typename Q> Atom(const Handle<Q>& e) : Handle<Object>(e) { }
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
    /// Default constructor
    Material() = default;
#ifndef __CINT__
    /// Constructorto be used for assignment from material handle
    Material(const Handle<Object>& e) : Handle<Object>(e) { }
#endif
    /// Constructorto be used for assignment from object handle
    template <typename Q> Material(const Handle<Q>& e) : Handle<Object>(e) {  }
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
    /// Default constructor
    VisAttr() = default;
    /// Copy constructor for handle
    VisAttr(const VisAttr& e) = default;
#ifndef __CINT__
    /// Copy constructor for handle
    VisAttr(const Handle<Object>& e) : Handle<Object>(e) {   }
#endif
    /// Constructor to be used for assignment from object handle
    template <typename Q> VisAttr(const Handle<Q>& e) : Handle<Object>(e) {  }
    /// Constructor to be used when creating a new registered visualization object
    VisAttr(const std::string& name);
    /// Constructor to be used when creating a new registered visualization object
    VisAttr(const char* name);
    /// Assignment operator
    VisAttr& operator=(const VisAttr& attr) = default;

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
    double value = 0.0;
    /// Default constructor
    Limit() = default;
    /// Copy constructor
    Limit(const Limit& c) = default;
    /// Assignment operator
    Limit& operator=(const Limit& c) = default;
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
    /// Constructor to be used when reading the already parsed DOM tree
    LimitSet() = default;
    /// Copy constructor for handle
    LimitSet(const LimitSet& e) = default;
#ifndef __CINT__
    /// Copy constructor for handles
    LimitSet(const Handle<LimitSetObject>& e) : Handle<LimitSetObject>(e) { }
#endif
    /// Constructor to be used when reading the already parsed DOM tree
    template <typename Q>  LimitSet(const Handle<Q>& e) : Handle<LimitSetObject>(e) { }
    /// Assignment operator
    LimitSet& operator=(const LimitSet& c) = default;
    /// Constructor to be used when creating a new object
    LimitSet(const std::string& name);
    /// Add new limit. Returns true if the new limit was added, false if it already existed.
    bool addLimit(const Limit& limit);
    /// Accessor to limits container
    const std::set<Limit>& limits() const;
  };

  /// Handle class describing a region as used in simulation
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_GEOMETRY
   */
  class Region: public Handle<RegionObject> {
  public:
    /// Default constructor
    Region() = default;
    /// Copy Constructor
    Region(const Region& e) = default;
#ifndef __CINT__
    /// Copy Constructor
    Region(const Handle<RegionObject>& e) : Handle<Object>(e) {  }
#endif
    /// Constructor to be used when assigning handle
    template <typename Q> Region(const Handle<Q>& e) : Handle<Object>(e) {  }
    /// Constructor to be used when creating a new object
    Region(const std::string& name);
    /// Assignment operator
    Region& operator=(const Region& c)  = default;

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
    /// Access use_default_cut flag
    bool useDefaultCut() const;
    /// Access was_threshold_set flag
    bool wasThresholdSet() const;
  };

}   /* End namespace dd4hep             */

#include "Math/Vector4D.h"
#include "Math/Point3D.h"

namespace ROOT {
  namespace Math {
    /// Dot product of 3-vectors.
    inline double operator *(const dd4hep::Position& l, const dd4hep::Position& r) {
      return std::sqrt(l.X() * r.X() + l.Y() * r.Y() + l.Z() * r.Z());
    }
    /// Calculate the mean length of two vectors
    inline double mean_length(const dd4hep::Position& p1, const dd4hep::Position& p2) {
      return 0.5 * (p1.R() + p2.R()) / 2.0;
    }
    /// Calculate the mean direction of two vectors
    inline dd4hep::Position mean_direction(const dd4hep::Position& p1, const dd4hep::Position& p2) {
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
