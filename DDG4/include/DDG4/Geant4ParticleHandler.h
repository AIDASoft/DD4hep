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
#include "DDG4/Geant4Action.h"
#include "DDG4/Geant4EventAction.h"
#include "DDG4/Geant4GeneratorAction.h"
#include "DDG4/Geant4MonteCarloTruth.h"
#include "Math/PxPyPzE4D.h"
#include "G4VUserTrackInformation.hh"

// C/C++ include files
#include <map>
#include <set>
#include <string>
#include <typeinfo>

// Forward declarations
class G4Step;
class G4Track;
class G4Event;
class G4VProcess;
class G4SteppingManager;
class G4ParticleDefinition;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {

    // Forward declarations

    /** @class Geant4ParticleHandler Geant4ParticleHandler.h DDG4/Geant4ParticleHandler.h
     *
     * Default base class for all geant 4 actions and derivates thereof.
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4ParticleHandler : public Geant4GeneratorAction, public Geant4MonteCarloTruth 
      {
      public:
	class Particle {
	public:
	  int id, parent, theParent, reason;
	  double vx, vy, vz;
	  double px, py, pz, energy, time;
	  const G4VProcess *process;
	  const G4ParticleDefinition *definition;
	  std::set<int> daughters;
	  /// Default constructor
	  Particle();
	  /// Copy constructor
	  Particle(const Particle& c);
	  /// Default destructor
	  virtual ~Particle();
	  /// Remove daughter from set
	  void removeDaughter(int id_daughter);
	};

	typedef std::map<int,Particle*>  ParticleMap;
	typedef std::map<int,int>        TrackEquivalents;
	typedef std::vector<std::string> Processes;
      protected:
      
	/// Property: Steer printout at tracking action begin
	bool m_printStartTracking;
	/// Property: Steer printout at tracking action end
	bool m_printEndTracking;

	Particle m_currTrack;
	ParticleMap m_particleMap;
	TrackEquivalents m_equivalentTracks;
	Processes m_processNames;

	int recombineParents();
	/// Print record of kept particles
	void printParticles();
	/// Clear particle maps
	void clear();
	/// Check the record consistency
	void checkConsistency()  const;

      public:
	/// Standard constructor
	Geant4ParticleHandler(Geant4Context* context, const std::string& nam);
	/// Default destructor
	virtual ~Geant4ParticleHandler();
	/// Access the Geant4 tracking manager. Only use between tracking pre- and post action
	G4TrackingManager* trackMgr() const { return m_context.trackMgr();   }
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

	/// Mark a Geant4 track to be kept for later MC truth analysis
	virtual void mark(const G4Track* track);
	/// Store a track
	virtual void mark(const G4Track* track, bool created_hit);
	/// Mark a Geant4 track of the step to be kept for later MC truth analysis
	virtual void mark(const G4Step* step);
	/// Store a track produced in a step to be kept for later MC truth analysis
	virtual void mark(const G4Step* step, bool created_hit);

      };
  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4PARTICLEHANDLER_H
