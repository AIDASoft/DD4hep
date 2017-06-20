// $Id: $
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
#ifndef DD4HEP_DDG4_GEANT4TRACKINGPOSTACTION_H
#define DD4HEP_DDG4_GEANT4TRACKINGPOSTACTION_H

// Framework include files
#include "DDG4/Geant4TrackingAction.h"
#include <vector>

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
    class Geant4TrackingPostAction: public Geant4TrackingAction {
    protected:
      typedef std::vector<std::string> StringV;
      StringV m_requiredProcs;
      StringV m_ignoredProcs;
      bool m_storeMarkedTracks;
    public:
      /// Standard constructor
      Geant4TrackingPostAction(Geant4Context* context, const std::string& name = "");
      /// Default destructor
      virtual ~Geant4TrackingPostAction();

      ///
      void saveTrack(const G4Track* track);

      /// Begin-of-tracking callback
      virtual void begin(const G4Track* track);
      /// End-of-tracking callback
      virtual void end(const G4Track* track);
    };
  }    // End namespace sim
}      // End namespace dd4hep

#endif // DD4HEP_DDG4_GEANT4TRACKINGPOSTACTION_H
