// $Id: Handle.h 570 2013-05-17 07:47:11Z markus.frank $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation européenne pour la Recherche nucléaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================
#ifndef DD4HEP_DDG4_GEANT4TRACKINGPREACTION_H
#define DD4HEP_DDG4_GEANT4TRACKINGPREACTION_H

// Framework include files
#include "DDG4/Geant4TrackingAction.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

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
  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4TRACKINGPREACTION_H
