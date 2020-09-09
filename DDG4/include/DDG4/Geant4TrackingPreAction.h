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
#ifndef DDG4_GEANT4TRACKINGPREACTION_H
#define DDG4_GEANT4TRACKINGPREACTION_H

// Framework include files
#include "DDG4/Geant4TrackingAction.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    /// Default base class for all geant 4 tracking actions.
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4TrackingPreAction: public Geant4TrackingAction {
    public:
      /// Standard constructor
      Geant4TrackingPreAction(Geant4Context* context, const std::string& name = "");
      /// Default destructor
      virtual ~Geant4TrackingPreAction();
      /// Begin-of-tracking callback
      virtual void begin(const G4Track* track);
      /// End-of-tracking callback
      virtual void end(const G4Track* track);
    };
  }    // End namespace sim
}      // End namespace dd4hep

#endif // DDG4_GEANT4TRACKINGPREACTION_H
