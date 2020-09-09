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
#include "DD4hep/Memory.h"
#include "Math/Vector3D.h"

// C/C++ include files
#include <set>
#include <vector>

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
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class SimpleEvent {
    public:
      typedef std::vector<long> Seeds;
      /// Run identifiers
      int    runID;
      /// Event identifier
      int    eventID;
      /// Random number generator seeds
      Seeds  seeds;
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
      dd4hep_ptr<DataExtension> extension;   //! not persisten. ROOT cannot handle

      /// Utility class describing the monte carlo contribution of a given particle to a hit.
      /**
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_SIMULATION
       */
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
        /// Length of this step
        double length = 0.0;
        /// Proper position of the hit contribution
        float  x,y,z;

        /// Default constructor
        MonteCarloContrib()
          : trackID(-1), pdgID(-1), deposit(0.0), time(0.0), x(0), y(0), z(0) {
        }
        /// Initializing constructor
        MonteCarloContrib(int track_id, int pdg, double dep, double time_stamp, double len)
          : trackID(track_id), pdgID(pdg), deposit(dep), time(time_stamp), length(len), x(0), y(0), z(0) {
        }
        /// Initializing constructor
        MonteCarloContrib(int track_id, int pdg, double dep, double time_stamp, double len, float* pos)
          : trackID(track_id), pdgID(pdg), deposit(dep), time(time_stamp), length(len),
            x(pos[0]), y(pos[1]), z(pos[2])
        {
        }
        /// Initializing constructor
        MonteCarloContrib(int track_id, int pdg, double dep, double time_stamp, double len, double* pos)
          : trackID(track_id), pdgID(pdg), deposit(dep), time(time_stamp), length(len),
            x(pos[0]), y(pos[1]), z(pos[2])
        {
        }
        /// Copy constructor
        MonteCarloContrib(const MonteCarloContrib& c)
          : trackID(c.trackID), pdgID(c.pdgID), deposit(c.deposit), time(c.time), length(c.length),
            x(c.x), y(c.y), z(c.z) {
        }
        /// Assignment operator
        MonteCarloContrib& operator=(const MonteCarloContrib& c)  {
          if ( this != &c )  {
            trackID = c.trackID;
            pdgID   = c.pdgID;
            deposit = c.deposit;
            time    = c.time;
            length  = c.length;
            x       = c.x;
            y       = c.y;
            z       = c.z;
          }
          return *this;
        }
        /// Clear data content
        void clear() {
          x = y = z = 0.0;
          time = deposit = length = 0.0;
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
      static Contribution extractContribution(const G4Step* step);
      /// Extract the MC contribution for a given hit from the step information with BirksLaw option
      static Contribution extractContribution(const G4Step* step, bool ApplyBirksLaw);
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
        /// Hit position
        Position      position;
        /// Hit direction
        Direction     momentum;
        /// Length of the track segment contributing to this hit
        double        length;
        /// Monte Carlo / Geant4 information
        Contribution  truth;
        /// Energy deposit in the tracker hit
        double        energyDeposit;
      public:
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
        Hit& storePoint(const G4Step* step, const G4StepPoint* point);
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
        /// Hit position
        Position      position;
        /// Hit contributions by individual particles
        Contributions truth;
        /// Total energy deposit
        double        energyDeposit;
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

  }    // End namespace sim
}      // End namespace dd4hep
#endif // DDG4_GEANT4DATA_H
