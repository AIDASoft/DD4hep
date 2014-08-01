// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4MONTECARLOTRUTH_H
#define DD4HEP_DDG4_GEANT4MONTECARLOTRUTH_H

// Framework include files
#include "DDG4/Geant4Action.h"

// C/C++ include files
#include <map>

// Forward declarations
class G4Step;
class G4Track;
class G4Event;

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

    /** @class Geant4MonteCarloTruth Geant4MonteCarloTruth.h DDG4/Geant4MonteCarloTruth.h
     *
     * Default Interface class to handle monte carlo truth records
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4MonteCarloTruth   {
    public:
      typedef std::map<int,Particle*>  ParticleMap;
      typedef std::map<int,int>        TrackEquivalents;
    protected:
      /// Standard constructor
      Geant4MonteCarloTruth();
    public:
      /// Default destructor
      virtual ~Geant4MonteCarloTruth();
      /// Access the particle map
      virtual const ParticleMap& particles() const = 0;
      /// Access the map of track equivalents
      virtual const TrackEquivalents& equivalents() const = 0;
      /// Mark a Geant4 track to be kept for later MC truth analysis
      virtual void mark(const G4Track* track) = 0;
      /// Store a track, with a flag
      virtual void mark(const G4Track* track, int reason) = 0;
      /// Mark a Geant4 track of the step to be kept for later MC truth analysis
      virtual void mark(const G4Step* step) = 0;
      /// Store a track produced in a step to be kept for later MC truth analysis
      virtual void mark(const G4Step* step, int reason) = 0;
    };

    /** @class Geant4DummyTruthHandler Geant4DummyTruthHandler.h DDG4/Geant4DummyTruthHandler.h
     *
     * Void implementation doing nothing at all.
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4DummyTruthHandler : 
    public Geant4Action, public Geant4MonteCarloTruth  
      {
      protected:
	/// Map with stored MC Particles
	ParticleMap m_particleMap;
	/// Map associating the G4Track identifiers with identifiers of existing MCParticles
	TrackEquivalents m_equivalentTracks;
    public:
      /// Standard constructor
      Geant4DummyTruthHandler(Geant4Context* ctxt,const std::string& nam);
      /// Default destructor
      virtual ~Geant4DummyTruthHandler();
      /// Access the particle map
      virtual const ParticleMap& particles() const { return m_particleMap; }
      /// Access the map of track equivalents
      virtual const TrackEquivalents& equivalents() const { return m_equivalentTracks; }      
      /// Mark a Geant4 track to be kept for later MC truth analysis. Default flag: CREATED_HIT
      virtual void mark(const G4Track* track);
      /// Store a track, with a flag
      virtual void mark(const G4Track* track, int reason);
      /// Mark a Geant4 track of the step to be kept for later MC truth analysis. Default flag: CREATED_HIT
      virtual void mark(const G4Step* step);
      /// Store a track produced in a step to be kept for later MC truth analysis
      virtual void mark(const G4Step* step, int reason);
    };

  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4MONTECARLOTRUTH_H
