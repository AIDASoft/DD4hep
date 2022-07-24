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
#include <DD4hep/Printout.h>
#include <DD4hep/DD4hepUnits.h>
#include <DDG4/Geant4Helpers.h>

#include <CLHEP/Units/SystemOfUnits.h>

// ROOT include files
#include <TGeoMatrix.h>

namespace  {
  static constexpr const double CM_2_MM = (CLHEP::centimeter/dd4hep::centimeter);
  
  /// Overload to access protected constructor
  struct MyTransform3D : public G4Transform3D {
    MyTransform3D(double XX, double XY, double XZ, double DX, double YX, double YY, double YZ, double DY, double ZX, double ZY,
                  double ZZ, double DZ)
      : G4Transform3D(XX, XY, XZ, DX, YX, YY, YZ, DY, ZX, ZY, ZZ, DZ) {
    }
    MyTransform3D(const double* t, const double* r)
      : G4Transform3D(r[0],r[1],r[2],t[0]*CM_2_MM,r[3],r[4],r[5],t[1]*CM_2_MM,r[6],r[7],r[8],t[2]*CM_2_MM)  {
    }
    MyTransform3D(const double* t)
      : G4Transform3D(1.0, 0.0, 0.0, t[0]*CM_2_MM, 0.0, 1.0, 0.0, t[1]*CM_2_MM, 0.0, 0.0, 1.0, t[2]*CM_2_MM)  {
    }
    MyTransform3D(Transform3D&& copy) : Transform3D(copy) {}
  };
  /// Overload to access protected constructor
  class MyG4RotationMatrix : public G4RotationMatrix   {
  public:
    MyG4RotationMatrix() : G4RotationMatrix() {}
    MyG4RotationMatrix(const double* r)
      : G4RotationMatrix(r[0],r[1],r[2],r[3],r[4],r[5],r[6],r[7],r[8])  {
    }
  };
  const double identity[9] = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};

}

G4RotationMatrix dd4hep::sim::g4Rotation(const TGeoMatrix* matrix)   {
  return matrix->IsRotation() ? MyG4RotationMatrix(matrix->GetRotationMatrix()) : MyG4RotationMatrix();
}

G4RotationMatrix dd4hep::sim::g4Rotation(const TGeoMatrix& matrix)   {
  return g4Rotation(&matrix);
}

G4RotationMatrix dd4hep::sim::g4Rotation(const TGeoRotation* matrix)   {
  return matrix->IsRotation() ? MyG4RotationMatrix(matrix->GetRotationMatrix()) : MyG4RotationMatrix();
}

G4RotationMatrix dd4hep::sim::g4Rotation(const TGeoRotation& matrix)   {
  return g4Rotation(&matrix);
}

G4RotationMatrix dd4hep::sim::g4Rotation(const Rotation3D& rot)   {
  double r[9];
  rot.GetComponents(r, r+9);
  return MyG4RotationMatrix(r);
}

G4RotationMatrix dd4hep::sim::g4Rotation(const RotationZYX& rot)   {
  return g4Rotation(Rotation3D(rot));
}


G4Transform3D dd4hep::sim::g4Transform(const double translation[])    {
  return MyTransform3D(translation);
}

G4Transform3D dd4hep::sim::g4Transform(const double translation[], const double rotation[])    {
  return MyTransform3D(translation, rotation);
}

G4Transform3D dd4hep::sim::g4Transform(const TGeoMatrix& matrix)   {
  return g4Transform(&matrix);
}

G4Transform3D dd4hep::sim::g4Transform(const TGeoMatrix* matrix)   {
  return matrix->IsRotation()
    ? g4Transform(matrix->GetTranslation(), matrix->GetRotationMatrix())
    : g4Transform(matrix->GetTranslation());
}

G4Transform3D dd4hep::sim::g4Transform(const TGeoTranslation& translation, const TGeoRotation& rotation)   {
  return g4Transform(&translation, &rotation);
}

G4Transform3D dd4hep::sim::g4Transform(const TGeoTranslation* translation, const TGeoRotation* rotation)   {
  return rotation->IsRotation()
    ? g4Transform(translation->GetTranslation(), rotation->GetRotationMatrix())
    : g4Transform(translation->GetTranslation());
}

G4Transform3D dd4hep::sim::g4Transform(const Position& pos, const Rotation3D& rot)   {
  double r[9], t[3] = {pos.X(), pos.Y(), pos.Z()};
  rot.GetComponents(r, r+9);
  return g4Transform(t, r);
}

G4Transform3D dd4hep::sim::g4Transform(const Position& pos, const RotationZYX& rot)   {
  return g4Transform(pos, Rotation3D(rot));
}

G4Transform3D dd4hep::sim::g4Transform(const Transform3D& matrix)   {
  Position pos;
  Rotation3D rot;
  matrix.GetDecomposition(rot, pos);
  return g4Transform(pos, rot);
}

/// Generate parameterised placements in 2 dimension according to transformation delta
G4Transform3D
dd4hep::sim::generate_placements(const G4Transform3D& start,
				 const G4Transform3D& delta,
				 std::size_t count,
				 const std::function<void(const G4Transform3D& delta)>& callback)
{
  G4Transform3D transform(start);
  for( std::size_t i = 0; i < count; ++i )    {
    callback(transform);
    transform = transform * delta;
  }
  return transform;
}

/// Generate parameterised placements in 2 dimensions according to transformation delta
G4Transform3D
dd4hep::sim::generate_placements(const G4Transform3D& start,
				 const G4Transform3D& delta1,
				 std::size_t count1,
				 const G4Transform3D& delta2,
				 std::size_t count2,
				 const std::function<void(const G4Transform3D& delta)>& callback)
{
  G4Transform3D transform2 = start;
  for(std::size_t j = 0; j < count2; ++j)    {
    G4Transform3D transform1 = transform2;
    generate_placements(transform1, delta1, count1, callback);
    transform2 = transform2 * delta2;
  }
  return transform2;
}

/// Generate parameterised placements in 3 dimensions according to transformation delta
G4Transform3D
dd4hep::sim::generate_placements(const G4Transform3D& start,
				 const G4Transform3D& delta1,
				 std::size_t count1,
				 const G4Transform3D& delta2,
				 std::size_t count2,
				 const G4Transform3D& delta3,
				 std::size_t count3,
				 const std::function<void(const G4Transform3D& delta)>& callback)
{
  G4Transform3D transform3 = start;
  for(std::size_t k = 0; k < count3; ++k)    {
    G4Transform3D transform2 = transform3;
    generate_placements(transform2, delta1, count1, delta2, count2, callback);
    transform3 = transform3 * delta3;
  }
  return transform3;
}

std::pair<double, EAxis> dd4hep::sim::extract_axis(const Transform3D& trafo)    {
  constexpr double eps = 2e-14;
  G4Transform3D tr = g4Transform(trafo);
  if ( fabs(tr.xx()) > (1e0-eps) && 
       fabs(tr.yy()) > (1e0-eps) &&
       fabs(tr.zz()) > (1e0-eps) )   {
    if ( fabs(tr.dx()) > eps )
      return { tr.dx(), kYAxis };
    else if ( fabs(tr.dy()) > eps )
      return { tr.dy(), kYAxis };
    else if ( fabs(tr.dz()) > eps )
      return { tr.dz(), kZAxis };
  }
  else if ( tr.getTranslation().mag() > eps && fabs(tr.dz()) < eps )  {
    return { tr.getTranslation().rho(), kRho };
  }
  else if ( fabs(tr.dz()) < eps )  {
    return { tr.getRotation().phi(), kPhi };   // Is this correct ?
  }
  except("Geant4Converter","Invalid volume parametrization matrix. Unknown Axis!");
  return { 0e0, kUndefined };
}
