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

#ifndef DD4HEP_DDG4_GEANT4GENERATORACTIONINIT_H
#define DD4HEP_DDG4_GEANT4GENERATORACTIONINIT_H

// Framework include files
#include "DDG4/Geant4GeneratorAction.h"

// Forward declarations
class G4Event;
class G4Run;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    /// Initialize the Geant4Event objects to host generator and MC truth related information
    /** Geant4 actions to collect the MC particle information.
     *
     *  This action should register all event extension required for the further
     *  processing. We want to avoid that every client has to check if a given
     *  object is present or not and than later install the required data structures.
     *
     *  These by default are extensions of type:
     *  -- Geant4PrimaryEvent with multiple interaction sections, one for each interaction
     *     This is the MAIN and ONLY information to feed Geant4
     *
     *  -- Geant4PrimaryInteraction containing the track/vertex information to create
     *     the primary particles for Geant4. This record is build from the Geant4PrimaryEvent
     *     information.
     *  -- Geant4PrimaryMap a map of the Geant4Particles converted to G4PrimaryParticles
     *     to ease particle handling later.
     *  -- Geant4ParticleMap the map of particles created during the event simulation.
     *     This map has directly the correct particle offsets, so that the merging of
     *     Geant4PrimaryInteraction particles and the simulation particles is easy....
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4GeneratorActionInit : public Geant4GeneratorAction    {
    protected:
      /// Current run identifier
      int m_run;
      /// Counter for total number of events
      int m_evtTotal;
      /// Counter for total number of events in current run
      int m_evtRun;
    public:
      /// Standard constructor
      Geant4GeneratorActionInit(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4GeneratorActionInit();
      /// Event generation action callback
      virtual void operator()(G4Event* event);
      /// Begin-run action callback
      void begin(const G4Run* run);
      /// End-run action callback
      void end(const G4Run* run);

    };
  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4GENERATORACTIONINIT_H
