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
#include <DD4hep/MatrixHelpers.h>
#include <DD4hep/DD4hepUnits.h>

#ifdef DD4HEP_USE_GEANT4_UNITS
#define MM_2_CM 1.0
#else
#define MM_2_CM 0.1
#endif

// ROOT includes
#include <TGeoMatrix.h>

TGeoIdentity* dd4hep::detail::matrix::_identity() {
  return gGeoIdentity;
}

ROOT::Math::XYZVector dd4hep::detail::matrix::_scale(const TGeoMatrix* matrix) {
  if ( matrix->IsScale() )   {
    const Double_t* mat_scale = matrix->GetScale();
    return ROOT::Math::XYZVector(mat_scale[0],mat_scale[1],mat_scale[2]);
  }
  return ROOT::Math::XYZVector(1e0,1e0,1e0);
}

ROOT::Math::XYZVector dd4hep::detail::matrix::_scale(const TGeoMatrix& matrix) {
  return _scale(&matrix);
}

dd4hep::Position dd4hep::detail::matrix::_translation(const TGeoMatrix* matrix) {
  if ( matrix->IsTranslation() )   {
    const Double_t* trans = matrix->GetTranslation();
    return Position(trans[0]*MM_2_CM,trans[1]*MM_2_CM,trans[2]*MM_2_CM);
  }
  return Position(0e0,0e0,0e0);
}

dd4hep::Position dd4hep::detail::matrix::_translation(const TGeoMatrix& matrix) {
  return _translation(&matrix);
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

/// Extract the rotational part of a TGeoMatrix as a Rotation3D                           \ingroup DD4HEP \ingroup DD4HEP_CORE
dd4hep::Rotation3D dd4hep::detail::matrix::_rotation3D(const TGeoMatrix* matrix)   {
  if ( matrix->IsRotation() )  {
    const Double_t* rot = matrix->GetRotationMatrix();
    return Rotation3D(rot[0],rot[1],rot[2],
                      rot[3],rot[4],rot[5],
                      rot[6],rot[7],rot[8]);
  }
  return Rotation3D(0e0,0e0,0e0,
                    0e0,0e0,0e0,
                    0e0,0e0,0e0);
}

/// Extract the rotational part of a TGeoMatrix as a Rotation3D                           \ingroup DD4HEP \ingroup DD4HEP_CORE
dd4hep::Rotation3D dd4hep::detail::matrix::_rotation3D(const TGeoMatrix& matrix)   {
  return _rotation3D(&matrix);
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
    const Double_t* rot = matrix->GetRotationMatrix();
    return Transform3D(rot[0],rot[1],rot[2],t[0]*MM_2_CM,
                       rot[3],rot[4],rot[5],t[1]*MM_2_CM,
                       rot[6],rot[7],rot[8],t[2]*MM_2_CM);
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

/// Decompose a generic ROOT Matrix into a generic transformation Transform3D            \ingroup DD4HEP \ingroup DD4HEP_CORE
void dd4hep::detail::matrix::_transform(const TGeoMatrix& matrix, Transform3D& tr)   {
  tr = _transform(matrix);
}

/// Decompose a generic ROOT Matrix into a generic transformation Transform3D            \ingroup DD4HEP \ingroup DD4HEP_CORE
void dd4hep::detail::matrix::_transform(const TGeoMatrix* matrix, Transform3D& tr)   {
  if ( matrix )   {
    _transform(*matrix, tr);
    return;
  }
  tr = Transform3D();
}

dd4hep::XYZAngles dd4hep::detail::matrix::_xyzAngles(const TGeoMatrix* matrix) {
  return matrix->IsRotation() ? _xyzAngles(matrix->GetRotationMatrix()) : XYZAngles(0,0,0);
}

dd4hep::XYZAngles dd4hep::detail::matrix::_xyzAngles(const double* rot) {
  Double_t cosb = std::sqrt(rot[0]*rot[0] + rot[1]*rot[1]);
  if (cosb > 0.00001) {
    return XYZAngles(atan2(rot[5], rot[8]), atan2(-rot[2], cosb), atan2(rot[1], rot[0]));
  }
  return XYZAngles(atan2(-rot[7], rot[4]),atan2(-rot[2], cosb),0);
}

void dd4hep::detail::matrix::_decompose(const TGeoMatrix& trafo, Position& pos, Rotation3D& rot)  {
  _decompose(_transform(trafo), pos, rot);
}

void dd4hep::detail::matrix::_decompose(const TGeoMatrix& trafo, Position& pos, RotationZYX& rot)   {
  _decompose(_transform(trafo), pos, rot);
}

void dd4hep::detail::matrix::_decompose(const TGeoMatrix& trafo, Position& pos, XYZAngles& rot)   {
  _decompose(_transform(trafo), pos, rot);
}

void dd4hep::detail::matrix::_decompose(const Transform3D& trafo, Position& pos, Rotation3D& rot)  {
  trafo.GetDecomposition(rot, pos);  
}

void dd4hep::detail::matrix::_decompose(const Rotation3D& rot, Position& x, Position& y, Position& z)  {
  rot.GetComponents(x,y,z);
}

void dd4hep::detail::matrix::_decompose(const Transform3D& trafo, Translation3D& pos, RotationZYX& rot)   {
  trafo.GetDecomposition(rot, pos);
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

/// Access determinant of rotation component (0 if no rotation present)
double dd4hep::detail::matrix::determinant(const TGeoMatrix* matrix)   {
  return (matrix) ? determinant(*matrix) : 0e0;
}

/// Access determinant of rotation component (0 if no rotation present)
double dd4hep::detail::matrix::determinant(const TGeoMatrix& matrix)   {
  const double* r = matrix.GetRotationMatrix();
  if ( r )   {
    double det =
      r[0]*r[4]*r[8] + r[3]*r[7]*r[2] + r[6]*r[1]*r[5] -
      r[2]*r[4]*r[6] - r[5]*r[7]*r[0] - r[8]*r[1]*r[3];
    return det;
  }
  return 0.0;
}

/// Access determinant of the rotation component of a Transform3D
double dd4hep::detail::matrix::determinant(const Transform3D& tr)   {
  Position p;
  Rotation3D r;
  tr.GetDecomposition(r, p);
  return determinant(r);
}

/// Access determinant of a Rotation3D
double dd4hep::detail::matrix::determinant(const Rotation3D& rot)   {
  Position x, y, z;
  rot.GetComponents(x,y,z);
  double det = (x.Cross(y)).Dot(z);
  return det;
}

/// Check matrices for equality
int dd4hep::detail::matrix::_matrixEqual(const TGeoMatrix& left, const TGeoMatrix& right)   {
  static constexpr double epsilon = 1e-12;
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

    
