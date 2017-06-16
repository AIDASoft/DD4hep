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

#ifndef DDG4_DEFS_H
#define DDG4_DEFS_H

// Framework include files
#include "DD4hep/Detector.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {
    // Forward declarations;
    typedef Position Direction;
    typedef Position Momentum;

    class Geant4StepHandler;
    class Geant4Hit;
    class Geant4TrackerHit;
    class Geant4CalorimeterHit;
    class Geant4SensitiveDetector;
    template <class T> class Geant4GenericSD;

  }    // End namespace sim
}      // End namespace dd4hep
#endif // DDG4_DEFS_H
