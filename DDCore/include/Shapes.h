#ifndef DETDESC_GEOMETRY_SOLIDS_H
#define DETDESC_GEOMETRY_SOLIDS_H

// Framework include files
#include "Elements.h"

// C/C++ include files
#define _USE_MATH_DEFINES
#include <cmath>
#ifndef M_PI
  #define M_PI 3.14159265358979323846
#endif
#include <vector>

// Forward declarations
class TGeoShape;

/*
 *   DetDesc namespace declaration
 */
namespace DetDesc {

  /*
  *   Geometry namespace declaration
  */
  namespace Geometry  {

    // Forward declarations
    struct Position;
    struct Rotation;

    /**@class ZPlane
    *  Not identifyable object - no RefElement
    *
    *   @author  M.Frank
    *   @version 1.0
    */
    struct ZPlane  {
      double m_rmin, m_rmax, m_z;
      /// Constructor to be used when creating a new object
      ZPlane(double rmin, double rmax, double z);
      /// Set values
      ZPlane& setRmin(double value);
      ZPlane& setRmax(double value);
      ZPlane& setZ(double value);
      double rmin() const { return m_rmin; }
      double rmax() const { return m_rmax; }
      double z()    const { return m_z;    }
    };

    struct Solid : public RefElement  {
      /// Constructor to be used when reading the already parsed object
      Solid(Handle_t e) : RefElement(e) {}
      /// Constructor to be used when creating a new object
      Solid(const Document& doc, const std::string& type, const std::string& name) : RefElement(doc,type,name) {}
      /// Access to the shape pointer
      TGeoShape& shape()  const;
      /// Set dimensions and update bounding box
      void setDimensions(double values[]);
    };

    struct Box : public Solid  {
      /// Constructor to be used when reading the already parsed box object
      Box(Handle_t e) : Solid(e) {}
      /// Constructor to be used when creating a new box object
      Box(const Document& doc, const std::string& name);
      /// Constructor to be used when creating a new box object
      Box(const Document& doc, const std::string& name, double x, double y, double z);
      /// Constructor to be used when creating a new box object
      template<class X, class Y, class Z>
      Box(const Document& doc, const std::string& name, const X& x, const Y& y, const Z& z)
      : Solid(doc,"box",name)
      {
        setDimensions(_toDouble(x),_toDouble(y),_toDouble(z));
      }
      /// Set the box dimensions
      Box& setDimensions(double x, double y, double z);
    };

    struct Polycone : public Solid {
      /// Constructor to be used when reading the already parsed polycone object
      Polycone(Handle_t e) : Solid(e) {}
      /// Constructor to be used when creating a new polycone object
      Polycone(const Document& doc, const std::string& name);
      /// Constructor to be used when creating a new polycone object
      Polycone(const Document& doc, const std::string& name, double start, double delta);
      /// Constructor to be used when creating a new polycone object. Add at the same time all Z planes
      Polycone(const Document& doc, const std::string& name, double start, double delta, const std::vector<double>& rmin, const std::vector<double>& rmax, const std::vector<double>& z);
      /// Add Z-planes to the Polycone
      void addZPlanes(const std::vector<double>& rmin, const std::vector<double>& rmax, const std::vector<double>& z);
    };

    struct Tube : public Solid  {
      /// Constructor to be used when reading the already parsed object
      Tube(Handle_t e) : Solid(e) {}
      /// Constructor to be used when creating a new tube object
      Tube(const Document& doc, const std::string& name);
      /// Constructor to be used when creating a new tube object with attribute initialization
      Tube(const Document& doc, const std::string& name, double rmin, double rmax, double z, double deltaPhi=2*M_PI);
      /// Constructor to be used when creating a new tube object with attribute initialization
#if 0
      template<class RMIN, class RMAX, class Z, class DELTAPHI>
      Tube(const Document& doc, const std::string& name, const RMIN& rmin, const RMAX& rmax, const Z& z, const DELTAPHI& deltaPhi)  
      : Solid(doc,"tube",name)
      {
        setDimensions(_toDouble(rmin),_toDouble(rmax),_toDouble(z),_toDouble(deltaPhi));
      }
#endif
      /// Set the box dimensions
      Tube& setDimensions(double rmin, double rmax, double z, double deltaPhi=2*M_PI);
    };

    struct Cone : public Solid  {
      /// Constructor to be used when reading the already parsed object
      Cone(Handle_t e) : Solid(e) {}
      /// Constructor to be used when creating a new object
      Cone(const Document& doc, const std::string& name);
      /// Constructor to be used when creating a new object with attribute initialization
      Cone(const Document& doc, const std::string& name,
           double z,
           double rmin1,
           double rmax1,
           double rmin2,
           double rmax2);
      template<typename Z, typename RMIN1, typename RMAX1, typename RMIN2, typename RMAX2>
      Cone(const Document& doc, const std::string& name, const Z& z, const RMIN1& rmin1, const RMAX1& rmax1, const RMIN2& rmin2, const RMAX2& rmax2)
      : Solid(doc,"Cone",name)
      {
        setDimensions(_toDouble(z),_toDouble(rmin1),_toDouble(rmax1),_toDouble(rmin2),_toDouble(rmax2));
      }
      /// Set the box dimensions
      Cone& setDimensions(double z,double rmin1,double rmax1,double rmin2,double rmax2);
    };

    struct Trap : public Solid   {
      /// Constructor to be used when reading the already parsed object
      Trap( Handle_t e) : Solid(e) {}
      /// Constructor to be used when creating a new object with attribute initialization
      Trap( const Document& doc, const std::string& name,
            double z,
            double theta,
            double phi,
            double y1,
            double x1,
            double x2,
            double alpha1,
            double y2,
            double x3,
            double x4,
            double alpha2);
      /// Set the trap dimensions
      Trap& setDimensions(double z,double theta,double phi,
                          double y1,double x1,double x2,double alpha1,
                          double y2,double x3,double x4,double alpha2);
    };

    struct Trapezoid : public Solid {
      /// Constructor to be used when reading the already parsed object
      Trapezoid(Handle_t e) : Solid(e) {}
      /// Constructor to be used when creating a new object
      Trapezoid(const Document& doc, const std::string& name);
      /// Constructor to be used when creating a new object with attribute initialization
      Trapezoid(const Document& doc, const std::string& name, double x1, double x2, double y1, double y2, double z);
      /// Set the Trapezoid dimensions
      Trapezoid& setDimensions(double x1, double x2, double y1, double y2, double z);
      Trapezoid& setX1(double value);
      Trapezoid& setX2(double value);
      Trapezoid& setY1(double value);
      Trapezoid& setY2(double value);
      Trapezoid& setZ(double value);
      double x1() const;
      double x2() const;
      double y1() const;
      double y2() const;
      double z() const;
    };

    struct PolyhedraRegular : public Solid {
      /// Constructor to be used when reading the already parsed object
      PolyhedraRegular(Handle_t e) : Solid(e) {}
      /// Constructor to be used when creating a new object
      PolyhedraRegular(const Document& doc, const std::string& name, int nsides, double rmin, double rmax, double zlen);
    };

    struct BooleanSolid : public Solid  {
      /// Constructor to be used when reading the already parsed object
      BooleanSolid(Handle_t e) : Solid(e) {}
      /// Constructor to be used when creating a new object
      BooleanSolid(const Document& doc, const std::string& type, const std::string& name);
      //BooleanSolid& setSolids(const Solid& s1, const Solid& s2);
      //BooleanSolid& setPosition(const Position& pos);
      //BooleanSolid& setRotation(const Rotation& rot);
    };

    struct SubtractionSolid : public BooleanSolid  {
      /// Constructor to be used when reading the already parsed object
      SubtractionSolid(Handle_t e) : BooleanSolid(e) {}
      /// Constructor to be used when creating a new object
      SubtractionSolid(const Document& doc, const std::string& name, const std::string& expr);
      /// Constructor to be used when creating a new object
      SubtractionSolid(const Document& doc, const std::string& name, const Solid& shape1, const Solid& shape2);
      /// Constructor to be used when creating a new object
      SubtractionSolid(const Document& doc, const std::string& name, const Solid& shape1, const Solid& shape2, const Position& pos, const Rotation& rot);
    };

  }       /* End namespace Geometry           */
}         /* End namespace DetDesc            */
#endif    /* DETDESC_GEOMETRY_SOLIDS_H        */
