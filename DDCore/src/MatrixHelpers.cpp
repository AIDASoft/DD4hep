// $Id: Handle.cpp 575 2013-05-17 20:41:52Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/MatrixHelpers.h"

// ROOT includes
#include "TGeoMatrix.h"

TGeoIdentity* DD4hep::Geometry::identityTransform() {
  return gGeoIdentity;
}

TGeoTranslation* DD4hep::Geometry::_translation(const Position& pos) {
  return new TGeoTranslation("", pos.X(), pos.Y(), pos.Z());
}

TGeoRotation* DD4hep::Geometry::_rotationZYX(const RotationZYX& rot) {
  return new TGeoRotation("", rot.Phi() * RAD_2_DEGREE, rot.Theta() * RAD_2_DEGREE, rot.Psi() * RAD_2_DEGREE);
}

TGeoRotation* DD4hep::Geometry::_rotation3D(const Rotation3D& rot3D) {
  EulerAngles rot(rot3D);
  return new TGeoRotation("", rot.Phi() * RAD_2_DEGREE, rot.Theta() * RAD_2_DEGREE, rot.Psi() * RAD_2_DEGREE);
}

TGeoHMatrix* DD4hep::Geometry::_transform(const Position& pos)   {
  double t[3];
  TGeoHMatrix *tr = new TGeoHMatrix();
  pos.GetCoordinates(t);
  tr->SetDx(t[0]);
  tr->SetDy(t[1]);
  tr->SetDz(t[2]);
  return tr;
}

TGeoHMatrix* DD4hep::Geometry::_transform(const RotationZYX& rot)   {
  TGeoHMatrix *tr = new TGeoHMatrix();
  tr->RotateZ(rot.Phi() * RAD_2_DEGREE);
  tr->RotateY(rot.Theta() * RAD_2_DEGREE);
  tr->RotateX(rot.Psi() * RAD_2_DEGREE);
  return tr;
}

TGeoHMatrix* DD4hep::Geometry::_transform(const Transform3D& trans) {
  Position pos;
  RotationZYX rot;
  trans.GetDecomposition(rot, pos);
  return _transform(pos,rot);
}

TGeoHMatrix* DD4hep::Geometry::_transform(const Position& pos, const RotationZYX& rot) {
  double t[3];
  TGeoHMatrix *tr = _transform(rot);
  pos.GetCoordinates(t);
  tr->SetDx(t[0]);
  tr->SetDy(t[1]);
  tr->SetDz(t[2]);
  return tr;
}

DD4hep::Geometry::Transform3D DD4hep::Geometry::_transform(const TGeoMatrix* matrix)    {
  const Double_t* t = matrix->GetTranslation();
  if ( matrix->IsRotation() )  {
    const Double_t* r = matrix->GetRotationMatrix();
    return Transform3D(r[0],r[1],r[2],t[0]*MM_2_CM,
		       r[3],r[4],r[5],t[1]*MM_2_CM,
		       r[6],r[7],r[8],t[2]*MM_2_CM);
#if 0
    return Transform3D(r[0],r[3],r[6],t[0]*MM_2_CM,
		       r[1],r[4],r[7],t[1]*MM_2_CM,
		       r[2],r[5],r[8],t[2]*MM_2_CM);
#endif
  }
  return Transform3D(0e0,0e0,0e0,t[0]*MM_2_CM,
		     0e0,0e0,0e0,t[1]*MM_2_CM,
		     0e0,0e0,0e0,t[2]*MM_2_CM);
}

DD4hep::Geometry::XYZAngles DD4hep::Geometry::_XYZangles(const TGeoMatrix* m) {
  return m->IsRotation() ? _XYZangles(m->GetRotationMatrix()) : XYZAngles(0,0,0);
}

DD4hep::Geometry::XYZAngles DD4hep::Geometry::_XYZangles(const double* r) {
  Double_t cosb = sqrt(r[0]*r[0] + r[1]*r[1]);
  if (cosb > 0.00001) {
    return XYZAngles(atan2(r[5], r[8]), atan2(-r[2], cosb), atan2(r[1], r[0]));
  }
  return XYZAngles(atan2(-r[7], r[4]),atan2(-r[2], cosb),0);
}

void DD4hep::Geometry::_decompose(const Transform3D& trafo, Translation3D& pos, RotationZYX& rot)   {
  trafo.GetDecomposition(rot,pos);
}

void DD4hep::Geometry::_decompose(const Transform3D& trafo, Translation3D& pos, XYZAngles& rot)   {
  EulerAngles r;
  trafo.GetDecomposition(r,pos);
  rot.SetXYZ(r.Psi(),r.Theta(),r.Phi());
}

void DD4hep::Geometry::_decompose(const Transform3D& trafo, Position& pos, RotationZYX& rot)  {
  trafo.GetDecomposition(rot,pos);
}

void DD4hep::Geometry::_decompose(const Transform3D& trafo, Position& pos, XYZAngles& rot)  {
  EulerAngles r;
  trafo.GetDecomposition(r,pos);
  rot.SetXYZ(r.Psi(),r.Theta(),r.Phi());
}
