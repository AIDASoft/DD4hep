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
#ifndef DDG4_SRC_GEANT4SHAPECONVERTER_H
#define DDG4_SRC_GEANT4SHAPECONVERTER_H

// Framework include files

// C/C++ include files

// Forward declarations
class TGeoShape;
class G4VSolid;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    /// Convert a specific TGeo shape into the geant4 equivalent
    template <typename T> G4VSolid* convertShape(const TGeoShape* shape);

  }    // End namespace sim
}      // End namespace dd4hep
#endif // DDG4_SRC_GEANT4SHAPECONVERTER_H
