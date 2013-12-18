// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4TRACKMANAGER_H
#define DD4HEP_DDG4_GEANT4TRACKMANAGER_H

// Framework include files
#include "DDG4/Geant4Action.h"
#include "DDG4/Geant4TrackingAction.h"
#include "Math/PxPyPzE4D.h"
#include "G4VUserTrackInformation.hh"

// C/C++ include files
#include <map>
#include <string>
#include <typeinfo>

// Forward declarations
class G4Track;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {

    // Forward declarations

    /** @class Geant4TrackManager Geant4TrackManager.h DDG4/Geant4TrackManager.h
     *
     * Default base class for all geant 4 actions and derivates thereof.
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4TrackManager : public Geant4Action {
    public:
      typedef ROOT::Math::PxPyPzE4D<double> FourMomentum;

      /** @class Geant4TrackManager::TrackInfo Geant4TrackManager.h DDG4/Geant4TrackManager.h
       *
       * 
       *
       * @author  M.Frank
       * @version 1.0
       */
      class TrackInfo : public G4VUserTrackInformation  {
      public:
	/// Pointer to self
	Geant4TrackManager* manager;
	/// Pointer to the track
	const G4Track*      track;
	/// Flag to store the track
	bool                store;
	/// Initial 4-momentum at the beginning of the tracking action
	FourMomentum        initialMomentum;

      public:
	/// Standard constructor
	TrackInfo();
	/// Default destructor
	virtual ~TrackInfo();
	/// Clear alkl stored information for this track
	void set(const G4Track* trk);
      };
    protected:

      /// Information block for current track 
      Geant4TrackManager::TrackInfo m_current;

    public:
      /// Standard constructor
      Geant4TrackManager(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4TrackManager();
      /// Access the Geant4 tracking manager. Only use between tracking pre- and post action
      G4TrackingManager* trackMgr() const {
        return m_context->trackMgr();
      }
      /// Store a track
      void mark(const G4Track* track);
      /// Pre-track action callback
      virtual void begin(const G4Track* track);
      /// Post-track action callback
      virtual void end(const G4Track* track);
    };
  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4TRACKMANAGER_H
