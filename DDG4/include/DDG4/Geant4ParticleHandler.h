// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4PARTICLEHANDLER_H
#define DD4HEP_DDG4_GEANT4PARTICLEHANDLER_H

// Framework include files
#include "DDG4/Geant4Data.h"
#include "DDG4/Geant4GeneratorAction.h"
#include "DDG4/Geant4MonteCarloTruth.h"

// Forward declarations
class G4Step;
class G4Track;
class G4Event;
class G4SteppingManager;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {

    // Forward declarations
    class Particle;
    class Geant4UserParticleHandler;

    /** Geant4Action to collect the MC particle information.
     *
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4ParticleHandler : public Geant4GeneratorAction, public Geant4MonteCarloTruth 
    {
    public:
      typedef std::vector<std::string> Processes;
    protected:
      
      /// Property: Steer printout at tracking action begin
      bool m_printStartTracking;
      /// Property: Steer printout at tracking action end
      bool m_printEndTracking;
      /// Property: Flag to keep all particles generated
      bool m_keepAll;
      /// Property: Energy cut below which particles are not collected, but assigned to the parent
      double m_kinEnergyCut;
      /// User action pointer
      Geant4UserParticleHandler* m_userHandler;
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
      /// Get proper equivalent track from the particle map according to the given geant4 track ID
      int equivalentTrack(int g4_id) const;

    public:
      /// Standard constructor
      Geant4ParticleHandler(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4ParticleHandler();
      /// Adopt the user particle handler
      bool adopt(Geant4UserParticleHandler* action);
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

      /// Access the particle map
      virtual const ParticleMap& particles() const { return m_particleMap; }
      /// Access the map of track equivalents
      virtual const TrackEquivalents& equivalents() const { return m_equivalentTracks; }
      /// Access the equivalent track id (shortcut to the usage of TrackEquivalents)
      virtual int particleID(int track, bool throw_if_not_found=true) const;
    
      /// Mark a Geant4 track to be kept for later MC truth analysis. Default flag: CREATED_HIT
      virtual void mark(const G4Track* track);
      /// Store a track
      virtual void mark(const G4Track* track, int reason);
      /// Mark a Geant4 track of the step to be kept for later MC truth analysis. Default flag: CREATED_HIT
      virtual void mark(const G4Step* step);
      /// Store a track produced in a step to be kept for later MC truth analysis
      virtual void mark(const G4Step* step, int reason);

    };
  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4PARTICLEHANDLER_H
