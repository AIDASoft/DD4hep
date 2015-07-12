// $Id: $
//==========================================================================
//  AIDA Detector description implementation for LCD
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
#ifndef DD4HEP_DDG4_GEANT4INTERACTIONMERGER_H
#define DD4HEP_DDG4_GEANT4INTERACTIONMERGER_H

// Framework include files
#include "DDG4/Geant4GeneratorAction.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    // Forward declarations
    class Geant4PrimaryInteraction;

    /// Geant4Action to merge several independent interaction to one
    /** Merge all interactions created by each \tt{Geant4InputAction} into one single
     *  record. The input records are taken from the item \tt{Geant4PrimaryEvent}
     *  and are merged into the \tt{Geant4PrimaryInteraction} object attached to the
     *  \tt{Geant4Event} event context.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4InteractionMerger : public Geant4GeneratorAction    {
      /// Append input interaction to global output
      void appendInteraction(Geant4PrimaryInteraction* output, Geant4PrimaryInteraction* input);
    public:
      /// Standard constructor
      Geant4InteractionMerger(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4InteractionMerger();
      /// Event generation action callback
      virtual void operator()(G4Event* event);
    };
  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4INTERACTIONMERGER_H
