// $Id:$
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_GEANT4HITS_H
#define DD4HEP_GEANT4HITS_H

// Framework include files
#include "DD4hep/Objects.h"
#include "DDG4/Defs.h"

// Geant4 include files
#include "G4VHit.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"


/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation   {


    /** @class HitCompare Geant4Hits.h DDG4/Geant4Hits.h
     * 
     *  Base class for hit comparisons.
     *
     * @author  M.Frank
     * @version 1.0
     */
    template<class HIT> struct HitCompare {
      /// Comparison function
      virtual bool operator()(const HIT* h) const = 0;
    };

    /** @class HitPositionCompare Geant4Hits.h DDG4/Geant4Hits.h
     * 
     * Seek the hits of an arbitrary collection for the same position.
     *
     * @author  M.Frank
     * @version 1.0
     */
    template<class HIT> struct HitPositionCompare : public HitCompare<HIT> {
      const Position& pos;
      /// Constructor
      HitPositionCompare(const Position& p) : pos(p) {      }
      /// Comparison function
      virtual bool operator()(const HIT* h) const { return pos == h->position; }
    };

    /** @class Geant4Hit Geant4Hits.h DDG4/Geant4Hits.h
     * 
     * Geant4 hit base class. Here only the basic 
     * quantites are stored such as the energy deposit and
     * the time stamp.
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4Hit : public G4VHit {
    public:

      struct MonteCarloContrib {
	/// Geant 4 Track identifier
	int    trackID;
	/// Particle ID from the PDG table
	int    pdgID;
	/// Total energy deposit in this hit
	double deposit;
	/// Timestamp when this energy was deposited
	double time;
	MonteCarloContrib() : trackID(-1), pdgID(-1), deposit(0.0), time(0.0) {}
	MonteCarloContrib(int track_id, double dep, double time_stamp)
	  : trackID(track_id), pdgID(-1), deposit(dep), time(time_stamp) {}
	MonteCarloContrib(int track_id, int pdg, double dep, double time_stamp)
	  : trackID(track_id), pdgID(pdg), deposit(dep), time(time_stamp) {}
 	MonteCarloContrib(const MonteCarloContrib& c)
	  : trackID(c.trackID), pdgID(c.pdgID), deposit(c.deposit), time(c.time) {}
	void clear() {
	  time = deposit = 0.0;
	  pdgID = trackID = -1;
	}
     };
      typedef MonteCarloContrib              Contribution;
      typedef std::vector<MonteCarloContrib> Contributions;

    public:
      /// Standard constructor
      Geant4Hit()          {}
      /// Default destructor
      virtual ~Geant4Hit() {}
      /// Check if the Geant4 track is a Geantino
      static bool isGeantino(G4Track* track);
      /// Create tracker hit from step information
      static Geant4Hit* createTrackerHit(G4Step* step);
      /// Extract the MC contribution for a given hit from the step information
      static Contribution extractContribution(G4Step* step);
    };


    /** @class Geant4TrackerHit Geant4Hits.h DDG4/Geant4Hits.h
     * 
     * Geant4 tracker hit class. Tracker hits contain the momentum
     * direction as well as the hit position.
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4TrackerHit : public Geant4Hit {
    public:
      /// Hit position
      Position           position;
      /// Hit direction
      Direction          momentum;
      /// Length of the track segment contributing to this hit
      double             length;
      /// Monte Carlo / Geant4 information 
      Contribution       truth;

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

    /** @class Geant4CalorimeterHit Geant4Hits.h DDG4/Geant4Hits.h
     * 
     * Geant4 tracker hit class. Calorimeter hits contain the momentum
     * direction as well as the hit position.
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4CalorimeterHit : public Geant4Hit {
    public:
      /// Hit position
      Position      position;
      /// Hit contributions by individual particles
      Contributions truth;
      /// Total energy deposit
      double        energyDeposit;
    public:
      /// Standard constructor
      Geant4CalorimeterHit(const Position& cell_pos);
      /// Default destructor
      virtual ~Geant4CalorimeterHit() {}
      /// Geant4 required object allocator
      void *operator new(size_t);
      /// Geat4 required object destroyer
      void operator delete(void *ptr);
    };

  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_GEANT4HITS_H
