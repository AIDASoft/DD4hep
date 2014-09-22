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
#include "DDG4/Geant4GeneratorAction.h"
#include "DDG4/Geant4Particle.h"

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
    class Geant4ParticlePrint : public Geant4EventAction	{
    public:
      typedef Geant4ParticleMap::Particle Particle;
      typedef Geant4ParticleMap::ParticleMap ParticleMap;
      typedef Geant4ParticleMap::TrackEquivalents TrackEquivalents;
    protected:
      /// Property: Flag to indicate output type: 1: TABLE, 2:TREE, 3:BOTH (default)
      int m_outputType;
      /// Property: Flag to indicate output type at begin of event
      bool m_printBegin;
      /// Property: Flag to indicate output type at end of event
      bool m_printEnd;
      /// Property: Flag to indicate output type as part of the generator action
      bool m_printGeneration;

      void printParticle(const std::string& prefix, Geant4ParticleHandle p) const;
      /// Print record of kept particles
      void printParticles(const ParticleMap& particles) const;
      /// Print tree of kept particles
      void printParticleTree(const ParticleMap& particles, int level, Geant4ParticleHandle p)  const;
      /// Print tree of kept particles
      void printParticleTree(const ParticleMap& particles)  const;
      /// Print particle table
      void makePrintout(int event_id)  const;


    public:
      /// Standard constructor
      Geant4ParticlePrint(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4ParticlePrint();
      /// Pre-event action callback
      virtual void begin(const G4Event* event);
      /// Post-event action callback
      virtual void end(const G4Event* event);
      /// Generation action callback
      virtual void operator()(G4Event* event);
      
    };
  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4PARTICLEPRINT_H
