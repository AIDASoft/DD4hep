// $Id$
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

#ifndef DD4HEP_DDG4_GEANT4PARTICLEHANDLER_H
#define DD4HEP_DDG4_GEANT4PARTICLEHANDLER_H

// Framework include files
#include "DDG4/Geant4Primary.h"
#include "DDG4/Geant4GeneratorAction.h"
#include "DDG4/Geant4MonteCarloTruth.h"

// Forward declarations
class G4Step;
class G4Track;
class G4Event;
class G4SteppingManager;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    // Forward declarations
    class Geant4UserParticleHandler;

    /// Geant4Action to collect the MC particle information.
    /**
     *  Extract the relevant particle information during the simulation step.
     *
     *  This procedure works as follows:
     *  - At the beginning of the event generation the object registers itself as
     *    Monte-Carlo truth handler to the event context.
     *  - At the begin of each track action a particle candidate is created and filled
     *    with all properties known at this time.
     *  - At each stepping action a flag is set if the step produced secondaries.
     *  - Sensitive detectors call the MC truth handler if a hit was created.
     *    This fact is remembered.
     *  - At the end of the tracking action a first decision is taken if the candidate is to be
     *    kept for the final record.
     *  - At the end of the event action finally all particles are reduced to the
     *    final record. This logic can be overridden by a user handler to be attached.
     *  .
     *  Any of these actions may be intercepted by a {\tt{Geant4UserParticleHandler}}
     *  attached to the particle handler.
     *  See class {\tt{Geant4UserParticleHandler}} for details.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4ParticleHandler : public Geant4GeneratorAction, public Geant4MonteCarloTruth  {
    public:
      typedef Geant4ParticleMap::Particle         Particle;
      typedef Geant4ParticleMap::ParticleMap      ParticleMap;
      typedef Geant4ParticleMap::TrackEquivalents TrackEquivalents;
#ifdef __MAKECINT__
    public:
#else
    private:
#endif
      /// No default constructor
      Geant4ParticleHandler();
      /// No assignment operator
      Geant4ParticleHandler& operator=(const Geant4ParticleHandler& c);

    public:
      typedef std::vector<std::string> Processes;

    protected:

      /// Property: Steer printout at tracking action begin
      bool m_printStartTracking;
      /// Property: Steer printout at tracking action end
      bool m_printEndTracking;
      /// Property: Flag to keep all particles generated
      bool m_keepAll;
      /// Property: Flag if the handler is executed in standalone mode and hence must manage particles
      bool m_ownsParticles;
      /// Property: Energy cut below which particles are not collected, but assigned to the parent
      double m_kinEnergyCut;

      /// Global particle identifier. Obtained at the begin of the event.
      int m_globalParticleID;
      /// User action pointer
      Geant4UserParticleHandler* m_userHandler;
      /// Primary map
      Geant4PrimaryMap* m_primaryMap;
      /// Local buffer about the 'current' G4Track
      Particle m_currTrack;
      /// Map with stored MC Particles
      ParticleMap m_particleMap;
      /// Map associating the G4Track identifiers with identifiers of existing MCParticles
      TrackEquivalents m_equivalentTracks;
      /// All the processes of which the decay products will be explicitly stored
      Processes m_processNames;

      /// Recombine particles and associate the to parents with cleanup
      int recombineParents();
      /// Clear particle maps
      void clear();
      /// Check the record consistency
      void checkConsistency()  const;

      /// Rebase the simulated tracks, so that they fit to the generator particles
      void rebaseSimulatedTracks(int base);
      /// Debugging: Dump Geant4 particle map
      void dumpMap(const char* tag) const;

    public:
      /// Standard constructor
      Geant4ParticleHandler(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4ParticleHandler();
      /// Adopt the user particle handler
      bool adopt(Geant4Action* action);
      /// Event generation action callback
      virtual void operator()(G4Event* event);
      /// User stepping callback
      virtual void step(const G4Step* step, G4SteppingManager* mgr);
      /// Pre-event action callback
      virtual void beginEvent(const G4Event* event);
      /// Post-event action callback
      virtual void endEvent(const G4Event* event);
      /// Pre-track action callback
      virtual void begin(const G4Track* track);
      /// Post-track action callback
      virtual void end(const G4Track* track);

      /// Mark a Geant4 track to be kept for later MC truth analysis. Default flag: CREATED_HIT
      virtual void mark(const G4Track* track);
      /// Store a track
      virtual void mark(const G4Track* track, int reason);
      /// Mark a Geant4 track of the step to be kept for later MC truth analysis. Default flag: CREATED_HIT
      virtual void mark(const G4Step* step);
      /// Store a track produced in a step to be kept for later MC truth analysis
      virtual void mark(const G4Step* step, int reason);

      /// Default callback to be answered if the particle should be kept if NO user handler is installed
      static bool defaultKeepParticle(Particle& particle);

    };
  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4PARTICLEHANDLER_H
