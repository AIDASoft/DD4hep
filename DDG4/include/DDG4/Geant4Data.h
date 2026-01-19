//==========================================================================
//  AIDA Detector description implementation 
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

#ifndef DDG4_GEANT4DATA_H
#define DDG4_GEANT4DATA_H

// Framework include files
#include <Math/Vector3D.h>

// C/C++ include files
#include <set>
#include <vector>
#include <memory>

// Forward declarations
class G4Step;
class G4StepPoint;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  // Forward type definitions
  typedef ROOT::Math::XYZVector Position;
  typedef ROOT::Math::XYZVector Direction;
  
  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    // Forward declarations
    class Geant4FastSimSpot;

    /// Simple run description structure. Used in the default I/O mechanism.
    /**
     * Example class to store the run related information.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class SimpleRun {
    public:
      /// Run identifiers
      int runID     { -1 };
      /// Number of events in this run
      int numEvents {  0 };
      /// Default constructor
      SimpleRun();
      /// Default destructor
      virtual ~SimpleRun();
    };

    /// Simple event description structure. Used in the default I/O mechanism.
    /**
     * Example class to store the event related information.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class SimpleEvent {
    public:
      typedef std::vector<long> Seeds;
      /// Run identifiers
      int    runID   { -1 };
      /// Event identifier
      int    eventID { -1 };
      /// Random number generator seeds
      Seeds  seeds { };
      /// Default constructor
      SimpleEvent();
      /// Default destructor
      virtual ~SimpleEvent();
    };

    /// Generic user-extendible data extension class.
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
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
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4HitData {
    public:
      enum HitFlags {
        HIT_KILLED_TRACK    = 1<<0,
        HIT_SECONDARY_TRACK = 1<<1,
        HIT_NEW_TRACK       = 1<<2,
        HIT_STARTED_INSIDE  = 1<<10,
        HIT_STARTED_SURFACE = 1<<11,
        HIT_STARTED_OUTSIDE = 1<<12,
        HIT_ENDED_INSIDE    = 1<<13,
        HIT_ENDED_SURFACE   = 1<<14,
        HIT_ENDED_OUTSIDE   = 1<<15
      };
      /// cellID
      long long int cellID = 0;
      /// User flag to classify hits
      long flag = 0;
      /// Original Geant 4 track identifier of the creating track (debugging)
      long g4ID = -1;
      /// User data extension if required
      std::unique_ptr<DataExtension> extension;

      /// Utility class describing the monte carlo contribution of a given particle to a hit.
      /**
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_SIMULATION
       */
      class MonteCarloContrib {
      public:
        /// Geant 4 Track identifier
        int trackID = -1;
        /// Particle ID from the PDG table
        int pdgID = -1;
        /// Total energy deposit in this hit
        double deposit = 0.0;
        /// Timestamp when this energy was deposited
        double time;
        /// Length of this step
        double length = 0.0;
        /// Proper position of the hit contribution
        float  x = 0.0, y = 0.0, z = 0.0;
        /// Proper particle momentum when generating the hit of the contributing particle
        float  px = 0.0, py = 0.0, pz = 0.0;

        /// Default constructor
        MonteCarloContrib() = default;
        /// Copy constructor
        MonteCarloContrib(const MonteCarloContrib& c) = default;
        /// Move constructor
        MonteCarloContrib(MonteCarloContrib&& c) = default;
#if 0
        /// Initializing constructor
        MonteCarloContrib(int track_id, int pdg, double dep, double time_stamp, double len)
          : trackID(track_id), pdgID(pdg), deposit(dep), time(time_stamp), length(len) {
        }
#endif
        /// Initializing constructor
        MonteCarloContrib(int track_id, int pdg, double dep, double time_stamp, double len, float* pos, float* mom)
          : trackID(track_id), pdgID(pdg), deposit(dep), time(time_stamp), length(len),
            x(pos[0]), y(pos[1]), z(pos[2]), px(mom[0]), py(mom[1]), pz(mom[2])
        {
        }
        /// Initializing constructor
        MonteCarloContrib(int track_id, int pdg, double dep, double time_stamp, double len, double* pos, double* mom)
          : trackID(track_id), pdgID(pdg), deposit(dep), time(time_stamp), length(len),
            x(float(pos[0])), y(float(pos[1])), z(float(pos[2])),
            px(float(mom[0])), py(float(mom[1])), pz(float(mom[2]))
        {
        }
        /// Initializing constructor
        MonteCarloContrib(int track_id, int pdg, double dep, double time_stamp, double len, const Position& pos, const Direction& mom)
          : trackID(track_id), pdgID(pdg), deposit(dep), time(time_stamp), length(len),
            x(float(pos.x())), y(float(pos.y())), z(float(pos.z())),
            px(float(mom.x())), py(float(mom.y())), pz(float(mom.z()))
        {
        }
        /// Assignment operator (move)
        MonteCarloContrib& operator=(MonteCarloContrib&& c) = default;
        /// Assignment operator (copy)
        MonteCarloContrib& operator=(const MonteCarloContrib& c) = default;
        /// Clear data content
        void clear() {
          x = y = z = 0.0;
          px = py = pz = 0.0;
          time  = deposit = length = 0.0;
          pdgID = trackID = -1;
        }
        /// Access position
        Position position() const   {
          return Position(x, y, z);
        }
        /// Set position of the contribution
        void setPosition(const Position& pos)   {
          x = pos.x();
          y = pos.y();
          z = pos.z();
        }
        /// Set position of the contribution
        void setPosition(double pos_x, double pos_y, double pos_z)   {
          x = float(pos_x);
          y = float(pos_y);
          z = float(pos_z);
        }
        /// Access momentum
        Direction momentum() const   {
          return Direction(px, py, pz);
        }
        /// Set memonetum of the contribution
        void setMomentum(const Direction& dir)   {
          px = dir.x();
          py = dir.y();
          pz = dir.z();
        }
        /// Set memonetum of the contribution
        void setMomentum(double mom_x, double mom_y, double mom_z)   {
          px = float(mom_x);
          py = float(mom_y);
          pz = float(mom_z);
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
      static Contribution extractContribution(const G4Step* step);
      /// Extract the MC contribution for a given hit from the step information with BirksLaw option
      static Contribution extractContribution(const G4Step* step, bool ApplyBirksLaw);
      /// Extract the MC contribution for a given hit from the GFlash/FastSim spot information
      static Contribution extractContribution(const Geant4FastSimSpot* spot);
    };

    /// Helper class to define structures used by the generic DDG4 tracker sensitive detector
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4Tracker {
    public:
      /// DDG4 tracker hit class used by the generic DDG4 tracker sensitive detector
      /**
       * Geant4 tracker hit class. Tracker hits contain the momentum
       * direction as well as the hit position.
       *
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_SIMULATION
       */
      class Hit : public Geant4HitData {
      public:
        typedef Geant4HitData base_t;

        /// Hit position
        Position      position;
        /// Hit direction
        Direction     momentum;
        /// Length of the track segment contributing to this hit
        double        length  { 0e0 };
        /// Energy deposit in the tracker hit
        double        energyDeposit { 0e0 };
        /// Monte Carlo / Geant4 information
        Contribution  truth;
      public:
        /// Default constructor
        Hit();
        /// Move constructor
        Hit(Hit&& c) = delete;
        /// copy constructor
        Hit(const Hit& c) = delete;
        /// Initializing constructor
        Hit(int track_id, int pdg_id, double deposit, double time_stamp, double len=0.0, const Position& p={0.0, 0.0, 0.0}, const Direction& d={0.0, 0.0, 0.0});
        /// Optimized constructor for sensitive detectors
        Hit(const Geant4HitData::Contribution& contrib, const Direction& mom, double deposit);
        /// Default destructor
        virtual ~Hit();
        /// Move assignment operator
        Hit& operator=(Hit&& c) = delete;
        /// Copy assignment operator
        Hit& operator=(const Hit& c) = delete;
        /// Explicit assignment operation
        void copyFrom(const Hit& c);
        /// Clear hit content
        Hit& clear();
        /// Store Geant4 point and step information into tracker hit structure.
        Hit& storePoint(const G4Step* step, const G4StepPoint* point);
        /// Store Geant4 spot information into tracker hit structure.
        Hit& storePoint(const Geant4FastSimSpot* spot);
      };
    };

    /// Helper class to define structures used by the generic DDG4 calorimeter sensitive detector
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4Calorimeter {
    public:

      /// DDG4 calorimeter hit class used by the generic DDG4 calorimeter sensitive detector
      /**
       * Geant4 tracker hit class. Calorimeter hits contain the momentum
       * direction as well as the hit position.
       *
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_SIMULATION
       */
      class Hit : public Geant4HitData {
      public:
        typedef Geant4HitData base_t;

        /// Hit position
        Position      position      {     };
        /// Hit contributions by individual particles
        Contributions truth         {     };
        /// Total energy deposit
        double        energyDeposit { 0e0 };
      public:
        /// Default constructor (for ROOT)
        Hit();
        /// Move constructor
        Hit(Hit&& c) = delete;
        /// copy constructor
        Hit(const Hit& c) = delete;
        /// Standard constructor
        Hit(const Position& cell_pos);
        /// Default destructor
        virtual ~Hit();
        /// Move assignment operator
        Hit& operator=(Hit&& c) = delete;
        /// Copy assignment operator
        Hit& operator=(const Hit& c) = delete;
      };
    };

    /// Backward compatibility definitions
    typedef Geant4HitData SimpleHit;
    typedef Geant4Tracker SimpleTracker;
    typedef Geant4Calorimeter SimpleCalorimeter;

  }    // End namespace sim
}      // End namespace dd4hep
#endif // DDG4_GEANT4DATA_H
