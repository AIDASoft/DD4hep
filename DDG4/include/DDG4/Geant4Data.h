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
#ifndef __DD4HEP_DDEVE_EXCLUSIVE__
#include "DD4hep/Objects.h"
#include "G4Step.hh"
#endif

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {
    typedef Geometry::Position Position;
    typedef Geometry::Position Direction;

    /** @class HitCompare Geant4Data.h DDG4/Geant4Data.h
     *
     *  Base class for hit comparisons.
     *
     * @author  M.Frank
     * @version 1.0
     */
    template <class HIT> struct HitCompare {
      /// Comparison function
      virtual bool operator()(const HIT* h) const = 0;
    };

    /** @class HitPositionCompare Geant4Data.h DDG4/Geant4Data.h
     *
     * Seek the hits of an arbitrary collection for the same position.
     *
     * @author  M.Frank
     * @version 1.0
     */
    template <class HIT> struct HitPositionCompare : public HitCompare<HIT> {
      const Position& pos;
      /// Constructor
      HitPositionCompare(const Position& p)
          : pos(p) {
      }
      /// Comparison function
      virtual bool operator()(const HIT* h) const {
        return pos == h->position;
      }
    };

    /** @class SimpleRun Geant4Data.h DDG4/Geant4Data.h
     *
     * Example class to store the run related information.
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct SimpleRun {
      /// Run identifiers
      int runID;
      /// Number of events in this run
      int numEvents;
      /// Default constructor
      SimpleRun();
      /// Default destructor
      virtual ~SimpleRun();
    };

    /** @class SimpleEvent Geant4Data.h DDG4/Geant4Data.h
     *
     * Example class to store the event related information.
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct SimpleEvent {
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

    /// Track properties
    enum ParticleProperties {
      G4PARTICLE_CREATED_HIT = 1<<0, 
      G4PARTICLE_PRIMARY = 1<<1,
      G4PARTICLE_HAS_SECONDARIES = 1<<2,
      G4PARTICLE_ABOVE_ENERGY_THRESHOLD = 1<<3,
      G4PARTICLE_KEEP_PROCESS = 1<<4,
      G4PARTICLE_KEEP_PARENT = 1<<5,
      G4PARTICLE_CREATED_CALORIMETER_HIT = 1<<6,
      G4PARTICLE_CREATED_TRACKER_HIT = 1<<7,
      G4PARTICLE_LAST_NOTHING = 1<<31
    };

    /// Data structure to store the MC particle information 
    /**
     * @author  M.Frank
     * @version 1.0
     */
    class Particle {
    public:
      int id, g4Parent, parent, reason, steps, secondaries;
      double vsx, vsy, vsz;
      double vex, vey, vez;
      double psx, psy, psz, pex, pey, pez, energy, time;
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

    /** @class SimpleHit Geant4Data.h DDG4/Geant4Data.h
     *
     * Base class for geant4 hit structures created by the
     * example sensitive detectors.
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct SimpleHit {
      // cellID
      long long int cellID;

      /** @class MonteCarloContrib
       */
      struct MonteCarloContrib {
        /// Geant 4 Track identifier
        int trackID;
        /// Particle ID from the PDG table
        int pdgID;
        /// Total energy deposit in this hit
        double deposit;
        /// Timestamp when this energy was deposited
        double time;
        /// Default constructor
        MonteCarloContrib()
            : trackID(-1), pdgID(-1), deposit(0.0), time(0.0) {
        }
        /// Initializing constructor
        MonteCarloContrib(int track_id, double dep, double time_stamp)
            : trackID(track_id), pdgID(-1), deposit(dep), time(time_stamp) {
        }
        /// Initializing constructor
        MonteCarloContrib(int track_id, int pdg, double dep, double time_stamp)
            : trackID(track_id), pdgID(pdg), deposit(dep), time(time_stamp) {
        }
        /// Copy constructor
        MonteCarloContrib(const MonteCarloContrib& c)
            : trackID(c.trackID), pdgID(c.pdgID), deposit(c.deposit), time(c.time) {
        }
	/// Assignment operator
	MonteCarloContrib& operator=(const MonteCarloContrib& c)  {
	  if ( this != &c )  {
	    trackID = c.trackID;
	    pdgID = c.pdgID;
	    deposit = c.deposit;
	    time = c.time;
	  }
	  return *this;
	}
        /// Clear data content
        void clear() {
          time = deposit = 0.0;
          pdgID = trackID = -1;
        }
      };
      typedef MonteCarloContrib Contribution;
      typedef std::vector<MonteCarloContrib> Contributions;
    public:
      /// Default constructor
      SimpleHit();
      /// Default destructor
      virtual ~SimpleHit();
      /// Extract the MC contribution for a given hit from the step information
      static Contribution extractContribution(G4Step* step);
    };

    struct SimpleTracker {
      /** @class SimpleTracker::Hit Geant4Data.h DDG4/Geant4Data.h
       *
       * Geant4 tracker hit class. Tracker hits contain the momentum
       * direction as well as the hit position.
       *
       * @author  M.Frank
       * @version 1.0
       */
      struct Hit : public SimpleHit {
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

    struct SimpleCalorimeter {
      /** @class SimpleCalorimeter::Hit Geant4Data.h DDG4/Geant4Data.h
       *
       * Geant4 tracker hit class. Calorimeter hits contain the momentum
       * direction as well as the hit position.
       *
       * @author  M.Frank
       * @version 1.0
       */
      struct Hit : public SimpleHit {
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
  }    // End namespace Simulation
}      // End namespace DD4hep
#endif // DD4HEP_GEANT4DATA_H
