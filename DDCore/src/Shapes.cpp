#define _USE_MATH_DEFINES
#include "Shapes.h"
#include "Objects.h"
#include "Internals.h"

// C/C++ include files
#include <stdexcept>

// ROOT includes
#include "TGeoShape.h"
#include "TGeoPcon.h"
#include "TGeoTube.h"
#include "TGeoTrd2.h"
#include "TGeoBoolNode.h"
#include "TGeoCompositeShape.h"

using namespace std;
using namespace DetDesc::Geometry;

ZPlane::ZPlane(double rmin, double rmax, double z)
: m_rmin(rmin), m_rmax(rmax), m_z(z)
{
}

ZPlane& ZPlane::setRmin(double value)  {
  m_rmin = value;
  return *this;
}

ZPlane& ZPlane::setRmax(double value)  {
  m_rmax = value;
  return *this;
}

ZPlane& ZPlane::setZ(double value)  {
  m_z = value;
  return *this;
}

/// Access to the shape pointer
TGeoShape& Solid::shape()  const  {
  TObject* o = m_element.ptr();
  TGeoShape* s = dynamic_cast<TGeoShape*>(o);
  if ( s ) return *s;
  throw std::runtime_error("Invalid cast to TGeoshape");
}

/// Access to the shape pointer
void Solid::setDimensions(double params[])   {
  TGeoShape& s = shape();
  s.SetDimensions(params);
  s.ComputeBBox();
}

/// Constructor to be used when creating a new DOM tree
Box::Box(const Document& document, const string& name)
: Solid(document,"box",name)
{
  setDimensions(0.,0.,0.);
}

/// Constructor to be used when creating a new DOM tree
Box::Box(const Document& document, const string& name, double x, double y, double z)
: Solid(document,"box", name)
{
  setDimensions(x,y,z);
}

/// Set the box dimensions
Box& Box::setDimensions(double x, double y, double z)   {
  double params[] = {x,y,z};
  this->Solid::setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new DOM tree
Polycone::Polycone(const Document& document, const string& name)
: Solid(document, "polycone", name)
{
  TGeoPcon* s = (TGeoPcon*)m_element.ptr();
  s->Phi1() = 0.;
  s->Dphi() = RAD_2_DEGREE * (2.*M_PI);
}

/// Constructor to be used when creating a new DOM tree
Polycone::Polycone(const Document& document, const string& name, double start, double delta)  
: Solid(document, "polycone", name)
{
  TGeoPcon* s = new TGeoPcon();//(TGeoPcon*)m_element.ptr();
  m_element = s;
  s->Phi1() = RAD_2_DEGREE * start;
  s->Dphi() = RAD_2_DEGREE * delta;
}

/// Constructor to be used when creating a new polycone object. Add at the same time all Z planes
Polycone::Polycone(const Document& document, const std::string& name, double start, double delta, const vector<double>& rmin, const vector<double>& rmax, const vector<double>& z)
: Solid(document, "polycone", name)
{
  vector<double> params;
  params.push_back(RAD_2_DEGREE * start);
  params.push_back(RAD_2_DEGREE * delta);
  params.push_back(rmin.size());
  for( size_t i=0; i<rmin.size(); ++i )  {
    params.push_back(z[i]);
    params.push_back(rmin[i]);
    params.push_back(rmax[i]);
  }
  this->Solid::setDimensions(&params[0]);
  if ( rmin.size() < 2 )  {
    throw runtime_error("PolyCone::addZPlanes> Not enough Z planes. minimum is 2!");
  }
}

/// Add Z-planes to the Polycone
void Polycone::addZPlanes(const vector<double>& rmin, const vector<double>& rmax, const vector<double>& z)  {
  TGeoPcon* s = dynamic_cast<TGeoPcon*>(&shape());
  vector<double> params;
  size_t num = s->GetNz();
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
  if ( rmin.size() < 2 )  {
    throw runtime_error("PolyCone::addZPlanes> Not enough Z planes. minimum is 2!");
  }
  this->Solid::setDimensions(&params[0]);
}

/// Constructor to be used when creating a new DOM tree
Tube::Tube(const Document& doc, const string& name) : Solid(doc,"tube",name) 
{
  m_element.m_node = new TGeoTubeSeg(name.c_str(),0.,0.,0.,0.,0.);
}

/// Constructor to be used when creating a new DOM tree with attribute initialization
Tube::Tube(const Document& doc, const string& name, double rmin, double rmax, double z, double deltaPhi)
: Solid(doc,"tube",name)
{
  m_element.m_node = new TGeoTubeSeg(name.c_str(),0.,0.,0.,0.,0.);
  setDimensions(rmin,rmax,z,deltaPhi);
}

/// Set the tube dimensions
Tube& Tube::setDimensions(double rmin, double rmax, double z, double deltaPhi)
{
  double params[] = {rmin,rmax,z,0.,RAD_2_DEGREE * deltaPhi};
  this->Solid::setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new DOM tree
Cone::Cone(const Document& doc, const string& name)
: Solid(doc, "cone", name)
{
  setDimensions(0.,0.,0.,0.,0.);
}

/// Constructor to be used when creating a new DOM tree with attribute initialization
Cone::Cone(const Document& doc, const string& name,
           double z,
           double rmin1,
           double rmax1,
           double rmin2,
           double rmax2)
: Solid(doc, "cone", name)
{
  setDimensions(z,rmin1,rmax1,rmin2,rmax2);
}

Cone& Cone::setDimensions(double z,double rmin1,double rmax1,double rmin2,double rmax2)  {
  double params[] = {z,rmin1,rmax1,rmin2,rmax2};
  this->Solid::setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new DOM tree
Trapezoid::Trapezoid(const Document& doc, const string& name)
: Solid(doc,"trd",name) 
{
  setDimensions(0.,0.,0.,0.,0.);
}

/// Constructor to be used when creating a new DOM tree with attribute initialization
Trapezoid::Trapezoid(const Document& doc, const string& name, double x1, double x2, double y1, double y2, double z)
: Solid(doc,"trd2",name)
{
  setDimensions(x1,x2,y1,y2,z);
}

/// Set the Trapezoid dimensions
Trapezoid& Trapezoid::setDimensions(double x1, double x2, double y1, double y2, double z)
{
  double params[] = {x1,x2,y1,y2,z};
  this->Solid::setDimensions(params);
  return *this;
}

Trapezoid& Trapezoid::setX1(double value)  {
  TGeoTrd2* s = (TGeoTrd2*)(&shape());
  double params[] = {value,s->GetDx2(),s->GetDy1(),s->GetDy2(),s->GetDz()};
  this->Solid::setDimensions(params);
  return *this;
}

Trapezoid& Trapezoid::setX2(double value)  {
  TGeoTrd2* s = (TGeoTrd2*)(&shape());
  double params[] = {s->GetDx2(),value,s->GetDy1(),s->GetDy2(),s->GetDz()};
  this->Solid::setDimensions(params);
  return *this;
}

Trapezoid& Trapezoid::setY1(double value)  {
  TGeoTrd2* s = (TGeoTrd2*)(&shape());
  double params[] = {s->GetDx1(),s->GetDx2(),value,s->GetDy2(),s->GetDz()};
  this->Solid::setDimensions(params);
  return *this;
}

Trapezoid& Trapezoid::setY2(double value)  {
  TGeoTrd2* s = (TGeoTrd2*)(&shape());
  double params[] = {s->GetDx1(),s->GetDx2(),s->GetDy1(),value,s->GetDz()};
  this->Solid::setDimensions(params);
  return *this;
}

Trapezoid& Trapezoid::setZ(double value)  {
  TGeoTrd2* s = (TGeoTrd2*)(&shape());
  double params[] = {s->GetDx1(),s->GetDx2(),s->GetDy1(),s->GetDy2(),value};
  this->Solid::setDimensions(params);
  return *this;
}


Trap::Trap( const Document& doc, const string& name,
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
: Solid(doc,"trap", name)
{
  setDimensions(z,theta,phi,y1,x1,x2,alpha1,y2,x3,x4,alpha2);
}

/// Set the trap dimensions
Trap& Trap::setDimensions(double z,double theta,double phi,
                         double y1,double x1,double x2,double alpha1,
                         double y2,double x3,double x4,double alpha2)  {
  double params[] = {z,RAD_2_DEGREE * theta,RAD_2_DEGREE * phi,y1,x1,x2,alpha1,y2,x3,x4,alpha2};
  this->Solid::setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new DOM tree
PolyhedraRegular::PolyhedraRegular(const Document& doc, const string& name, int nsides, double rmin, double rmax, double zlen)  
: Solid(doc, "polyhedra", name)
{
  if ( rmin<0e0 || rmin>rmax )
    throw runtime_error("PolyhedraRegular: Illegal argument rmin:<"+_toString(rmin)+"> is invalid!");
  else if ( rmax<0e0 )
    throw runtime_error("PolyhedraRegular: Illegal argument rmax:<"+_toString(rmax)+"> is invalid!");

  double params[] = {
    RAD_2_DEGREE * 2e0 * M_PI,
    RAD_2_DEGREE * 2e0 * M_PI,
    double(nsides),
    2e0,
    zlen/2e0,rmin,rmax,
    -zlen/2e0,rmin,rmax
  };
  this->Solid::setDimensions(&params[0]);
}

/// Constructor to be used when creating a new DOM tree
BooleanSolid::BooleanSolid(const Document& doc, const string& type, const string& name)
: Solid(doc, type, name)
{
}

/// Constructor to be used when creating a new object
SubtractionSolid::SubtractionSolid(const Document& doc, const std::string& name, const std::string& expr)
: BooleanSolid(doc, "subtraction", name)
{
  //TGeoShape& s1 = shape1.shape();
  //TGeoShape& s2 = shape2.shape();
  m_element = new TGeoCompositeShape(name.c_str(),expr.c_str());
}


/// Constructor to be used when creating a new DOM tree
SubtractionSolid::SubtractionSolid(const Document& doc, const string& name, const Solid& shape1, const Solid& shape2)
: BooleanSolid(doc, "subtraction", name)
{
  //TGeoShape& s1 = shape1.shape();
  //TGeoShape& s2 = shape2.shape();
  shape1.shape().ComputeBBox();
  shape2.shape().ComputeBBox();
  string expr = shape1.name()+string("-")+shape2.name();
  m_element = new TGeoCompositeShape(name.c_str(),expr.c_str());
}
#include "TGeoMatrix.h"

/// Constructor to be used when creating a new object
SubtractionSolid::SubtractionSolid(const Document& doc, const std::string& name, const Solid& shape1, const Solid& shape2, const Position& pos, const Rotation& rot)
: BooleanSolid(doc, "subtraction", name)
{
  string combi1 = name+"_combitrans_first";
  string combi2 = name+"_combitrans_first";
  TGeoTranslation* t = value<TGeoTranslation>(pos);
  TGeoRotation*    r = value<TGeoRotation>(rot);
  //const double*    v = t->GetTranslation();

  TGeoTranslation* firstPos = new TGeoTranslation(0,0,0);
  TGeoRotation* firstRot = new TGeoRotation();
  firstRot->RotateZ(0);
  firstRot->RotateY(0);
  firstRot->RotateX(0);

  TGeoMatrix* firstMatrix  = new TGeoCombiTrans(*firstPos,firstRot->Inverse());
  TGeoMatrix* secondMatrix = new TGeoCombiTrans(*t,r->Inverse());

  TGeoCompositeShape* composite = 
    new TGeoCompositeShape(name.c_str(),
                           new TGeoSubtraction(&shape1.shape(),&shape2.shape(),
                                               firstMatrix,secondMatrix));
  composite->ComputeBBox();
  m_element = composite;
}
#if 0

   TGeoMatrix* firstMatrix = new TGeoCombiTrans(*firstPos,firstRot->Inverse());
   TGeoMatrix* secondMatrix = new TGeoCombiTrans(*secondPos,secondRot->Inverse());

   TGeoCompositeShape* boolean = 0;
   if (!first || !second) {
      Fatal("BooSolid", "Incomplete solid %s, missing shape components", name.Data());
      return child;
   }   
   switch (num) {
   case 1: boolean = new TGeoCompositeShape(NameShort(name),new TGeoSubtraction(first,second,firstMatrix,secondMatrix)); break;      // SUBTRACTION
   case 2: boolean = new TGeoCompositeShape(NameShort(name),new TGeoIntersection(first,second,firstMatrix,secondMatrix)); break;     // INTERSECTION 
#endif
