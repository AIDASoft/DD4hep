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

#ifndef DD4HEP_GEANT4HITS_H
#define DD4HEP_GEANT4HITS_H

// Framework include files
#include "DD4hep/Objects.h"
#include "DDG4/Geant4StepHandler.h"

// Geant4 include files
#include "G4VHit.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    // Forward declarations;
    class Geant4Hit;
    class Geant4TrackerHit;
    class Geant4CalorimeterHit;

    /// Deprecated:  Base class for hit comparisons.
    /** @class HitCompare Geant4Hits.h DDG4/Geant4Hits.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    template <class HIT> class HitCompare {
    public:
      /// Default destructor
      virtual ~HitCompare() {}
      /// Comparison function
      virtual bool operator()(const HIT* h) const = 0;
    };

    /// Deprecated: Seek the hits of an arbitrary collection for the same position.
    /** @class HitPositionCompare Geant4Hits.h DDG4/Geant4Hits.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    template <class HIT> struct HitPositionCompare: public HitCompare<HIT> {
      /// Reference to the hit position
      const Position& pos;
      /// Constructor
      HitPositionCompare(const Position& p) : pos(p) {}
      /// Default destructor
      virtual ~HitPositionCompare() {}
      /// Comparison function
      virtual bool operator()(const HIT* h) const {
        return pos == h->position;
      }
    };

    /// Deprecated: basic geant4 hit class for deprecated sensitive detectors
    /** @class Geant4Hit Geant4Hits.h DDG4/Geant4Hits.h
     *
     * Geant4 hit base class. Here only the basic
     * quantites are stored such as the energy deposit and
     * the time stamp.
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4Hit: public G4VHit {
    public:

      // cellID
      unsigned long cellID = 0;

      /// Deprecated!!!
      struct MonteCarloContrib {
        /// Geant 4 Track identifier
        int trackID = -1;
        /// Particle ID from the PDG table
        int pdgID = -1;
        /// Total energy deposit in this hit
        double deposit = 0.0;
        /// Timestamp when this energy was deposited
        double time = 0.0;
        /// Default constructor
        MonteCarloContrib() = default;
        /// Copy constructor
        MonteCarloContrib(const MonteCarloContrib& c) = default;
        /// Initializing constructor
        MonteCarloContrib(int track_id, double dep, double time_stamp)
          : trackID(track_id), pdgID(-1), deposit(dep), time(time_stamp) {}
        /// Initializing constructor
        MonteCarloContrib(int track_id, int pdg, double dep, double time_stamp)
          : trackID(track_id), pdgID(pdg), deposit(dep), time(time_stamp) {}
        /// Assignment operator
        MonteCarloContrib& operator=(const MonteCarloContrib& c) = default;
        /// Clear data
        void clear() {
          time = deposit = 0.0;
          pdgID = trackID = -1;
        }
      };
      typedef MonteCarloContrib Contribution;
      typedef std::vector<MonteCarloContrib> Contributions;

    public:
      /// Standard constructor
      Geant4Hit() = default;
      /// Default destructor
      virtual ~Geant4Hit() { }
      /// Check if the Geant4 track is a Geantino
      static bool isGeantino(G4Track* track);
      /// Extract the MC contribution for a given hit from the step information
      static Contribution extractContribution(G4Step* step);
    };

    /// Deprecated: Geant4 tracker hit class for deprecated sensitive detectors
    /** @class Geant4TrackerHit Geant4Hits.h DDG4/Geant4Hits.h
     *
     * Geant4 tracker hit class. Tracker hits contain the momentum
     * direction as well as the hit position.
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4TrackerHit: public Geant4Hit {
    public:
      /// Hit position
      Position position;
      /// Hit direction
      Direction momentum;
      /// Length of the track segment contributing to this hit
      double length;
      /// Monte Carlo / Geant4 information
      Contribution truth;
      /// Energy deposit of the hit
      double energyDeposit;

    public:
      /// Default constructor
      Geant4TrackerHit();
      /// Standard initializing constructor
      Geant4TrackerHit(int track_id, int pdg_id, double deposit, double time_stamp);
      /// Default destructor
      virtual ~Geant4TrackerHit() {}
      /// Clear hit content
      Geant4TrackerHit& clear();
      /// Store Geant4 point and step information into tracker hit structure.
      Geant4TrackerHit& storePoint(G4Step* step, G4StepPoint* point);

      /// Assignment operator
      Geant4TrackerHit& operator=(const Geant4TrackerHit& c);
      /// Geant4 required object allocator
      void *operator new(size_t);
      /// Geat4 required object destroyer
      void operator delete(void *ptr);
    };

    /// Deprecated: Geant4 calorimeter hit class for deprecated sensitive detectors
    /** @class Geant4CalorimeterHit Geant4Hits.h DDG4/Geant4Hits.h
     *
     * Geant4 calorimeter hit class. Calorimeter hits contain the momentum
     * direction as well as the hit position.
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4CalorimeterHit: public Geant4Hit {
    public:
      /// Hit position
      Position position;
      /// Hit contributions by individual particles
      Contributions truth;
      /// Total energy deposit
      double energyDeposit;
    public:
      /// Standard constructor
      Geant4CalorimeterHit(const Position& cell_pos);
      /// Default destructor
      virtual ~Geant4CalorimeterHit() { }
      /// Geant4 required object allocator
      void *operator new(size_t);
      /// Geat4 required object destroyer
      void operator delete(void *ptr);
    };

  }    // End namespace sim
}      // End namespace dd4hep

#endif // DD4HEP_GEANT4HITS_H
