
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

#ifndef DDG4_GEANT4USERPARTICLEHANDLERHELPER_H
#define DDG4_GEANT4USERPARTICLEHANDLERHELPER_H

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    // Forward declarations
    class Geant4Particle;

    /// determines if particle should be kept and sets p.reason = 0 otherwise
    void setReason(Geant4Particle& p, bool starts_in_trk_volume, bool ends_in_trk_volume);

    /// determines if particle has ended in the tracker, calorimeter or if it is backscatter and sets simulator status accordingly
    void setSimulatorStatus(Geant4Particle& p, bool starts_in_trk_volume, bool ends_in_trk_volume);

  }
}

#endif // DDG4_GEANT4USERPARTICLEHANDLERHELPER_H