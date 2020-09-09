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

#ifndef DDG4_GEANT4ISOTROPEGENERATOR_H
#define DDG4_GEANT4ISOTROPEGENERATOR_H

// Framework include files
#include "DDG4/Geant4ParticleGenerator.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {
    /// Generate particles isotrop in space around origine (0,0,0)
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4IsotropeGenerator: public Geant4ParticleGenerator {
    protected:
      /// Property: Distribution name. Default: "uniform". Allowed: "uniform", "cos(theta)", "ffbar", "eta"
      std::string m_distribution;
      /// Property: Minimal phi angular value
      double      m_phiMin;
      /// Property: Maximal phi angular value
      double      m_phiMax;
      /// Property: Minimal theta angular value
      double      m_thetaMin;
      /// Property: Maximal theta angular value
      double      m_thetaMax;
      
      /// Particle modification. Caller presets defaults to: ( direction = m_direction, momentum = m_energy)
      /** Use this function to implement isotrop guns, multiple guns etc. 
          User must return a UNIT vector, which gets scaled with momentum.
      */
      virtual void getParticleDirection(int num, ROOT::Math::XYZVector& direction, double& momentum) const;
      /// e+e- --> ffbar particle distribution ~ 1 + cos^2(theta)
      void getParticleDirectionFFbar(int num, ROOT::Math::XYZVector& direction, double& momentum) const;
      /// e+e- --> ffbar particle distribution ~ 1 + cos^2(theta)
      void getParticleDirectionEta(int num, ROOT::Math::XYZVector& direction, double& momentum) const;
      /// Particle distribution ~ cos(theta)
      void getParticleDirectionCosTheta(int num, ROOT::Math::XYZVector& direction, double& momentum) const;
      /// Uniform particle distribution
      void getParticleDirectionUniform(int num, ROOT::Math::XYZVector& direction, double& momentum) const;

    public:
      /// Inhibit default constructor
      Geant4IsotropeGenerator() = delete;
      /// Inhibit copy constructor
      Geant4IsotropeGenerator(const Geant4IsotropeGenerator& copy) = delete;
      /// Standard constructor
      Geant4IsotropeGenerator(Geant4Context* context, const std::string& name);
      /// Default destructor
      virtual ~Geant4IsotropeGenerator();
    };
  }    // End namespace sim
}      // End namespace dd4hep
#endif // DDG4_GEANT4ISOTROPEGENERATOR_H
