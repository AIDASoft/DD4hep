// $Id: Geant4Data.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_GEANT4DATA_H
#define DD4HEP_GEANT4DATA_H

// Framework include files
#include "Math/Vector3D.h"

// C/C++ include files
#include <set>
#include <memory>
#include <vector>

// Forward declarations
class G4Step;
class G4StepPoint;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    // Forward type definitions
    typedef ROOT::Math::XYZVector Position;
    typedef ROOT::Math::XYZVector Direction;

    /// Simple run description structure. Used in the default I/O mechanism.
    /**
     * Example class to store the run related information.
     *
     * @author  M.Frank
     * @version 1.0
     */
    class SimpleRun {
      public:
      /// Run identifiers
      int runID;
      /// Number of events in this run
      int numEvents;
      /// Default constructor
      SimpleRun();
      /// Default destructor
      virtual ~SimpleRun();
    };

    /// Simple event description structure. Used in the default I/O mechanism.
    /**
     * Example class to store the event related information.
     *
     * @author  M.Frank
     * @version 1.0
     */
    class SimpleEvent {
      public:
      typedef std::vector<long> Seeds;
      /// Run identifiers
      int runID;
      /// Event identifier
      int eventID;
      /// Random number generator seeds
      Seeds seeds;
      /// Default constructor
      SimpleEvent();
      /// Default destructor
      virtual ~SimpleEvent();
    };

    /// Generic user-extendible data extension class.
    class DataExtension  {
    public:
      /// Default constructor
      DataExtension() {}
      /// Default destructor
      virtual ~DataExtension();      
    };

    /// Base class for geant4 hit structures used by the default DDG4 sensitive detector implementations
    /*
     *  Base class for geant4 hit structures created by the
     *  example sensitive detectors. This is a generic class
     *  only dealing with the cellID. Users may add an extension 
     *  object, which normally should not be necessary.
     *
     *  @author  M.Frank
     *  @version 1.0
     */
    class Geant4HitData {
      public:
      /// cellID
      long long int cellID;
      /// User data extension if required
      std::auto_ptr<DataExtension> extension;  

      /// Utility class describing the monte carlo contribution of a given particle to a hit.
      class MonteCarloContrib {
      public:
        /// Geant 4 Track identifier
        int trackID;
        /// Particle ID from the PDG table
        int pdgID;
        /// Total energy deposit in this hit
        double deposit;
        /// Timestamp when this energy was deposited
        double time;
	/// Proper position of the hit contribution
	float  x,y,z;

        /// Default constructor
        MonteCarloContrib()
	: trackID(-1), pdgID(-1), deposit(0.0), time(0.0), x(0), y(0), z(0) {
        }
        /// Initializing constructor
        MonteCarloContrib(int track_id, int pdg, double dep, double time_stamp)
	: trackID(track_id), pdgID(pdg), deposit(dep), time(time_stamp), x(0), y(0), z(0) {
        }
        /// Initializing constructor
        MonteCarloContrib(int track_id, int pdg, double dep, double time_stamp, float* pos)
	: trackID(track_id), pdgID(pdg), deposit(dep), time(time_stamp),
	  x(pos[0]), y(pos[1]), z(pos[2]) 
	{
        }
        /// Initializing constructor
        MonteCarloContrib(int track_id, int pdg, double dep, double time_stamp, double* pos)
	: trackID(track_id), pdgID(pdg), deposit(dep), time(time_stamp),
	  x(pos[0]), y(pos[1]), z(pos[2]) 
	{
        }
        /// Copy constructor
        MonteCarloContrib(const MonteCarloContrib& c)
	: trackID(c.trackID), pdgID(c.pdgID), deposit(c.deposit), time(c.time),
	  x(c.x), y(c.y), z(c.z) {
        }
	/// Assignment operator
	MonteCarloContrib& operator=(const MonteCarloContrib& c)  {
	  if ( this != &c )  {
	    trackID = c.trackID;
	    pdgID   = c.pdgID;
	    deposit = c.deposit;
	    time    = c.time;
	    x       = c.x;
	    y       = c.y;
	    z       = c.z;
	  }
	  return *this;
	}
        /// Clear data content
        void clear() {
	  x = y = z = 0.0;
          time = deposit = 0.0;
          pdgID = trackID = -1;
        }
      };
      typedef MonteCarloContrib Contribution;
      typedef std::vector<MonteCarloContrib> Contributions;
    public:
      /// Default constructor
      Geant4HitData();
      /// Default destructor
      virtual ~Geant4HitData();
      /// Extract the MC contribution for a given hit from the step information
      static Contribution extractContribution(G4Step* step);
    };

    /// Helper class to define structures used by the generic DDG4 tracker sensitive detector
    class Geant4Tracker {
    public:
      /// DDG4 tracker hit class used by the generic DDG4 tracker sensitive detector
      /**
       * Geant4 tracker hit class. Tracker hits contain the momentum
       * direction as well as the hit position.
       *
       * @author  M.Frank
       * @version 1.0
       */
      class Hit : public Geant4HitData {
      public:
        /// Hit position
        Position position;
        /// Hit direction
        Direction momentum;
        /// Length of the track segment contributing to this hit
        double length;
        /// Monte Carlo / Geant4 information
        Contribution truth;
        /// Energy deposit in the tracker hit
        double energyDeposit;
        /// Default constructor
        Hit();
        /// Initializing constructor
        Hit(int track_id, int pdg_id, double deposit, double time_stamp);
        /// Default destructor
        virtual ~Hit();
        /// Assignment operator
        Hit& operator=(const Hit& c);
        /// Clear hit content
        Hit& clear();
        /// Store Geant4 point and step information into tracker hit structure.
        Hit& storePoint(G4Step* step, G4StepPoint* point);
      };
    };

    /// Helper class to define structures used by the generic DDG4 calorimeter sensitive detector
    class Geant4Calorimeter {
    public:

      /// DDG4 calorimeter hit class used by the generic DDG4 calorimeter sensitive detector
      /**
       * Geant4 tracker hit class. Calorimeter hits contain the momentum
       * direction as well as the hit position.
       *
       * @author  M.Frank
       * @version 1.0
       */
      class Hit : public Geant4HitData {
      public:
        /// Hit position
        Position position;
        /// Hit contributions by individual particles
        Contributions truth;
        /// Total energy deposit
        double energyDeposit;
      public:
        /// Default constructor (for ROOT)
        Hit();
        /// Standard constructor
        Hit(const Position& cell_pos);
        /// Default destructor
        virtual ~Hit();
      };
    };

    /// Backward compatibility definitions
    typedef Geant4HitData SimpleHit;
    typedef Geant4Tracker SimpleTracker;
    typedef Geant4Calorimeter SimpleCalorimeter;

  }    // End namespace Simulation
}      // End namespace DD4hep
#endif // DD4HEP_GEANT4DATA_H
