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
#ifndef DD4HEP_DDG4_GEANT4PRIMARYHANDLER_H
#define DD4HEP_DDG4_GEANT4PRIMARYHANDLER_H

// Framework include files
#include "DDG4/Geant4GeneratorAction.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    /// Geant4Action to convert the particle information to Geant4
    /** Convert the primary interaction (object \tt{Geant4PrimaryInteraction} object
     *  attached to the \tt{Geant4Event} event context) and pass the result
     *  to Geant4 for simulation.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4PrimaryHandler : public Geant4GeneratorAction    {
    public:
      /// Standard constructor
      Geant4PrimaryHandler(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4PrimaryHandler();
      /// Event generation action callback
      virtual void operator()(G4Event* event);
    };
  }    // End namespace sim
}      // End namespace dd4hep

#endif // DD4HEP_DDG4_GEANT4PRIMARYHANDLER_H
