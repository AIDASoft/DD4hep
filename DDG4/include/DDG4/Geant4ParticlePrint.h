// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4PARTICLEPRINT_H
#define DD4HEP_DDG4_GEANT4PARTICLEPRINT_H

// Framework include files
#include "DDG4/Geant4EventAction.h"
#include "DDG4/Geant4MonteCarloTruth.h"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {

    /** Geant4Action to collect the MC particle information.
     *
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4ParticlePrint : public Geant4EventAction    {
    public:
      typedef Geant4MonteCarloTruth::ParticleMap ParticleMap;
      typedef Geant4MonteCarloTruth::TrackEquivalents TrackEquivalents;
    protected:
      /// Property: Flag to indicate output type: 1: TABLE, 2:TREE, 3:BOTH (default)
      int m_outputType;

      void printParticle(const std::string& prefix, const Particle* p) const;
      /// Print record of kept particles
      void printParticles(const ParticleMap& particles) const;
      /// Print tree of kept particles
      void printParticleTree(const ParticleMap& particles, int level, const Particle* p)  const;
      /// Print tree of kept particles
      void printParticleTree(const ParticleMap& particles)  const;


    public:
      /// Standard constructor
      Geant4ParticlePrint(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4ParticlePrint();
      /// Pre-event action callback
      virtual void begin(const G4Event* event);
      /// Post-event action callback
      virtual void end(const G4Event* event);
    };
  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4PARTICLEPRINT_H
