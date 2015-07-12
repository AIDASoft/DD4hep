// $Id$
//==========================================================================
//  AIDA Detector description implementation for LCD
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

#ifndef DD4HEP_DDG4_GEANT4PARTICLEPRINT_H
#define DD4HEP_DDG4_GEANT4PARTICLEPRINT_H

// Framework include files
#include "DDG4/Geant4EventAction.h"
#include "DDG4/Geant4GeneratorAction.h"
#include "DDG4/Geant4Particle.h"

// Forward declarations
class G4Event;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    /// Geant4Action to print MC particle information.
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4ParticlePrint : public Geant4EventAction        {
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
      /// Property: Flag to indicate output of hit data in tree
      bool m_printHits;

      void printParticle(const std::string& prefix, const G4Event* e, Geant4ParticleHandle p) const;
      /// Print record of kept particles
      void printParticles(const G4Event* e, const ParticleMap& particles) const;
      /// Print tree of kept particles
      void printParticleTree(const G4Event* e, const ParticleMap& particles, int level, Geant4ParticleHandle p)  const;
      /// Print tree of kept particles
      void printParticleTree(const G4Event* e, const ParticleMap& particles)  const;
      /// Print particle table
      void makePrintout(const G4Event* e)  const;


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
