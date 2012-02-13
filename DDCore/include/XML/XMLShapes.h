#ifndef DD4hep_XMLSHAPES_H
#define DD4hep_XMLSHAPES_H
#include "XML/XMLTags.h"

// C/C++ include files
#define _USE_MATH_DEFINES
#include <cmath>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
  *   XML namespace declaration
  */
  namespace XML  {

    struct Position;
    struct Rotation;

    struct ZPlane : public Element  {
      /// Constructor to be used when reading the already parsed DOM tree
      ZPlane(Handle_t e) : Element(e) {}
      /// Constructor to be used when creating a new DOM tree
      ZPlane(const Document& doc, double rmin, double rmax, double z);
      /// Constructor to be used when creating a new DOM tree
      ZPlane(const Document& doc, const XMLCh* rmin, const XMLCh* rmax, const XMLCh* z);
      /// Set values
      ZPlane& setRmin(double value);
      ZPlane& setRmax(double value);
      ZPlane& setZ(double value);
    };

    struct Solid : public RefElement  {
      /// Constructor to be used when reading the already parsed DOM tree
      Solid(const Element& e) : RefElement(e) {}
      /// Constructor to be used when creating a new DOM tree
      Solid(const Document& doc, const XMLCh* type, const XMLCh* name) : RefElement(doc,type,name) {}
    };

    struct Box : public Solid  {
      /// Constructor to be used when reading the already parsed DOM tree
      Box(const Element& e) : Solid(e) {}
      /// Constructor to be used when creating a new DOM tree
      Box(const Document& doc, const XMLCh* name);
      /// Constructor to be used when creating a new DOM tree
      Box(const Document& doc, const XMLCh* name, double x, double y, double z);
      /// Set the box dimensions
      Box& setDimensions(double x, double y, double z);
    };

    struct Polycone : public Solid {
      /// Constructor to be used when reading the already parsed DOM tree
      Polycone(const Element& e) : Solid(e) {}
      /// Constructor to be used when creating a new DOM tree
      Polycone(const Document& doc, const XMLCh* name);
      /// Constructor to be used when creating a new DOM tree
      Polycone(const Document& doc, const XMLCh* name, double start, double delta);
      /// Add all Z-planes to the Polycone. Take the information from the given element
      void addZPlanes(Handle_t e);
    };

    struct Tube : public Solid  {
      /// Constructor to be used when reading the already parsed DOM tree
      Tube(const Element& e) : Solid(e) {}
      /// Constructor to be used when creating a new DOM tree
      Tube(const Document& doc, const XMLCh* name);
      /// Constructor to be used when creating a new DOM tree with attribute initialization
      Tube(const Document& doc, const XMLCh* name, double rmin, double rmax, double z, double deltaPhi=2*M_PI);
      /// Set the box dimensions
      Tube& setDimensions(double rmin, double rmax, double z, double deltaPhi=2*M_PI);
      void fromCompact(Handle_t h);
    };

    struct Cone : public Solid  {
      /// Constructor to be used when reading the already parsed DOM tree
      Cone(const Element& e) : Solid(e) {}
      /// Constructor to be used when creating a new DOM tree
      Cone(const Document& doc, const XMLCh* name);
      /// Constructor to be used when creating a new DOM tree with attribute initialization
      Cone(const Document& doc, const XMLCh* name,
           double rmin1,
           double rmin2,
           double rmax1,
           double rmax2,
           double z,
           double startphi,
           double deltaphi);
      Cone& setZ(double z);
      Cone& setRmin1(double r);
      Cone& setRmax1(double r);
      Cone& setRmin2(double r);
      Cone& setRmax2(double r);
      Cone& setStartPhi(double phi);
      Cone& setDeltaPhi(double phi);
      void fromCompact(Handle_t h);
    };

    struct Trap : public Solid   {
      /// Constructor to be used when reading the already parsed DOM tree
      Trap(const Element& e) : Solid(e) {}
      /// Constructor to be used when creating a new DOM tree with attribute initialization
      Trap( const Document& doc, const XMLCh* name,
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
      double zHalfLength()  const;
      double theta()  const;
      double phi()  const;
      double yHalfLength1()  const;
      double xHalfLength1()  const;
      double xHalfLength2()  const;
      double alpha1()  const;
      double yHalfLength2()  const;
      double xHalfLength3()  const;
      double xHalfLength4()  const;
      double alpha2()  const;
    };

    struct Trapezoid : public Solid {
      /// Constructor to be used when reading the already parsed DOM tree
      Trapezoid(const Element& e) : Solid(e) {}
      /// Constructor to be used when creating a new DOM tree
      Trapezoid(const Document& doc, const XMLCh* name);
      /// Constructor to be used when creating a new DOM tree with attribute initialization
      Trapezoid(const Document& doc, const XMLCh* name, double x1, double x2, double y1, double y2, double z);
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
      /// Constructor to be used when reading the already parsed DOM tree
      PolyhedraRegular(const Element& e) : Solid(e) {}
      /// Constructor to be used when creating a new DOM tree
      PolyhedraRegular(const Document& doc, const XMLCh* name, int nsides, double rmin, double rmax, double zlen);
    };

    struct BooleanSolid : public Solid  {
      /// Constructor to be used when reading the already parsed DOM tree
      BooleanSolid(Handle_t e) : Solid(e) {}
      /// Constructor to be used when creating a new DOM tree
      BooleanSolid(const Document& doc, const XMLCh* type, const XMLCh* name);
      BooleanSolid& setFirstSolid(const Solid& s);
      BooleanSolid& setSecondSolid(const Solid& s);
      BooleanSolid& setPosition(const Position& pos);
      BooleanSolid& setRotation(const Rotation& rot);
    };

    struct SubtractionSolid : public BooleanSolid  {
      /// Constructor to be used when reading the already parsed DOM tree
      SubtractionSolid(Handle_t e) : BooleanSolid(e) {}
      /// Constructor to be used when creating a new DOM tree
      SubtractionSolid(const Document& doc, const XMLCh* name);
    };
  }
}         /* End namespace DD4hep   */
#endif    /* DD4hep_XMLSHAPES_H     */
