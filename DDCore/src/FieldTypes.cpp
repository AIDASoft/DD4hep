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

#include <DD4hep/FieldTypes.h>
#include <DD4hep/detail/Handle.inl>
#include <cmath>

using namespace dd4hep;

#ifndef INFINITY
#define INFINITY (numeric_limits<double>::max())
#endif

// fallthrough only exists from c++17
#if defined __has_cpp_attribute
#if __has_cpp_attribute(fallthrough)
#define ATTR_FALLTHROUGH [[fallthrough]]
#else
#define ATTR_FALLTHROUGH
#endif
#else
#define ATTR_FALLTHROUGH
#endif

DD4HEP_INSTANTIATE_HANDLE(ConstantField);
DD4HEP_INSTANTIATE_HANDLE(SolenoidField);
DD4HEP_INSTANTIATE_HANDLE(DipoleField);
DD4HEP_INSTANTIATE_HANDLE(MultipoleField);

/// Compute  the field components at a given location and add to given field
void ConstantField::fieldComponents(const double* /* pos */, double* field) {
  field[0] += direction.X();
  field[1] += direction.Y();
  field[2] += direction.Z();
}

/// Initializing constructor
SolenoidField::SolenoidField()
  : innerField(0), outerField(0), minZ(-INFINITY), maxZ(INFINITY), innerRadius(0), outerRadius(INFINITY)
{
  field_type = CartesianField::MAGNETIC;
}

/// Compute  the field components at a given location and add to given field
void SolenoidField::fieldComponents(const double* pos, double* field) {
  double z = pos[2] ;
  //  std::cout << " field z=" << z << " maxZ=" << maxZ << " minZ = " << minZ << std::endl ;
  if( z > minZ && z < maxZ ){
    double radius = std::sqrt(pos[0] * pos[0] + pos[1] * pos[1]);
    if (radius < innerRadius)
      field[2] += innerField;
    else if (radius < outerRadius)
      field[2] += outerField;
  }
}

/// Initializing constructor
DipoleField::DipoleField() : zmax(INFINITY), zmin(-INFINITY), rmax(INFINITY) {
  field_type = CartesianField::MAGNETIC;
}

/// Compute  the field components at a given location and add to given field
void DipoleField::fieldComponents(const double* pos, double* field) {
  double z = pos[2], r = std::sqrt(pos[0] * pos[0] + pos[1] * pos[1]);
  // Check if z coordinate is within dipole z bounds.
  if (z > zmin && z < zmax && r < rmax) {
    // Apply all coefficients to this z coordinate.
    double pp    = 1.0;
    double abs_z = fabs(z);
    double bx    = coefficents[0];
    for (size_t i = 1; i < coefficents.size(); ++i) {
      pp *= abs_z;
      bx += coefficents[i] * pp;
    }
    // Flip sign for negative z.
    if (z < 0)
      bx = -bx;
    // Add Bx to the input field.
    field[0] += bx;
  }
}

namespace   {
  constexpr static unsigned char FIELD_INITIALIZED   = 1<<0;
  constexpr static unsigned char FIELD_IDENTITY      = 1<<1;
  constexpr static unsigned char FIELD_ROTATION_ONLY = 1<<2;
  constexpr static unsigned char FIELD_POSITION_ONLY = 1<<3;
}

/// Initializing constructor
MultipoleField::MultipoleField()   {
  field_type = CartesianField::MAGNETIC;
}

/// Compute  the field components at a given location and add to given field
void MultipoleField::fieldComponents(const double* pos, double* field) {
  if ( 0 == flag )   {
    constexpr static double eps = 1e-10;
    double xx, xy, xz, dx, yx, yy, yz, dy, zx, zy, zz, dz;
    transform.GetComponents(xx, xy, xz, dx, yx, yy, yz, dy, zx, zy, zz, dz);
    flag |= FIELD_INITIALIZED;
    if ( (xx + yy + zz) < (3e0 - eps) )   {
      flag |= FIELD_ROTATION_ONLY;
    }
    else  {
      flag |= FIELD_POSITION_ONLY;
      if ( (std::abs(dx) + std::abs(dy) + std::abs(dz)) < eps )
	flag |= FIELD_IDENTITY;
    }
    this->inverse  = this->transform.Inverse();
    this->transform.GetRotation(this->rotation);
    this->transform.GetTranslation(this->translation);
  }
  Transform3D::Point p, p0(pos[0],pos[1],pos[2]);
  if      ( flag&FIELD_IDENTITY      ) p = std::move(p0);
  else if ( flag&FIELD_POSITION_ONLY ) p = p0 - this->translation;
  else      p = this->inverse * p0;

  double x = p.X(), y = p.Y(), z = p.Z();
  double coord[3] = {x, y, z};
  // Debug printout:
  //::printf("Pos: %+15.8e  %+15.8e  %+15.8e  Inverse: %+15.8e  %+15.8e  %+15.8e\n",
  //         pos[0]/dd4hep::cm,pos[1]/dd4hep::cm,pos[2]/dd4hep::cm, p.X()/dd4hep::cm, p.Y()/dd4hep::cm, p.Z()/dd4hep::cm);

  if ( 0 == volume.ptr() || volume->Contains(coord) )  {
    double bx = 0.0;
    double by = 0.0;
    double xy = x*y;
    double x2 = x*x;
    double y2 = y*y;
    switch(coefficents.size())  {
    case 4:      // Octupole momentum
      by += (1./6.) * ( coefficents[3] * (x2*x - 3.0*x*y2) + skews[3]*(y2*y - 3.0*x2*y) );
      bx += (1./6.) * ( coefficents[3] * (3.0*x2*y - y2*y) + skews[3]*(x2*x - 3.0*x*y2) );
      ATTR_FALLTHROUGH;
    case 3:      // Sextupole momentum:
      by +=  (1./2.) * ( coefficents[2] * (x2 - y2) - skews[2] * 2.0 * xy );
      bx +=  (1./2.) * ( coefficents[2] * 2.0 * xy + skews[2] * (x2 - y2) );
      ATTR_FALLTHROUGH;
    case 2:      // Quadrupole momentum:
      bx += coefficents[1] * y + skews[1]*x;
      by += coefficents[1] * x - skews[1]*y;
      ATTR_FALLTHROUGH;
    case 1:      // Dipole momentum:
      bx += skews[0];
      by += coefficents[0];
      ATTR_FALLTHROUGH;
    case 0:      // Nothing, but still valid
      break;
    default:     // Error condition
      throw std::runtime_error("Invalid multipole field definition!");
    }
    Transform3D::Point f = this->rotation * Transform3D::Point(bx, by, B_z);
    field[0] += f.X();
    field[1] += f.Y();
    field[2] += f.Z();
  }
}
