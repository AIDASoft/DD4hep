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
#include "TGeoParaboloid.h"
#include "TGeoPgon.h"
#include "TGeoPcon.h"
#include "TGeoSphere.h"
#include "TGeoArb8.h"
#include "TGeoTrd1.h"
#include "TGeoTrd2.h"
#include "TGeoTube.h"
#include "TGeoEltu.h"
#include "TGeoXtru.h"
#include "TGeoHype.h"
#include "TGeoTorus.h"
#include "TGeoHalfSpace.h"
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
  protected:
    void _setDimensions(double* param);
    /// Assign pointrs and register solid to geometry
    void _assign(T* n, const std::string& nam, const std::string& tit, bool cbbox);

  public:

    /// Default constructor for uninitialized object
    Solid_type() = default;
    /// Constructor to be used when passing an already created object
    Solid_type(const Solid_type& e) = default;
    /// Direct assignment using the implementation pointer
    Solid_type(T* p) : Handle<T>(p) {  }
    /// Constructor to be used when passing an already created object
    Solid_type(const Handle<T>& e) : Handle<T>(e) {  }
    /// Constructor to be used when passing an already created object: need to check pointers
    template <typename Q> Solid_type(const Handle<Q>& e) : Handle<T>(e) {  }
    /// Assignment operator
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
    /// Constructor to be used when passing an already created object
    ShapelessSolid(const ShapelessSolid& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> ShapelessSolid(const Q* p) : Solid_type<TGeoShapeAssembly>(p) { }
    /// Constructor to be used with an existing object
    template <typename Q> ShapelessSolid(const Handle<Q>& e) : Solid_type<TGeoShapeAssembly>(e) { }
    /// Constructor to create an anonymous new shapeless solid
    ShapelessSolid(const std::string& name);
    /// Assignment operator
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
  class Box: public Solid_type<TGeoBBox> {
  protected:
    /// Internal helper method to support object construction
    void make(double x_val, double y_val, double z_val);

  public:
    /// Default constructor
    Box() = default;
    /// Constructor to be used when passing an already created object
    Box(const Box& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> Box(const Q* p) : Solid_type<TGeoBBox>(p) { }
    /// Constructor to be used with an existing object
    template <typename Q> Box(const Handle<Q>& e) : Solid_type<TGeoBBox>(e) { }
    /// Constructor to create an anonymous new box object (retrieves name from volume)
    Box(double x_val, double y_val, double z_val) {  make(x_val, y_val, z_val);  }
    /// Constructor to create an anonymous new box object (retrieves name from volume)
    template <typename X, typename Y, typename Z>
    Box(const X& x_val, const Y& y_val, const Z& z_val)  {
      make(_toDouble(x_val), _toDouble(y_val), _toDouble(z_val));
    }
    /// Assignment operator
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
  class HalfSpace: public Solid_type<TGeoHalfSpace> {
  protected:
    /// Internal helper method to support object construction
    void make(const double* const point, const double* const normal);

  public:
    /// Default constructor
    HalfSpace() = default;
    /// Constructor to be used when passing an already created object
    HalfSpace(const HalfSpace& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> HalfSpace(const Q* p) : Solid_type<Object>(p) { }
    /// Constructor to be used with an existing object
    template <typename Q> HalfSpace(const Handle<Q>& e) : Solid_type<Object>(e) { }
    /// Constructor to create an new halfspace object from a point on a plane and the plane normal
    HalfSpace(const double* const point, const double* const normal) { make(point,normal);  }
    /// Assignment operator
    HalfSpace& operator=(const HalfSpace& copy) = default;
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
  class Polycone: public Solid_type<TGeoPcon> {
  public:
    /// Default constructor
    Polycone() = default;
    /// Constructor to be used when passing an already created object
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
    /// Assignment operator
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
  class ConeSegment: public Solid_type<TGeoConeSeg> {
  public:
    /// Default constructor
    ConeSegment() = default;
    /// Constructor to be used when passing an already created object
    ConeSegment(const ConeSegment& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> ConeSegment(const Q* p) : Solid_type<Object>(p) {  }
    /// Constructor to be used when reading the already parsed ConeSegment object
    template <typename Q> ConeSegment(const Handle<Q>& e) : Solid_type<Object>(e) {  }
    /// Constructor to create a new ConeSegment object
    ConeSegment(double dz, double rmin1, double rmax1,
                double rmin2, double rmax2,
                double phi1 = 0.0, double phi2 = 2.0 * M_PI);
    /// Assignment operator
    ConeSegment& operator=(const ConeSegment& copy) = default;

    /// Set the cone segment dimensions
    ConeSegment& setDimensions(double dz, double rmin1, double rmax1,
                               double rmin2, double rmax2,
                               double phi1 = 0.0, double phi2 = 2.0 * M_PI);
  };
#if 0
  /// Intermediate class to overcome drawing probles with the TGeoTubeSeg
  class MyConeSeg: public TGeoConeSeg {
  public:
    MyConeSeg() : TGeoConeSeg(0.0,0.0,0.0,0.0,0.0,0.0,0.0) { }
    virtual ~MyConeSeg() { }
    double GetRmin() const {        return GetRmin1();      }
    double GetRmax() const {        return GetRmax1();      }
  };
#endif
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
  class Tube: public Solid_type<TGeoTubeSeg> {
  protected:
    /// Internal helper method to support object construction
    void make(const std::string& nam, double rmin, double rmax, double z, double startPhi, double deltaPhi);

  public:
    /// Default constructor
    Tube() = default;
    /// Constructor to be used when passing an already created object
    Tube(const Tube& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> Tube(const Q* p) : Solid_type<Object>(p) {  }
    /// Constructor to assign an object
    template <typename Q> Tube(const Handle<Q>& e) : Solid_type<Object>(e) {  }
    /// Constructor to create a new anonymous tube object with attribute initialization
    Tube(double rmin, double rmax, double dz)
    {   make("", rmin, rmax, dz, 0, 2*M_PI);               }
    /// Constructor to create a new anonymous tube object with attribute initialization
    Tube(double rmin, double rmax, double dz, double deltaPhi)
    {   make("", rmin, rmax, dz, 0, deltaPhi);             }
    /// Constructor to create a new anonymous tube object with attribute initialization
    Tube(double rmin, double rmax, double dz, double startPhi, double deltaPhi)
    {   make("", rmin, rmax, dz, startPhi, deltaPhi);      }
    /// Legacy: Constructor to create a new identifiable tube object with attribute initialization
    Tube(const std::string& nam, double rmin, double rmax, double dz)
    {   make(nam, rmin, rmax, dz, 0, 2*M_PI);              }
    /// Legacy: Constructor to create a new identifiable tube object with attribute initialization
    Tube(const std::string& nam, double rmin, double rmax, double dz, double deltaPhi)
    {  make(nam, rmin, rmax, dz, 0, deltaPhi);             }
    /// Legacy: Constructor to create a new identifiable tube object with attribute initialization
    Tube(const std::string& nam, double rmin, double rmax, double dz, double startPhi, double deltaPhi)
    {  make(nam, rmin, rmax, dz, startPhi, startPhi+deltaPhi); }
    /// Constructor to create a new anonymous tube object with attribute initialization
    template <typename RMIN, typename RMAX, typename Z, typename DELTAPHI=double>
    Tube(const RMIN& rmin, const RMAX& rmax, const Z& dz, const DELTAPHI& deltaPhi = 2.0*M_PI)
    {  make("", _toDouble(rmin), _toDouble(rmax), _toDouble(dz), 0, _toDouble(deltaPhi));   }
    /// Assignment operator
    Tube& operator=(const Tube& copy) = default;
    /// Set the tube dimensions
    Tube& setDimensions(double rmin, double rmax, double dz, double startPhi=0.0, double deltaPhi=2*M_PI);
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
  class CutTube: public Solid_type<TGeoCtub> {
  protected:
    /// Internal helper method to support object construction
    void make(double rmin, double rmax, double dz, double phi1, double phi2,
              double lx, double ly, double lz, double tx, double ty, double tz);

  public:
    /// Default constructor
    CutTube() = default;
    /// Constructor to be used when passing an already created object
    CutTube(const CutTube& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> CutTube(const Q* p) : Solid_type<Object>(p) {  }
    /// Constructor to assign an object
    template <typename Q> CutTube(const Handle<Q>& e) : Solid_type<Object>(e) {  }
    /// Legacy: Constructor to create a new identifiable tube object with attribute initialization
    CutTube(double rmin, double rmax, double dz, double phi1, double phi2,
            double lx, double ly, double lz, double tx, double ty, double tz);
    /// Assignment operator
    CutTube& operator=(const CutTube& copy) = default;
  };

  
  /// Class describing a truncated tube shape (CMS'ism)
  /**
   *   No real correspondance to TGeo. In principle it's a boolean Solid based on a tube.
   *   \see http://cmssdt.cern.ch/lxr/source/DetectorDescription/Core/src/TruncTubs.h
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class TruncatedTube: public Solid_type<TGeoCompositeShape> {
  protected:
    /// Internal helper method to support object construction
    void make(double zhalf, double rmin, double rmax, double startPhi, double deltaPhi,
                  double cutAtStart, double cutAtDelta, bool cutInside);

  public:
    /// Default constructor
    TruncatedTube() = default;
    /// Constructor to be used when passing an already created object
    TruncatedTube(const TruncatedTube& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> TruncatedTube(const Q* p) : Solid_type<Object>(p) {  }
    /// Constructor to assign an object
    template <typename Q> TruncatedTube(const Handle<Q>& e) : Solid_type<Object>(e) {  }
    /// Constructor to create a truncated tube object with attribute initialization
    TruncatedTube(double zhalf, double rmin, double rmax, double startPhi, double deltaPhi,
                  double cutAtStart, double cutAtDelta, bool cutInside);
    /// Assignment operator
    TruncatedTube& operator=(const TruncatedTube& copy) = default;
  };
  
  /// Class describing a elliptical tube shape
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
  class EllipticalTube: public Solid_type<TGeoEltu> {
  protected:
    /// Internal helper method to support object construction
    void make(double a, double b, double dz);

  public:
    /// Default constructor
    EllipticalTube() = default;
    /// Constructor to be used when passing an already created object
    EllipticalTube(const EllipticalTube& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> EllipticalTube(const Q* p) : Solid_type<Object>(p) {   }
    /// Constructor to assign an object
    template <typename Q> EllipticalTube(const Handle<Q>& e) : Solid_type<Object>(e) {   }
    /// Constructor to create a new anonymous tube object with attribute initialization
    EllipticalTube(double a, double b, double dz) {  make(a, b, dz);  }
    /// Constructor to create a new anonymous tube object with attribute initialization
    template <typename A, typename B, typename DZ>
    EllipticalTube(const A& a, const B& b, const DZ& dz)
    {  make(_toDouble(a), _toDouble(b), _toDouble(dz));   }
    /// Assignment operator
    EllipticalTube& operator=(const EllipticalTube& copy) = default;
    /// Set the tube dimensions
    EllipticalTube& setDimensions(double a, double b, double dz);
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
  class Cone: public Solid_type<TGeoCone> {
  protected:
    /// Internal helper method to support object construction
    void make(double z, double rmin1, double rmax1, double rmin2, double rmax2);
  public:
    /// Default constructor
    Cone() = default;
    /// Constructor to be used when passing an already created object
    Cone(const Cone& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> Cone(const Q* p) : Solid_type<Object>(p) { }
    /// Constructor to be used when passing an already created object
    template <typename Q> Cone(const Handle<Q>& e) : Solid_type<Object>(e) { }
    /// Constructor to create a new anonymous object with attribute initialization
    Cone(double z, double rmin1, double rmax1, double rmin2, double rmax2)
    {     make(z, rmin1, rmax1, rmin2, rmax2);                                 }
    template <typename Z, typename RMIN1, typename RMAX1, typename RMIN2, typename RMAX2>
    Cone(const Z& z, const RMIN1& rmin1, const RMAX1& rmax1, const RMIN2& rmin2, const RMAX2& rmax2)
    {     make(_toDouble(z), _toDouble(rmin1), _toDouble(rmax1), _toDouble(rmin2), _toDouble(rmax2)); }
    /// Assignment operator
    Cone& operator=(const Cone& copy) = default;
    /// Set the box dimensions
    Cone& setDimensions(double z, double rmin1, double rmax1, double rmin2, double rmax2);
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
  class Trap: public Solid_type<TGeoTrap> {
  private:
    /// Internal helper method to support object construction
    void make(double pz, double py, double px, double pLTX);
  public:
    /// Default constructor
    Trap() = default;
    /// Constructor to be used when passing an already created object
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
    Trap(double pz, double py, double px, double pLTX)  {  make(pz,py,px,pLTX);  }
    /// Constructor to create a new anonymous object with attribute initialization
    template <typename PZ,typename PY,typename PX,typename PLTX> Trap(PZ pz, PY py, PX px, PLTX pLTX)
    { make(_toDouble(pz),_toDouble(py),_toDouble(px),_toDouble(pLTX)); }
    /// Assignment operator
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
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class PseudoTrap: public Solid_type<TGeoCompositeShape> {
  private:
    /// Internal helper method to support object construction
    void make(double x1, double x2, double y1, double y2, double z, double radius, bool minusZ);
  public:
    /// Default constructor
    PseudoTrap() = default;
    /// Constructor to be used when passing an already created object
    PseudoTrap(const PseudoTrap& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> PseudoTrap(const Q* p) : Solid_type<Object>(p) { }
    /// Constructor to be used when passing an already created object
    template <typename Q> PseudoTrap(const Handle<Q>& e) : Solid_type<Object>(e) { }
    /// Constructor to create a new anonymous object with attribute initialization
    PseudoTrap(double x1, double x2, double y1, double y2, double z, double radius, bool minusZ)
    {  make(x1, x2, y1, y2, z, radius, minusZ);    }
    /// Assignment operator
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
  class Trd1: public Solid_type<TGeoTrd1> {
  private:
    /// Internal helper method to support object construction
    void make(double x1, double x2, double y, double z);

  public:
    /// Default constructor
    Trd1() = default;
    /// Constructor to be used when passing an already created object
    Trd1(const Trd1& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> Trd1(const Q* p) : Solid_type<Object>(p) { }
    /// Constructor to be used when passing an already created object
    template <typename Q> Trd1(const Handle<Q>& e) : Solid_type<Object>(e) { }
    /// Constructor to create a new anonymous object with attribute initialization
    Trd1(double x1, double x2, double y, double z);
    /// Constructor to create a new anonymous object with attribute initialization
    template <typename X1,typename X2,typename Y,typename Z>
    Trd1(X1 x1, X2 x2, Y y, Z z)
    { make(_toDouble(x1),_toDouble(x2),_toDouble(y),_toDouble(z)); }
    /// Assignment operator
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
  class Trd2: public Solid_type<TGeoTrd2> {
  private:
    /// Internal helper method to support object construction
    void make(double x1, double x2, double y1, double y2, double z);

  public:
    /// Default constructor
    Trd2() = default;
    /// Constructor to be used when passing an already created object
    Trd2(const Trd2& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> Trd2(const Q* p) : Solid_type<Object>(p) { }
    /// Constructor to be used when passing an already created object
    template <typename Q> Trd2(const Handle<Q>& e) : Solid_type<Object>(e) { }
    /// Constructor to create a new anonymous object with attribute initialization
    Trd2(double x1, double x2, double y1, double y2, double z);
    /// Constructor to create a new anonymous object with attribute initialization
    template <typename X1,typename X2,typename Y1,typename Y2,typename Z>
    Trd2(X1 x1, X2 x2, Y1 y1, Y2 y2, Z z)
    { make(_toDouble(x1),_toDouble(x2),_toDouble(y1),_toDouble(y2),_toDouble(z)); }
    /// Assignment operator
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
  class Torus: public Solid_type<TGeoTorus> {
  private:
    /// Internal helper method to support object construction
    void make(double r, double rmin, double rmax, double phi, double delta_phi);
  public:
    /// Default constructor
    Torus() = default;
    /// Constructor to be used when passing an already created object
    Torus(const Torus& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> Torus(const Q* p) : Solid_type<Object>(p) { }
    /// Constructor to be used when passing an already created object
    template <typename Q> Torus(const Handle<Q>& e) : Solid_type<Object>(e) {  }
    /// Constructor to create a new anonymous object with attribute initialization
    template<typename R, typename RMIN, typename RMAX, typename PHI, typename DELTA_PHI>
    Torus(R r, RMIN rmin, RMAX rmax, PHI phi=M_PI, DELTA_PHI delta_phi = 2.*M_PI)
    {   make(_toDouble(r),_toDouble(rmin),_toDouble(rmax),_toDouble(phi),_toDouble(delta_phi));  }
    /// Constructor to create a new anonymous object with attribute initialization
    Torus(double r, double rmin, double rmax, double phi=M_PI, double delta_phi = 2.*M_PI)
    {   make(r,rmin,rmax,phi,delta_phi);  }
    /// Assignment operator
    Torus& operator=(const Torus& copy) = default;
    /// Set the Torus dimensions
    Torus& setDimensions(double r, double rmin, double rmax, double phi, double delta_phi);
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
  class Sphere: public Solid_type<TGeoSphere> {
  protected:
    /// Constructor function to be used when creating a new object with attribute initialization
    void make(double rmin, double rmax, double theta, double delta_theta, double phi, double delta_phi);
  public:
    /// Default constructor
    Sphere() = default;
    /// Constructor to be used when passing an already created object
    Sphere(const Sphere& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> Sphere(const Q* p) : Solid_type<Object>(p) { }
    /// Constructor to be used when passing an already created object
    template <typename Q> Sphere(const Handle<Q>& e) : Solid_type<Object>(e) {  }
    /// Constructor to create a new anonymous object with attribute initialization
    Sphere(double rmin,        double rmax,
           double theta = 0.0, double delta_theta = M_PI,
           double phi   = 0.0, double delta_phi   = 2. * M_PI)
    {  make(rmin, rmax, theta, delta_theta, phi, delta_phi);     }
    /// Constructor to create a new anonymous object with generic attribute initialization
    template<typename RMIN,         typename RMAX,
             typename THETA=double, typename DELTA_THETA=double,
             typename PHI=double,   typename DELTA_PHI=double>
    Sphere(RMIN  rmin,         RMAX        rmax,
           THETA theta = 0.0,  DELTA_THETA delta_theta = M_PI,
           PHI   phi   = 0.0,  DELTA_PHI   delta_phi   = 2. * M_PI)  {
      make(_toDOuble(rmin),  _toDouble(rmax),
           _toDouble(theta), _toDouble(delta_theta),
           _toDouble(phi),   _toDouble(delta_phi));
    }
    /// Assignment operator
    Sphere& operator=(const Sphere& copy) = default;
    /// Set the Sphere dimensions
    Sphere& setDimensions(double rmin, double rmax, double theta, double delta_theta, double phi, double delta_phi);
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
  class Paraboloid: public Solid_type<TGeoParaboloid> {
    /// Constructor function to create a new anonymous object with attribute initialization
    void make(double r_low, double r_high, double delta_z);
  public:
    /// Default constructor
    Paraboloid() = default;
    /// Constructor to be used when passing an already created object
    Paraboloid(const Paraboloid& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> Paraboloid(const Q* p) : Solid_type<Object>(p) { }
    /// Constructor to be used when passing an already created object
    template <typename Q> Paraboloid(const Handle<Q>& e) : Solid_type<Object>(e) { }
    /// Constructor to create a new anonymous object with attribute initialization
    Paraboloid(double r_low, double r_high, double delta_z)
    {  make(r_low, r_high, delta_z);  }
    /// Constructor to create a new anonymous object with attribute initialization
    template<typename R_LOW, typename R_HIGH, typename DELTA_Z>
    Paraboloid(R_LOW r_low, R_HIGH r_high, DELTA_Z delta_z)
    {  make(_toDouble(r_low), _toDouble(r_high), _toDouble(delta_z));  }
    /// Assignment operator
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
  class Hyperboloid: public Solid_type<TGeoHype> {
    /// Constructor function to create a new anonymous object with attribute initialization
    void make(double rin, double stin, double rout, double stout, double dz);
  public:
    /// Default constructor
    Hyperboloid() = default;
    /// Constructor to be used when passing an already created object
    Hyperboloid(const Hyperboloid& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> Hyperboloid(const Q* p) : Solid_type<Object>(p) {  }
    /// Constructor to be used when passing an already created object
    template <typename Q> Hyperboloid(const Handle<Q>& e) : Solid_type<Object>(e) {   }
    /// Constructor to create a new anonymous object with attribute initialization
    Hyperboloid(double rin, double stin, double rout, double stout, double dz)   {
      make(rin, stin, rout, stout, dz);
    }
    /// Constructor to create a new anonymous object with attribute initialization
    template <typename RIN, typename STIN, typename ROUT, typename STOUT, typename DZ>
    Hyperboloid(RIN rin, STIN stin, ROUT rout, STOUT stout, DZ dz)  {
      make(_toDouble(rin), _toDouble(stin), _toDouble(rout), _toDouble(stout), _toDouble(dz));
    }
    /// Assignment operator
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
  class PolyhedraRegular: public Solid_type<TGeoPgon> {
  protected:
    /// Helper function to create the polyhedron
    void make(int nsides, double rmin, double rmax, double zpos, double zneg, double start, double delta);
  public:
    /// Default constructor
    PolyhedraRegular() = default;
    /// Constructor to be used when passing an already created object
    PolyhedraRegular(const PolyhedraRegular& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> PolyhedraRegular(const Q* p) : Solid_type<Object>(p) {  }
    /// Constructor to be used when passing an already created object
    template <typename Q> PolyhedraRegular(const Handle<Q>& e) : Solid_type<Object>(e) {  }
    /// Constructor to create a new object. Phi(start)=0, deltaPhi=2PI, Z-planes at -zlen/2 and +zlen/2
    PolyhedraRegular(int nsides, double rmin, double rmax, double zlen);
    /// Constructor to create a new object. Phi(start)=0, deltaPhi=2PI, Z-planes a zplanes[0] and zplanes[1]
    PolyhedraRegular(int nsides, double rmin, double rmax, double zplanes[2]);
    /// Constructor to create a new object with phi_start, deltaPhi=2PI, Z-planes at -zlen/2 and +zlen/2
    PolyhedraRegular(int nsides, double phi_start, double rmin, double rmax, double zlen);
    /// Assignment operator
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
    void make(int nsides, double start, double delta,
              const std::vector<double>& z, const std::vector<double>& rmin, const std::vector<double>& rmax);
  public:
    /// Default constructor
    Polyhedra() = default;
    /// Constructor to be used when passing an already created object
    Polyhedra(const Polyhedra& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> Polyhedra(const Q* p) : Solid_type<Object>(p) {  }
    /// Constructor to be used when passing an already created object
    template <typename Q> Polyhedra(const Handle<Q>& e) : Solid_type<Object>(e) {  }
    /// Constructor to create a new object. Phi(start), deltaPhi, Z-planes at specified positions
    Polyhedra(int nsides, double start, double delta,
              const std::vector<double>& z, const std::vector<double>& r);
    /// Constructor to create a new object. Phi(start), deltaPhi, Z-planes at specified positions
    Polyhedra(int nsides, double start, double delta,
              const std::vector<double>& z, const std::vector<double>& rmin, const std::vector<double>& rmax);
    /// Assignment operator
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
    void make(const std::vector<double> & pt_x,
              const std::vector<double> & pt_y,
              const std::vector<double> & sec_z,
              const std::vector<double> & sec_x,
              const std::vector<double> & sec_y,
              const std::vector<double> & zscale);
  public:
    /// Default constructor
    ExtrudedPolygon() = default;
    /// Constructor to be used when passing an already created object
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
                    const std::vector<double> & zscale);
    /// Assignment operator
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
  class EightPointSolid: public Solid_type<TGeoArb8> {
  private:
    /// Internal helper method to support object construction
    void make(double dz, const double* vtx);
  public:
    /// Default constructor
    EightPointSolid() = default;
    /// Constructor to be used when passing an already created object
    EightPointSolid(const EightPointSolid& e) = default;
    /// Constructor to be used with an existing object
    template <typename Q> EightPointSolid(const Q* p) : Solid_type<Object>(p) { }
    /// Constructor to be used when passing an already created object
    template <typename Q> EightPointSolid(const Handle<Q>& e) : Solid_type<Object>(e) {}
    /// Constructor to create a new anonymous object with attribute initialization
    EightPointSolid(double dz, const double* vertices) { make(dz,vertices);  }
    /// Assignment operator
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
  class BooleanSolid: public Solid_type<TGeoCompositeShape> {
  protected:
    /// Default constructor
    BooleanSolid() = default;
    /// Constructor to be used when passing an already created object
    BooleanSolid(const BooleanSolid& b) = default;
      
  public:
    /// Constructor to be used when passing an already created object
    template <typename Q>
    BooleanSolid(const Handle<Q>& e) : Solid_type<Object>(e) { }
    /// Assignment operator
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
  class SubtractionSolid: public BooleanSolid {
  public:
    /// Default constructor
    SubtractionSolid() = default;
    /// Constructor to be used when passing an already created object
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
    /// Assignment operator
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
  class UnionSolid: public BooleanSolid {
  public:
    /// Default constructor
    UnionSolid() = default;
    /// Constructor to be used when passing an already created object
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
    /// Assignment operator
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
  class IntersectionSolid: public BooleanSolid {
  public:
    /// Default constructor
    IntersectionSolid() = default;
    /// Constructor to be used when passing an already created object
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
    /// Assignment operator
    IntersectionSolid& operator=(const IntersectionSolid& copy) = default;
  };

}         /* End namespace dd4hep             */
#endif    /* DD4HEP_DDCORE_SOLIDS_H         */
