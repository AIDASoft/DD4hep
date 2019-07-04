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

// Framework include files
#include "DD4hep/MatrixHelpers.h"
#include "DD4hep/DD4hepUnits.h"

#ifdef HAVE_GEANT4_UNITS
#define MM_2_CM 1.0
#else
#define MM_2_CM 0.1
#endif

// ROOT includes
#include "TGeoMatrix.h"

TGeoIdentity* dd4hep::detail::matrix::_identity() {
  return gGeoIdentity;
}

TGeoTranslation* dd4hep::detail::matrix::_translation(const Position& pos) {
  return new TGeoTranslation("", pos.X(), pos.Y(), pos.Z());
}

TGeoRotation* dd4hep::detail::matrix::_rotationZYX(const RotationZYX& rot) {
  return new TGeoRotation("", rot.Phi() * RAD_2_DEGREE, rot.Theta() * RAD_2_DEGREE, rot.Psi() * RAD_2_DEGREE);
}

TGeoRotation* dd4hep::detail::matrix::_rotation3D(const Rotation3D& rot3D) {
  EulerAngles rot(rot3D);
  return new TGeoRotation("", rot.Phi() * RAD_2_DEGREE, rot.Theta() * RAD_2_DEGREE, rot.Psi() * RAD_2_DEGREE);
}

/// Set a RotationZYX object to a TGeoHMatrix            \ingroup DD4HEP \ingroup DD4HEP_CORE
TGeoHMatrix& dd4hep::detail::matrix::_transform(TGeoHMatrix& tr, const RotationZYX& rot)   {
  tr.RotateZ(rot.Phi()   * RAD_2_DEGREE);
  tr.RotateY(rot.Theta() * RAD_2_DEGREE);
  tr.RotateX(rot.Psi()   * RAD_2_DEGREE);
  return tr;
}

/// Set a Position object (translation) to a TGeoHMatrix \ingroup DD4HEP \ingroup DD4HEP_CORE
TGeoHMatrix& dd4hep::detail::matrix::_transform(TGeoHMatrix& tr, const Position& pos)   {
  double t[3];
  pos.GetCoordinates(t);
  tr.SetDx(t[0]);
  tr.SetDy(t[1]);
  tr.SetDz(t[2]);
  return tr;
}

/// Set a Rotation3D object to a TGeoHMatrix           \ingroup DD4HEP \ingroup DD4HEP_CORE
TGeoHMatrix& dd4hep::detail::matrix::_transform(TGeoHMatrix& tr, const Rotation3D& rot)   {
  Double_t* r = tr.GetRotationMatrix();
  rot.GetComponents(r);
  return tr;
}

/// Set a Transform3D object to a TGeoHMatrix            \ingroup DD4HEP \ingroup DD4HEP_CORE
TGeoHMatrix& dd4hep::detail::matrix::_transform(TGeoHMatrix& tr, const Transform3D& trans) {
  Position pos;
  RotationZYX rot;
  trans.GetDecomposition(rot, pos);
  return _transform(tr, pos, rot);
}

/// Set a Position followed by a RotationZYX to a TGeoHMatrix  \ingroup DD4HEP \ingroup DD4HEP_CORE
TGeoHMatrix& dd4hep::detail::matrix::_transform(TGeoHMatrix& tr, const Position& pos, const RotationZYX& rot) {
  return _transform(_transform(tr, rot), pos);
}

/// Convert a Position object to a TGeoTranslation         \ingroup DD4HEP \ingroup DD4HEP_CORE
TGeoHMatrix* dd4hep::detail::matrix::_transform(const Position& pos)   {
  return &_transform(*(new TGeoHMatrix()), pos);
}

/// Convert a RotationZYX object to a TGeoHMatrix          \ingroup DD4HEP \ingroup DD4HEP_CORE
TGeoHMatrix* dd4hep::detail::matrix::_transform(const RotationZYX& rot)   {
  return &_transform(*(new TGeoHMatrix()), rot);
}

/// Convert a Rotation3D object to a TGeoHMatrix           \ingroup DD4HEP \ingroup DD4HEP_CORE
TGeoHMatrix* dd4hep::detail::matrix::_transform(const Rotation3D& rot)   {
  return &_transform(*(new TGeoHMatrix()), rot);
}

/// Convert a Transform3D object to a TGeoHMatrix          \ingroup DD4HEP \ingroup DD4HEP_CORE
TGeoHMatrix* dd4hep::detail::matrix::_transform(const Transform3D& trans) {
  return &_transform(*(new TGeoHMatrix()), trans);
}

/// Convert a Position followed by a RotationZYX to a TGeoHMatrix  \ingroup DD4HEP \ingroup DD4HEP_CORE
TGeoHMatrix* dd4hep::detail::matrix::_transform(const Position& pos, const RotationZYX& rot) {
  return &_transform(*(new TGeoHMatrix()), pos, rot);
}

/// Convert a TGeoMatrix object to a generic Transform3D  \ingroup DD4HEP \ingroup DD4HEP_CORE
dd4hep::Transform3D dd4hep::detail::matrix::_transform(const TGeoMatrix* matrix)    {
  const Double_t* t = matrix->GetTranslation();
  if ( matrix->IsRotation() )  {
    const Double_t* r = matrix->GetRotationMatrix();
    return Transform3D(r[0],r[1],r[2],t[0]*MM_2_CM,
                       r[3],r[4],r[5],t[1]*MM_2_CM,
                       r[6],r[7],r[8],t[2]*MM_2_CM);
  }
  return Transform3D(0e0,0e0,0e0,t[0]*MM_2_CM,
                     0e0,0e0,0e0,t[1]*MM_2_CM,
                     0e0,0e0,0e0,t[2]*MM_2_CM);
}

// add another implementation that takes const reference
dd4hep::Transform3D dd4hep::detail::matrix::_transform(const TGeoMatrix& matrix)    {
  const Double_t* t = matrix.GetTranslation();
  if ( matrix.IsRotation() )  {
    const Double_t* r = matrix.GetRotationMatrix();
    return Transform3D(r[0],r[1],r[2],t[0]*MM_2_CM,
                       r[3],r[4],r[5],t[1]*MM_2_CM,
                       r[6],r[7],r[8],t[2]*MM_2_CM);
  }
  return Transform3D(0e0,0e0,0e0,t[0]*MM_2_CM,
                     0e0,0e0,0e0,t[1]*MM_2_CM,
                     0e0,0e0,0e0,t[2]*MM_2_CM);
}

dd4hep::XYZAngles dd4hep::detail::matrix::_xyzAngles(const TGeoMatrix* matrix) {
  return matrix->IsRotation() ? _xyzAngles(matrix->GetRotationMatrix()) : XYZAngles(0,0,0);
}

dd4hep::XYZAngles dd4hep::detail::matrix::_xyzAngles(const double* r) {
  Double_t cosb = std::sqrt(r[0]*r[0] + r[1]*r[1]);
  if (cosb > 0.00001) {
    return XYZAngles(atan2(r[5], r[8]), atan2(-r[2], cosb), atan2(r[1], r[0]));
  }
  return XYZAngles(atan2(-r[7], r[4]),atan2(-r[2], cosb),0);
}

void dd4hep::detail::matrix::_decompose(const Transform3D& trafo, Translation3D& pos, RotationZYX& rot)   {
  trafo.GetDecomposition(rot,pos);
}

void dd4hep::detail::matrix::_decompose(const Transform3D& trafo, Translation3D& pos, XYZAngles& rot)   {
  EulerAngles r;
  trafo.GetDecomposition(r,pos);
  rot.SetXYZ(r.Psi(),r.Theta(),r.Phi());
}

void dd4hep::detail::matrix::_decompose(const Transform3D& trafo, Position& pos, RotationZYX& rot)  {
  trafo.GetDecomposition(rot,pos);
}

void dd4hep::detail::matrix::_decompose(const Transform3D& trafo, Position& pos, XYZAngles& rot)  {
  EulerAngles r;
  trafo.GetDecomposition(r,pos);
  rot.SetXYZ(r.Psi(),r.Theta(),r.Phi());
}

/// Check matrices for equality
int dd4hep::detail::matrix::_matrixEqual(const TGeoMatrix& left, const TGeoMatrix& right)   {
  double epsilon = 1e-12;
  int result = MATRICES_EQUAL;
  const Double_t* t1 = left.GetTranslation();
  const Double_t* t2 = right.GetTranslation();
  for(int i=0; i<3; ++i)   {
    if ( std::fabs(t1[i]-t2[i]) > epsilon )  {
      result = MATRICES_DIFFER_TRANSLATION;
      break;
    }
  }
  const Double_t* r1 = left.GetRotationMatrix();
  const Double_t* r2 = right.GetRotationMatrix();
  for(int i=0; i<9; ++i)   {
    if ( std::fabs(r1[i]-r2[i]) > epsilon )  {
      result |= MATRICES_DIFFER_ROTATION;
      break;
    }
  }
  return result;
}

    
