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

#ifndef DDG4_GEANT4HELPERS_H
#define DDG4_GEANT4HELPERS_H

/// Framework include files
#include <DD4hep/Objects.h>

/// Geant4 include files
#include <G4ThreeVector.hh>
#include <G4Transform3D.hh>
#include <G4RotationMatrix.hh>
#include <geomdefs.hh>

/// C/C++ include files
#include <functional>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    /// Extract G4 rotation matrix from ROOT matrices
    G4RotationMatrix g4Rotation(const TGeoMatrix* matrix);
    G4RotationMatrix g4Rotation(const TGeoMatrix& matrix);
    G4RotationMatrix g4Rotation(const TGeoRotation* rot);
    G4RotationMatrix g4Rotation(const TGeoRotation& rot);
    G4RotationMatrix g4Rotation(const Rotation3D& rot);
    G4RotationMatrix g4Rotation(const RotationZYX& rot);

    /// Generate parameterised placements in 1 dimension according to transformation delta
    G4Transform3D
    generate_placements(const G4Transform3D& start,
			const G4Transform3D& delta,
			std::size_t count,
			const std::function<void(const G4Transform3D& delta)>& callback);

    /// Generate parameterised placements in 2 dimensions according to transformation delta
    G4Transform3D
    generate_placements(const G4Transform3D& start,
			const G4Transform3D& delta1,
			std::size_t count1,
			const G4Transform3D& delta2,
			std::size_t count2,
			const std::function<void(const G4Transform3D& delta)>& callback);

    /// Generate parameterised placements in 3 dimensions according to transformation delta
    G4Transform3D
    generate_placements(const G4Transform3D& start,
			const G4Transform3D& delta1,
			std::size_t count1,
			const G4Transform3D& delta2,
			std::size_t count2,
			const G4Transform3D& delta3,
			std::size_t count3,
			const std::function<void(const G4Transform3D& delta)>& callback);
    
    /// These conversions automatically apply the conversion from CM to MM!

    void          g4Transform(const double* translation, const double* rotation, G4Transform3D& transform);
    void          g4Transform(const Position& pos, const Rotation3D& rot, G4Transform3D& transform);
    void          g4Transform(const double* translation, G4Transform3D& transform);
    void          g4Transform(const Transform3D& matrix, G4Transform3D& transform);
    void          g4Transform(const TGeoMatrix* matrix, G4Transform3D& transform);
    void          g4Transform(const TGeoMatrix& matrix, G4Transform3D& transform);

    G4Transform3D g4Transform(const double* translation);
    G4Transform3D g4Transform(const double* translation, const double* rotation);

    G4Transform3D g4Transform(const Transform3D& matrix);
    G4Transform3D g4Transform(const Position& pos, const Rotation3D& rot);
    G4Transform3D g4Transform(const Position& pos, const RotationZYX& rot);

    G4Transform3D g4Transform(const TGeoMatrix* matrix);
    G4Transform3D g4Transform(const TGeoMatrix& matrix);
    G4Transform3D g4Transform(const TGeoTranslation* translation, const TGeoRotation* rotation);
    G4Transform3D g4Transform(const TGeoTranslation& translation, const TGeoRotation& rotation);

    std::pair<double, EAxis> extract_axis(const Transform3D& trafo);

  }    // End namespace sim
}      // End namespace dd4hep
#endif // DDG4_GEANT4HELPERS_H
