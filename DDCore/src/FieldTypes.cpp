// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DD4hep/FieldTypes.h"
#include <cmath>

using namespace std;
using namespace DD4hep::Geometry;

#ifndef INFINITY
#define INFINITY (numeric_limits<double>::max())
#endif

/// Call to access the field components at a given location
void ConstantField::fieldComponents(const double* /* pos */, double* field) {
  field[0] += direction.X();
  field[1] += direction.Y();
  field[2] += direction.Z();
}

/// Initializing constructor
SolenoidField::SolenoidField() 
  : innerField(0),  outerField(0), 
    minZ(-INFINITY), maxZ(INFINITY), 
    innerRadius(0), outerRadius(INFINITY)
{
  type = CartesianField::MAGNETIC; 
}

/// Call to access the field components at a given location
void SolenoidField::fieldComponents(const double* pos, double* field) {
  double radius = sqrt( pos[0] * pos[0] + pos[1] * pos[1] );
  if ( radius < innerRadius )
    field[2] += innerField;
  else if ( radius < outerRadius )
    field[2] += outerField;
}

/// Initializing constructor
DipoleField::DipoleField()
  : zmax(INFINITY), zmin(-INFINITY), rmax(INFINITY)
{
}

/// Call to access the field components at a given location
void DipoleField::fieldComponents(const double* pos, double* field) {
  double bx = 0, z = pos[2], r = sqrt(pos[0] * pos[0] + pos[1] * pos[1]);

  // Check if z coordinate is within dipole z bounds.
  if (z > zmin && z < zmax && r < rmax)    {
    // Apply all coefficients to this z coordinate.
    double pp = 1.0;
    double abs_z = fabs(z);
    bx = coefficents[0];
    for (size_t i=1; i<coefficents.size(); ++i) {
      pp *= abs_z;
      bx += coefficents[i] * abs_z;
    }
    // Flip sign for negative z.
    if (z < 0) bx = -bx;
    // Add Bx to the input field.
    field[0] += bx;
  }
}
