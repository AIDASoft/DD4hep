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
#include "DD4hep/detail/ShapesInterna.h"
#include "ShapeTags.h"

// C/C++ include files
#include <stdexcept>
#include <iomanip>
#include <sstream>

// ROOT includes
#include "TClass.h"
#include "TGeoMatrix.h"
#include "TGeoBoolNode.h"
#include "TGeoScaledShape.h"
#include "TGeoCompositeShape.h"

using namespace std;
using namespace dd4hep;
namespace units = dd4hep;

template <typename T> void Solid_type<T>::_setDimensions(double* param)   const {
  auto p = this->access(); // Ensure we have a valid handle!
  p->SetDimensions(param);
  p->ComputeBBox();
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

/// Access to shape name
template <typename T> const char* Solid_type<T>::title() const {
  return this->ptr()->GetTitle();
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

/// Access the dimensions of the shape: inverse of the setDimensions member function
template <typename T> vector<double> Solid_type<T>::dimensions()  {
  return move( get_shape_dimensions(this->access()) );
}

/// Set the shape dimensions. As for the TGeo shape, but angles in rad rather than degrees.
template <typename T> Solid_type<T>& Solid_type<T>::setDimensions(const vector<double>& params)  {
  set_shape_dimensions(this->access(), params);
  return *this;
}

/// Divide volume into subsections (See the ROOT manuloa for details)
template <typename T> TGeoVolume*
Solid_type<T>::divide(const Volume& voldiv, const string& divname,
                      int iaxis, int ndiv, double start, double step)   const {
  T* p = this->ptr();
  if ( p )  {
    VolumeMulti mv(p->Divide(voldiv.ptr(), divname.c_str(), iaxis, ndiv, start, step));
    return mv.ptr();
  }
  except("dd4hep","Volume: Attempt to divide an invalid logical volume.");
  return 0;
}

/// Constructor to create an anonymous new box object (retrieves name from volume)
ShapelessSolid::ShapelessSolid(const string& nam)  {
  _assign(new TGeoShapeAssembly(), nam, SHAPELESS_TAG, true);
}

void Box::make(const string& nam, double x_val, double y_val, double z_val)   {
  _assign(new TGeoBBox(nam.c_str(), x_val, y_val, z_val), "", BOX_TAG, true);
}

/// Set the box dimensionsy
Box& Box::setDimensions(double x_val, double y_val, double z_val)   {
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
void HalfSpace::make(const string& nam, const double* const point, const double* const normal)   {
  _assign(new TGeoHalfSpace(nam.c_str(),(Double_t*)point, (Double_t*)normal), "", HALFSPACE_TAG,true);
}

/// Constructor to be used when creating a new object
Polycone::Polycone(double startPhi, double deltaPhi) {
  _assign(new TGeoPcon(startPhi/units::deg, deltaPhi/units::deg, 0), "", POLYCONE_TAG, false);
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
  params.emplace_back(startPhi/units::deg);
  params.emplace_back(deltaPhi/units::deg);
  params.emplace_back(rmin.size());
  for (size_t i = 0; i < rmin.size(); ++i) {
    params.emplace_back(z[i] );
    params.emplace_back(rmin[i] );
    params.emplace_back(rmax[i] );
  }
  _assign(new TGeoPcon(&params[0]), "", POLYCONE_TAG, true);
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
  params.emplace_back(startPhi/units::deg);
  params.emplace_back(deltaPhi/units::deg);
  params.emplace_back(r.size());
  for (size_t i = 0; i < r.size(); ++i) {
    params.emplace_back(z[i] );
    params.emplace_back(0.0  );
    params.emplace_back(r[i] );
  }
  _assign(new TGeoPcon(&params[0]), "", POLYCONE_TAG, true);
}

/// Constructor to be used when creating a new object
Polycone::Polycone(const string& nam, double startPhi, double deltaPhi) {
  _assign(new TGeoPcon(nam.c_str(), startPhi/units::deg, deltaPhi/units::deg, 0), "", POLYCONE_TAG, false);
}

/// Constructor to be used when creating a new polycone object. Add at the same time all Z planes
Polycone::Polycone(const string& nam, double startPhi, double deltaPhi,
                   const vector<double>& rmin, const vector<double>& rmax, const vector<double>& z) {
  vector<double> params;
  if (rmin.size() < 2) {
    throw runtime_error("dd4hep: PolyCone Not enough Z planes. minimum is 2!");
  }
  if((z.size()!=rmin.size()) || (z.size()!=rmax.size()) )    {
    throw runtime_error("dd4hep: Polycone: vectors z,rmin,rmax not of same length");
  }
  params.emplace_back(startPhi/units::deg);
  params.emplace_back(deltaPhi/units::deg);
  params.emplace_back(rmin.size());
  for (size_t i = 0; i < rmin.size(); ++i) {
    params.emplace_back(z[i] );
    params.emplace_back(rmin[i] );
    params.emplace_back(rmax[i] );
  }
  _assign(new TGeoPcon(&params[0]), nam, POLYCONE_TAG, true);
}

/// Constructor to be used when creating a new polycone object. Add at the same time all Z planes
Polycone::Polycone(const string& nam, double startPhi, double deltaPhi, const vector<double>& r, const vector<double>& z) {
  vector<double> params;
  if (r.size() < 2) {
    throw runtime_error("dd4hep: PolyCone Not enough Z planes. minimum is 2!");
  }
  if((z.size()!=r.size()) )    {
    throw runtime_error("dd4hep: Polycone: vectors z,r not of same length");
  } 
  params.emplace_back(startPhi/units::deg);
  params.emplace_back(deltaPhi/units::deg);
  params.emplace_back(r.size());
  for (size_t i = 0; i < r.size(); ++i) {
    params.emplace_back(z[i] );
    params.emplace_back(0.0  );
    params.emplace_back(r[i] );
  }
  _assign(new TGeoPcon(&params[0]), nam, POLYCONE_TAG, true);
}

/// Add Z-planes to the Polycone
void Polycone::addZPlanes(const vector<double>& rmin, const vector<double>& rmax, const vector<double>& z) {
  TGeoPcon* sh = *this;
  vector<double> params;
  size_t num = sh->GetNz();
  if (rmin.size() < 2)   {
    except("PolyCone","++ addZPlanes: Not enough Z planes. minimum is 2!");
  }
  params.emplace_back(sh->GetPhi1());
  params.emplace_back(sh->GetDphi());
  params.emplace_back(num + rmin.size());
  for (size_t i = 0; i < num; ++i) {
    params.emplace_back(sh->GetZ(i));
    params.emplace_back(sh->GetRmin(i));
    params.emplace_back(sh->GetRmax(i));
  }
  for (size_t i = 0; i < rmin.size(); ++i) {
    params.emplace_back(z[i] );
    params.emplace_back(rmin[i] );
    params.emplace_back(rmax[i] );
  }
  _setDimensions(&params[0]);
}

/// Constructor to be used when creating a new cone segment object
ConeSegment::ConeSegment(double dz, 
                         double rmin1,     double rmax1,
                         double rmin2,     double rmax2,
                         double startPhi,  double endPhi)
{
  _assign(new TGeoConeSeg(dz, rmin1, rmax1, rmin2, rmax2,
                          startPhi/units::deg, endPhi/units::deg), "", CONESEGMENT_TAG, true);
}

/// Constructor to be used when creating a new cone segment object
ConeSegment::ConeSegment(const string& nam,
                         double dz, 
                         double rmin1,     double rmax1,
                         double rmin2,     double rmax2,
                         double startPhi,  double endPhi)
{
  _assign(new TGeoConeSeg(nam.c_str(), dz, rmin1, rmax1, rmin2, rmax2,
                          startPhi/units::deg, endPhi/units::deg), "", CONESEGMENT_TAG, true);
}

/// Set the cone segment dimensions
ConeSegment& ConeSegment::setDimensions(double dz, 
                                        double rmin1, double rmax1,
                                        double rmin2, double rmax2,
                                        double startPhi,  double endPhi) {
  double params[] = { dz, rmin1, rmax1, rmin2, rmax2, startPhi/units::deg, endPhi/units::deg };
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new object with attribute initialization
void Cone::make(const string& nam, double z, double rmin1, double rmax1, double rmin2, double rmax2) {
  _assign(new TGeoCone(nam.c_str(), z, rmin1, rmax1, rmin2, rmax2 ), "", CONE_TAG, true);
}

/// Set the box dimensions (startPhi=0.0, endPhi=2*pi)
Cone& Cone::setDimensions(double z, double rmin1, double rmax1, double rmin2, double rmax2) {
  double params[] = { z, rmin1, rmax1, rmin2, rmax2  };
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new object with attribute initialization
void Tube::make(const string& nam, double rmin, double rmax, double z, double startPhi, double endPhi) {
  _assign(new TGeoTubeSeg(nam.c_str(), rmin, rmax, z, startPhi/units::deg, endPhi/units::deg),nam,TUBE_TAG,true);
}

/// Set the tube dimensions
Tube& Tube::setDimensions(double rmin, double rmax, double z, double startPhi, double endPhi) {
  double params[] = {rmin,rmax,z,startPhi/units::deg,endPhi/units::deg};
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new object with attribute initialization
CutTube::CutTube(double rmin, double rmax, double dz, double startPhi, double endPhi,
                 double lx, double ly, double lz, double tx, double ty, double tz)  {
  make("", rmin,rmax,dz,startPhi/units::deg,endPhi/units::deg,lx,ly,lz,tx,ty,tz);
}

/// Constructor to be used when creating a new object with attribute initialization
CutTube::CutTube(const string& nam,
                 double rmin, double rmax, double dz, double startPhi, double endPhi,
                 double lx, double ly, double lz, double tx, double ty, double tz)  {
  make(nam, rmin,rmax,dz,startPhi/units::deg,endPhi/units::deg,lx,ly,lz,tx,ty,tz);
}

/// Constructor to be used when creating a new object with attribute initialization
void CutTube::make(const string& nam, double rmin, double rmax, double dz, double startPhi, double endPhi,
                   double lx, double ly, double lz, double tx, double ty, double tz)  {
  _assign(new TGeoCtub(nam.c_str(), rmin,rmax,dz,startPhi,endPhi,lx,ly,lz,tx,ty,tz),"",CUTTUBE_TAG,true);
}

/// Constructor to create a truncated tube object with attribute initialization
TruncatedTube::TruncatedTube(double zhalf, double rmin, double rmax, double startPhi, double deltaPhi,
                             double cutAtStart, double cutAtDelta, bool cutInside)
{  make("", zhalf, rmin, rmax, startPhi/units::deg, deltaPhi/units::deg, cutAtStart, cutAtDelta, cutInside);    }

/// Constructor to create a truncated tube object with attribute initialization
TruncatedTube::TruncatedTube(const string& nam,
                             double zhalf, double rmin, double rmax, double startPhi, double deltaPhi,
                             double cutAtStart, double cutAtDelta, bool cutInside)
{  make(nam, zhalf, rmin, rmax, startPhi/units::deg, deltaPhi/units::deg, cutAtStart, cutAtDelta, cutInside);    }

/// Internal helper method to support object construction
void TruncatedTube::make(const string& nam,
                         double zhalf, double rmin, double rmax, double startPhi, double deltaPhi,
                         double cutAtStart, double cutAtDelta, bool cutInside)   {
  // check the parameters
  if( rmin <= 0 || rmax <= 0 || cutAtStart <= 0 || cutAtDelta <= 0 )
    except(TRUNCATEDTUBE_TAG,"++ 0 <= rIn,cutAtStart,rOut,cutAtDelta,rOut violated!");
  else if( rmin >= rmax )
    except(TRUNCATEDTUBE_TAG,"++ rIn<rOut violated!");
  else if( startPhi != 0. )
    except(TRUNCATEDTUBE_TAG,"++ startPhi != 0 not supported!");

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
  rot.RotateZ( -alpha/dd4hep::deg );
  TGeoTranslation trans(xBox, 0., 0.);  
  TGeoBBox* box  = new TGeoBBox((nam+"Box").c_str(), boxX, boxY, boxZ);
  TGeoTubeSeg* tubs = new TGeoTubeSeg((nam+"Tubs").c_str(), rmin, rmax, zhalf, startPhi, deltaPhi);
  TGeoCombiTrans* combi = new TGeoCombiTrans(trans, rot);
  TGeoSubtraction* sub  = new TGeoSubtraction(tubs, box, nullptr, combi);
  _assign(new TGeoCompositeShape(nam.c_str(), sub),"",TRUNCATEDTUBE_TAG,true);
  stringstream params;
  params << zhalf               << " " << endl
         << rmin                << " " << endl
         << rmax                << " " << endl
         << startPhi*units::deg << " " << endl
         << deltaPhi*units::deg << " " << endl
         << cutAtStart          << " " << endl
         << cutAtDelta          << " " << endl
         << char(cutInside ? '1' : '0') << endl;
  combi->SetTitle(params.str().c_str());
  //cout << "Params: " << params.str() << endl;
#if 0
  params << TRUNCATEDTUBE_TAG << ":" << endl
         << "\t zhalf:       " << zhalf << " " << endl
         << "\t rmin:        " << rmin << " " << endl
         << "\t rmax:        " << rmax << " " << endl
         << "\t startPhi:    " << startPhi << " " << endl
         << "\t deltaPhi:    " << deltaPhi << " " << endl
         << "\t r/cutAtStart:" << cutAtStart << " " << endl
         << "\t R/cutAtDelta:" << cutAtDelta << " " << endl
         << "\t cutInside:   " << char(cutInside ? '1' : '0') << endl
         << "\t\t alpha:     " << alpha << endl
         << "\t\t sin_alpha: " << sin_alpha << endl
         << "\t\t boxY:      " << boxY << endl
         << "\t\t xBox:      " << xBox << endl;
#endif
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
void EllipticalTube::make(const string& nam, double a, double b, double dz) {
  _assign(new TGeoEltu(nam.c_str(), a, b, dz), "", ELLIPTICALTUBE_TAG, true);
}

/// Internal helper method to support TwistedTube object construction
void TwistedTube::make(const std::string& nam, double twist_angle, double rmin, double rmax,
                       double zneg, double zpos, int nsegments, double totphi)   {
  _assign(new TwistedTubeObject(nam.c_str(), twist_angle, rmin, rmax, zneg, zpos, nsegments, totphi/units::deg),
          "", TWISTEDTUBE_TAG, true);
}

/// Constructor to be used when creating a new object with attribute initialization
void Trd1::make(const string& nam, double x1, double x2, double y, double z) {
  _assign(new TGeoTrd1(nam.c_str(), x1, x2, y, z ), "", TRD1_TAG, true);
}

/// Set the Trd1 dimensions
Trd1& Trd1::setDimensions(double x1, double x2, double y, double z) {
  double params[] = { x1, x2, y, z  };
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new object with attribute initialization
void Trd2::make(const string& nam, double x1, double x2, double y1, double y2, double z) {
  _assign(new TGeoTrd2(nam.c_str(), x1, x2, y1, y2, z ), "", TRD2_TAG, true);
}

/// Set the Trd2 dimensions
Trd2& Trd2::setDimensions(double x1, double x2, double y1, double y2, double z) {
  double params[] = { x1, x2, y1, y2, z  };
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new object with attribute initialization
void Paraboloid::make(const string& nam, double r_low, double r_high, double delta_z) {
  _assign(new TGeoParaboloid(nam.c_str(), r_low, r_high, delta_z ), "", PARABOLOID_TAG, true);
}

/// Set the Paraboloid dimensions
Paraboloid& Paraboloid::setDimensions(double r_low, double r_high, double delta_z) {
  double params[] = { r_low, r_high, delta_z  };
  _setDimensions(params);
  return *this;
}

/// Constructor to create a new anonymous object with attribute initialization
void Hyperboloid::make(const string& nam, double rin, double stin, double rout, double stout, double dz) {
  _assign(new TGeoHype(nam.c_str(), rin, stin/units::deg, rout, stout/units::deg, dz), "", HYPERBOLOID_TAG, true);
}

/// Set the Hyperboloid dimensions
Hyperboloid& Hyperboloid::setDimensions(double rin, double stin, double rout, double stout, double dz)  {
  double params[] = { rin, stin/units::deg, rout, stout/units::deg, dz};
  _setDimensions(params);
  return *this;
}

/// Constructor function to be used when creating a new object with attribute initialization
void Sphere::make(const string& nam, double rmin, double rmax, double startTheta, double endTheta, double startPhi, double endPhi) {
  _assign(new TGeoSphere(nam.c_str(), rmin, rmax,
                         startTheta/units::deg, endTheta/units::deg,
                         startPhi/units::deg,   endPhi/units::deg), "", SPHERE_TAG, true);
}

/// Set the Sphere dimensions
Sphere& Sphere::setDimensions(double rmin, double rmax, double startTheta, double endTheta, double startPhi, double endPhi) {
  double params[] = { rmin, rmax, startTheta/units::deg, endTheta/units::deg, startPhi/units::deg, endPhi/units::deg };
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new object with attribute initialization
void Torus::make(const string& nam, double r, double rmin, double rmax, double startPhi, double deltaPhi) {
  _assign(new TGeoTorus(nam.c_str(), r, rmin, rmax, startPhi/units::deg, deltaPhi/units::deg), "", TORUS_TAG, true);
}

/// Set the Torus dimensions
Torus& Torus::setDimensions(double r, double rmin, double rmax, double startPhi, double deltaPhi) {
  double params[] = { r, rmin, rmax, startPhi/units::deg, deltaPhi/units::deg };
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new anonymous object with attribute initialization
Trap::Trap(double z, double theta, double phi,
           double h1, double bl1, double tl1, double alpha1,
           double h2, double bl2, double tl2, double alpha2) {
  _assign(new TGeoTrap(z, theta/units::deg, phi/units::deg,
                       h1, bl1, tl1, alpha1/units::deg,
                       h2, bl2, tl2, alpha2/units::deg), "", TRAP_TAG, true);
}

/// Constructor to be used when creating a new anonymous object with attribute initialization
Trap::Trap(const string& nam,
           double z, double theta, double phi,
           double h1, double bl1, double tl1, double alpha1,
           double h2, double bl2, double tl2, double alpha2) {
  _assign(new TGeoTrap(nam.c_str(), z, theta/units::deg, phi/units::deg,
                       h1, bl1, tl1, alpha1/units::deg,
                       h2, bl2, tl2, alpha2/units::deg), "", TRAP_TAG, true);
}

/// Constructor to be used when creating a new anonymous object with attribute initialization
void Trap::make(const string& nam, double pz, double py, double px, double pLTX) {
  double z      = pz / 2e0;
  double theta  = 0e0;
  double phi    = 0e0;
  double h      = py / 2e0;
  double bl     = px / 2e0;
  double tl     = pLTX / 2e0;
  double alpha1 = (pLTX - px) / py;
  _assign(new TGeoTrap(nam.c_str(), z, theta, phi,
                       h, bl, tl, alpha1/units::deg,
                       h, bl, tl, alpha1/units::deg), "", TRAP_TAG, true);
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
void PseudoTrap::make(const string& nam, double x1, double x2, double y1, double y2, double z, double r, bool atMinusZ)    {
  double x            = atMinusZ ? x1 : x2;
  double h            = 0;
  bool   intersec     = false; // union or intersection solid
  double displacement = 0;
  double startPhi     = 0;
  double halfZ        = z;
  double halfOpeningAngle = std::asin( x / std::abs( r ))/units::deg;

  /// calculate the displacement of the tubs w.r.t. to the trap, determine the opening angle of the tubs
  double delta        = std::sqrt( r * r - x * x );
 
#if 0
  // Implementation from : (Crappy)
  // https://cmssdt.cern.ch/lxr/source/SimG4Core/Geometry/src/DDG4SolidConverter.cc#0362
  if( r < 0 && std::abs(r) >= x )  {
    intersec = true;       // intersection solid
    h = y1 < y2 ? y2 : y1; // tubs half height
    h += h/20.;            // enlarge a bit - for subtraction solid
    if( atMinusZ )    {
      displacement = -halfZ - delta; 
      startPhi     =  270.0 - halfOpeningAngle;
    }
    else    {
      displacement =  halfZ + delta;
      startPhi     =  90.0  - halfOpeningAngle;
    }
  }
  else if( r > 0 && std::abs(r) >= x )  {
    if( atMinusZ )    {
      displacement = -halfZ + delta;
      startPhi     =  90.0  - halfOpeningAngle;
      h = y1;
    }
    else
    {
      displacement =  halfZ - delta; 
      startPhi     =  270.0 - halfOpeningAngle;
      h = y2;
    }    
  }
  else  {
    except(PSEUDOTRAP_TAG,"Check parameters of the PseudoTrap!");   
  }
#endif

  // Implementation from :
  // https://cmssdt.cern.ch/lxr/source/Fireworks/Geometry/src/TGeoMgrFromDdd.cc#0538
  if( r < 0 && std::abs(r) >= x )  {
    intersec = true;       // intersection solid
    h = y1 < y2 ? y2 : y1; // tubs half height
    h += h/20.;            // enlarge a bit - for subtraction solid
    if( atMinusZ )    {
      displacement = - halfZ - delta; 
      startPhi     =  90.0 - halfOpeningAngle;
    }
    else    {
      displacement =   halfZ + delta;
      startPhi     = -90.0 - halfOpeningAngle;
    }
  }
  else if( r > 0 && std::abs(r) >= x )  {
    if( atMinusZ )    {
      displacement = - halfZ + delta;
      startPhi     = 270.0 - halfOpeningAngle;
      h = y1;
    }
    else
    {
      displacement =   halfZ - delta; 
      startPhi     =  90.0 - halfOpeningAngle;
      h = y2;
    }    
  }
  else  {
    except(PSEUDOTRAP_TAG,"Check parameters of the PseudoTrap!");   
  }

  Solid trap(new TGeoTrd2((nam+"Trd2").c_str(), x1, x2, y1, y2, halfZ));
  Solid tubs(new TGeoTubeSeg((nam+"Tubs").c_str(), 0.,std::abs(r),h,startPhi,startPhi + halfOpeningAngle*2.));
  TGeoCompositeShape* solid = 0;
  if( intersec )  {
    solid = SubtractionSolid(nam, trap, tubs, Transform3D(RotationX(M_PI/2.), Position(0.,0.,displacement))).ptr();
  }
  else  {
    SubtractionSolid sub((nam+"Subs").c_str(), tubs, Box(1.1*x, 1.1*h, std::sqrt(r*r-x*x)), Transform3D(RotationX(M_PI/2.)));
    solid = UnionSolid(nam, trap, sub, Transform3D(RotationX(M_PI/2.), Position(0,0,displacement))).ptr();
  }
  stringstream params;
  params << x1 << " " << x2 << " " << y1 << " " << y2 << " " << z << " "
         << r << " " << char(atMinusZ ? '1' : '0') << " ";
  solid->GetBoolNode()->GetRightMatrix()->SetTitle(params.str().c_str());
  _assign(solid,"",PSEUDOTRAP_TAG, true);
}

/// Helper function to create poly hedron
void PolyhedraRegular::make(const string& nam, int nsides, double rmin, double rmax,
                            double zpos, double zneg, double start, double delta) {
  if (rmin < 0e0 || rmin > rmax)
    throw runtime_error("dd4hep: PolyhedraRegular: Illegal argument rmin:<" + _toString(rmin) + "> is invalid!");
  else if (rmax < 0e0)
    throw runtime_error("dd4hep: PolyhedraRegular: Illegal argument rmax:<" + _toString(rmax) + "> is invalid!");
  double params[] = { start/units::deg, delta/units::deg, double(nsides), 2e0, zpos, rmin, rmax, zneg, rmin, rmax };
  _assign(new TGeoPgon(params), nam, POLYHEDRA_TAG, false);
  //_setDimensions(&params[0]);
}

/// Helper function to create poly hedron
void Polyhedra::make(const string& nam, int nsides, double start, double delta,
                     const vector<double>& z, const vector<double>& rmin, const vector<double>& rmax)  {
  vector<double> temp;
  if ( rmin.size() != z.size() || rmax.size() != z.size() )  {
    except("Polyhedra",
           "Number of values to define zplanes are incorrect: z:%ld rmin:%ld rmax:%ld",
           z.size(), rmin.size(), rmax.size());
  }
  // No need to transform coordinates to cm. We are in the dd4hep world: all is already in cm.
  temp.reserve(4+z.size()*2);
  temp.emplace_back(start/units::deg);
  temp.emplace_back(delta/units::deg);
  temp.emplace_back(double(nsides));
  temp.emplace_back(double(z.size()));
  for(size_t i=0; i<z.size(); ++i)   {
    temp.emplace_back(z[i]);
    temp.emplace_back(rmin[i]);
    temp.emplace_back(rmax[i]);
  }
  _assign(new TGeoPgon(&temp[0]), nam, POLYHEDRA_TAG, false);
}

/// Helper function to create the polyhedron
void ExtrudedPolygon::make(const string& nam,
                           const vector<double>& pt_x,
                           const vector<double>& pt_y,
                           const vector<double>& sec_z,
                           const vector<double>& sec_x,
                           const vector<double>& sec_y,
                           const vector<double>& sec_scale)
{
  TGeoXtru* solid = new TGeoXtru(sec_z.size());
  _assign(solid, nam, EXTRUDEDPOLYGON_TAG, false);
  // No need to transform coordinates to cm. We are in the dd4hep world: all is already in cm.
  solid->DefinePolygon(pt_x.size(), &(*pt_x.begin()), &(*pt_y.begin()));
  for( size_t i = 0; i < sec_z.size(); ++i )
    solid->DefineSection(i, sec_z[i], sec_x[i], sec_y[i], sec_scale[i]);
}

/// Creator method
void EightPointSolid::make(const string& nam, double dz, const double* vtx)   {
  _assign(new TGeoArb8(nam.c_str(), dz, (double*)vtx), "", EIGHTPOINTSOLID_TAG, true);
}

/// Constructor to be used when creating a new object. Position is identity, Rotation is the identity rotation
SubtractionSolid::SubtractionSolid(const Solid& shape1, const Solid& shape2) {
  TGeoSubtraction* sub = new TGeoSubtraction(shape1, shape2, detail::matrix::_identity(), detail::matrix::_identity());
  _assign(new TGeoCompositeShape("", sub), "", SUBTRACTION_TAG, true);
}

/// Constructor to be used when creating a new object. Placement by a generic transformation within the mother
SubtractionSolid::SubtractionSolid(const Solid& shape1, const Solid& shape2, const Transform3D& trans) {
  TGeoSubtraction* sub = new TGeoSubtraction(shape1, shape2, detail::matrix::_identity(), detail::matrix::_transform(trans));
  _assign(new TGeoCompositeShape("", sub), "", SUBTRACTION_TAG, true);
}

/// Constructor to be used when creating a new object. Rotation is the identity rotation
SubtractionSolid::SubtractionSolid(const Solid& shape1, const Solid& shape2, const Position& pos) {
  TGeoSubtraction* sub = new TGeoSubtraction(shape1, shape2, detail::matrix::_identity(), detail::matrix::_translation(pos));
  _assign(new TGeoCompositeShape("", sub), "", SUBTRACTION_TAG, true);
}

/// Constructor to be used when creating a new object
SubtractionSolid::SubtractionSolid(const Solid& shape1, const Solid& shape2, const RotationZYX& rot) {
  TGeoSubtraction* sub = new TGeoSubtraction(shape1, shape2, detail::matrix::_identity(), detail::matrix::_rotationZYX(rot));
  _assign(new TGeoCompositeShape("", sub), "", SUBTRACTION_TAG, true);
}

/// Constructor to be used when creating a new object
SubtractionSolid::SubtractionSolid(const Solid& shape1, const Solid& shape2, const Rotation3D& rot) {
  TGeoSubtraction* sub = new TGeoSubtraction(shape1, shape2, detail::matrix::_identity(), detail::matrix::_rotation3D(rot));
  _assign(new TGeoCompositeShape("", sub), "", SUBTRACTION_TAG, true);
}

/// Constructor to be used when creating a new object. Position is identity, Rotation is the identity rotation
SubtractionSolid::SubtractionSolid(const string& nam, const Solid& shape1, const Solid& shape2) {
  TGeoSubtraction* sub = new TGeoSubtraction(shape1, shape2, detail::matrix::_identity(), detail::matrix::_identity());
  _assign(new TGeoCompositeShape(nam.c_str(), sub), "", SUBTRACTION_TAG, true);
}

/// Constructor to be used when creating a new object. Placement by a generic transformation within the mother
SubtractionSolid::SubtractionSolid(const string& nam, const Solid& shape1, const Solid& shape2, const Transform3D& trans) {
  TGeoSubtraction* sub = new TGeoSubtraction(shape1, shape2, detail::matrix::_identity(), detail::matrix::_transform(trans));
  _assign(new TGeoCompositeShape(nam.c_str(), sub), "", SUBTRACTION_TAG, true);
}

/// Constructor to be used when creating a new object. Rotation is the identity rotation
SubtractionSolid::SubtractionSolid(const string& nam, const Solid& shape1, const Solid& shape2, const Position& pos) {
  TGeoSubtraction* sub = new TGeoSubtraction(shape1, shape2, detail::matrix::_identity(), detail::matrix::_translation(pos));
  _assign(new TGeoCompositeShape(nam.c_str(), sub), "", SUBTRACTION_TAG, true);
}

/// Constructor to be used when creating a new object
SubtractionSolid::SubtractionSolid(const string& nam, const Solid& shape1, const Solid& shape2, const RotationZYX& rot) {
  TGeoSubtraction* sub = new TGeoSubtraction(shape1, shape2, detail::matrix::_identity(), detail::matrix::_rotationZYX(rot));
  _assign(new TGeoCompositeShape(nam.c_str(), sub), "", SUBTRACTION_TAG, true);
}

/// Constructor to be used when creating a new object
SubtractionSolid::SubtractionSolid(const string& nam, const Solid& shape1, const Solid& shape2, const Rotation3D& rot) {
  TGeoSubtraction* sub = new TGeoSubtraction(shape1, shape2, detail::matrix::_identity(), detail::matrix::_rotation3D(rot));
  _assign(new TGeoCompositeShape(nam.c_str(), sub), "", SUBTRACTION_TAG, true);
}

/// Constructor to be used when creating a new object. Position is identity, Rotation is identity rotation
UnionSolid::UnionSolid(const Solid& shape1, const Solid& shape2) {
  TGeoUnion* uni = new TGeoUnion(shape1, shape2, detail::matrix::_identity(), detail::matrix::_identity());
  _assign(new TGeoCompositeShape("", uni), "", UNION_TAG, true);
}

/// Constructor to be used when creating a new object. Placement by a generic transformation within the mother
UnionSolid::UnionSolid(const Solid& shape1, const Solid& shape2, const Transform3D& trans) {
  TGeoUnion* uni = new TGeoUnion(shape1, shape2, detail::matrix::_identity(), detail::matrix::_transform(trans));
  _assign(new TGeoCompositeShape("", uni), "", UNION_TAG, true);
}

/// Constructor to be used when creating a new object. Rotation is identity rotation
UnionSolid::UnionSolid(const Solid& shape1, const Solid& shape2, const Position& pos) {
  TGeoUnion* uni = new TGeoUnion(shape1, shape2, detail::matrix::_identity(), detail::matrix::_translation(pos));
  _assign(new TGeoCompositeShape("", uni), "", UNION_TAG, true);
}

/// Constructor to be used when creating a new object
UnionSolid::UnionSolid(const Solid& shape1, const Solid& shape2, const RotationZYX& rot) {
  TGeoUnion *uni = new TGeoUnion(shape1, shape2, detail::matrix::_identity(), detail::matrix::_rotationZYX(rot));
  _assign(new TGeoCompositeShape("", uni), "", UNION_TAG, true);
}

/// Constructor to be used when creating a new object
UnionSolid::UnionSolid(const Solid& shape1, const Solid& shape2, const Rotation3D& rot) {
  TGeoUnion *uni = new TGeoUnion(shape1, shape2, detail::matrix::_identity(), detail::matrix::_rotation3D(rot));
  _assign(new TGeoCompositeShape("", uni), "", UNION_TAG, true);
}

/// Constructor to be used when creating a new object. Position is identity, Rotation is identity rotation
UnionSolid::UnionSolid(const string& nam, const Solid& shape1, const Solid& shape2) {
  TGeoUnion* uni = new TGeoUnion(shape1, shape2, detail::matrix::_identity(), detail::matrix::_identity());
  _assign(new TGeoCompositeShape(nam.c_str(), uni), "", UNION_TAG, true);
}

/// Constructor to be used when creating a new object. Placement by a generic transformation within the mother
UnionSolid::UnionSolid(const string& nam, const Solid& shape1, const Solid& shape2, const Transform3D& trans) {
  TGeoUnion* uni = new TGeoUnion(shape1, shape2, detail::matrix::_identity(), detail::matrix::_transform(trans));
  _assign(new TGeoCompositeShape(nam.c_str(), uni), "", UNION_TAG, true);
}

/// Constructor to be used when creating a new object. Rotation is identity rotation
UnionSolid::UnionSolid(const string& nam, const Solid& shape1, const Solid& shape2, const Position& pos) {
  TGeoUnion* uni = new TGeoUnion(shape1, shape2, detail::matrix::_identity(), detail::matrix::_translation(pos));
  _assign(new TGeoCompositeShape(nam.c_str(), uni), "", UNION_TAG, true);
}

/// Constructor to be used when creating a new object
UnionSolid::UnionSolid(const string& nam, const Solid& shape1, const Solid& shape2, const RotationZYX& rot) {
  TGeoUnion *uni = new TGeoUnion(shape1, shape2, detail::matrix::_identity(), detail::matrix::_rotationZYX(rot));
  _assign(new TGeoCompositeShape(nam.c_str(), uni), "", UNION_TAG, true);
}

/// Constructor to be used when creating a new object
UnionSolid::UnionSolid(const string& nam, const Solid& shape1, const Solid& shape2, const Rotation3D& rot) {
  TGeoUnion *uni = new TGeoUnion(shape1, shape2, detail::matrix::_identity(), detail::matrix::_rotation3D(rot));
  _assign(new TGeoCompositeShape(nam.c_str(), uni), "", UNION_TAG, true);
}

/// Constructor to be used when creating a new object. Position is identity, Rotation is identity rotation
IntersectionSolid::IntersectionSolid(const Solid& shape1, const Solid& shape2) {
  TGeoIntersection* inter = new TGeoIntersection(shape1, shape2, detail::matrix::_identity(), detail::matrix::_identity());
  _assign(new TGeoCompositeShape("", inter), "", INTERSECTION_TAG, true);
}

/// Constructor to be used when creating a new object. Placement by a generic transformation within the mother
IntersectionSolid::IntersectionSolid(const Solid& shape1, const Solid& shape2, const Transform3D& trans) {
  TGeoIntersection* inter = new TGeoIntersection(shape1, shape2, detail::matrix::_identity(), detail::matrix::_transform(trans));
  _assign(new TGeoCompositeShape("", inter), "", INTERSECTION_TAG, true);
}

/// Constructor to be used when creating a new object. Position is identity.
IntersectionSolid::IntersectionSolid(const Solid& shape1, const Solid& shape2, const Position& pos) {
  TGeoIntersection* inter = new TGeoIntersection(shape1, shape2, detail::matrix::_identity(), detail::matrix::_translation(pos));
  _assign(new TGeoCompositeShape("", inter), "", INTERSECTION_TAG, true);
}

/// Constructor to be used when creating a new object
IntersectionSolid::IntersectionSolid(const Solid& shape1, const Solid& shape2, const RotationZYX& rot) {
  TGeoIntersection* inter = new TGeoIntersection(shape1, shape2, detail::matrix::_identity(), detail::matrix::_rotationZYX(rot));
  _assign(new TGeoCompositeShape("", inter), "", INTERSECTION_TAG, true);
}

/// Constructor to be used when creating a new object
IntersectionSolid::IntersectionSolid(const Solid& shape1, const Solid& shape2, const Rotation3D& rot) {
  TGeoIntersection* inter = new TGeoIntersection(shape1, shape2, detail::matrix::_identity(), detail::matrix::_rotation3D(rot));
  _assign(new TGeoCompositeShape("", inter), "", INTERSECTION_TAG, true);
}

/// Constructor to be used when creating a new object. Position is identity, Rotation is identity rotation
IntersectionSolid::IntersectionSolid(const string& nam, const Solid& shape1, const Solid& shape2) {
  TGeoIntersection* inter = new TGeoIntersection(shape1, shape2, detail::matrix::_identity(), detail::matrix::_identity());
  _assign(new TGeoCompositeShape(nam.c_str(), inter), "", INTERSECTION_TAG, true);
}

/// Constructor to be used when creating a new object. Placement by a generic transformation within the mother
IntersectionSolid::IntersectionSolid(const string& nam, const Solid& shape1, const Solid& shape2, const Transform3D& trans) {
  TGeoIntersection* inter = new TGeoIntersection(shape1, shape2, detail::matrix::_identity(), detail::matrix::_transform(trans));
  _assign(new TGeoCompositeShape(nam.c_str(), inter), "", INTERSECTION_TAG, true);
}

/// Constructor to be used when creating a new object. Position is identity.
IntersectionSolid::IntersectionSolid(const string& nam, const Solid& shape1, const Solid& shape2, const Position& pos) {
  TGeoIntersection* inter = new TGeoIntersection(shape1, shape2, detail::matrix::_identity(), detail::matrix::_translation(pos));
  _assign(new TGeoCompositeShape(nam.c_str(), inter), "", INTERSECTION_TAG, true);
}

/// Constructor to be used when creating a new object
IntersectionSolid::IntersectionSolid(const string& nam, const Solid& shape1, const Solid& shape2, const RotationZYX& rot) {
  TGeoIntersection* inter = new TGeoIntersection(shape1, shape2, detail::matrix::_identity(), detail::matrix::_rotationZYX(rot));
  _assign(new TGeoCompositeShape(nam.c_str(), inter), "", INTERSECTION_TAG, true);
}

/// Constructor to be used when creating a new object
IntersectionSolid::IntersectionSolid(const string& nam, const Solid& shape1, const Solid& shape2, const Rotation3D& rot) {
  TGeoIntersection* inter = new TGeoIntersection(shape1, shape2, detail::matrix::_identity(), detail::matrix::_rotation3D(rot));
  _assign(new TGeoCompositeShape(nam.c_str(), inter), "", INTERSECTION_TAG, true);
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
INSTANTIATE(TGeoGtra);
INSTANTIATE(TGeoHype);
INSTANTIATE(TGeoTrap);
INSTANTIATE(TGeoTrd1);
INSTANTIATE(TGeoTrd2);
INSTANTIATE(TGeoCompositeShape);
