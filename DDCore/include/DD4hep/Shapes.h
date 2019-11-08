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

#ifndef DD4HEP_DDCORE_SOLIDS_H
#define DD4HEP_DDCORE_SOLIDS_H

// Framework include files
#include "DD4hep/Handle.h"
#include "DD4hep/Objects.h"

// C/C++ include files
#include <vector>

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated" // Code that causes warning goes here
#endif
// ROOT include files
#include "TGeoCone.h"
#include "TGeoPgon.h"
#include "TGeoPcon.h"
#include "TGeoArb8.h"
#include "TGeoTrd1.h"
#include "TGeoTrd2.h"
#include "TGeoTube.h"
#include "TGeoEltu.h"
#include "TGeoXtru.h"
#include "TGeoHype.h"
#include "TGeoTorus.h"
#include "TGeoSphere.h"
#include "TGeoHalfSpace.h"
#include "TGeoParaboloid.h"
#include "TGeoCompositeShape.h"
#include "TGeoShapeAssembly.h"
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  class Volume;

  /** Utitlity functions   */
  /// Pretty print of solid attributes
  std::string toStringSolid(const TGeoShape* shape, int precision=2);

  /// Output mesh vertices to string
  std::string toStringMesh(const TGeoShape* shape, int precision=2);
  
  /// Access Shape dimension parameters (As in TGeo, but angles in radians rather than degrees)
  std::vector<double> get_shape_dimensions(TGeoShape* shape);
  
  /// Set the shape dimensions (As for the TGeo shape, but angles in rad rather than degrees)
  void set_shape_dimensions(TGeoShape* shape, const std::vector<double>& params);

  /// Type check of various shapes. Result like dynamic_cast. Compare with python's isinstance(obj,type)
  template <typename SOLID> bool isInstance(const Handle<TGeoShape>& solid);
  /// Type check of various shapes. Do not allow for polymorphism. Types must match exactly
  template <typename SOLID> bool isA(const Handle<TGeoShape>& solid);
  /// Access Shape dimension parameters (As in TGeo, but angles in radians rather than degrees)
  template <typename SOLID> std::vector<double> dimensions(const Handle<TGeoShape>& solid);
  /// Set the shape dimensions. As for the TGeo shape, but angles in rad rather than degrees.
  template <typename SOLID> void set_dimensions(SOLID solid, const std::vector<double>& params);


  ///  Base class for Solid (shape) objects
  /**
   *   Generic handle holding an object of base TGeoShape.
   *
   *   One note about divisions:
   *   =========================
   *   Since dd4hep requires Volumes (aka TGeoVolume) and PlacedVolumes (aka TGeoNode)
   *   to be enhaced with the user extension mechanism shape divisions MUST be
   *   done using the division mechanism of the dd4hep shape or volume wrapper.
   *   Otherwise the enhancements are not added and you shall get exception
   *   when dd4hep is closing the geometry.
   *   The same argument holds when a division is made from a Volume.
   *   Unfortunately there is no reasonable way to intercept this call to the
   *   TGeo objects - except to sub-class each of them, which is not really 
   *   acceptable either.
   *
   *   For any further documentation please see the following ROOT documentation:
   *   \see http://root.cern.ch/root/html/TGeoShape.html
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  template <typename T> class Solid_type: public Handle<T> {
  public:
    template <typename Q> friend void set_dimensions(Q ptr, const std::vector<double>& params);
    
  protected:
    void _setDimensions(double* param)   const;
    /// Assign pointrs and register solid to geometry
    void _assign(T* n, const std::string& nam, const std::string& tit, bool cbbox);

  public:

    /// Default constructor for uninitialized object
    Solid_type() = default;
    /// Move constructor
    Solid_type(Solid_type&& e) = default;
    /// Copy constructor
    Solid_type(const Solid_type& e) = default;
    /// Direct assignment using the implementation pointer
    Solid_type(T* p) : Handle<T>(p) {  }
    /// Move Constructor from handle
    Solid_type(Handle<T>&& e) : Handle<T>(e) {  }
    /// Copy Constructor from handle
    Solid_type(const Handle<T>& e) : Handle<T>(e) {  }
    /// Constructor to be used when passing an already created object: need to check pointers
    template <typename Q> Solid_type(const Handle<Q>& e) : Handle<T>(e) {  }
    /// Assignment move operator
    Solid_type& operator=(Solid_type&& copy) = default;
    /// Assignment copy operator
    Solid_type& operator=(const Solid_type& copy) = default;

    /// Access to shape name
    const char* name() const;
    /// Set new shape name
    Solid_type<T>& setName(const char* value);
    /// Set new shape name
    Solid_type<T>& setName(const std::string& value);

    /// Access to shape type (The TClass name of the ROOT implementation)
    const char* type() const;
    /// Auto conversion to underlying ROOT object
    operator T*() const {
      return this->m_element;
    }
    /// Overloaded operator -> to access underlying object
    T* operator->() const {
      return this->m_element;
    }
    /// Access the dimensions of the shape: inverse of the setDimensions member function
    std::vector<double> dimensions();
    /// Set the shape dimensions. As for the TGeo shape, but angles in rad rather than degrees.
    Solid_type& setDimensions(const std::vector<double>& params);
    /// Conversion to string for pretty print
    std::string toString(int precision=2) const   {
      return toStringSolid(this->m_element,precision);
    }
    /// Divide volume into subsections (See the ROOT manuloa for details)
    TGeoVolume* divide(const Volume& voldiv, const std::string& divname,
                       int iaxis, int ndiv, double start, double step)  const;
  };
  typedef Solid_type<TGeoShape> Solid;

  /// Class describing a shape-less solid shape
  /**
   *   For any further documentation please see the following ROOT documentation:
   *   \see http://root.cern.ch/root/html/TGeoShapeAssembly.html
   *
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class ShapelessSolid: public Solid_type<TGeoShapeAssembly> {
  public:
    /// Default constructor
    ShapelessSolid() = default;
    /// Move constructor from handle
    ShapelessSolid(ShapelessSolid&& e) = default;
    /// Copy constructor from handle
    ShapelessSolid(const ShapelessSolid& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> ShapelessSolid(const Q* p) : Solid_type<TGeoShapeAssembly>(p) { }
    /// Constructor to be used with an existing object
    template <typename Q> ShapelessSolid(const Handle<Q>& e) : Solid_type<TGeoShapeAssembly>(e) { }
    /// Constructor to create an anonymous new shapeless solid
    ShapelessSolid(const std::string& name);
    /// Move Assignment operator
    ShapelessSolid& operator=(ShapelessSolid&& copy) = default;
    /// Copy Assignment operator
    ShapelessSolid& operator=(const ShapelessSolid& copy) = default;
  };

  /// Class describing a box shape
  /**
   *   For any further documentation please see the following ROOT documentation:
   *   \see http://root.cern.ch/root/html/TGeoBBox.html
   *
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class Box : public Solid_type<TGeoBBox> {
  protected:
    /// Internal helper method to support object construction
    void make(const std::string& name, double x_val, double y_val, double z_val);

  public:
    /// Default constructor
    Box() = default;
    /// Move constructor
    Box(Box&& e) = default;
    /// Copy constructor
    Box(const Box& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> Box(const Q* p) : Solid_type<TGeoBBox>(p) { }
    /// Copy Constructor to be used with an existing object handle
    template <typename Q> Box(const Handle<Q>& e) : Solid_type<TGeoBBox>(e) { }

    /// Constructor to create an anonymous new box object (retrieves name from volume)
    Box(double x_val, double y_val, double z_val)
    { make("", x_val, y_val, z_val);    }
    /// Constructor to create a named new box object (retrieves name from volume)
    Box(const std::string& nam, double x_val, double y_val, double z_val)
    { make(nam.c_str(), x_val, y_val, z_val);    }

    /// Constructor to create an anonymous new box object (retrieves name from volume)
    template <typename X, typename Y, typename Z>
    Box(const X& x_val, const Y& y_val, const Z& z_val)
    { make("", _toDouble(x_val), _toDouble(y_val), _toDouble(z_val));    }
    /// Constructor to create a named new box object (retrieves name from volume)
    template <typename X, typename Y, typename Z>
    Box(const std::string& nam, const X& x_val, const Y& y_val, const Z& z_val)
    { make(nam.c_str(), _toDouble(x_val), _toDouble(y_val), _toDouble(z_val));  }

    /// Move Assignment operator
    Box& operator=(Box&& copy) = default;
    /// Copy Assignment operator
    Box& operator=(const Box& copy) = default;
    /// Set the box dimensions
    Box& setDimensions(double x_val, double y_val, double z_val);
    /// Access half "length" of the box
    double x() const;
    /// Access half "width" of the box
    double y() const;
    /// Access half "depth" of the box
    double z() const;
  };

  /// Class describing half-space
  /**
   *   For any further documentation please see the following ROOT documentation:
   *   \see http://root.cern.ch/root/html/TGeoHalfSpace.html
   *
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class HalfSpace : public Solid_type<TGeoHalfSpace> {
  protected:
    /// Internal helper method to support object construction
    void make(const std::string& name, const double* const point, const double* const normal);

  public:
    /// Default constructor
    HalfSpace() = default;
    /// Move Constructor
    HalfSpace(HalfSpace&& e) = default;
    /// Copy Constructor
    HalfSpace(const HalfSpace& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> HalfSpace(const Q* p) : Solid_type<Object>(p) { }
    /// Constructor to be used with an existing object
    template <typename Q> HalfSpace(const Handle<Q>& e) : Solid_type<Object>(e) { }

    /// Constructor to create an new halfspace object from a point on a plane and the plane normal
    HalfSpace(const double* const point, const double* const normal)
    { make("", point, normal);              }

    /// Constructor to create an new named halfspace object from a point on a plane and the plane normal
    HalfSpace(const std::string& nam, const double* const point, const double* const normal)
    { make(nam.c_str(), point, normal);    }

    /// Move Assignment operator
    HalfSpace& operator=(HalfSpace&& copy) = default;
    /// Copy Assignment operator
    HalfSpace& operator=(const HalfSpace& copy) = default;
 };

  /// Class describing a cone shape
  /**
   *   For any further documentation please see the following ROOT documentation:
   *   \see http://root.cern.ch/root/html/TGeoCone.html
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class Cone : public Solid_type<TGeoCone> {
  protected:
    /// Internal helper method to support object construction
    void make(const std::string& name, double z, double rmin1, double rmax1, double rmin2, double rmax2);
  public:
    /// Default constructor
    Cone() = default;
    /// Move Constructor
    Cone(Cone&& e) = default;
    /// Copy Constructor
    Cone(const Cone& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> Cone(const Q* p) : Solid_type<Object>(p) { }
    /// Constructor to be used when passing an already created object
    template <typename Q> Cone(const Handle<Q>& e) : Solid_type<Object>(e) { }

    /// Constructor to create a new anonymous object with attribute initialization
    Cone(double z, double rmin1, double rmax1, double rmin2, double rmax2)
    {     this->make("", z, rmin1, rmax1, rmin2, rmax2);                                 }
    /// Constructor to create a new anonymous object with attribute initialization
    template <typename Z, typename RMIN1, typename RMAX1, typename RMIN2, typename RMAX2>
    Cone(const Z& z, const RMIN1& rmin1, const RMAX1& rmax1, const RMIN2& rmin2, const RMAX2& rmax2)
    {     this->make("", _toDouble(z), _toDouble(rmin1), _toDouble(rmax1), _toDouble(rmin2), _toDouble(rmax2)); }

    /// Constructor to create a new anonymous object with attribute initialization
    Cone(const std::string& nam, double z, double rmin1, double rmax1, double rmin2, double rmax2)
    {     this->make(nam, z, rmin1, rmax1, rmin2, rmax2);                                 }
    /// Constructor to create a new anonymous object with attribute initialization
    template <typename Z, typename RMIN1, typename RMAX1, typename RMIN2, typename RMAX2>
    Cone(const std::string& nam, const Z& z, const RMIN1& rmin1, const RMAX1& rmax1, const RMIN2& rmin2, const RMAX2& rmax2)
    {     this->make(nam, _toDouble(z), _toDouble(rmin1), _toDouble(rmax1), _toDouble(rmin2), _toDouble(rmax2)); }

    /// Move Assignment operator
    Cone& operator=(Cone&& copy) = default;
    /// Copy Assignment operator
    Cone& operator=(const Cone& copy) = default;
    /// Set the box dimensions
    Cone& setDimensions(double z, double rmin1, double rmax1, double rmin2, double rmax2);
  };

  /// Class describing a Polycone shape
  /**
   *   Polycone. It has at least 9 parameters :
   *      - the lower phi limit;
   *      - the range in phi;
   *      - the number of z planes (at least two) where the inner/outer
   *        radii are changing;
   *      - z coordinate, inner and outer radius for each z plane
   *
   *
   *   For any further documentation please see the following ROOT documentation:
   *   \see http://root.cern.ch/root/html/TGeoPcon.html
   *
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class Polycone : public Solid_type<TGeoPcon> {
  public:
    /// Default constructor
    Polycone() = default;
    /// Move constructor
    Polycone(Polycone&& e) = default;
    /// Copy constructor
    Polycone(const Polycone& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> Polycone(const Q* p) : Solid_type<Object>(p) {  }
    /// Constructor to be used when reading the already parsed polycone object
    template <typename Q> Polycone(const Handle<Q>& e) : Solid_type<Object>(e) { }

    /// Constructor to create a new polycone object
    Polycone(double startPhi, double deltaPhi);
    /// Constructor to create a new polycone object. Add at the same time all Z planes
    Polycone(double startPhi, double deltaPhi,
             const std::vector<double>& r, const std::vector<double>& z);
    /// Constructor to create a new polycone object. Add at the same time all Z planes
    Polycone(double startPhi, double deltaPhi,
             const std::vector<double>& rmin, const std::vector<double>& rmax, const std::vector<double>& z);
    
    /// Constructor to create a new named polycone object
    Polycone(const std::string& name, double startPhi, double deltaPhi);
    /// Constructor to create a new named polycone object. Add at the same time all Z planes
    Polycone(const std::string& name, double startPhi, double deltaPhi,
             const std::vector<double>& r, const std::vector<double>& z);
    /// Constructor to create a new named polycone object. Add at the same time all Z planes
    Polycone(const std::string& name, double startPhi, double deltaPhi,
             const std::vector<double>& rmin, const std::vector<double>& rmax, const std::vector<double>& z);

    /// Move Assignment operator
    Polycone& operator=(Polycone&& copy) = default;
    /// Copy Assignment operator
    Polycone& operator=(const Polycone& copy) = default;

    /// Add Z-planes to the Polycone
    void addZPlanes(const std::vector<double>& rmin, const std::vector<double>& rmax, const std::vector<double>& z);
  };

  /// Class describing a cone segment shape
  /**
   *   A ConeSegment is, in the general case, a Phi segment of a cone, with
   *   half-length dz, inner and outer radii specified at -dz and +dz.
   *
   *   For any further documentation please see the following ROOT documentation:
   *   \see http://root.cern.ch/root/html/TGeoConeSeg.html
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class ConeSegment : public Solid_type<TGeoConeSeg> {
  public:
    /// Default constructor
    ConeSegment() = default;
    /// Move Constructor
    ConeSegment(ConeSegment&& e) = default;
    /// Copy Constructor
    ConeSegment(const ConeSegment& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> ConeSegment(const Q* p) : Solid_type<Object>(p) {  }
    /// Constructor to be used when reading the already parsed ConeSegment object
    template <typename Q> ConeSegment(const Handle<Q>& e) : Solid_type<Object>(e) {  }

    /// Constructor to create a new ConeSegment object
    ConeSegment(double dz, double rmin1, double rmax1,
                double rmin2, double rmax2, double startPhi = 0.0, double endPhi = 2.0 * M_PI);

    /// Constructor to create a new named ConeSegment object
    ConeSegment(const std::string& name, double dz, double rmin1, double rmax1,
                double rmin2, double rmax2, double startPhi = 0.0, double endPhi = 2.0 * M_PI);

    /// Move Assignment operator
    ConeSegment& operator=(ConeSegment&& copy) = default;
    /// Copy Assignment operator
    ConeSegment& operator=(const ConeSegment& copy) = default;
    /// Set the cone segment dimensions
    ConeSegment& setDimensions(double dz, double rmin1, double rmax1,
                               double rmin2, double rmax2,
                               double startPhi = 0.0, double endPhi = 2.0 * M_PI);
  };

  /// Class describing a tube shape of a section of a tube
  /**
   *   TGeoTube - cylindrical tube class. It takes 3 parameters :
   *            inner radius, outer radius and half-length dz.
   *
   *   For any further documentation please see the following ROOT documentation:
   *   \see http://root.cern.ch/root/html/TGeoConeSeg.html
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class Tube : public Solid_type<TGeoTubeSeg> {
  protected:
    /// Internal helper method to support object construction
    void make(const std::string& nam, double rmin, double rmax, double z, double startPhi, double endPhi);

  public:
    /// Default constructor
    Tube() = default;
    /// Move Constructor
    Tube(Tube&& e) = default;
    /// Copy Constructor
    Tube(const Tube& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> Tube(const Q* p) : Solid_type<Object>(p) {  }
    /// Constructor to assign an object
    template <typename Q> Tube(const Handle<Q>& e) : Solid_type<Object>(e) {  }

    /// Constructor to create a new anonymous tube object with attribute initialization
    Tube(double rmin, double rmax, double dz)
    {  this->make("", rmin, rmax, dz, 0, 2*M_PI);                   }
    /// Constructor to create a new anonymous tube object with attribute initialization
    Tube(double rmin, double rmax, double dz, double endPhi)
    {  this->make("", rmin, rmax, dz, 0, endPhi);                   }
    /// Constructor to create a new anonymous tube object with attribute initialization
    Tube(double rmin, double rmax, double dz, double startPhi, double endPhi)
    {  this->make("", rmin, rmax, dz, startPhi, endPhi);            }

    /// Legacy: Constructor to create a new identifiable tube object with attribute initialization
    Tube(const std::string& nam, double rmin, double rmax, double dz)
    {  this->make(nam, rmin, rmax, dz, 0, 2*M_PI);                  }
    /// Legacy: Constructor to create a new identifiable tube object with attribute initialization
    Tube(const std::string& nam, double rmin, double rmax, double dz, double endPhi)
    {  this->make(nam, rmin, rmax, dz, 0, endPhi);                   }
    /// Legacy: Constructor to create a new identifiable tube object with attribute initialization
    Tube(const std::string& nam, double rmin, double rmax, double dz, double startPhi, double endPhi)
    {  this->make(nam, rmin, rmax, dz, startPhi, endPhi);            }
    /// Constructor to create a new anonymous tube object with attribute initialization
    template <typename RMIN, typename RMAX, typename Z, typename ENDPHI=double>
    Tube(const RMIN& rmin, const RMAX& rmax, const Z& dz, const ENDPHI& endPhi = 2.0*M_PI)
    {  this->make("", _toDouble(rmin), _toDouble(rmax), _toDouble(dz), 0, _toDouble(endPhi));   }

    /// Move Assignment operator
    Tube& operator=(Tube&& copy) = default;
    /// Copy Assignment operator
    Tube& operator=(const Tube& copy) = default;
    /// Set the tube dimensions
    Tube& setDimensions(double rmin, double rmax, double dz, double startPhi=0.0, double endPhi=2*M_PI);
  };

  /// Class describing a tube shape of a section of a cut tube segment
  /**
   *   For any further documentation please see the following ROOT documentation:
   *   \see http://root.cern.ch/root/html/TGeoCtub.html
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class CutTube : public Solid_type<TGeoCtub> {
  protected:
    /// Internal helper method to support object construction
    void make(const std::string& name,
              double rmin, double rmax, double dz, double startPhi, double endPhi,
              double lx, double ly, double lz, double tx, double ty, double tz);

  public:
    /// Default constructor
    CutTube() = default;
    /// Move Constructor
    CutTube(CutTube&& e) = default;
    /// Copy Constructor
    CutTube(const CutTube& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> CutTube(const Q* p) : Solid_type<Object>(p) {  }
    /// Constructor to assign an object
    template <typename Q> CutTube(const Handle<Q>& e) : Solid_type<Object>(e) {  }

    /// Constructor to create a new cut-tube object with attribute initialization
    CutTube(double rmin, double rmax, double dz, double startPhi, double endPhi,
            double lx, double ly, double lz, double tx, double ty, double tz);

    /// Constructor to create a new identifiable cut-tube object with attribute initialization
    CutTube(const std::string& name,
            double rmin, double rmax, double dz, double startPhi, double endPhi,
            double lx, double ly, double lz, double tx, double ty, double tz);

    /// Move Assignment operator
    CutTube& operator=(CutTube&& copy) = default;
    /// Copy Assignment operator
    CutTube& operator=(const CutTube& copy) = default;
  };

  
  /// Class describing a truncated tube shape (CMS'ism)
  /**
   *   No real correspondance to TGeo. In principle it's a boolean Solid based on a tube.
   *   \see http://cmssdt.cern.ch/lxr/source/DetectorDescription/Core/src/TruncTubs.h
   *
   *   The Solid::dimension() and Solid::setDimension() calls for the TruncatedTube
   *   deliver/expect the parameters in the same order as the constructor:
   *   (zhalf, rmin, rmax, startPhi, deltaPhi, cutAtStart, cutAtDelta, cutInside)
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class TruncatedTube : public Solid_type<TGeoCompositeShape> {
  protected:
    /// Internal helper method to support object construction
    void make(const std::string& name,
              double zhalf, double rmin, double rmax, double startPhi, double deltaPhi,
              double cutAtStart, double cutAtDelta, bool cutInside);

  public:
    /// Default constructor
    TruncatedTube() = default;
    /// Move Constructor
    TruncatedTube(TruncatedTube&& e) = default;
    /// Copy Constructor
    TruncatedTube(const TruncatedTube& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> TruncatedTube(const Q* p) : Solid_type<Object>(p) {  }
    /// Constructor to assign an object
    template <typename Q> TruncatedTube(const Handle<Q>& e) : Solid_type<Object>(e) {  }

    /// Constructor to create a truncated tube object with attribute initialization
    TruncatedTube(double zhalf, double rmin, double rmax, double startPhi, double deltaPhi,
                  double cutAtStart, double cutAtDelta, bool cutInside);

    /// Constructor to create a truncated tube object with attribute initialization
    TruncatedTube(const std::string& name,
                  double zhalf, double rmin, double rmax, double startPhi, double deltaPhi,
                  double cutAtStart, double cutAtDelta, bool cutInside);

    /// Move Assignment operator
    TruncatedTube& operator=(TruncatedTube&& copy) = default;
    /// Copy Assignment operator
    TruncatedTube& operator=(const TruncatedTube& copy) = default;
  };
  
  /// Class describing a elliptical tube shape
  /**
   *   This is actually no TGeo shape. This implementation is a placeholder
   *   for the Geant4 implementation G4TwistedTube.
   *   In root it is implemented by a simple tube segment.
   *   When converted to geant4 it will become a G4TwistedTube.
   *
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class EllipticalTube : public Solid_type<TGeoEltu> {
  protected:
    /// Internal helper method to support object construction
    void make(const std::string& nam, double a, double b, double dz);

  public:
    /// Default constructor
    EllipticalTube() = default;
    /// Move Constructor
    EllipticalTube(EllipticalTube&& e) = default;
    /// Copy Constructor
    EllipticalTube(const EllipticalTube& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> EllipticalTube(const Q* p) : Solid_type<Object>(p) {   }
    /// Constructor to assign an object
    template <typename Q> EllipticalTube(const Handle<Q>& e) : Solid_type<Object>(e) {   }

    /// Constructor to create a new anonymous tube object with attribute initialization
    EllipticalTube(double a, double b, double dz) {  this->make("", a, b, dz);  }
    /// Constructor to create a new anonymous tube object with attribute initialization
    template <typename A, typename B, typename DZ>
    EllipticalTube(const A& a, const B& b, const DZ& dz)
    {  this->make("",_toDouble(a), _toDouble(b), _toDouble(dz));     }

    /// Constructor to create a new identified tube object with attribute initialization
    EllipticalTube(const std::string& nam, double a, double b, double dz)
    {  this->make(nam, a, b, dz);                                    }
    /// Constructor to create a new identified tube object with attribute initialization
    template <typename A, typename B, typename DZ>
    EllipticalTube(const std::string& nam, const A& a, const B& b, const DZ& dz)
    {  this->make(nam, _toDouble(a), _toDouble(b), _toDouble(dz));   }

    /// Move Assignment operator
    EllipticalTube& operator=(EllipticalTube&& copy) = default;
    /// Copy Assignment operator
    EllipticalTube& operator=(const EllipticalTube& copy) = default;
    /// Set the tube dimensions
    EllipticalTube& setDimensions(double a, double b, double dz);
  };

  /// Class describing a twisted tube shape
  /**
   *   TGeoEltu - cylindrical tube class. It takes 3 parameters :
   *            Semi axis of ellipsis in x and y and half-length dz.
   *
   *   For any further documentation please see the following ROOT documentation:
   *   \see http://root.cern.ch/root/html/TGeoElTu.html
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class TwistedTube : public Solid_type<TGeoTubeSeg> {
  protected:
    /// Internal helper method to support TwistedTube object construction
    void make(const std::string& nam, double twist_angle, double rmin, double rmax,
              double zneg, double zpos, int nsegments, double totphi);

  public:
    /// Default constructor
    TwistedTube() = default;
    /// Move Constructor
    TwistedTube(TwistedTube&& e) = default;
    /// Copy Constructor
    TwistedTube(const TwistedTube& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> TwistedTube(const Q* p) : Solid_type<Object>(p) {   }
    /// Constructor to assign an object
    template <typename Q> TwistedTube(const Handle<Q>& e) : Solid_type<Object>(e) {   }

    /// Constructor to create a new anonymous tube object with attribute initialization
    TwistedTube(double twist_angle, double rmin, double rmax,
                double dz, double dphi)
    {  this->make("", twist_angle, rmin, rmax, -dz, dz, 1, dphi);  }
    /// Constructor to create a new anonymous tube object with attribute initialization
    TwistedTube(double twist_angle, double rmin, double rmax,
                double dz, int nsegments, double totphi)
    {  this->make("", twist_angle, rmin, rmax, -dz, dz, nsegments, totphi);  }
    /// Constructor to create a new anonymous tube object with attribute initialization
    TwistedTube(double twist_angle, double rmin, double rmax,
                double zneg, double zpos, double totphi)
    {  this->make("", twist_angle, rmin, rmax, zneg, zpos, 1, totphi);  }
    /// Constructor to create a new anonymous tube object with attribute initialization
    TwistedTube(double twist_angle, double rmin, double rmax,
                double zneg, double zpos, int nsegments, double totphi)
    {  this->make("", twist_angle, rmin, rmax, zneg, zpos, nsegments, totphi);  }

    /// Constructor to create a new anonymous tube object with attribute initialization
    TwistedTube(const std::string& nam, double twist_angle, double rmin, double rmax,
                double dz, double dphi)
    {  this->make(nam, twist_angle, rmin, rmax, -dz, dz, 1, dphi);  }
    /// Constructor to create a new anonymous tube object with attribute initialization
    TwistedTube(const std::string& nam, double twist_angle, double rmin, double rmax,
                double dz, int nsegments, double totphi)
    {  this->make(nam, twist_angle, rmin, rmax, -dz, dz, nsegments, totphi);  }
    /// Constructor to create a new anonymous tube object with attribute initialization
    TwistedTube(const std::string& nam, double twist_angle, double rmin, double rmax,
                double zneg, double zpos, double totphi)
    {  this->make(nam, twist_angle, rmin, rmax, zneg, zpos, 1, totphi);  }
    /// Constructor to create a new anonymous tube object with attribute initialization
    TwistedTube(const std::string& nam, double twist_angle, double rmin, double rmax,
                double zneg, double zpos, int nsegments, double totphi)
    {  this->make(nam, twist_angle, rmin, rmax, zneg, zpos, nsegments, totphi);  }

    /// Constructor to create a new identified tube object with attribute initialization
    template <typename A, typename B, typename DZ>
    TwistedTube(const std::string& nam, const A& a, const B& b, const DZ& dz)
    {  this->make(nam, _toDouble(a), _toDouble(b), _toDouble(dz));   }

    /// Move Assignment operator
    TwistedTube& operator=(TwistedTube&& copy) = default;
    /// Copy Assignment operator
    TwistedTube& operator=(const TwistedTube& copy) = default;
    /// Set the tube dimensions
    TwistedTube& setDimensions(double a, double b, double dz);
  };

  /// Class describing a trap shape
  /**
   *   For any further documentation please see the following ROOT documentation:
   *   \see http://root.cern.ch/root/html/TGeoTrap.html
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class Trap : public Solid_type<TGeoTrap> {
  private:
    /// Internal helper method to support object construction
    void make(const std::string& name, double pz, double py, double px, double pLTX);
  public:
    /// Default constructor
    Trap() = default;
    /// Move Constructor
    Trap(Trap&& e) = default;
    /// Copy Constructor
    Trap(const Trap& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> Trap(const Q* p) : Solid_type<Object>(p) { }
    /// Constructor to be used when passing an already created object
    template <typename Q> Trap(const Handle<Q>& e) : Solid_type<Object>(e) { }

    /// Constructor to create a new anonymous object with attribute initialization
    Trap(double z, double theta, double phi,
         double h1, double bl1, double tl1, double alpha1,
         double h2, double bl2, double tl2, double alpha2);
    /// Constructor to create a new anonymous object for right angular wedge from STEP (Se G4 manual for details)
    Trap(double pz, double py, double px, double pLTX)
    { this->make("", pz,py,px,pLTX);  }
    /// Constructor to create a new anonymous object with attribute initialization
    template <typename PZ,typename PY,typename PX,typename PLTX> Trap(PZ pz, PY py, PX px, PLTX pLTX)
    { this->make("", _toDouble(pz),_toDouble(py),_toDouble(px),_toDouble(pLTX)); }

    /// Constructor to create a new identified object with attribute initialization
    Trap(const std::string& name,
         double z, double theta, double phi,
         double h1, double bl1, double tl1, double alpha1,
         double h2, double bl2, double tl2, double alpha2);
    /// Constructor to create a new identified object for right angular wedge from STEP (Se G4 manual for details)
    Trap(const std::string& nam, double pz, double py, double px, double pLTX)
    { this->make(nam, pz,py,px,pLTX);  }
    /// Constructor to create a new identified object with attribute initialization
    template <typename PZ,typename PY,typename PX,typename PLTX>
    Trap(const std::string& nam, PZ pz, PY py, PX px, PLTX pLTX)
    { this->make(nam, _toDouble(pz),_toDouble(py),_toDouble(px),_toDouble(pLTX)); }

    /// Move Assignment operator
    Trap& operator=(Trap&& copy) = default;
    /// Copy Assignment operator
    Trap& operator=(const Trap& copy) = default;
    /// Set the trap dimensions
    Trap& setDimensions(double z, double theta, double phi,
                        double h1, double bl1, double tl1, double alpha1,
                        double h2, double bl2, double tl2, double alpha2);
  };

  /// Class describing a pseudo trap shape (CMS'ism)
  /**
   *   No real correspondance to TGeo. In principle it's a boolean Solid based on a tube.
   *   \see http://cmssdt.cern.ch/lxr/source/DetectorDescription/Core/src/PseudoTrap.h
   *
   *   The Solid::dimension() and Solid::setDimension() calls for the PseudoTrap
   *   deliver/expect the parameters in the same order as the constructor:
   *   (x1, x2, y1, y2, z, radius, minusZ)
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class PseudoTrap : public Solid_type<TGeoCompositeShape> {
  private:
    /// Internal helper method to support object construction
    void make(const std::string& nam, double x1, double x2, double y1, double y2, double z, double radius, bool minusZ);
  public:
    /// Default constructor
    PseudoTrap() = default;
    /// Move Constructor
    PseudoTrap(PseudoTrap&& e) = default;
    /// Copy Constructor
    PseudoTrap(const PseudoTrap& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> PseudoTrap(const Q* p) : Solid_type<Object>(p) { }
    /// Constructor to be used when passing an already created object
    template <typename Q> PseudoTrap(const Handle<Q>& e) : Solid_type<Object>(e) { }

    /// Constructor to create a new anonymous object with attribute initialization
    PseudoTrap(double x1, double x2, double y1, double y2, double z, double radius, bool minusZ)
    {  this->make("", x1, x2, y1, y2, z, radius, minusZ);    }

    /// Constructor to create a new identified object with attribute initialization
    PseudoTrap(const std::string& nam,
               double x1, double x2,
               double y1, double y2,
               double z,  double radius, bool minusZ)
    {  this->make(nam, x1, x2, y1, y2, z, radius, minusZ);    }

    /// Move Assignment operator
    PseudoTrap& operator=(PseudoTrap&& copy) = default;
    /// Copy Assignment operator
    PseudoTrap& operator=(const PseudoTrap& copy) = default;
  };

  /// Class describing a Trd1 shape
  /**
   *   For any further documentation please see the following ROOT documentation:
   *   \see http://root.cern.ch/root/html/TGeoTrd1.html
   *
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class Trd1 : public Solid_type<TGeoTrd1> {
  private:
    /// Internal helper method to support object construction
    void make(const std::string& nam, double x1, double x2, double y, double z);

  public:
    /// Default constructor
    Trd1() = default;
    /// Move Constructor
    Trd1(Trd1&& e) = default;
    /// Copy Constructor
    Trd1(const Trd1& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> Trd1(const Q* p) : Solid_type<Object>(p) { }
    /// Constructor to be used when passing an already created object
    template <typename Q> Trd1(const Handle<Q>& e) : Solid_type<Object>(e) { }

    /// Constructor to create a new anonymous object with attribute initialization
    Trd1(double x1, double x2, double y, double z)
    { this->make("", x1, x2, y, z);                                           }
    /// Constructor to create a new anonymous object with attribute initialization
    template <typename X1,typename X2,typename Y,typename Z>
    Trd1(X1 x1, X2 x2, Y y, Z z)
    { this->make("", _toDouble(x1),_toDouble(x2),_toDouble(y),_toDouble(z));  }

    /// Constructor to create a new anonymous object with attribute initialization
    Trd1(const std::string& nam, double x1, double x2, double y, double z)
    { this->make(nam, x1, x2, y, z);                                          }
    /// Constructor to create a new anonymous object with attribute initialization
    template <typename X1,typename X2,typename Y,typename Z>
    Trd1(const std::string& nam, X1 x1, X2 x2, Y y, Z z)
    { this->make(nam, _toDouble(x1),_toDouble(x2),_toDouble(y),_toDouble(z)); }

    /// Move Assignment operator
    Trd1& operator=(Trd1&& copy) = default;
    /// Copy Assignment operator
    Trd1& operator=(const Trd1& copy) = default;
    /// Set the Trd1 dimensions
    Trd1& setDimensions(double x1, double x2, double y, double z);
  };
  
  /// Class describing a Trd2 shape
  /**
   *   For any further documentation please see the following ROOT documentation:
   *   \see http://root.cern.ch/root/html/TGeoTrd2.html
   *
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class Trd2 : public Solid_type<TGeoTrd2> {
  private:
    /// Internal helper method to support object construction
    void make(const std::string& nam, double x1, double x2, double y1, double y2, double z);

  public:
    /// Default constructor
    Trd2() = default;
    /// Move Constructor
    Trd2(Trd2&& e) = default;
    /// Copy Constructor
    Trd2(const Trd2& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> Trd2(const Q* p) : Solid_type<Object>(p) { }
    /// Constructor to be used when passing an already created object
    template <typename Q> Trd2(const Handle<Q>& e) : Solid_type<Object>(e) { }

    /// Constructor to create a new anonymous object with attribute initialization
    Trd2(double x1, double x2, double y1, double y2, double z)
    { this->make("", x1, x2, y1, y2, z);  }
    /// Constructor to create a new anonymous object with attribute initialization
    template <typename X1,typename X2,typename Y1,typename Y2,typename Z>
    Trd2(X1 x1, X2 x2, Y1 y1, Y2 y2, Z z)
    { this->make("", _toDouble(x1),_toDouble(x2),_toDouble(y1),_toDouble(y2),_toDouble(z)); }

    /// Constructor to create a new identified object with attribute initialization
    Trd2(const std::string& nam, double x1, double x2, double y1, double y2, double z)
    { this->make(nam, x1, x2, y1, y2, z);  }
    /// Constructor to create a new identified object with attribute initialization
    template <typename X1,typename X2,typename Y1,typename Y2,typename Z>
    Trd2(const std::string& nam, X1 x1, X2 x2, Y1 y1, Y2 y2, Z z)
    { this->make(nam, _toDouble(x1),_toDouble(x2),_toDouble(y1),_toDouble(y2),_toDouble(z)); }

    /// Copy Assignment operator
    Trd2& operator=(Trd2&& copy) = default;
    /// Move Assignment operator
    Trd2& operator=(const Trd2& copy) = default;
    /// Set the Trd2 dimensions
    Trd2& setDimensions(double x1, double x2, double y1, double y2, double z);
  };
  /// Shortcut name definition
  typedef Trd2 Trapezoid;
  
  /// Class describing a Torus shape
  /**
   *   For any further documentation please see the following ROOT documentation:
   *   \see http://root.cern.ch/root/html/TGeoTorus.html
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class Torus : public Solid_type<TGeoTorus> {
  private:
    /// Internal helper method to support object construction
    void make(const std::string& nam, double r, double rmin, double rmax, double startPhi, double deltaPhi);
  public:
    /// Default constructor
    Torus() = default;
    /// Move Constructor
    Torus(Torus&& e) = default;
    /// Copy Constructor
    Torus(const Torus& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> Torus(const Q* p) : Solid_type<Object>(p) { }
    /// Constructor to be used when passing an already created object
    template <typename Q> Torus(const Handle<Q>& e) : Solid_type<Object>(e) {  }

    /// Constructor to create a new anonymous object with attribute initialization
    template<typename R, typename RMIN, typename RMAX, typename STARTPHI, typename DELTAPHI>
    Torus(R r, RMIN rmin, RMAX rmax, STARTPHI startPhi=M_PI, DELTAPHI deltaPhi = 2.*M_PI)
    {   this->make("", _toDouble(r),_toDouble(rmin),_toDouble(rmax),_toDouble(startPhi),_toDouble(deltaPhi));  }
    /// Constructor to create a new anonymous object with attribute initialization
    Torus(double r, double rmin, double rmax, double startPhi=M_PI, double deltaPhi = 2.*M_PI)
    {   this->make("", r, rmin, rmax, startPhi, deltaPhi);  }

    /// Constructor to create a new identified object with attribute initialization
    template<typename R, typename RMIN, typename RMAX, typename STARTPHI, typename DELTAPHI>
    Torus(const std::string& nam, R r, RMIN rmin, RMAX rmax, STARTPHI startPhi=M_PI, DELTAPHI deltaPhi = 2.*M_PI)
    {   this->make(nam, _toDouble(r),_toDouble(rmin),_toDouble(rmax),_toDouble(startPhi),_toDouble(deltaPhi));  }
    /// Constructor to create a new identified object with attribute initialization
    Torus(const std::string& nam, double r, double rmin, double rmax, double startPhi=M_PI, double deltaPhi = 2.*M_PI)
    {   this->make(nam, r, rmin, rmax, startPhi, deltaPhi);  }

    /// Move Assignment operator
    Torus& operator=(Torus&& copy) = default;
    /// Copy Assignment operator
    Torus& operator=(const Torus& copy) = default;
    /// Set the Torus dimensions
    Torus& setDimensions(double r, double rmin, double rmax, double startPhi, double deltaPhi);
  };

  /// Class describing a sphere shape
  /**
   *   For any further documentation please see the following ROOT documentation:
   *   \see http://root.cern.ch/root/html/TGeoSphere.html
   *
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class Sphere : public Solid_type<TGeoSphere> {
  protected:
    /// Constructor function to be used when creating a new object with attribute initialization
    void make(const std::string& nam,
              double rmin,       double rmax,
              double startTheta, double endTheta,
              double startPhi,   double endPhi);
  public:
    /// Default constructor
    Sphere() = default;
    /// Move Constructor
    Sphere(Sphere&& e) = default;
    /// Copy Constructor
    Sphere(const Sphere& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> Sphere(const Q* p) : Solid_type<Object>(p) { }
    /// Constructor to be used when passing an already created object
    template <typename Q> Sphere(const Handle<Q>& e) : Solid_type<Object>(e) {  }

    /// Constructor to create a new anonymous object with attribute initialization
    Sphere(double rmin,            double rmax,
           double startTheta= 0.0, double endTheta = M_PI,
           double startPhi  = 0.0, double endPhi   = 2. * M_PI)
    {  this->make("", rmin, rmax, startTheta, endTheta, startPhi, endPhi);     }
    /// Constructor to create a new anonymous object with generic attribute initialization
    template<typename RMIN,              typename RMAX,
             typename STARTTHETA=double, typename ENDTHETA=double,
             typename STARTPHI=double,   typename ENDPHI=double>
    Sphere(RMIN  rmin,                   RMAX     rmax,
           STARTTHETA startTheta = 0.0,  ENDTHETA endTheta = M_PI,
           STARTPHI   startPhi   = 0.0,  ENDPHI   endPhi   = 2. * M_PI)  {
      this->make("",
                 _toDOuble(rmin),       _toDouble(rmax),
                 _toDouble(startTheta), _toDouble(endTheta),
                 _toDouble(startPhi),   _toDouble(endPhi));
    }

    /// Constructor to create a new identified object with attribute initialization
    Sphere(const std::string& nam,
           double rmin,            double rmax,
           double startTheta= 0.0, double endTheta = M_PI,
           double startPhi  = 0.0, double endPhi   = 2. * M_PI)
    {  this->make(nam, rmin, rmax, startTheta, endTheta, startPhi, endPhi);     }
    /// Constructor to create a new identified object with generic attribute initialization
    template<typename RMIN,              typename RMAX,
             typename STARTTHETA=double, typename ENDTHETA=double,
             typename STARTPHI=double,   typename ENDPHI=double>
    Sphere(const std::string& nam,
           RMIN  rmin,                   RMAX     rmax,
           STARTTHETA startTheta = 0.0,  ENDTHETA endTheta = M_PI,
           STARTPHI   startPhi   = 0.0,  ENDPHI   endPhi   = 2. * M_PI)  {
      this->make(nam,
                 _toDOuble(rmin),       _toDouble(rmax),
                 _toDouble(startTheta), _toDouble(endTheta),
                 _toDouble(startPhi),   _toDouble(endPhi));
    }

    /// Move Assignment operator
    Sphere& operator=(Sphere&& copy) = default;
    /// Copy Assignment operator
    Sphere& operator=(const Sphere& copy) = default;
    /// Set the Sphere dimensions
    Sphere& setDimensions(double rmin,       double rmax,
                          double startTheta, double endTheta,
                          double startPhi,   double endPhi);
  };

  /// Class describing a Paraboloid shape
  /**
   *   For any further documentation please see the following ROOT documentation:
   *   \see http://root.cern.ch/root/html/TGeoParaboloid.html
   *
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class Paraboloid : public Solid_type<TGeoParaboloid> {
    /// Constructor function to create a new anonymous object with attribute initialization
    void make(const std::string& nam, double r_low, double r_high, double delta_z);
  public:
    /// Default constructor
    Paraboloid() = default;
    /// Move Constructor
    Paraboloid(Paraboloid&& e) = default;
    /// Copy Constructor
    Paraboloid(const Paraboloid& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> Paraboloid(const Q* p) : Solid_type<Object>(p) { }
    /// Constructor to be used when passing an already created object
    template <typename Q> Paraboloid(const Handle<Q>& e) : Solid_type<Object>(e) { }

    /// Constructor to create a new anonymous object with attribute initialization
    Paraboloid(double r_low, double r_high, double delta_z)
    {  this->make("", r_low, r_high, delta_z);  }
    /// Constructor to create a new anonymous object with attribute initialization
    template<typename R_LOW, typename R_HIGH, typename DELTA_Z>
    Paraboloid(R_LOW r_low, R_HIGH r_high, DELTA_Z delta_z)
    {  this->make("", _toDouble(r_low), _toDouble(r_high), _toDouble(delta_z));  }

    /// Constructor to create a new identified object with attribute initialization
    Paraboloid(const std::string& nam, double r_low, double r_high, double delta_z)
    {  this->make(nam, r_low, r_high, delta_z);  }
    /// Constructor to create a new identified object with attribute initialization
    template<typename R_LOW, typename R_HIGH, typename DELTA_Z>
    Paraboloid(const std::string& nam, R_LOW r_low, R_HIGH r_high, DELTA_Z delta_z)
    {  this->make(nam, _toDouble(r_low), _toDouble(r_high), _toDouble(delta_z));  }

    /// Move Assignment operator
    Paraboloid& operator=(Paraboloid&& copy) = default;
    /// Copy Assignment operator
    Paraboloid& operator=(const Paraboloid& copy) = default;
    /// Set the Paraboloid dimensions
    Paraboloid& setDimensions(double r_low, double r_high, double delta_z);
  };

  /// Class describing a Hyperboloid shape
  /**
   *   For any further documentation please see the following ROOT documentation:
   *   \see http://root.cern.ch/root/html/TGeoHype.html
   *
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class Hyperboloid : public Solid_type<TGeoHype> {
    /// Constructor function to create a new anonymous object with attribute initialization
    void make(const std::string& nam, double rin, double stin, double rout, double stout, double dz);
  public:
    /// Default constructor
    Hyperboloid() = default;
    /// Move Constructor
    Hyperboloid(Hyperboloid&& e) = default;
    /// Copy Constructor
    Hyperboloid(const Hyperboloid& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> Hyperboloid(const Q* p) : Solid_type<Object>(p) {  }
    /// Constructor to be used when passing an already created object
    template <typename Q> Hyperboloid(const Handle<Q>& e) : Solid_type<Object>(e) {   }

    /// Constructor to create a new anonymous object with attribute initialization
    Hyperboloid(double rin, double stin, double rout, double stout, double dz)
    { make("", rin, stin, rout, stout, dz);    }
    /// Constructor to create a new anonymous object with attribute initialization
    template <typename RIN, typename STIN, typename ROUT, typename STOUT, typename DZ>
    Hyperboloid(RIN rin, STIN stin, ROUT rout, STOUT stout, DZ dz)
    { make("", _toDouble(rin), _toDouble(stin), _toDouble(rout), _toDouble(stout), _toDouble(dz));   }

    /// Constructor to create a new identified object with attribute initialization
    Hyperboloid(const std::string& nam, double rin, double stin, double rout, double stout, double dz)
    { make(nam, rin, stin, rout, stout, dz);    }
    /// Constructor to create a new identified object with attribute initialization
    template <typename RIN, typename STIN, typename ROUT, typename STOUT, typename DZ>
    Hyperboloid(const std::string& nam, RIN rin, STIN stin, ROUT rout, STOUT stout, DZ dz)
    { make(nam, _toDouble(rin), _toDouble(stin), _toDouble(rout), _toDouble(stout), _toDouble(dz));  }

    /// Move Assignment operator
    Hyperboloid& operator=(Hyperboloid&& copy) = default;
    /// Copy Assignment operator
    Hyperboloid& operator=(const Hyperboloid& copy) = default;
    /// Set the Hyperboloid dimensions
    Hyperboloid& setDimensions(double rin, double stin, double rout, double stout, double dz);
  };

  /// Class describing a regular polyhedron shape
  /**
   *   For any further documentation please see the following ROOT documentation:
   *   \see http://root.cern.ch/root/html/TGeoPgon.html
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class PolyhedraRegular : public Solid_type<TGeoPgon> {
  protected:
    /// Helper function to create the polyhedron
    void make(const std::string& nam, int nsides, double rmin, double rmax, double zpos, double zneg, double start, double delta);
  public:
    /// Default constructor
    PolyhedraRegular() = default;
    /// Move Constructor
    PolyhedraRegular(PolyhedraRegular&& e) = default;
    /// Copy Constructor
    PolyhedraRegular(const PolyhedraRegular& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> PolyhedraRegular(const Q* p) : Solid_type<Object>(p) {  }
    /// Constructor to be used when passing an already created object
    template <typename Q> PolyhedraRegular(const Handle<Q>& e) : Solid_type<Object>(e) {  }

    /// Constructor to create a new object. Phi(start)=0, deltaPhi=2PI, Z-planes at -zlen/2 and +zlen/2
    PolyhedraRegular(int nsides, double rmin, double rmax, double zlen)
    { this->make("", nsides, rmin, rmax, zlen / 2, -zlen / 2, 0, 2.0*M_PI);           }
    /// Constructor to create a new object with phi_start, deltaPhi=2PI, Z-planes at -zlen/2 and +zlen/2
    PolyhedraRegular(int nsides, double phi_start, double rmin, double rmax, double zlen)
    { this->make("", nsides, rmin, rmax, zlen / 2, -zlen / 2, phi_start, 2.0*M_PI);   }
    /// Constructor to create a new object. Phi(start)=0, deltaPhi=2PI, Z-planes a zplanes[0] and zplanes[1]
    PolyhedraRegular(int nsides, double rmin, double rmax, double zplanes[2])
    { this->make("", nsides, rmin, rmax, zplanes[0], zplanes[1], 0, 2.0*M_PI);        }

    /// Constructor to create a new object. Phi(start)=0, deltaPhi=2PI, Z-planes at -zlen/2 and +zlen/2
    PolyhedraRegular(const std::string& nam, int nsides, double rmin, double rmax, double zlen)
    { this->make(nam, nsides, rmin, rmax, zlen / 2, -zlen / 2, 0, 2.0*M_PI);          }
    /// Constructor to create a new object with phi_start, deltaPhi=2PI, Z-planes at -zlen/2 and +zlen/2
    PolyhedraRegular(const std::string& nam, int nsides, double phi_start, double rmin, double rmax, double zlen)
    { this->make(nam, nsides, rmin, rmax, zlen / 2, -zlen / 2, phi_start, 2.0*M_PI);  }
    /// Constructor to create a new object. Phi(start)=0, deltaPhi=2PI, Z-planes a zplanes[0] and zplanes[1]
    PolyhedraRegular(const std::string& nam, int nsides, double rmin, double rmax, double zplanes[2])
    { this->make(nam, nsides, rmin, rmax, zplanes[0], zplanes[1], 0, 2.0*M_PI);       }
    /// Move Assignment operator
    PolyhedraRegular& operator=(PolyhedraRegular&& copy) = default;
    /// Copy Assignment operator
    PolyhedraRegular& operator=(const PolyhedraRegular& copy) = default;
  };

  /// Class describing a regular polyhedron shape
  /**
   *   For any further documentation please see the following ROOT documentation:
   *   \see http://root.cern.ch/root/html/TGeoPgon.html
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class Polyhedra : public Solid_type<TGeoPgon> {
  protected:
    /// Helper function to create the polyhedron
    void make(const std::string& nam, int nsides, double start, double delta,
              const std::vector<double>& z,
              const std::vector<double>& rmin,
              const std::vector<double>& rmax);
  public:
    /// Default constructor
    Polyhedra() = default;
    /// Move Constructor
    Polyhedra(Polyhedra&& e) = default;
    /// Copy Constructor
    Polyhedra(const Polyhedra& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> Polyhedra(const Q* p) : Solid_type<Object>(p)  {         }
    /// Constructor to be used when passing an already created object
    template <typename Q> Polyhedra(const Handle<Q>& e) : Solid_type<Object>(e) {  }

    /// Constructor to create a new object. Phi(start), deltaPhi, Z-planes at specified positions
    Polyhedra(int nsides, double start, double delta,
              const std::vector<double>& z, const std::vector<double>& r)   {
      std::vector<double> rmin(r.size(), 0e0);
      this->make("", nsides, start, delta, z, rmin, r);
    }
    /// Constructor to create a new object. Phi(start), deltaPhi, Z-planes at specified positions
    Polyhedra(int nsides, double start, double delta,
              const std::vector<double>& z, const std::vector<double>& rmin, const std::vector<double>& rmax)
    {  this->make("", nsides, start, delta, z, rmin, rmax);   }

    /// Constructor to create a new object. Phi(start), deltaPhi, Z-planes at specified positions
    Polyhedra(const std::string& nam, int nsides, double start, double delta,
              const std::vector<double>& z, const std::vector<double>& r)   {
      std::vector<double> rmin(r.size(), 0e0);
      this->make(nam, nsides, start, delta, z, rmin, r);
    }
    /// Constructor to create a new object. Phi(start), deltaPhi, Z-planes at specified positions
    Polyhedra(const std::string& nam, int nsides, double start, double delta,
              const std::vector<double>& z, const std::vector<double>& rmin, const std::vector<double>& rmax)
    {  this->make(nam, nsides, start, delta, z, rmin, rmax);   }
    /// Move Assignment operator
    Polyhedra& operator=(Polyhedra&& copy) = default;
    /// Copy Assignment operator
    Polyhedra& operator=(const Polyhedra& copy) = default;
  };

  /// Class describing a extruded polygon shape
  /**
   *   For any further documentation please see the following ROOT documentation:
   *   \see http://root.cern.ch/root/html/TGeoXtru.html
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class ExtrudedPolygon : public Solid_type<TGeoXtru> {
  protected:
    /// Helper function to create the polygon
    void make(const std::string& nam,
              const std::vector<double> & pt_x,
              const std::vector<double> & pt_y,
              const std::vector<double> & sec_z,
              const std::vector<double> & sec_x,
              const std::vector<double> & sec_y,
              const std::vector<double> & zscale);
  public:
    /// Default constructor
    ExtrudedPolygon() = default;
    /// Move Constructor
    ExtrudedPolygon(ExtrudedPolygon&& e) = default;
    /// Copy Constructor
    ExtrudedPolygon(const ExtrudedPolygon& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> ExtrudedPolygon(const Q* p) : Solid_type<Object>(p) {  }
    /// Constructor to be used when passing an already created object
    template <typename Q> ExtrudedPolygon(const Handle<Q>& e) : Solid_type<Object>(e) {  }

    /// Constructor to create a new object. 
    ExtrudedPolygon(const std::vector<double> & pt_x,
                    const std::vector<double> & pt_y,
                    const std::vector<double> & sec_z,
                    const std::vector<double> & sec_x,
                    const std::vector<double> & sec_y,
                    const std::vector<double> & zscale)
    {  this->make("", pt_x, pt_y, sec_z, sec_x, sec_y, zscale);   }

    /// Constructor to create a new identified object. 
    ExtrudedPolygon(const std::string& nam,
                    const std::vector<double> & pt_x,
                    const std::vector<double> & pt_y,
                    const std::vector<double> & sec_z,
                    const std::vector<double> & sec_x,
                    const std::vector<double> & sec_y,
                    const std::vector<double> & zscale)
    {  this->make(nam, pt_x, pt_y, sec_z, sec_x, sec_y, zscale);   }
    /// Move Assignment operator
    ExtrudedPolygon& operator=(ExtrudedPolygon&& copy) = default;
    /// Copy Assignment operator
    ExtrudedPolygon& operator=(const ExtrudedPolygon& copy) = default;
  };

  /// Class describing an arbitray solid defined by 8 vertices.
  /**
   *   For any further documentation please see the following ROOT documentation:
   *   \see http://root.cern.ch/root/html/TGeoArb8.html
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class EightPointSolid : public Solid_type<TGeoArb8> {
  private:
    /// Internal helper method to support object construction
    void make(const std::string& nam, double dz, const double* vtx);
  public:
    /// Default constructor
    EightPointSolid() = default;
    /// Move Constructor
    EightPointSolid(EightPointSolid&& e) = default;
    /// Copy Constructor
    EightPointSolid(const EightPointSolid& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> EightPointSolid(const Q* p) : Solid_type<Object>(p) { }
    /// Constructor to be used when passing an already created object
    template <typename Q> EightPointSolid(const Handle<Q>& e) : Solid_type<Object>(e) { }

    /// Constructor to create a new anonymous object with attribute initialization
    EightPointSolid(double dz, const double* vertices)
    { this->make("", dz, vertices);    }

    /// Constructor to create a new identified object with attribute initialization
    EightPointSolid(const std::string& nam, double dz, const double* vertices)
    { this->make(nam, dz, vertices);   }

    /// Move Assignment operator
    EightPointSolid& operator=(EightPointSolid&& copy) = default;
    /// Copy Assignment operator
    EightPointSolid& operator=(const EightPointSolid& copy) = default;
  };

  /// Base class describing boolean (=union,intersection,subtraction) solids
  /**
   *   For any further documentation please see the following ROOT documentation:
   *   \see http://root.cern.ch/root/html/TGeoCompositeShape.html
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class BooleanSolid : public Solid_type<TGeoCompositeShape> {
  public:
    /// Default constructor
    BooleanSolid() = default;
    /// Move Constructor
    BooleanSolid(BooleanSolid&& b) = default;
    /// Copy Constructor
    BooleanSolid(const BooleanSolid& b) = default;
    /// Constructor to be used when passing an already created object
    template <typename Q>
    BooleanSolid(const Handle<Q>& e) : Solid_type<Object>(e) { }
    /// Move Assignment operator
    BooleanSolid& operator=(BooleanSolid&& copy) = default;
    /// Copy Assignment operator
    BooleanSolid& operator=(const BooleanSolid& copy) = default;
  };

  /// Class describing boolean subtraction solid
  /**
   *   For any further documentation please see the following ROOT documentation:
   *   \see http://root.cern.ch/root/html/TGeoCompositeShape.html
   *   \see http://root.cern.ch/root/html/TGeoSubtraction.html
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class SubtractionSolid : public BooleanSolid {
  public:
    /// Default constructor
    SubtractionSolid() = default;
    /// Move Constructor
    SubtractionSolid(SubtractionSolid&& e) = default;
    /// Copy Constructor
    SubtractionSolid(const SubtractionSolid& e) = default;
    /// Constructor to be used when passing an already created object
    template <typename Q> SubtractionSolid(const Handle<Q>& e) : BooleanSolid(e) {  }

    /// Constructor to create a new object. Position is identity, Rotation is identity-rotation!
    SubtractionSolid(const Solid& shape1, const Solid& shape2);
    /// Constructor to create a new object. Placement by position, Rotation is identity-rotation!
    SubtractionSolid(const Solid& shape1, const Solid& shape2, const Position& pos);
    /// Constructor to create a new object. Placement by a RotationZYX within the mother
    SubtractionSolid(const Solid& shape1, const Solid& shape2, const RotationZYX& rot);
    /// Constructor to create a new object. Placement by a generic rotoation within the mother
    SubtractionSolid(const Solid& shape1, const Solid& shape2, const Rotation3D& rot);
    /// Constructor to create a new object. Placement by a generic transformation within the mother
    SubtractionSolid(const Solid& shape1, const Solid& shape2, const Transform3D& pos);

    /// Constructor to create a new identified object. Position is identity, Rotation is identity-rotation!
    SubtractionSolid(const std::string& name, const Solid& shape1, const Solid& shape2);
    /// Constructor to create a new identified object. Placement by position, Rotation is identity-rotation!
    SubtractionSolid(const std::string& name, const Solid& shape1, const Solid& shape2, const Position& pos);
    /// Constructor to create a new identified object. Placement by a RotationZYX within the mother
    SubtractionSolid(const std::string& name, const Solid& shape1, const Solid& shape2, const RotationZYX& rot);
    /// Constructor to create a new identified object. Placement by a generic rotoation within the mother
    SubtractionSolid(const std::string& name, const Solid& shape1, const Solid& shape2, const Rotation3D& rot);
    /// Constructor to create a new identified object. Placement by a generic transformation within the mother
    SubtractionSolid(const std::string& name, const Solid& shape1, const Solid& shape2, const Transform3D& pos);
    /// Move Assignment operator
    SubtractionSolid& operator=(SubtractionSolid&& copy) = default;
    /// Copy Assignment operator
    SubtractionSolid& operator=(const SubtractionSolid& copy) = default;
  };

  /// Class describing boolean union solid
  /**
   *   For any further documentation please see the following ROOT documentation:
   *   \see http://root.cern.ch/root/html/TGeoCompositeShape.html
   *   \see http://root.cern.ch/root/html/TGeoUnion.html
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class UnionSolid : public BooleanSolid {
  public:
    /// Default constructor
    UnionSolid() = default;
    /// Move Constructor
    UnionSolid(UnionSolid&& e) = default;
    /// Copy Constructor
    UnionSolid(const UnionSolid& e) = default;
    /// Constructor to be used when passing an already created object
    template <typename Q> UnionSolid(const Handle<Q>& e) : BooleanSolid(e) { }

    /// Constructor to create a new object. Position is identity, Rotation is identity-rotation!
    UnionSolid(const Solid& shape1, const Solid& shape2);
    /// Constructor to create a new object. Placement by position, Rotation is identity-rotation!
    UnionSolid(const Solid& shape1, const Solid& shape2, const Position& pos);
    /// Constructor to create a new object. Placement by a RotationZYX within the mother
    UnionSolid(const Solid& shape1, const Solid& shape2, const RotationZYX& rot);
    /// Constructor to create a new object. Placement by a generic rotoation within the mother
    UnionSolid(const Solid& shape1, const Solid& shape2, const Rotation3D& rot);
    /// Constructor to create a new object. Placement by a generic transformation within the mother
    UnionSolid(const Solid& shape1, const Solid& shape2, const Transform3D& pos);

    /// Constructor to create a new identified object. Position is identity, Rotation is identity-rotation!
    UnionSolid(const std::string& name, const Solid& shape1, const Solid& shape2);
    /// Constructor to create a new identified object. Placement by position, Rotation is identity-rotation!
    UnionSolid(const std::string& name, const Solid& shape1, const Solid& shape2, const Position& pos);
    /// Constructor to create a new identified object. Placement by a RotationZYX within the mother
    UnionSolid(const std::string& name, const Solid& shape1, const Solid& shape2, const RotationZYX& rot);
    /// Constructor to create a new identified object. Placement by a generic rotoation within the mother
    UnionSolid(const std::string& name, const Solid& shape1, const Solid& shape2, const Rotation3D& rot);
    /// Constructor to create a new identified object. Placement by a generic transformation within the mother
    UnionSolid(const std::string& name, const Solid& shape1, const Solid& shape2, const Transform3D& pos);
    /// Move Assignment operator
    UnionSolid& operator=(UnionSolid&& copy) = default;
    /// Copy Assignment operator
    UnionSolid& operator=(const UnionSolid& copy) = default;
  };

  /// Class describing boolean intersection solid
  /**
   *   For any further documentation please see the following ROOT documentation:
   *   \see http://root.cern.ch/root/html/TGeoCompositeShape.html
   *   \see http://root.cern.ch/root/html/TGeoIntersection.html
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class IntersectionSolid : public BooleanSolid {
  public:
    /// Default constructor
    IntersectionSolid() = default;
    /// Move Constructor
    IntersectionSolid(IntersectionSolid&& e) = default;
    /// Copy Constructor
    IntersectionSolid(const IntersectionSolid& e) = default;
    /// Constructor to be used when passing an already created object
    template <typename Q> IntersectionSolid(const Handle<Q>& e) : BooleanSolid(e) { }

    /// Constructor to create a new object. Position is identity, Rotation is identity-rotation!
    IntersectionSolid(const Solid& shape1, const Solid& shape2);
    /// Constructor to create a new object. Placement by position, Rotation is identity-rotation!
    IntersectionSolid(const Solid& shape1, const Solid& shape2, const Position& pos);
    /// Constructor to create a new object. Placement by a RotationZYX within the mother
    IntersectionSolid(const Solid& shape1, const Solid& shape2, const RotationZYX& rot);
    /// Constructor to create a new object. Placement by a generic rotoation within the mother
    IntersectionSolid(const Solid& shape1, const Solid& shape2, const Rotation3D& rot);
    /// Constructor to create a new object. Placement by a generic transformation within the mother
    IntersectionSolid(const Solid& shape1, const Solid& shape2, const Transform3D& pos);

    /// Constructor to create a new identified object. Position is identity, Rotation is identity-rotation!
    IntersectionSolid(const std::string& name, const Solid& shape1, const Solid& shape2);
    /// Constructor to create a new identified object. Placement by position, Rotation is identity-rotation!
    IntersectionSolid(const std::string& name, const Solid& shape1, const Solid& shape2, const Position& pos);
    /// Constructor to create a new identified object. Placement by a RotationZYX within the mother
    IntersectionSolid(const std::string& name, const Solid& shape1, const Solid& shape2, const RotationZYX& rot);
    /// Constructor to create a new identified object. Placement by a generic rotoation within the mother
    IntersectionSolid(const std::string& name, const Solid& shape1, const Solid& shape2, const Rotation3D& rot);
    /// Constructor to create a new identified object. Placement by a generic transformation within the mother
    IntersectionSolid(const std::string& name, const Solid& shape1, const Solid& shape2, const Transform3D& pos);
    /// Move Assignment operator
    IntersectionSolid& operator=(IntersectionSolid&& copy) = default;
    /// Copy Assignment operator
    IntersectionSolid& operator=(const IntersectionSolid& copy) = default;
  };

}         /* End namespace dd4hep             */
#endif    /* DD4HEP_DDCORE_SOLIDS_H         */
