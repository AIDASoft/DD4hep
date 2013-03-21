// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#define _USE_MATH_DEFINES
#include "DD4hep/LCDD.h"

// C/C++ include files
#include <stdexcept>

// ROOT includes
#include "TGeoShape.h"
#include "TGeoPcon.h"
#include "TGeoPgon.h"
#include "TGeoTube.h"
#include "TGeoTrd2.h"
#include "TGeoArb8.h"
#include "TGeoCone.h"
#include "TGeoParaboloid.h"
#include "TGeoSphere.h"
#include "TGeoTorus.h"
#include "TGeoMatrix.h"
#include "TGeoBoolNode.h"
#include "TGeoCompositeShape.h"

using namespace std;
using namespace DD4hep::Geometry;

TGeoIdentity* DD4hep::Geometry::identityTransform() {
  return gGeoIdentity;
}

template<typename T> void Solid_type<T>::_setDimensions(double* param) {
  this->ptr()->SetDimensions(param);
  this->ptr()->ComputeBBox();
}

/// Assign pointrs and register solid to geometry
template<typename T> 
void Solid_type<T>::_assign(T* n, const string& nam, const string& tit, bool cbbox) {
  this->assign(n,nam,tit);
  if ( cbbox ) n->ComputeBBox();
}

/// Assign pointrs and register solid to geometry
template<typename T> 
void Solid_type<T>::_assign(T* n, const string& tit, bool cbbox) {
  this->assign(n,"",tit);
  if ( cbbox ) n->ComputeBBox();
}

/// Access to shape name
template<typename T> const char* Solid_type<T>::name() const {
  return this->ptr()->GetName();
}

void Box::make(const string& name, double x, double y, double z)  {
  _assign(new TGeoBBox(x,y,z),name,"box");
}

/// Set the box dimensions
Box& Box::setDimensions(double x, double y, double z)   {
  double params[] = {x,y,z};
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

/// Constructor to be used when creating a new object
Polycone::Polycone(const string& name)   {
  _assign(new TGeoPcon(0,RAD_2_DEGREE*(2.*M_PI),0),name,"polycone",false);
}

/// Constructor to be used when creating a new object
Polycone::Polycone(double start, double delta)   {
  _assign(new TGeoPcon(RAD_2_DEGREE*start,RAD_2_DEGREE*delta,0),"polycone",false);
}

/// Constructor to be used when creating a new object
Polycone::Polycone(const string& name, double start, double delta)   {
  _assign(new TGeoPcon(RAD_2_DEGREE*start,RAD_2_DEGREE*delta,0),name,"polycone",false);
}

/// Constructor to be used when creating a new polycone object. Add at the same time all Z planes
Polycone::Polycone(double start, double delta, const vector<double>& rmin, const vector<double>& rmax, const vector<double>& z)
{
  vector<double> params;
  if ( rmin.size() < 2 )  {
    throw runtime_error("PolyCone::addZPlanes> Not enough Z planes. minimum is 2!");
  }
  params.push_back(RAD_2_DEGREE * start);
  params.push_back(RAD_2_DEGREE * delta);
  params.push_back(rmin.size());
  for( size_t i=0; i<rmin.size(); ++i )  {
    params.push_back(z[i]);
    params.push_back(rmin[i]);
    params.push_back(rmax[i]);
  }
  _assign( new TGeoPcon(&params[0]),"polycone");
}

/// Constructor to be used when creating a new polycone object. Add at the same time all Z planes
Polycone::Polycone(const string& name, double start, double delta, const vector<double>& rmin, const vector<double>& rmax, const vector<double>& z)
{
  vector<double> params;
  if ( rmin.size() < 2 )  {
    throw runtime_error("PolyCone::addZPlanes> Not enough Z planes. minimum is 2!");
  }
  params.push_back(RAD_2_DEGREE * start);
  params.push_back(RAD_2_DEGREE * delta);
  params.push_back(rmin.size());
  for( size_t i=0; i<rmin.size(); ++i )  {
    params.push_back(z[i]);
    params.push_back(rmin[i]);
    params.push_back(rmax[i]);
  }
  _assign( new TGeoPcon(&params[0]),name,"polycone");
}

/// Add Z-planes to the Polycone
void Polycone::addZPlanes(const vector<double>& rmin, const vector<double>& rmax, const vector<double>& z)  {
  TGeoPcon* s = *this;
  vector<double> params;
  size_t num = s->GetNz();
  if ( rmin.size() < 2 )  {
    throw runtime_error("PolyCone::addZPlanes> Not enough Z planes. minimum is 2!");
  }
  params.push_back(s->GetPhi1());
  params.push_back(s->GetDphi());
  params.push_back(num+rmin.size());
  for( size_t i=0; i<num; ++i )  {
    params.push_back(s->GetZ(i));
    params.push_back(s->GetRmin(i));
    params.push_back(s->GetRmax(i));
  }
  for( size_t i=0; i<rmin.size(); ++i )  {
    params.push_back(z[i]);
    params.push_back(rmin[i]);
    params.push_back(rmax[i]);
  }
  _setDimensions(&params[0]);
}

/// Constructor to be used when creating a new object
ConeSegment::ConeSegment(const string& name)   {
  _assign(new TGeoConeSeg(),name,"cone_segment",false);
}

/// Constructor to be used when creating a new cone segment object
ConeSegment::ConeSegment(double dz, double rmin1, double rmax1, double rmin2, double rmax2, double phi1, double phi2)  {
  _assign(new TGeoConeSeg(dz,rmin1,rmax1,rmin2,rmax2,RAD_2_DEGREE*phi1,RAD_2_DEGREE*phi2),"cone_segment");
}

/// Constructor to be used when creating a new cone segment object
ConeSegment::ConeSegment(const string& name, double dz, double rmin1, double rmax1, double rmin2, double rmax2, double phi1, double phi2)  {
  _assign(new TGeoConeSeg(dz,rmin1,rmax1,rmin2,rmax2,RAD_2_DEGREE*phi1,RAD_2_DEGREE*phi2),name,"cone_segment");
}

/// Set the cone segment dimensions
ConeSegment& ConeSegment::setDimensions(double dz, double rmin1, double rmax1, double rmin2, double rmax2, double phi1, double phi2) {
  double params[] = {dz,rmin1,rmax1,rmin2,rmax2,RAD_2_DEGREE*phi1,RAD_2_DEGREE*phi2};
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new object with attribute initialization
void Tube::make(const string& name, double rmin, double rmax, double z, double startPhi, double deltaPhi)
{
  //_assign((TGeoTubeSeg*)new TGeoTube(rmin,rmax,z),name,"tube");
  _assign(new TGeoTubeSeg(rmin,rmax,z,RAD_2_DEGREE*startPhi,RAD_2_DEGREE*deltaPhi),name,"tube");
  //MyConeSeg* s = new MyConeSeg();
  //_assign(s,name,"tube");
  //setDimensions(rmin,rmax,z,startPhi,deltaPhi);
}

/// Set the tube dimensions
Tube& Tube::setDimensions(double rmin, double rmax, double z, double startPhi, double deltaPhi)
{
  double params[] = {rmin,rmax,z,RAD_2_DEGREE*startPhi,RAD_2_DEGREE*deltaPhi};
  //double params[] = {z,rmin,rmax,rmin,rmax,RAD_2_DEGREE*startPhi,RAD_2_DEGREE*deltaPhi};
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new object with attribute initialization
void Cone::make(const string& name,
		double z,
		double rmin1,
		double rmax1,
		double rmin2,
		double rmax2)
{
  _assign(new TGeoCone(z,rmin1,rmax1,rmin2,rmax2),name,"cone");
}

Cone& Cone::setDimensions(double z,double rmin1,double rmax1,double rmin2,double rmax2)  {
  double params[] = {z,rmin1,rmax1,rmin2,rmax2};
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new object
Trapezoid::Trapezoid()  {
  _assign(new TGeoTrd2(0,0,0,0,0),"trd2");
}

/// Constructor to be used when creating a new object
Trapezoid::Trapezoid(const string& name)  {
  _assign(new TGeoTrd2(0,0,0,0,0),name,"trd2");
}

/// Constructor to be used when creating a new object with attribute initialization
Trapezoid::Trapezoid(const string& name, double x1, double x2, double y1, double y2, double z)  {
  _assign(new TGeoTrd2(x1,x2,y1,y2,z),name,"trd2");
}

/// Constructor to be used when creating a new object with attribute initialization
Trapezoid::Trapezoid(double x1, double x2, double y1, double y2, double z)  {
  _assign(new TGeoTrd2(x1,x2,y1,y2,z),"trd2");
}

/// Set the Trapezoid dimensions
Trapezoid& Trapezoid::setDimensions(double x1, double x2, double y1, double y2, double z)  {
  double params[] = {x1,x2,y1,y2,z};
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new object
Paraboloid::Paraboloid(const string& name)  {
  _assign(new TGeoParaboloid(0,0,0),name,"paraboloid");
}

/// Constructor to be used when creating a new identified object with attribute initialization
Paraboloid::Paraboloid(const string& name, double r_low, double r_high, double delta_z)  {
  _assign(new TGeoParaboloid(r_low,r_high,delta_z),name,"paraboloid");
}

/// Constructor to be used when creating a new object with attribute initialization
Paraboloid::Paraboloid(double r_low, double r_high, double delta_z)  {
  _assign(new TGeoParaboloid(r_low,r_high,delta_z),"","paraboloid");
}

/// Set the Paraboloid dimensions
Paraboloid& Paraboloid::setDimensions(double r_low, double r_high, double delta_z)  {
  double params[] = {r_low,r_high,delta_z};
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new anonymous object
Sphere::Sphere()  {
  _assign(new TGeoSphere(0,0),"sphere");
}

/// Constructor to be used when creating a new identified object
Sphere::Sphere(const string& name)  {
  _assign(new TGeoSphere(0,0),name,"sphere");
}

/// Constructor to be used when creating a new identified object with attribute initialization
Sphere::Sphere(const string& name, double rmin, double rmax, double theta, double delta_theta, double phi, double delta_phi)  {
  _assign(new TGeoSphere(rmin,rmax,theta,delta_theta,phi,delta_phi),name,"sphere");
}

/// Constructor to be used when creating a new object with attribute initialization
Sphere::Sphere(double rmin, double rmax, double theta, double delta_theta, double phi, double delta_phi)  {
  _assign(new TGeoSphere(rmin,rmax,theta,delta_theta,phi,delta_phi),"sphere");
}

/// Set the Sphere dimensions
Sphere& Sphere::setDimensions(double rmin, double rmax, double theta, double delta_theta, double phi, double delta_phi)  {
  double params[] = {rmin,rmax,theta,delta_theta,phi,delta_phi};
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new object
Torus::Torus()  {
  _assign(new TGeoTorus(0,0,0),"torus");
}

/// Constructor to be used when creating a new object
Torus::Torus(const string& name)  {
  _assign(new TGeoTorus(0,0,0),name,"torus");
}

/// Constructor to be used when creating a new identified object with attribute initialization
Torus::Torus(const string& name, double r, double rmin, double rmax, double phi, double delta_phi)  {
  _assign(new TGeoTorus(r,rmin,rmax,phi,delta_phi),name,"torus");
}

/// Constructor to be used when creating a new object with attribute initialization
Torus::Torus(double r, double rmin, double rmax, double phi, double delta_phi)  {
  _assign(new TGeoTorus(r,rmin,rmax,phi,delta_phi),"torus");
}

/// Set the Torus dimensions
Torus& Torus::setDimensions(double r, double rmin, double rmax, double phi, double delta_phi)  {
  double params[] = {r,rmin,rmax,phi,delta_phi};
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new identified object with attribute initialization
Trap::Trap( const string& name,
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
            double alpha2)
{
  _assign(new TGeoTrap(z,RAD_2_DEGREE*theta,RAD_2_DEGREE*phi,y1,x1,x2,alpha1,y2,x3,x4,alpha2),name,"trap");
}

/// Constructor to be used when creating a new anonymous object with attribute initialization
Trap::Trap( double z,
            double theta,
            double phi,
            double y1,
            double x1,
            double x2,
            double alpha1,
            double y2,
            double x3,
            double x4,
            double alpha2)
{
  _assign(new TGeoTrap(z,RAD_2_DEGREE*theta,RAD_2_DEGREE*phi,y1,x1,x2,alpha1,y2,x3,x4,alpha2),"trap");
}

/// Constructor to be used when creating a new anonymous object with attribute initialization
Trap::Trap( double pz, double py, double px, double pLTX) {
  double z      = pz/2e0;
  double theta  = 0e0;
  double phi    = 0e0;
  double y1     = py/2e0;
  double x1     = px/2e0;
  double x2     = pLTX/2e0;
  double alpha1 = (pLTX-px)/py;
  _assign(new TGeoTrap(z,RAD_2_DEGREE*theta,RAD_2_DEGREE*phi,y1,x1,x2,alpha1,y1,x1,x2,alpha1),"trap");
}

/// Set the trap dimensions
Trap& Trap::setDimensions(double z,double theta,double phi,
                         double y1,double x1,double x2,double alpha1,
                         double y2,double x3,double x4,double alpha2)  {
  double params[] = {z,RAD_2_DEGREE * theta,RAD_2_DEGREE * phi,y1,x1,x2,alpha1,y2,x3,x4,alpha2};
  _setDimensions(params);
  return *this;
}

/// Helper function to create holy hedron
void PolyhedraRegular::_create(const string& name, int nsides, double rmin, double rmax, 
			       double zpos, double zneg, double start, double delta)
{
  if ( rmin<0e0 || rmin>rmax )
    throw runtime_error("PolyhedraRegular: Illegal argument rmin:<"+_toString(rmin)+"> is invalid!");
  else if ( rmax<0e0 )
    throw runtime_error("PolyhedraRegular: Illegal argument rmax:<"+_toString(rmax)+"> is invalid!");
  _assign(new TGeoPgon(),name,"polyhedra",false);
  double params[] = {
    0,
    RAD_2_DEGREE * 2 * M_PI,
    double(nsides),
    2e0,
    zpos,rmin,rmax,
    zneg,rmin,rmax
  };
  _setDimensions(&params[0]);
}

/// Constructor to be used when creating a new object
PolyhedraRegular::PolyhedraRegular(const string& name, int nsides, double rmin, double rmax, double zlen)  
{
  _create(name,nsides,rmin,rmax,zlen/2,-zlen/2,0,2*M_PI);
}

/// Constructor to be used when creating a new object
PolyhedraRegular::PolyhedraRegular(int nsides, double rmin, double rmax, double zlen)  
{
  _create("",nsides,rmin,rmax,zlen/2,-zlen/2,0,2*M_PI);
}

/// Constructor to be used when creating a new object
PolyhedraRegular::PolyhedraRegular(int nsides, double phistart, double rmin, double rmax, double zlen)  
{
  _create("",nsides,rmin,rmax,zlen/2,-zlen/2,phistart,2*M_PI);
}

/// Constructor to be used when creating a new object
PolyhedraRegular::PolyhedraRegular(int nsides, double rmin, double rmax, double zplanes[2])  
{
  _create("",nsides,rmin,rmax,zplanes[0],zplanes[1],0,2*M_PI);
}

/// Constructor to be used when creating a new object. Position is identity, Rotation is the identity rotation
SubtractionSolid::SubtractionSolid(const Solid& shape1, const Solid& shape2)   {
  TGeoSubtraction* sub  = new TGeoSubtraction(shape1,shape2,identityTransform(),identityTransform());
  _assign(new TGeoCompositeShape("",sub),"subtraction");
}

/// Constructor to be used when creating a new object. Placement by a generic transformation within the mother
SubtractionSolid::SubtractionSolid(const Solid& shape1, const Solid& shape2, const Transform3D& trans)   {
  TGeoHMatrix* tr = new TGeoHMatrix();
  Double_t* t = tr->GetTranslation();
  Double_t* r = tr->GetRotationMatrix();
  trans.GetComponents(r[0],r[1],r[2],t[0],r[3],r[4],r[5],t[1],r[6],r[7],r[8],t[2]);
  TGeoSubtraction* sub  = new TGeoSubtraction(shape1,shape2,identityTransform(),tr);
  _assign(new TGeoCompositeShape("",sub),"subtraction");
}

/// Constructor to be used when creating a new object. Rotation is the identity rotation
SubtractionSolid::SubtractionSolid(const Solid& shape1, const Solid& shape2, const Position& pos)   {
  TGeoTranslation* tr = new TGeoTranslation(pos.X(),pos.Y(),pos.Z());
  TGeoSubtraction* sub  = new TGeoSubtraction(shape1,shape2,identityTransform(),tr);
  _assign(new TGeoCompositeShape("",sub),"subtraction");
}

/// Constructor to be used when creating a new object
SubtractionSolid::SubtractionSolid(const Solid& shape1, const Solid& shape2, const Position& pos, const Rotation& rot)
{
  TGeoRotation rotation("",rot.Phi()*RAD_2_DEGREE,rot.Theta()*RAD_2_DEGREE,rot.Psi()*RAD_2_DEGREE);
  TGeoCombiTrans*    trans = new TGeoCombiTrans(pos.X(),pos.Y(),pos.Z(),0);
  trans->SetRotation(rotation.Inverse());
  TGeoSubtraction*    sub  = new TGeoSubtraction(shape1,shape2,identityTransform(),trans);
  _assign(new TGeoCompositeShape("",sub),"subtraction");
}

/// Constructor to be used when creating a new object
SubtractionSolid::SubtractionSolid(const string& name, const Solid& shape1, const Solid& shape2, const Position& pos, const Rotation& rot)
{
  TGeoRotation rotation("",rot.Phi()*RAD_2_DEGREE,rot.Theta()*RAD_2_DEGREE,rot.Psi()*RAD_2_DEGREE);
  TGeoCombiTrans*    trans = new TGeoCombiTrans(pos.X(),pos.Y(),pos.Z(),0);
  trans->SetRotation(rotation.Inverse());
  TGeoSubtraction*    sub  = new TGeoSubtraction(shape1,shape2,identityTransform(),trans);
  _assign(new TGeoCompositeShape(name.c_str(),sub), name, "subtraction");
}

/// Constructor to be used when creating a new object. Position is identity, Rotation is identity rotation
UnionSolid::UnionSolid(const Solid& shape1, const Solid& shape2)
{
  TGeoUnion*        uni = new TGeoUnion(shape1,shape2,identityTransform(),identityTransform());
  _assign(new TGeoCompositeShape("",uni),"union");
}

/// Constructor to be used when creating a new object. Placement by a generic transformation within the mother
UnionSolid::UnionSolid(const Solid& shape1, const Solid& shape2, const Transform3D& trans)   {
  TGeoHMatrix* tr = new TGeoHMatrix();
  Double_t* t = tr->GetTranslation();
  Double_t* r = tr->GetRotationMatrix();
  trans.GetComponents(r[0],r[1],r[2],t[0],r[3],r[4],r[5],t[1],r[6],r[7],r[8],t[2]);
  TGeoUnion*        uni = new TGeoUnion(shape1,shape2,identityTransform(),tr);
  _assign(new TGeoCompositeShape("",uni),"union");
}

/// Constructor to be used when creating a new object. Rotation is identity rotation
UnionSolid::UnionSolid(const Solid& shape1, const Solid& shape2, const Position& pos)
{
  TGeoCombiTrans* trans = new TGeoCombiTrans(pos.X(),pos.Y(),pos.Z(),0);
  TGeoUnion*        uni = new TGeoUnion(shape1,shape2,identityTransform(),trans);
  _assign(new TGeoCompositeShape("",uni),"union");
}

/// Constructor to be used when creating a new object
UnionSolid::UnionSolid(const Solid& shape1, const Solid& shape2, const Position& pos, const Rotation& rot)
{
  TGeoRotation rotation("",rot.Phi()*RAD_2_DEGREE,rot.Theta()*RAD_2_DEGREE,rot.Psi()*RAD_2_DEGREE);
  TGeoCombiTrans* tr = new TGeoCombiTrans(pos.X(),pos.Y(),pos.Z(),0);
  tr->SetRotation(rotation.Inverse());
  TGeoUnion*        uni = new TGeoUnion(shape1,shape2,identityTransform(),tr);
  _assign(new TGeoCompositeShape("",uni),"union");
}

/// Constructor to be used when creating a new object
UnionSolid::UnionSolid(const string& name, const Solid& shape1, const Solid& shape2, const Position& pos, const Rotation& rot)
{
  TGeoRotation rotation("",rot.Phi()*RAD_2_DEGREE,rot.Theta()*RAD_2_DEGREE,rot.Psi()*RAD_2_DEGREE);
  TGeoCombiTrans* trans = new TGeoCombiTrans(pos.X(),pos.Y(),pos.Z(),0);
  trans->SetRotation(rotation.Inverse());
  TGeoUnion* uni = new TGeoUnion(shape1,shape2,identityTransform(),trans);
  _assign(new TGeoCompositeShape(name.c_str(),uni), name, "union");
}

/// Constructor to be used when creating a new object. Position is identity, Rotation is identity rotation
IntersectionSolid::IntersectionSolid(const Solid& shape1, const Solid& shape2)    {
  TGeoIntersection* inter = new TGeoIntersection(shape1,shape2,identityTransform(),identityTransform());
  _assign(new TGeoCompositeShape("",inter),"intersection");
}

/// Constructor to be used when creating a new object. Placement by a generic transformation within the mother
IntersectionSolid::IntersectionSolid(const Solid& shape1, const Solid& shape2, const Transform3D& trans)   {
  TGeoHMatrix* tr = new TGeoHMatrix();
  Double_t*    t  = tr->GetTranslation();
  Double_t*    r  = tr->GetRotationMatrix();
  trans.GetComponents(r[0],r[1],r[2],t[0],r[3],r[4],r[5],t[1],r[6],r[7],r[8],t[2]);
  TGeoIntersection*  inter = new TGeoIntersection(shape1,shape2,identityTransform(),tr);
  _assign(new TGeoCompositeShape("",inter),"intersection");
}

/// Constructor to be used when creating a new object. Position is identity.
IntersectionSolid::IntersectionSolid(const Solid& shape1, const Solid& shape2, const Position& pos)    {
  TGeoCombiTrans*   trans = new TGeoCombiTrans(pos.X(),pos.Y(),pos.Z(),0);
  TGeoIntersection* inter = new TGeoIntersection(shape1,shape2,identityTransform(),trans);
  _assign(new TGeoCompositeShape("",inter),"intersection");
}

/// Constructor to be used when creating a new object
IntersectionSolid::IntersectionSolid(const Solid& shape1, const Solid& shape2, const Position& pos, const Rotation& rot)
{
  TGeoRotation rotation("",rot.Phi()*RAD_2_DEGREE,rot.Theta()*RAD_2_DEGREE,rot.Psi()*RAD_2_DEGREE);
  TGeoCombiTrans*   trans = new TGeoCombiTrans(pos.X(),pos.Y(),pos.Z(),0);
  trans->SetRotation(rotation.Inverse());
  TGeoIntersection* inter = new TGeoIntersection(shape1,shape2,identityTransform(),trans);
  _assign(new TGeoCompositeShape("",inter),"intersection");
}

/// Constructor to be used when creating a new object
IntersectionSolid::IntersectionSolid(const string& name, const Solid& shape1, const Solid& shape2, const Position& pos, const Rotation& rot)
{
  TGeoRotation rotation("",rot.Phi()*RAD_2_DEGREE,rot.Theta()*RAD_2_DEGREE,rot.Psi()*RAD_2_DEGREE);
  TGeoCombiTrans*    trans = new TGeoCombiTrans(pos.X(),pos.Y(),pos.Z(),0);
  trans->SetRotation(rotation.Inverse());
  TGeoIntersection*  inter = new TGeoIntersection(shape1,shape2,identityTransform(),trans);
  _assign(new TGeoCompositeShape(name.c_str(),inter),name,"intersection");
}

#define INSTANTIATE(X) template class DD4hep::Geometry::Solid_type<X>

INSTANTIATE(TGeoShape);
INSTANTIATE(TGeoBBox);
INSTANTIATE(TGeoCone);
INSTANTIATE(TGeoConeSeg);
INSTANTIATE(TGeoParaboloid);
INSTANTIATE(TGeoPcon);
INSTANTIATE(TGeoPgon);
INSTANTIATE(TGeoSphere);
INSTANTIATE(TGeoTorus);
INSTANTIATE(TGeoTube);
INSTANTIATE(TGeoTubeSeg);
INSTANTIATE(TGeoTrap);
INSTANTIATE(TGeoTrd2);
INSTANTIATE(TGeoCompositeShape);
