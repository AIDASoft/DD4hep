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

#define _USE_MATH_DEFINES

// Framework include files
#include "DD4hep/Detector.h"
#include "DD4hep/MatrixHelpers.h"
#include "DD4hep/DD4hepUnits.h"
#include "DD4hep/Printout.h"

// C/C++ include files
#include <stdexcept>
#include <iomanip>

// ROOT includes
#include "TClass.h"
#include "TGeoMatrix.h"
#include "TGeoBoolNode.h"
#include "TGeoCompositeShape.h"

using namespace std;
using namespace dd4hep;
namespace units = dd4hep;

namespace {
}

/// Pretty print of solid attributes
string dd4hep::toStringSolid(const TGeoShape* shape, int precision)   {
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
    TGeoBBox* sh = (TGeoBBox*) shape;
    log << " x:" << sh->GetDX()
        << " y:" << sh->GetDY()
        << " z:" << sh->GetDZ();
  }
  else if (cl == TGeoHalfSpace::Class()) {
    TGeoHalfSpace* sh = (TGeoHalfSpace*)(const_cast<TGeoShape*>(shape));
    log << " Point:  (" << sh->GetPoint()[0] << ", " << sh->GetPoint()[1] << ", " << sh->GetPoint()[2] << ") " 
        << " Normal: (" << sh->GetNorm()[0]  << ", " << sh->GetNorm()[1]  << ", " << sh->GetNorm()[2]  << ") ";
  }
  else if (cl == TGeoTube::Class()) {
    const TGeoTube* sh = (const TGeoTube*) shape;
    log << " rmin:" << sh->GetRmin() << " rmax:" << sh->GetRmax() << " dz:" << sh->GetDz();
  }
  else if (cl == TGeoTubeSeg::Class()) {
    const TGeoTubeSeg* sh = (const TGeoTubeSeg*) shape;
    log << " rmin:" << sh->GetRmin() << " rmax:" << sh->GetRmax() << " dz:" << sh->GetDz()
        << " Phi1:" << sh->GetPhi1() << " Phi2:" << sh->GetPhi2();
  }
  else if (cl == TGeoCtub::Class()) {
    const TGeoCtub* sh = (const TGeoCtub*) shape;
    const Double_t*	hi = sh->GetNhigh();
    const Double_t*	lo = sh->GetNlow();
    log << " rmin:" << sh->GetRmin() << " rmax:" << sh->GetRmax() << " dz:" << sh->GetDz()
        << " Phi1:" << sh->GetPhi1() << " Phi2:" << sh->GetPhi2()
        << " lx:" << lo[0] << " ly:" << lo[1] << " lz:" << lo[2]
        << " hx:" << hi[0] << " hy:" << hi[1] << " hz:" << hi[2];
  }
  else if (cl == TGeoEltu::Class()) {
    const TGeoEltu* sh = (const TGeoEltu*) shape;
    log << " A:" << sh->GetA() << " B:" << sh->GetB() << " dz:" << sh->GetDz();
  }
  else if (cl == TGeoTrd1::Class()) {
    const TGeoTrd1* sh = (const TGeoTrd1*) shape;
    log << " x1:" << sh->GetDx1() << " x2:" << sh->GetDx2() << " y:" << sh->GetDy() << " z:" << sh->GetDz();
  }
  else if (cl == TGeoTrd2::Class()) {
    const TGeoTrd2* sh = (const TGeoTrd2*) shape;
    log << " x1:" << sh->GetDx1() << " x2:" << sh->GetDx2()
        << " y1:" << sh->GetDy1() << " y2:" << sh->GetDy2() << " z:" << sh->GetDz();
  }
  else if (cl == TGeoTrap::Class()) {
    const TGeoTrap* sh = (const TGeoTrap*) shape;
    log << " dz:" << sh->GetDz() << " Theta:" << sh->GetTheta() << " Phi:" << sh->GetPhi()
        << " H1:" << sh->GetH1() << " Bl1:"   << sh->GetBl1()   << " Tl1:" << sh->GetTl1() << " Alpha1:" << sh->GetAlpha1()
        << " H2:" << sh->GetH2() << " Bl2:"   << sh->GetBl2()   << " Tl2:" << sh->GetTl2() << " Alpha2:" << sh->GetAlpha2();
  }
  else if (cl == TGeoHype::Class()) {
    const TGeoHype* sh = (const TGeoHype*) shape;
    log << " rmin:" << sh->GetRmin() << " rmax:"  << sh->GetRmax() << " dz:" << sh->GetDz()
        << " StIn:" << sh->GetStIn() << " StOut:" << sh->GetStOut();
  }
  else if (cl == TGeoPgon::Class()) {
    const TGeoPgon* sh = (const TGeoPgon*) shape;
    log << " Phi1:"   << sh->GetPhi1()   << " dPhi:" << sh->GetDphi()
        << " NEdges:" << sh->GetNedges() << " Nz:" << sh->GetNz();
    for(int i=0, n=sh->GetNz(); i<n; ++i)  {
      log << " i=" << i << " z:" << sh->GetZ(i)
          << " r:[" << sh->GetRmin(i) << "," << sh->GetRmax(i) << "]";
    }
  }
  else if (cl == TGeoPcon::Class()) {
    const TGeoPcon* sh = (const TGeoPcon*) shape;
    log << " Phi1:" << sh->GetPhi1() << " dPhi:" << sh->GetDphi() << " Nz:" << sh->GetNz();
    for(int i=0, n=sh->GetNz(); i<n; ++i)  {
      log << " i=" << i << " z:" << sh->GetZ(i)
          << " r:[" << sh->GetRmin(i) << "," << sh->GetRmax(i) << "]";
    }
  }
  else if (cl == TGeoCone::Class()) {
    const TGeoCone* sh = (const TGeoCone*) shape;
    log << " rmin1:" << sh->GetRmin1() << " rmax1:" << sh->GetRmax1()
        << " rmin2:" << sh->GetRmin2() << " rmax2:" << sh->GetRmax2()
        << " dz:"    << sh->GetDz();
  }
  else if (cl == TGeoConeSeg::Class()) {
    const TGeoConeSeg* sh = (const TGeoConeSeg*) shape;
    log << " rmin1:" << sh->GetRmin1() << " rmax1:" << sh->GetRmax1()
        << " rmin2:" << sh->GetRmin2() << " rmax2:" << sh->GetRmax2()
        << " dz:"    << sh->GetDz()
        << " Phi1:"  << sh->GetPhi1() << " Phi2:" << sh->GetPhi2();
  }
  else if (cl == TGeoParaboloid::Class()) {
    const TGeoParaboloid* sh = (const TGeoParaboloid*) shape;
    log << " dz:" << sh->GetDz() << " RLo:" << sh->GetRlo() << " Rhi:" << sh->GetRhi();
  }
  else if (cl == TGeoSphere::Class()) {
    const TGeoSphere* sh = (const TGeoSphere*) shape;
    log << " rmin:" << sh->GetRmin() << " rmax:" << sh->GetRmax()
        << " Phi1:" << sh->GetPhi1() << " Phi2:" << sh->GetPhi2()
        << " Theta1:" << sh->GetTheta1() << " Theta2:" << sh->GetTheta2();
  }
  else if (cl == TGeoTorus::Class()) {
    const TGeoTorus* sh = (const TGeoTorus*) shape;
    log << " rmin:" << sh->GetRmin() << " rmax:" << sh->GetRmax() << " r:" << sh->GetR()
        << " Phi1:" << sh->GetPhi1() << " dPhi:" << sh->GetDphi();
  }
  else if (cl == TGeoArb8::Class()) {
    TGeoArb8* sh = (TGeoArb8*) shape;
    const Double_t* v = sh->GetVertices();
    log << " dz:" << sh->GetDz();
    for(int i=0; i<8; ++i) {
      log << " x[" << i << "]:" << *v; ++v;
      log << " y[" << i << "]:" << *v; ++v;
    }
  }
  else if (cl == TGeoXtru::Class()) {
    const TGeoXtru* sh = (const TGeoXtru*) shape;
    log << " X:   " << sh->GetX(0)
        << " Y:   " << sh->GetY(0)
        << " Z:   " << sh->GetZ(0)
        << " Xoff:" << sh->GetXOffset(0)
        << " Yoff:" << sh->GetYOffset(0)
        << " Nvtx:" << sh->GetNvert()
        << " Nz:  " << sh->GetNz();
  }
  else if (shape->IsA() == TGeoCompositeShape::Class()) {
    const TGeoCompositeShape* sh = (const TGeoCompositeShape*) shape;
    const TGeoBoolNode* boolean = sh->GetBoolNode();
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

/// Output mesh vertices to string
std::string dd4hep::toStringMesh(const TGeoShape* shape, int prec)  {
  Solid sol(shape);
  stringstream os;

  // Prints shape parameters
  Int_t nvert = 0, nsegs = 0, npols = 0;

  sol->GetMeshNumbers(nvert, nsegs, npols);
  Double_t* points = new Double_t[3*nvert];
  sol->SetPoints(points);

  os << setw(16) << left << sol->IsA()->GetName()
     << " " << nvert << " Mesh-points:" << endl;
  os << setw(16) << left << sol->IsA()->GetName() << " " << sol->GetName()
     << " N(mesh)=" << sol->GetNmeshVertices()
     << "  N(vert)=" << nvert << "  N(seg)=" << nsegs << "  N(pols)=" << npols << endl;
    
  for(Int_t i=0; i<nvert; ++i)   {
    Double_t* p = points + 3*i;
    os << setw(16) << left << sol->IsA()->GetName() << " " << setw(3) << left << i
       << " Local  ("  << setw(7) << setprecision(prec) << fixed << right << p[0]
       << ", "         << setw(7) << setprecision(prec) << fixed << right << p[1]
       << ", "         << setw(7) << setprecision(prec) << fixed << right << p[2]
       << ")" << endl;
  }
  Box box = sol;
  const Double_t* org = box->GetOrigin();
  os << setw(16) << left << sol->IsA()->GetName()
     << " Bounding box: "
     << " dx="        << setw(7) << setprecision(prec) << fixed << right << box->GetDX()
     << " dy="        << setw(7) << setprecision(prec) << fixed << right << box->GetDY()
     << " dz="        << setw(7) << setprecision(prec) << fixed << right << box->GetDZ()
     << " Origin: x=" << setw(7) << setprecision(prec) << fixed << right << org[0]
     << " y="         << setw(7) << setprecision(prec) << fixed << right << org[1]
     << " z="         << setw(7) << setprecision(prec) << fixed << right << org[2]
     << endl;
  
  /// -------------------- DONE --------------------
  delete [] points;
  return os.str();
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

/// Set new shape name
template <typename T> Solid_type<T>& Solid_type<T>::setName(const char* value)    {
  this->access()->SetName(value);
  return *this;
}

/// Set new shape name
template <typename T> Solid_type<T>& Solid_type<T>::setName(const string& value)    {
  this->access()->SetName(value.c_str());
  return *this;
}

/// Access to shape type (The TClass name of the ROOT implementation)
template <typename T> const char* Solid_type<T>::type() const  {
  if ( this->ptr() )  {
    return this->ptr()->IsA()->GetName();
  }
  return "";
}

/// Constructor to create an anonymous new box object (retrieves name from volume)
ShapelessSolid::ShapelessSolid(const string& nam)  {
  _assign(new TGeoShapeAssembly(), nam, "assembly", true);
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
Polycone::Polycone(double startPhi, double deltaPhi) {
  _assign(new TGeoPcon(startPhi/units::deg, deltaPhi/units::deg, 0), "", "polycone", false);
}

/// Constructor to be used when creating a new polycone object. Add at the same time all Z planes
Polycone::Polycone(double startPhi, double deltaPhi,
                   const vector<double>& rmin, const vector<double>& rmax, const vector<double>& z) {
  vector<double> params;
  if (rmin.size() < 2) {
    throw runtime_error("dd4hep: PolyCone Not enough Z planes. minimum is 2!");
  }
  if((z.size()!=rmin.size()) || (z.size()!=rmax.size()) )    {
    throw runtime_error("dd4hep: Polycone: vectors z,rmin,rmax not of same length");
  }
  params.push_back(startPhi/units::deg);
  params.push_back(deltaPhi/units::deg);
  params.push_back(rmin.size());
  for (size_t i = 0; i < rmin.size(); ++i) {
    params.push_back(z[i] );
    params.push_back(rmin[i] );
    params.push_back(rmax[i] );
  }
  _assign(new TGeoPcon(&params[0]), "", "polycone", true);
}

/// Constructor to be used when creating a new polycone object. Add at the same time all Z planes
Polycone::Polycone(double startPhi, double deltaPhi, const vector<double>& r, const vector<double>& z) {
  vector<double> params;
  if (r.size() < 2) {
    throw runtime_error("dd4hep: PolyCone Not enough Z planes. minimum is 2!");
  }
  if((z.size()!=r.size()) )    {
    throw runtime_error("dd4hep: Polycone: vectors z,r not of same length");
  } 
  params.push_back(startPhi/units::deg);
  params.push_back(deltaPhi/units::deg);
  params.push_back(r.size());
  for (size_t i = 0; i < r.size(); ++i) {
    params.push_back(z[i] );
    params.push_back(0.0 );
    params.push_back(r[i] );
  }
  _assign(new TGeoPcon(&params[0]), "", "polycone", true);
}

/// Add Z-planes to the Polycone
void Polycone::addZPlanes(const vector<double>& rmin, const vector<double>& rmax, const vector<double>& z) {
  TGeoPcon* sh = *this;
  vector<double> params;
  size_t num = sh->GetNz();
  if (rmin.size() < 2) {
    throw runtime_error("dd4hep: PolyCone::addZPlanes> Not enough Z planes. minimum is 2!");
  }
  params.push_back(sh->GetPhi1());
  params.push_back(sh->GetDphi());
  params.push_back(num + rmin.size());
  for (size_t i = 0; i < num; ++i) {
    params.push_back(sh->GetZ(i));
    params.push_back(sh->GetRmin(i));
    params.push_back(sh->GetRmax(i));
  }
  for (size_t i = 0; i < rmin.size(); ++i) {
    params.push_back(z[i] );
    params.push_back(rmin[i] );
    params.push_back(rmax[i] );
  }
  _setDimensions(&params[0]);
}

/// Constructor to be used when creating a new cone segment object
ConeSegment::ConeSegment(double dz, 
			 double rmin1, double rmax1,
			 double rmin2, double rmax2,
			 double phi1, double phi2)
{
  _assign(new TGeoConeSeg(dz, rmin1, rmax1, rmin2, rmax2, phi1/units::deg, phi2/units::deg), "", "cone_segment", true);
}

/// Set the cone segment dimensions
ConeSegment& ConeSegment::setDimensions(double dz, 
					double rmin1, double rmax1,
					double rmin2, double rmax2,
					double phi1,  double phi2) {
  double params[] = { dz, rmin1, rmax1, rmin2, rmax2, phi1/units::deg, phi2/units::deg };
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new object with attribute initialization
void Tube::make(const string& nam, double rmin, double rmax, double z, double startPhi, double deltaPhi) {
  _assign(new TGeoTubeSeg(rmin,rmax,z,startPhi/units::deg,deltaPhi/units::deg),nam,"tube",true);
  //_assign(new MyConeSeg(), nam, "tube", true);
  //setDimensions(rmin, rmax, z, startPhi, deltaPhi);
}

/// Set the tube dimensions
Tube& Tube::setDimensions(double rmin, double rmax, double z, double startPhi, double deltaPhi) {
  double params[] = {rmin,rmax,z,startPhi/units::deg,deltaPhi/units::deg};
  //double params[] = { z, rmin, rmax, rmin, rmax, startPhi/units::deg,deltaPhi/units::deg };
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new object with attribute initialization
CutTube::CutTube(double rmin, double rmax, double dz, double phi1, double phi2,
                 double lx, double ly, double lz, double tx, double ty, double tz)  {
  make(rmin,rmax,dz,phi1/units::deg,phi2/units::deg,lx,ly,lz,tx,ty,tz);
}

/// Constructor to be used when creating a new object with attribute initialization
void CutTube::make(double rmin, double rmax, double dz, double phi1, double phi2,
                   double lx, double ly, double lz, double tx, double ty, double tz)  {
  _assign(new TGeoCtub(rmin,rmax,dz,phi1,phi2,lx,ly,lz,tx,ty,tz),"","cuttube",true);
}

/// Constructor to create a truncated tube object with attribute initialization
TruncatedTube::TruncatedTube(double zhalf, double rmin, double rmax, double startPhi, double deltaPhi,
                             double cutAtStart, double cutAtDelta, bool cutInside)
{  make(zhalf, rmin, rmax, startPhi/units::deg, deltaPhi/units::deg, cutAtStart, cutAtDelta, cutInside);    }

/// Internal helper method to support object construction
void TruncatedTube::make(double zhalf, double rmin, double rmax, double startPhi, double deltaPhi,
                         double cutAtStart, double cutAtDelta, bool cutInside)   {
  // check the parameters
  if( rmin <= 0 || rmax <= 0 || cutAtStart <= 0 || cutAtDelta <= 0 )
    except("TruncatedTube","++ 0 <= rIn,cutAtStart,rOut,cutAtDelta,rOut violated!");
  else if( rmin >= rmax )
    except("TruncatedTube","++ rIn<rOut violated!");
  else if( startPhi != 0. )
    except("TruncatedTube","++ startPhi != 0 not supported!");

  double r         = cutAtStart;
  double R         = cutAtDelta;
  // angle of the box w.r.t. tubs
  double cath      = r - R * std::cos( deltaPhi*units::deg );
  double hypo      = std::sqrt( r*r + R*R - 2.*r*R * std::cos( deltaPhi*units::deg ));
  double cos_alpha = cath / hypo;
  double alpha     = std::acos( cos_alpha );
  double sin_alpha = std::sin( std::fabs(alpha) );
  
  // exaggerate dimensions - does not matter, it's subtracted!
  // If we don't, the **edge** of the box would cut into the tube segment
  // for larger delta-phi values
  double boxX      = 1.1*rmax + rmax/sin_alpha; // Need to adjust for move!
  double boxY      = rmax;
  // width of the box > width of the tubs
  double boxZ      = 1.1 * zhalf;
  double xBox;      // center point of the box
  if( cutInside )
    xBox = r - boxY / sin_alpha;
  else
    xBox = r + boxY / sin_alpha;

  // rotationmatrix of box w.r.t. tubs
  TGeoRotation rot;
  //rot.RotateX( 90.0 );
  rot.RotateZ( -alpha/dd4hep::deg );
  TGeoTranslation trans(xBox, 0., 0.);  
  TGeoBBox* box  = new TGeoBBox(boxX, boxY, boxZ);
  TGeoTubeSeg* tubs = new TGeoTubeSeg(rmin, rmax, zhalf, startPhi, deltaPhi);
  TGeoSubtraction* sub = new TGeoSubtraction(tubs, box, nullptr, new TGeoCombiTrans(trans, rot));
  // For debugging:
  // TGeoUnion* sub = new TGeoUnion(tubs, box, nullptr, new TGeoCombiTrans(trans, rot));
  _assign(new TGeoCompositeShape("", sub),"","trunctube",true);
#if 0
  cout << "Trans:";  trans.Print(); cout << endl;
  cout << "Rot:  ";  rot.Print();   cout << endl;
  cout << " Zhalf:        " << zhalf
       << " rmin:         " << rmin
       << " rmax:         " << rmax
       << " r/cutAtStart: " << r
       << " R/cutAtDelta: " << R
       << " cutInside:    " << (cutInside ? "YES" : "NO ")
       << endl;
  cout << " cath:      " << cath
       << " hypo:      " << hypo
       << " cos_alpha: " << cos_alpha
       << " alpha:     " << alpha
       << " alpha(deg):" << alpha/dd4hep::deg
       << endl;
  cout << " Deg:       " << dd4hep::deg
       << " cm:        " << dd4hep::cm
       << " xBox:      " << xBox
       << endl;
  cout << "Box:" << "x:" << box->GetDX() << " y:" << box->GetDY() << " z:" << box->GetDZ() << endl;
  cout << "Tubs:" << " rmin:" << rmin << " rmax" << rmax << "zhalf" << zhalf
       << " startPhi:" <<  startPhi << " deltaPhi:" << deltaPhi << endl;
#endif
}

/// Constructor to be used when creating a new object with attribute initialization
void EllipticalTube::make(double a, double b, double dz) {
  _assign(new TGeoEltu("", a, b, dz), "", "elliptic_tube", true);
}

/// Constructor to be used when creating a new object with attribute initialization
void Cone::make(double z, double rmin1, double rmax1, double rmin2, double rmax2) {
  _assign(new TGeoCone(z, rmin1, rmax1, rmin2, rmax2 ), "", "cone", true);
}

/// Set the box dimensions (startPhi=0.0, deltaPhi=2*pi)
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
  _assign(new TGeoHype(rin, stin/units::deg, rout, stout/units::deg, dz), "", "hyperboloid", true);
}

/// Set the Hyperboloid dimensions
Hyperboloid& Hyperboloid::setDimensions(double rin, double stin, double rout, double stout, double dz)  {
  double params[] = { rin, stin/units::deg, rout, stout/units::deg, dz};
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new object with attribute initialization
Sphere::Sphere(double rmin, double rmax, double theta, double delta_theta, double phi, double delta_phi) {
  _assign(new TGeoSphere(rmin, rmax,
                         theta/units::deg, delta_theta/units::deg,
                         phi/units::deg, delta_phi/units::deg), "", "sphere", true);
}

/// Set the Sphere dimensions
Sphere& Sphere::setDimensions(double rmin, double rmax, double theta, double delta_theta, double phi, double delta_phi) {
  double params[] = { rmin, rmax, theta, delta_theta/units::deg, phi/units::deg, delta_phi/units::deg };
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new object with attribute initialization
void Torus::make(double r, double rmin, double rmax, double phi, double delta_phi) {
  _assign(new TGeoTorus(r, rmin, rmax, phi/units::deg, delta_phi/units::deg), "", "torus", true);
}

/// Set the Torus dimensions
Torus& Torus::setDimensions(double r, double rmin, double rmax, double phi, double delta_phi) {
  double params[] = { r, rmin, rmax, phi/units::deg, delta_phi/units::deg };
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new anonymous object with attribute initialization
Trap::Trap(double z, double theta, double phi,
           double h1, double bl1, double tl1, double alpha1,
           double h2, double bl2, double tl2, double alpha2) {
  _assign(new TGeoTrap(z, theta/units::deg, phi/units::deg,
                       h1, bl1, tl1, alpha1/units::deg,
                       h2, bl2, tl2, alpha2/units::deg), "", "trap", true);
}

/// Constructor to be used when creating a new anonymous object with attribute initialization
void Trap::make(double pz, double py, double px, double pLTX) {
  double z      = pz / 2e0;
  double theta  = 0e0;
  double phi    = 0e0;
  double h      = py / 2e0;
  double bl     = px / 2e0;
  double tl     = pLTX / 2e0;
  double alpha1 = (pLTX - px) / py;
  _assign(new TGeoTrap(z, theta, phi,
                       h, bl, tl, alpha1/units::deg,
                       h, bl, tl, alpha1/units::deg), "", "trap", true);
}

/// Set the trap dimensions
Trap& Trap::setDimensions(double z, double theta, double phi,
                          double h1, double bl1, double tl1, double alpha1,
                          double h2, double bl2, double tl2, double alpha2) {
  double params[] = { z,  theta/units::deg, phi/units::deg,
                      h1, bl1, tl1, alpha1/units::deg,
                      h2, bl2, tl2, alpha2/units::deg };
  _setDimensions(params);
  return *this;
}

/// Internal helper method to support object construction
void PseudoTrap::make(double x1, double x2, double y1, double y2, double z, double r, bool atMinusZ)    {
  double x            = atMinusZ ? x1 : x2;
  double h            = 0;
  bool   intersec     = false; // union or intersection solid
  double displacement = 0;
  double startPhi     = 0;
  double halfZ        = z;
  double halfOpeningAngle = std::asin( x / std::abs( r ))/units::deg;
  
  /// calculate the displacement of the tubs w.r.t. to the trap, determine the opening angle of the tubs
  double delta        = std::sqrt( r * r - x * x );
 
  if( r < 0 && std::abs(r) >= x )  {
    intersec = true; // intersection solid
    h = y1 < y2 ? y2 : y1; // tubs half height
    h += h/20.; // enlarge a bit - for subtraction solid
    if( atMinusZ )    {
      displacement = - halfZ - delta; 
      startPhi     = 270.0 - halfOpeningAngle;
    }
    else    {
      displacement =   halfZ + delta;
      startPhi     = 90.0 - halfOpeningAngle;
    }
  }
  else if( r > 0 && std::abs(r) >= x )  {
    if( atMinusZ )    {
      displacement = - halfZ + delta;
      startPhi     = 90.0 - halfOpeningAngle;
      h = y1;
    }
    else
    {
      displacement =   halfZ - delta; 
      startPhi     = 270.0 - halfOpeningAngle;
      h = y2;
    }    
  }
  else  {
    except("PseudoTrap","Check parameters of the PseudoTrap!");   
  }
 
  Solid trap(new TGeoTrd2(x1, x2, y1, y2, halfZ));
  Solid tubs(new TGeoTubeSeg(0.,std::abs(r),h,startPhi,startPhi + halfOpeningAngle*2.));
  TGeoCompositeShape* solid = 0;
  if( intersec )  {
    solid = SubtractionSolid(trap, tubs, Transform3D(RotationX(M_PI/2.), Position(0.,0.,displacement))).ptr();
  }
  else  {
    SubtractionSolid sub(tubs, Box(1.1*x, 1.1*h, std::sqrt(r*r-x*x)), Transform3D(RotationX(M_PI/2.)));
    solid = UnionSolid(trap, sub, Transform3D(RotationX(M_PI/2.), Position(0,0,displacement))).ptr();
  }
  _assign(solid,"","pseudo-trap", true);
}

/// Helper function to create poly hedron
void PolyhedraRegular::make(int nsides, double rmin, double rmax, double zpos, double zneg, double start, double delta) {
  if (rmin < 0e0 || rmin > rmax)
    throw runtime_error("dd4hep: PolyhedraRegular: Illegal argument rmin:<" + _toString(rmin) + "> is invalid!");
  else if (rmax < 0e0)
    throw runtime_error("dd4hep: PolyhedraRegular: Illegal argument rmax:<" + _toString(rmax) + "> is invalid!");
  double params[] = { start, delta, double(nsides), 2e0, zpos, rmin, rmax, zneg, rmin, rmax };
  _assign(new TGeoPgon(params), "", "polyhedra", false);
  //_setDimensions(&params[0]);
}

/// Constructor to be used when creating a new object
PolyhedraRegular::PolyhedraRegular(int nsides, double rmin, double rmax, double zlen) {
  make(nsides, rmin, rmax, zlen / 2, -zlen / 2, 0, 360.);
}

/// Constructor to be used when creating a new object
PolyhedraRegular::PolyhedraRegular(int nsides, double phistart, double rmin, double rmax, double zlen) {
  make(nsides, rmin, rmax, zlen / 2, -zlen / 2, phistart/units::deg, 360.);
}

/// Constructor to be used when creating a new object
PolyhedraRegular::PolyhedraRegular(int nsides, double rmin, double rmax, double zplanes[2]) {
  make(nsides, rmin, rmax, zplanes[0], zplanes[1], 0, 360.);
}

/// Helper function to create poly hedron
void Polyhedra::make(int nsides, double start, double delta,
                     const vector<double>& z, const vector<double>& rmin, const vector<double>& rmax)  {
  vector<double> temp;
  if ( rmin.size() != z.size() || rmax.size() != z.size() )  {
    except("Polyhedra",
           "Number of values to define zplanes are incorrect: z:%ld rmin:%ld rmax:%ld",
           z.size(), rmin.size(), rmax.size());
  }
  // No need to transform coordinates to cm. We are in the dd4hep world: all is already in cm.
  temp.reserve(4+z.size()*2);
  temp.push_back(start);
  temp.push_back(delta);
  temp.push_back(double(nsides));
  temp.push_back(double(z.size()));
  for(size_t i=0; i<z.size(); ++i)   {
    temp.push_back(z[i]);
    temp.push_back(rmin[i]);
    temp.push_back(rmax[i]);
  }
  _assign(new TGeoPgon(&temp[0]), "", "polyhedra", false);
}

/// Constructor to create a new object. Phi(start), deltaPhi, Z-planes at specified positions
Polyhedra::Polyhedra(int nsides, double start, double delta, const vector<double>& z, const vector<double>& r)  {
  vector<double> rmin(r.size(), 0.);
  make(nsides, start/units::deg, delta/units::deg, z, rmin, r);
}

/// Constructor to create a new object. Phi(start), deltaPhi, Z-planes at specified positions
Polyhedra::Polyhedra(int nsides, double start, double delta,
                     const vector<double>& z, const vector<double>& rmin, const vector<double>& rmax)  {
  make(nsides, start/units::deg, delta/units::deg, z, rmin, rmax);
}

/// Helper function to create the polyhedron
void ExtrudedPolygon::make(const vector<double>& pt_x,
                           const vector<double>& pt_y,
                           const vector<double>& sec_z,
                           const vector<double>& sec_x,
                           const vector<double>& sec_y,
                           const vector<double>& sec_scale)
{
  TGeoXtru* solid = new TGeoXtru(sec_z.size());
  _assign(solid, "", "polyhedra", false);
  // No need to transform coordinates to cm. We are in the dd4hep world: all is already in cm.
  solid->DefinePolygon(pt_x.size(), &(*pt_x.begin()), &(*pt_y.begin()));
  for( size_t i = 0; i < sec_z.size(); ++i )
    solid->DefineSection(i, sec_z[i], sec_x[i], sec_y[i], sec_scale[i]);
}

/// Constructor to create a new object. 
ExtrudedPolygon::ExtrudedPolygon(const vector<double>& pt_x,
                                 const vector<double>& pt_y,
                                 const vector<double>& sec_z,
                                 const vector<double>& sec_x,
                                 const vector<double>& sec_y,
                                 const vector<double>& sec_scale)
{
  make(pt_x, pt_y, sec_z, sec_x, sec_y, sec_scale);
}

/// Creator method
void EightPointSolid::make(double dz, const double* vtx)   {
  _assign(new TGeoArb8(dz, (double*)vtx), "", "Arb8", true);
}

/// Constructor to be used when creating a new object. Position is identity, Rotation is the identity rotation
SubtractionSolid::SubtractionSolid(const Solid& shape1, const Solid& shape2) {
  TGeoSubtraction* sub = new TGeoSubtraction(shape1, shape2, detail::matrix::_identity(), detail::matrix::_identity());
  _assign(new TGeoCompositeShape("", sub), "", "subtraction", true);
}

/// Constructor to be used when creating a new object. Placement by a generic transformation within the mother
SubtractionSolid::SubtractionSolid(const Solid& shape1, const Solid& shape2, const Transform3D& trans) {
  TGeoSubtraction* sub = new TGeoSubtraction(shape1, shape2, detail::matrix::_identity(), detail::matrix::_transform(trans));
  _assign(new TGeoCompositeShape("", sub), "", "subtraction", true);
}

/// Constructor to be used when creating a new object. Rotation is the identity rotation
SubtractionSolid::SubtractionSolid(const Solid& shape1, const Solid& shape2, const Position& pos) {
  TGeoSubtraction* sub = new TGeoSubtraction(shape1, shape2, detail::matrix::_identity(), detail::matrix::_translation(pos));
  _assign(new TGeoCompositeShape("", sub), "", "subtraction", true);
}

/// Constructor to be used when creating a new object
SubtractionSolid::SubtractionSolid(const Solid& shape1, const Solid& shape2, const RotationZYX& rot) {
  TGeoSubtraction* sub = new TGeoSubtraction(shape1, shape2, detail::matrix::_identity(), detail::matrix::_rotationZYX(rot));
  _assign(new TGeoCompositeShape("", sub), "", "subtraction", true);
}

/// Constructor to be used when creating a new object
SubtractionSolid::SubtractionSolid(const Solid& shape1, const Solid& shape2, const Rotation3D& rot) {
  TGeoSubtraction* sub = new TGeoSubtraction(shape1, shape2, detail::matrix::_identity(), detail::matrix::_rotation3D(rot));
  _assign(new TGeoCompositeShape("", sub), "", "subtraction", true);
}

/// Constructor to be used when creating a new object. Position is identity, Rotation is identity rotation
UnionSolid::UnionSolid(const Solid& shape1, const Solid& shape2) {
  TGeoUnion* uni = new TGeoUnion(shape1, shape2, detail::matrix::_identity(), detail::matrix::_identity());
  _assign(new TGeoCompositeShape("", uni), "", "union", true);
}

/// Constructor to be used when creating a new object. Placement by a generic transformation within the mother
UnionSolid::UnionSolid(const Solid& shape1, const Solid& shape2, const Transform3D& trans) {
  TGeoUnion* uni = new TGeoUnion(shape1, shape2, detail::matrix::_identity(), detail::matrix::_transform(trans));
  _assign(new TGeoCompositeShape("", uni), "", "union", true);
}

/// Constructor to be used when creating a new object. Rotation is identity rotation
UnionSolid::UnionSolid(const Solid& shape1, const Solid& shape2, const Position& pos) {
  TGeoUnion* uni = new TGeoUnion(shape1, shape2, detail::matrix::_identity(), detail::matrix::_translation(pos));
  _assign(new TGeoCompositeShape("", uni), "", "union", true);
}

/// Constructor to be used when creating a new object
UnionSolid::UnionSolid(const Solid& shape1, const Solid& shape2, const RotationZYX& rot) {
  TGeoUnion *uni = new TGeoUnion(shape1, shape2, detail::matrix::_identity(), detail::matrix::_rotationZYX(rot));
  _assign(new TGeoCompositeShape("", uni), "", "union", true);
}

/// Constructor to be used when creating a new object
UnionSolid::UnionSolid(const Solid& shape1, const Solid& shape2, const Rotation3D& rot) {
  TGeoUnion *uni = new TGeoUnion(shape1, shape2, detail::matrix::_identity(), detail::matrix::_rotation3D(rot));
  _assign(new TGeoCompositeShape("", uni), "", "union", true);
}

/// Constructor to be used when creating a new object. Position is identity, Rotation is identity rotation
IntersectionSolid::IntersectionSolid(const Solid& shape1, const Solid& shape2) {
  TGeoIntersection* inter = new TGeoIntersection(shape1, shape2, detail::matrix::_identity(), detail::matrix::_identity());
  _assign(new TGeoCompositeShape("", inter), "", "intersection", true);
}

/// Constructor to be used when creating a new object. Placement by a generic transformation within the mother
IntersectionSolid::IntersectionSolid(const Solid& shape1, const Solid& shape2, const Transform3D& trans) {
  TGeoIntersection* inter = new TGeoIntersection(shape1, shape2, detail::matrix::_identity(), detail::matrix::_transform(trans));
  _assign(new TGeoCompositeShape("", inter), "", "intersection", true);
}

/// Constructor to be used when creating a new object. Position is identity.
IntersectionSolid::IntersectionSolid(const Solid& shape1, const Solid& shape2, const Position& pos) {
  TGeoIntersection* inter = new TGeoIntersection(shape1, shape2, detail::matrix::_identity(), detail::matrix::_translation(pos));
  _assign(new TGeoCompositeShape("", inter), "", "intersection", true);
}

/// Constructor to be used when creating a new object
IntersectionSolid::IntersectionSolid(const Solid& shape1, const Solid& shape2, const RotationZYX& rot) {
  TGeoIntersection* inter = new TGeoIntersection(shape1, shape2, detail::matrix::_identity(), detail::matrix::_rotationZYX(rot));
  _assign(new TGeoCompositeShape("", inter), "", "intersection", true);
}

/// Constructor to be used when creating a new object
IntersectionSolid::IntersectionSolid(const Solid& shape1, const Solid& shape2, const Rotation3D& rot) {
  TGeoIntersection* inter = new TGeoIntersection(shape1, shape2, detail::matrix::_identity(), detail::matrix::_rotation3D(rot));
  _assign(new TGeoCompositeShape("", inter), "", "intersection", true);
}

#define INSTANTIATE(X) template class dd4hep::Solid_type<X>

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
INSTANTIATE(TGeoXtru);
INSTANTIATE(TGeoHype);
INSTANTIATE(TGeoTrap);
INSTANTIATE(TGeoTrd1);
INSTANTIATE(TGeoTrd2);
INSTANTIATE(TGeoCompositeShape);
