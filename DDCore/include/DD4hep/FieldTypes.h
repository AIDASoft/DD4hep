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
#ifndef DD4HEP_FIELDTYPES_H
#define DD4HEP_FIELDTYPES_H

// Framework include files
#include "DD4hep/Fields.h"
#include "DD4hep/Shapes.h"
#include <vector>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Implementation object of a field with constant strength
  /**
   *  Generic constant uniform field
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CORE
   */
  class ConstantField : public CartesianField::Object {
  public:
    /// Field direction
    Direction direction;
  public:
    /// Initializing constructor
    ConstantField() = default;
    /// Call to access the field components at a given location
    virtual void fieldComponents(const double* /* pos */, double* field);
  };

  /// Implementation object of a solenoidal magnetic field.
  /**
   *  Generic solonoid magnetic field
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CORE
   */
  class SolenoidField : public CartesianField::Object {
  public:
    double innerField;
    double outerField;
    double minZ;
    double maxZ;
    double innerRadius;
    double outerRadius;

  public:
    /// Initializing constructor
    SolenoidField();
    /// Call to access the field components at a given location
    virtual void fieldComponents(const double* pos, double* field);
  };

  /// Implementation object of a dipole magnetic field.
  /**
   *  Generic dipole magnetic field
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CORE
   */
  class DipoleField : public CartesianField::Object {
  public:
    double zmax;
    double zmin;
    double rmax;
    Coefficents coefficents;
  public:
    /// Initializing constructor
    DipoleField();
    /// Call to access the field components at a given location
    virtual void fieldComponents(const double* pos, double* field);
  };

  /// Implementation object of a Multipole magnetic field.
  /**
   *  Generic multipole magnetic field using the
   *  Multipole approach. The dipole is assumed to be horizontal
   *  as it is used for bending beams in large colliders
   *  ie. the dipole field lines are vertical.
   *
   *  The different momenta are given by:
   *
   *  \f{eqnarray*}{
   *  B_y + i*B_x         &=& ( 1/(n-1)! )* C_n * (x + iy)^{n-1}                        \\
   *  B_sum = B_y + i B_x &=& Sum_{n=1..4} ( 1/(n-1)! )*(b_n + ia_n) (x + iy)^{n-1}    \\
   *  \f}
   *  With C_n being the complex multipole coefficients and
   *  b_n the "normal multipole coefficients" and a_n the "skew multipole coefficients".
   *
   *  The maximal momentum used is the octopole momentum.
   *
   *  The lower momenta are:
   *
   *  \li Dipole (n=1):
   *  \f{eqnarray*}{
   *  B_y &=& b_1                                       \\
   *  B_x &=& a_1                                       \\
   *  B_z &=& constant                                  \\
   *  \f}
   *
   *  \li Quadrupole (n=2):
   *
   *  \f{eqnarray*}{
   *  B_y &=& b_2 x - a_2 y                             \\
   *  B_x &=& b_2 y + a_2 x                             \\
   *  \f}
   *
   *  \li Sextupole (n=3):
   *
   *  \f{eqnarray*}{
   *  B_y + i B_x &=& (1/2) * (b_3 +ia_3) (x^2 + 2ixy - y^2)    \\
   *  B_y         &=& (1/2) * ( b_3 x^2 - b_3 y^2 - 2 a_3 xy)      \\
   *  B_x         &=& (1/2) * (a_3 x^2 - a_3 y^2 + 2 b_3 xy)      \\
   *  \f}
   *
   *  \li Octopole (n=4):
   *
   *  \f{eqnarray*}{
   *  B_y + i B_x &=& (1/6) * (b_4 +ia_4) (x^3 + 3ix^2y - 3xy^2 -iy^3)  \\
   *  B_y &=& (1/6) * (b_4 x^3 - 3 b_4 x y^2 - 3 a_4 x^2 y + a_4 y^3)     \\
   *  B_x &=& (1/6) * (3 b_4 x^2 y - b_4 y^3 + a_4 x^3 - 3 a_4 x y^2)     \	\
   *  \f}
   *
   *  The defined field components only apply within the shape 'volume'.
   *  If 'volume' is an invalid shape (ie. not defined), then the field
   *  components are valied throughout the 'universe'.
   *
   *  \see http://cas.web.cern.ch/sites/cas.web.cern.ch/files/lectures/bruges-2009/wolski-1.pdf
   *  \see http://cas.web.cern.ch/sites/cas.web.cern.ch/files/lectures/varna-2010/brandt-1-web.pdf
   *  \see https://en.wikipedia.org/wiki/Multipole_magnet
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CORE
   */
  class MultipoleField : public CartesianField::Object {
  public:
    Coefficents  coefficents;
    Coefficents  skews;
    Solid volume;
    Transform3D  transform;
    double       B_z;

  public:
    /// Initializing constructor
    MultipoleField();
    /// Call to access the field components at a given location
    virtual void fieldComponents(const double* pos, double* field);
  };

}         /* End namespace dd4hep             */
#endif // DD4HEP_FIELDTYPES_H
