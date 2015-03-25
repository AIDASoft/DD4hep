// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#define _USE_MATH_DEFINES
#include "DD4hep/LCDD.h"
#include "DD4hep/MatrixHelpers.h"
#include "DD4hep/DD4hepUnits.h"
// C/C++ include files
#include <stdexcept>

// ROOT includes
#include "TClass.h"
#include "TGeoShape.h"
#include "TGeoHype.h"
#include "TGeoPcon.h"
#include "TGeoPgon.h"
#include "TGeoTube.h"
#include "TGeoEltu.h"
#include "TGeoTrd1.h"
#include "TGeoTrd2.h"
#include "TGeoArb8.h"
#include "TGeoCone.h"
#include "TGeoParaboloid.h"
#include "TGeoSphere.h"
#include "TGeoTorus.h"
#include "TGeoMatrix.h"
#include "TGeoHalfSpace.h"
#include "TGeoBoolNode.h"
#include "TGeoCompositeShape.h"

using namespace std;
using namespace DD4hep::Geometry;

/// Pretty print of solid attributes
std::string DD4hep::Geometry::toStringSolid(const TGeoShape* shape, int precision)   {
  stringstream log;

  if ( !shape )  {
    return "[Invalid shape]";
  }

  TClass* cl = shape->IsA();
  int prec = log.precision();
  log.setf(ios::fixed,ios::floatfield);
  log << setprecision(precision);
  log << cl->GetName();
  if ( cl == TGeoBBox::Class() )   {
    TGeoBBox* s = (TGeoBBox*) shape;
    log << " x:" << s->GetDX()
        << " y:" << s->GetDY()
        << " z:" << s->GetDZ();
  }
  else if (cl == TGeoHalfSpace::Class()) {
    TGeoHalfSpace* s = (TGeoHalfSpace*)(const_cast<TGeoShape*>(shape));
    log << " Point:  (" << s->GetPoint()[0] << ", " << s->GetPoint()[1] << ", " << s->GetPoint()[2] << ") " 
	<< " Normal: (" << s->GetNorm()[0]  << ", " << s->GetNorm()[1]  << ", " << s->GetNorm()[2]  << ") ";
  }
  else if (cl == TGeoTube::Class()) {
    const TGeoTube* s = (const TGeoTube*) shape;
    log << " rmin:" << s->GetRmin() << " rmax:" << s->GetRmax() << " dz:" << s->GetDz();
  }
  else if (cl == TGeoTubeSeg::Class()) {
    const TGeoTubeSeg* s = (const TGeoTubeSeg*) shape;
    log << " rmin:" << s->GetRmin() << " rmax:" << s->GetRmax() << " dz:" << s->GetDz()
        << " Phi1:" << s->GetPhi1() << " Phi2:" << s->GetPhi2();
  }
  else if (cl == TGeoTrd1::Class()) {
    const TGeoTrd1* s = (const TGeoTrd1*) shape;
    log << " x1:" << s->GetDx1() << " x2:" << s->GetDx2() << " y:" << s->GetDy() << " z:" << s->GetDz();
  }
  else if (cl == TGeoTrd2::Class()) {
    const TGeoTrd2* s = (const TGeoTrd2*) shape;
    log << " x1:" << s->GetDx1() << " x2:" << s->GetDx2()
        << " y1:" << s->GetDy1() << " y2:" << s->GetDy2() << " z:" << s->GetDz();
  }
  else if (cl == TGeoHype::Class()) {
    const TGeoHype* s = (const TGeoHype*) shape;
    log << " rmin:" << s->GetRmin() << " rmax:"  << s->GetRmax() << " dz:" << s->GetDz()
        << " StIn:" << s->GetStIn() << " StOut:" << s->GetStOut();
  }
  else if (cl == TGeoPgon::Class()) {
    const TGeoPgon* s = (const TGeoPgon*) shape;
    log << " Phi1:"   << s->GetPhi1()   << " dPhi:" << s->GetDphi()
        << " NEdges:" << s->GetNedges() << " Nz:" << s->GetNz();
    for(int i=0, n=s->GetNz(); i<n; ++i)  {
      log << " i=" << i << " z:" << s->GetZ(i)
          << " r:[" << s->GetRmin(i) << "," << s->GetRmax(i) << "]";
    }
  }
  else if (cl == TGeoPcon::Class()) {
    const TGeoPcon* s = (const TGeoPcon*) shape;
    log << " Phi1:" << s->GetPhi1() << " dPhi:" << s->GetDphi() << " Nz:" << s->GetNz();
    for(int i=0, n=s->GetNz(); i<n; ++i)  {
      log << " i=" << i << " z:" << s->GetZ(i)
          << " r:[" << s->GetRmin(i) << "," << s->GetRmax(i) << "]";
    }
  }
  else if (cl == TGeoConeSeg::Class()) {
    const TGeoConeSeg* s = (const TGeoConeSeg*) shape;
    log << " rmin1:" << s->GetRmin1() << " rmax1:" << s->GetRmax1()
        << " rmin2:" << s->GetRmin2() << " rmax2:" << s->GetRmax2()
        << " dz:"    << s->GetDz()
        << " Phi1:"  << s->GetPhi1() << " Phi2:" << s->GetPhi2();
  }
  else if (cl == TGeoParaboloid::Class()) {
    const TGeoParaboloid* s = (const TGeoParaboloid*) shape;
    log << " dz:" << s->GetDz() << " RLo:" << s->GetRlo() << " Rhi:" << s->GetRhi();
  }
  else if (cl == TGeoSphere::Class()) {
    const TGeoSphere* s = (const TGeoSphere*) shape;
    log << " rmin:" << s->GetRmin() << " rmax:" << s->GetRmax()
        << " Phi1:" << s->GetPhi1() << " Phi2:" << s->GetPhi2()
        << " Theta1:" << s->GetTheta1() << " Theta2:" << s->GetTheta2();
  }
  else if (cl == TGeoTorus::Class()) {
    const TGeoTorus* s = (const TGeoTorus*) shape;
    log << " rmin:" << s->GetRmin() << " rmax:" << s->GetRmax() << " r:" << s->GetR()
        << " Phi1:" << s->GetPhi1() << " dPhi:" << s->GetDphi();
  }
  else if (cl == TGeoTrap::Class()) {
    const TGeoTrap* s = (const TGeoTrap*) shape;
    log << " dz:" << s->GetDz() << " Theta:" << s->GetTheta() << " Phi:" << s->GetPhi()
        << " H1:" << s->GetH1() << " Bl1:"   << s->GetBl1()   << " Tl1:" << s->GetTl1() << " Alpha1:" << s->GetAlpha1()
        << " H2:" << s->GetH2() << " Bl2:"   << s->GetBl2()   << " Tl2:" << s->GetTl2() << " Alpha2:" << s->GetAlpha2();
  }
  else if (shape->IsA() == TGeoCompositeShape::Class()) {
    const TGeoCompositeShape* s = (const TGeoCompositeShape*) shape;
    const TGeoBoolNode* boolean = s->GetBoolNode();
    const TGeoShape* left = boolean->GetLeftShape();
    const TGeoShape* right = boolean->GetRightShape();
    TGeoBoolNode::EGeoBoolType oper = boolean->GetBooleanOperator();
    if (oper == TGeoBoolNode::kGeoSubtraction)
      log << "Subtraction: ";
    else if (oper == TGeoBoolNode::kGeoUnion)
      log << "Union: ";
    else if (oper == TGeoBoolNode::kGeoIntersection)
      log << "Intersection: ";
    log << " Left:" << toStringSolid(left) << " Right:" << toStringSolid(right);
  }
  log << setprecision(prec);
  return log.str();
}

template <typename T> void Solid_type<T>::_setDimensions(double* param) {
  this->ptr()->SetDimensions(param);
  this->ptr()->ComputeBBox();
}

/// Assign pointrs and register solid to geometry
template <typename T>
void Solid_type<T>::_assign(T* n, const string& nam, const string& tit, bool cbbox) {
  this->assign(n, nam, tit);
  if (cbbox)
    n->ComputeBBox();
}

/// Access to shape name
template <typename T> const char* Solid_type<T>::name() const {
  return this->ptr()->GetName();
}

void Box::make(double x_val, double y_val, double z_val) {
  _assign(new TGeoBBox(x_val, y_val, z_val), "", "box", true);
}

/// Set the box dimensionsy
Box& Box::setDimensions(double x_val, double y_val, double z_val) {
  double params[] = { x_val, y_val, z_val};
  _setDimensions(params);
  return *this;
}

/// Access half "length" of the box
double Box::x() const {
  return this->ptr()->GetDX();
}

/// Access half "width" of the box
double Box::y() const {
  return this->ptr()->GetDY();
}

/// Access half "depth" of the box
double Box::z() const {
  return this->ptr()->GetDZ();
}

/// Internal helper method to support object construction
void HalfSpace::make(const double* const point, const double* const normal)   {
  _assign(new TGeoHalfSpace("",(Double_t*)point, (Double_t*)normal), "", "halfspace",true);
}

/// Constructor to be used when creating a new object
Polycone::Polycone(double start, double delta) {
  _assign(new TGeoPcon(start/dd4hep::deg, delta/dd4hep::deg, 0), "", "polycone", false);
}

/// Constructor to be used when creating a new polycone object. Add at the same time all Z planes
Polycone::Polycone(double start, double delta, const vector<double>& rmin, const vector<double>& rmax,
                   const vector<double>& z) {
  vector<double> params;
  if (rmin.size() < 2) {
    throw runtime_error("DD4hep: PolyCone::addZPlanes> Not enough Z planes. minimum is 2!");
  }
  params.push_back(start/dd4hep::deg);
  params.push_back(delta/dd4hep::deg);
  params.push_back(rmin.size());
  for (size_t i = 0; i < rmin.size(); ++i) {
    params.push_back(z[i] );
    params.push_back(rmin[i] );
    params.push_back(rmax[i] );
  }
  _assign(new TGeoPcon(&params[0]), "", "polycone", true);
}

/// Add Z-planes to the Polycone
void Polycone::addZPlanes(const vector<double>& rmin, const vector<double>& rmax, const vector<double>& z) {
  TGeoPcon* s = *this;
  vector<double> params;
  size_t num = s->GetNz();
  if (rmin.size() < 2) {
    throw runtime_error("DD4hep: PolyCone::addZPlanes> Not enough Z planes. minimum is 2!");
  }
  params.push_back(s->GetPhi1());
  params.push_back(s->GetDphi());
  params.push_back(num + rmin.size());
  for (size_t i = 0; i < num; ++i) {
    params.push_back(s->GetZ(i));
    params.push_back(s->GetRmin(i));
    params.push_back(s->GetRmax(i));
  }
  for (size_t i = 0; i < rmin.size(); ++i) {
    params.push_back(z[i] );
    params.push_back(rmin[i] );
    params.push_back(rmax[i] );
  }
  _setDimensions(&params[0]);
}

/// Constructor to be used when creating a new cone segment object
ConeSegment::ConeSegment(double dz, double rmin1, double rmax1, double rmin2, double rmax2, double phi1, double phi2) {
  _assign(
          new TGeoConeSeg(dz, rmin1, rmax1, rmin2, rmax2, phi1/dd4hep::deg, phi2/dd4hep::deg), "", "cone_segment", true);
}

/// Set the cone segment dimensions
ConeSegment& ConeSegment::setDimensions(double dz, double rmin1, double rmax1, double rmin2, double rmax2, double phi1,
                                        double phi2) {
  double params[] = { dz, rmin1, rmax1, rmin2, rmax2, phi1/dd4hep::deg, phi2/dd4hep::deg };
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new object with attribute initialization
void Tube::make(const string& nam, double rmin, double rmax, double z, double startPhi, double deltaPhi) {
  //_assign(new TGeoTubeSeg(rmin,rmax,z,startPhi/dd4hep::deg,deltaPhi/dd4hep::deg),name,"tube",true);
  _assign(new MyConeSeg(), nam, "tube", true);
  setDimensions(rmin, rmax, z, startPhi, deltaPhi);
}

/// Set the tube dimensions
Tube& Tube::setDimensions(double rmin, double rmax, double z, double startPhi, double deltaPhi) {
  //double params[] = {rmin,rmax,z,startPhi,deltaPhi};
  double params[] = { z, rmin, rmax, rmin, rmax, startPhi/dd4hep::deg,deltaPhi/dd4hep::deg };
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new object with attribute initialization
void EllipticalTube::make(double a, double b, double dz) {
  _assign(new TGeoEltu(), "", "elliptic_tube", true);
  setDimensions(a, b, dz);
}

/// Set the tube dimensions
EllipticalTube& EllipticalTube::setDimensions(double a, double b, double dz) {
  double params[] = { a, b, dz };
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new object with attribute initialization
void Cone::make(double z, double rmin1, double rmax1, double rmin2, double rmax2) {
  _assign(new TGeoCone(z, rmin1, rmax1, rmin2, rmax2 ), "", "cone", true);
}

Cone& Cone::setDimensions(double z, double rmin1, double rmax1, double rmin2, double rmax2) {
  double params[] = { z, rmin1, rmax1, rmin2, rmax2  };
  _setDimensions(params);
  return *this;
}

/// Constructor to create a new anonymous object with attribute initialization
Trapezoid::Trapezoid(double x1, double x2, double y1, double y2, double z)   {
  make(x1,x2,y1,y2,z);
}

/// Constructor to be used when creating a new object with attribute initialization
void Trapezoid::make(double x1, double x2, double y1, double y2, double z) {
  _assign(new TGeoTrd2(x1, x2, y1, y2, z ), "", "trd2", true);
}

/// Set the Trapezoid dimensions
Trapezoid& Trapezoid::setDimensions(double x1, double x2, double y1, double y2, double z) {
  double params[] = { x1, x2, y1, y2, z  };
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new object with attribute initialization
Paraboloid::Paraboloid(double r_low, double r_high, double delta_z) {
  _assign(new TGeoParaboloid(r_low, r_high, delta_z ), "", "paraboloid", true);
}

/// Set the Paraboloid dimensions
Paraboloid& Paraboloid::setDimensions(double r_low, double r_high, double delta_z) {
  double params[] = { r_low, r_high, delta_z  };
  _setDimensions(params);
  return *this;
}

/// Constructor to create a new anonymous object with attribute initialization
Hyperboloid::Hyperboloid(double rin, double stin, double rout, double stout, double dz) {
  _assign(new TGeoHype(rin, stin/dd4hep::deg, rout, stout/dd4hep::deg, dz), "", "hyperboloid", true);
}

/// Set the Hyperboloid dimensions
Hyperboloid& Hyperboloid::setDimensions(double rin, double stin, double rout, double stout, double dz)  {
  double params[] = { rin, stin/dd4hep::deg, rout, stout/dd4hep::deg, dz};
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new object with attribute initialization
Sphere::Sphere(double rmin, double rmax, double theta, double delta_theta, double phi, double delta_phi) {
  _assign(new TGeoSphere(rmin, rmax, theta/dd4hep::deg, delta_theta/dd4hep::deg, phi/dd4hep::deg, delta_phi/dd4hep::deg), "", "sphere", true);
}

/// Set the Sphere dimensions
Sphere& Sphere::setDimensions(double rmin, double rmax, double theta, double delta_theta, double phi, double delta_phi) {
  double params[] = { rmin, rmax, theta, delta_theta/dd4hep::deg, phi/dd4hep::deg, delta_phi/dd4hep::deg };
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new object with attribute initialization
void Torus::make(double r, double rmin, double rmax, double phi, double delta_phi) {
  _assign(new TGeoTorus(r, rmin, rmax, phi/dd4hep::deg, delta_phi/dd4hep::deg), "", "torus", true);
}

/// Set the Torus dimensions
Torus& Torus::setDimensions(double r, double rmin, double rmax, double phi, double delta_phi) {
  double params[] = { r, rmin, rmax, phi/dd4hep::deg, delta_phi/dd4hep::deg };
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new anonymous object with attribute initialization
Trap::Trap(double z, double theta, double phi, double y1, double x1, double x2, double alpha1, double y2, double x3, double x4,
           double alpha2) {
  _assign(new TGeoTrap(z, theta, phi, y1, x1, x2, alpha1/dd4hep::deg, y2, x3, x4, alpha2/dd4hep::deg), "", "trap", true);
}

/// Constructor to be used when creating a new anonymous object with attribute initialization
void Trap::make(double pz, double py, double px, double pLTX) {
  double z = pz / 2e0;
  double theta = 0e0;
  double phi = 0e0;
  double y1 = py / 2e0;
  double x1 = px / 2e0;
  double x2 = pLTX / 2e0;
  double alpha1 = (pLTX - px) / py;
  _assign(new TGeoTrap(z, theta, phi, y1, x1, x2, alpha1/dd4hep::deg, y1, x1, x2, alpha1/dd4hep::deg), "", "trap", true);
}

/// Set the trap dimensions
Trap& Trap::setDimensions(double z, double theta, double phi, double y1, double x1, double x2, double alpha1, double y2,
                          double x3, double x4, double alpha2) {
  double params[] = { z, theta, phi, y1, x1, x2, alpha1/dd4hep::deg, y2, x3, x4, alpha2/dd4hep::deg };
  _setDimensions(params);
  return *this;
}

/// Helper function to create holy hedron
void PolyhedraRegular::_create(int nsides, double rmin, double rmax, double zpos, double zneg, double start, double delta) {
  if (rmin < 0e0 || rmin > rmax)
    throw runtime_error("DD4hep: PolyhedraRegular: Illegal argument rmin:<" + _toString(rmin) + "> is invalid!");
  else if (rmax < 0e0)
    throw runtime_error("DD4hep: PolyhedraRegular: Illegal argument rmax:<" + _toString(rmax) + "> is invalid!");
  _assign(new TGeoPgon(), "", "polyhedra", false);
  double params[] = { start, delta, double(nsides), 2e0, zpos, rmin, rmax, zneg, rmin, rmax };
  _setDimensions(&params[0]);
}

/// Constructor to be used when creating a new object
PolyhedraRegular::PolyhedraRegular(int nsides, double rmin, double rmax, double zlen) {
  _create(nsides, rmin, rmax, zlen / 2, -zlen / 2, 0, 360.);
}

/// Constructor to be used when creating a new object
PolyhedraRegular::PolyhedraRegular(int nsides, double phistart, double rmin, double rmax, double zlen) {
  _create(nsides, rmin, rmax, zlen / 2, -zlen / 2, phistart/dd4hep::deg, 360.);
}

/// Constructor to be used when creating a new object
PolyhedraRegular::PolyhedraRegular(int nsides, double rmin, double rmax, double zplanes[2]) {
  _create(nsides, rmin, rmax, zplanes[0], zplanes[1], 0, 360.);
}

/// Creator method
void EightPointSolid::make(double dz, const double* vtx)   {
  _assign(new TGeoArb8(dz, (double*)vtx), "", "Arb8", true);
}

/// Constructor to be used when creating a new object. Position is identity, Rotation is the identity rotation
SubtractionSolid::SubtractionSolid(const Solid& shape1, const Solid& shape2) {
  TGeoSubtraction* sub = new TGeoSubtraction(shape1, shape2, identityTransform(), identityTransform());
  _assign(new TGeoCompositeShape("", sub), "", "subtraction", true);
}

/// Constructor to be used when creating a new object. Placement by a generic transformation within the mother
SubtractionSolid::SubtractionSolid(const Solid& shape1, const Solid& shape2, const Transform3D& trans) {
  TGeoSubtraction* sub = new TGeoSubtraction(shape1, shape2, identityTransform(), _transform(trans));
  _assign(new TGeoCompositeShape("", sub), "", "subtraction", true);
}

/// Constructor to be used when creating a new object. Rotation is the identity rotation
SubtractionSolid::SubtractionSolid(const Solid& shape1, const Solid& shape2, const Position& pos) {
  TGeoSubtraction* sub = new TGeoSubtraction(shape1, shape2, identityTransform(), _translation(pos));
  _assign(new TGeoCompositeShape("", sub), "", "subtraction", true);
}

/// Constructor to be used when creating a new object
SubtractionSolid::SubtractionSolid(const Solid& shape1, const Solid& shape2, const RotationZYX& rot) {
  TGeoSubtraction* sub = new TGeoSubtraction(shape1, shape2, identityTransform(), _rotationZYX(rot));
  _assign(new TGeoCompositeShape("", sub), "", "subtraction", true);
}

/// Constructor to be used when creating a new object
SubtractionSolid::SubtractionSolid(const Solid& shape1, const Solid& shape2, const Rotation3D& rot) {
  TGeoSubtraction* sub = new TGeoSubtraction(shape1, shape2, identityTransform(), _rotation3D(rot));
  _assign(new TGeoCompositeShape("", sub), "", "subtraction", true);
}

/// Constructor to be used when creating a new object. Position is identity, Rotation is identity rotation
UnionSolid::UnionSolid(const Solid& shape1, const Solid& shape2) {
  TGeoUnion* uni = new TGeoUnion(shape1, shape2, identityTransform(), identityTransform());
  _assign(new TGeoCompositeShape("", uni), "", "union", true);
}

/// Constructor to be used when creating a new object. Placement by a generic transformation within the mother
UnionSolid::UnionSolid(const Solid& shape1, const Solid& shape2, const Transform3D& trans) {
  TGeoUnion* uni = new TGeoUnion(shape1, shape2, identityTransform(), _transform(trans));
  _assign(new TGeoCompositeShape("", uni), "", "union", true);
}

/// Constructor to be used when creating a new object. Rotation is identity rotation
UnionSolid::UnionSolid(const Solid& shape1, const Solid& shape2, const Position& pos) {
  TGeoUnion* uni = new TGeoUnion(shape1, shape2, identityTransform(), _translation(pos));
  _assign(new TGeoCompositeShape("", uni), "", "union", true);
}

/// Constructor to be used when creating a new object
UnionSolid::UnionSolid(const Solid& shape1, const Solid& shape2, const RotationZYX& rot) {
  TGeoUnion *uni = new TGeoUnion(shape1, shape2, identityTransform(), _rotationZYX(rot));
  _assign(new TGeoCompositeShape("", uni), "", "union", true);
}

/// Constructor to be used when creating a new object
UnionSolid::UnionSolid(const Solid& shape1, const Solid& shape2, const Rotation3D& rot) {
  TGeoUnion *uni = new TGeoUnion(shape1, shape2, identityTransform(), _rotation3D(rot));
  _assign(new TGeoCompositeShape("", uni), "", "union", true);
}

/// Constructor to be used when creating a new object. Position is identity, Rotation is identity rotation
IntersectionSolid::IntersectionSolid(const Solid& shape1, const Solid& shape2) {
  TGeoIntersection* inter = new TGeoIntersection(shape1, shape2, identityTransform(), identityTransform());
  _assign(new TGeoCompositeShape("", inter), "", "intersection", true);
}

/// Constructor to be used when creating a new object. Placement by a generic transformation within the mother
IntersectionSolid::IntersectionSolid(const Solid& shape1, const Solid& shape2, const Transform3D& trans) {
  TGeoIntersection* inter = new TGeoIntersection(shape1, shape2, identityTransform(), _transform(trans));
  _assign(new TGeoCompositeShape("", inter), "", "intersection", true);
}

/// Constructor to be used when creating a new object. Position is identity.
IntersectionSolid::IntersectionSolid(const Solid& shape1, const Solid& shape2, const Position& pos) {
  TGeoIntersection* inter = new TGeoIntersection(shape1, shape2, identityTransform(), _translation(pos));
  _assign(new TGeoCompositeShape("", inter), "", "intersection", true);
}

/// Constructor to be used when creating a new object
IntersectionSolid::IntersectionSolid(const Solid& shape1, const Solid& shape2, const RotationZYX& rot) {
  TGeoIntersection* inter = new TGeoIntersection(shape1, shape2, identityTransform(), _rotationZYX(rot));
  _assign(new TGeoCompositeShape("", inter), "", "intersection", true);
}

/// Constructor to be used when creating a new object
IntersectionSolid::IntersectionSolid(const Solid& shape1, const Solid& shape2, const Rotation3D& rot) {
  TGeoIntersection* inter = new TGeoIntersection(shape1, shape2, identityTransform(), _rotation3D(rot));
  _assign(new TGeoCompositeShape("", inter), "", "intersection", true);
}

#define INSTANTIATE(X) template class DD4hep::Geometry::Solid_type<X>

INSTANTIATE(TGeoShape);
INSTANTIATE(TGeoBBox);
INSTANTIATE(TGeoHalfSpace);
INSTANTIATE(TGeoCone);
INSTANTIATE(TGeoConeSeg);
INSTANTIATE(TGeoParaboloid);
INSTANTIATE(TGeoPcon);
INSTANTIATE(TGeoPgon);
INSTANTIATE(TGeoSphere);
INSTANTIATE(TGeoTorus);
INSTANTIATE(TGeoTube);
INSTANTIATE(TGeoTubeSeg);
INSTANTIATE(TGeoEltu);
INSTANTIATE(TGeoHype);
INSTANTIATE(TGeoTrap);
INSTANTIATE(TGeoTrd1);
INSTANTIATE(TGeoTrd2);
INSTANTIATE(TGeoCompositeShape);
