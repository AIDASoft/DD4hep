// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4TRACKPERSISTENCY_H
#define DD4HEP_DDG4_GEANT4TRACKPERSISTENCY_H

// Framework include files
#include "DDG4/Geant4Action.h"
#include "DDG4/Geant4EventAction.h"
#include "DDG4/Geant4GeneratorAction.h"
#include "DDG4/Geant4MonteCarloTruth.h"
#include "Math/PxPyPzE4D.h"
#include "G4VUserTrackInformation.hh"

// C/C++ include files
#include <map>
#include <string>
#include <typeinfo>

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

    /** @class Geant4TrackPersistency Geant4TrackPersistency.h DDG4/Geant4TrackPersistency.h
     *
     * Default base class for all geant 4 actions and derivates thereof.
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4TrackPersistency 
      : public Geant4GeneratorAction, 
      public Geant4MonteCarloTruth 
      {
    public:
      typedef ROOT::Math::PxPyPzE4D<double> FourMomentum;
      typedef std::map<int,void*> TrackMap;

      /** @class Geant4TrackPersistency::TrackInfo Geant4TrackPersistency.h DDG4/Geant4TrackPersistency.h
       *
       * 
       *
       * @author  M.Frank
       * @version 1.0
       */
      class TrackInfo : public G4VUserTrackInformation  {
      public:
	/// Pointer to self
	Geant4TrackPersistency* manager;
	/// Pointer to the track
	const G4Track*      track;
	/// Pointer to MC tracking history
	void*               info;
	/// Flag to store the track
	bool                store;
	/// Flag to know if this track created a Geant4 hit
	bool                createdHit;
	/// Initial 4-momentum at the beginning of the tracking action
	FourMomentum        initialMomentum;
	double              initialEnergy;

      public:
	/// Standard constructor
	TrackInfo();
	/// Default destructor
	virtual ~TrackInfo();
	/// Clear all stored information for this track
	void set(const G4Track* trk, void* opt);
      };
    protected:

      /// Information block for current track 
      Geant4TrackPersistency::TrackInfo m_current;
      /// Property: Steer printout at tracking action begin
      bool m_printStartTracking;
      /// Property: Steer printout at tracking action end
      bool m_printEndTracking;

    public:
      TrackMap m_trackMap;

    public:
      /// Standard constructor
      Geant4TrackPersistency(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4TrackPersistency();
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

#endif // DD4HEP_DDG4_GEANT4TRACKPERSISTENCY_H
