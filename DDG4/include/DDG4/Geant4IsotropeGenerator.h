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
#include <DDG4/Geant4ParticleGenerator.h>

// C/C++ include files
#include <array>
#include <cstdint>
#include <functional>
#include <mutex>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    // Forward declarations
    class Geant4Random;

    /// Generate particles isotrop in space around origine (0,0,0)
    /**
     *  Supports both standard PRNG-based sampling and Randomized Quasi-Monte Carlo
     *  sampling via scrambled Halton sequences (Cranley-Patterson rotation).
     *
     *  When Halton mode is enabled (property "Halton" = true), the three sampling
     *  dimensions use low-discrepancy Halton sequences in bases 2 (phi), 3 (theta),
     *  and 5 (momentum), each additively scrambled by a per-dimension shift derived
     *  from the Geant4Random seed.  This provides superior phase-space coverage
     *  relative to a PRNG while remaining reproducible and independent across jobs
     *  (using different seeds or "HaltonOffset" values).
     *
     *  Note: the "ffbar" distribution is incompatible with Halton mode because its
     *  acceptance-rejection loop cannot be driven by a fixed per-particle Halton point.
     *
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
      /// Property: Enable scrambled Halton sequence sampling (RQMC mode)
      bool        m_halton;
      /// Property: Starting index in the Halton sequence (use for parallel-job partitioning)
      uint64_t    m_haltonOffset;
      /// Current Halton sequence index (incremented per generated particle)
      mutable uint64_t m_haltonIndex;
      /// Per-dimension additive scramble shifts in [0,1), sampled from Geant4Random on first use
      mutable std::array<double,3> m_haltonShift;
      /// Ensures initHalton() runs exactly once across all events
      mutable std::once_flag m_haltonOnce;

      /// Initialize Cranley-Patterson shifts using @a rnd; set m_haltonIndex = m_haltonOffset
      void initHalton(Geant4Random& rnd) const;
      /// Compute the radical inverse of @a index in the given @a base (standard Halton value)
      static double haltonValue(uint64_t index, int base);
      /// Return the scrambled Halton sample for dimension @a dim (0=phi,1=theta,2=momentum)
      double haltonScrambled(uint64_t index, unsigned int dim) const;
      /// Build and return a per-particle sampler: either PRNG or Halton, depending on m_halton.
      /// For Halton mode, advances m_haltonIndex by one.
      std::function<double(unsigned int)> makeSampler(Geant4Random& rnd) const;
      /// Sample momentum from [m_momentumMin, m_momentumMax] using a pre-computed [0,1) value @a h
      void sampleMomentum(double h, double& momentum) const;

      /// Particle modification. Caller presets defaults to: ( direction = m_direction,  momentum = [m_momentumMin, m_momentumMax])
      /** Use this function to implement isotrop guns, multiple guns etc. 
          User must return a UNIT vector, which gets scaled with momentum.
      */
      virtual void getParticleDirection(int num, ROOT::Math::XYZVector& direction, double& momentum) const  override;
      /// e+e- --> ffbar particle distribution ~ 1 + cos^2(theta) (PRNG only; incompatible with Halton)
      void getParticleDirectionFFbar(int num, ROOT::Math::XYZVector& direction, double& momentum) const;
      /// Flat pseudorapidity (eta) distribution
      void getParticleDirectionEta(int num, const std::function<double(unsigned int)>& sample, ROOT::Math::XYZVector& direction, double& momentum) const;
      /// Particle distribution ~ cos(theta)
      void getParticleDirectionCosTheta(int num, const std::function<double(unsigned int)>& sample, ROOT::Math::XYZVector& direction, double& momentum) const;
      /// Uniform particle distribution
      void getParticleDirectionUniform(int num, const std::function<double(unsigned int)>& sample, ROOT::Math::XYZVector& direction, double& momentum) const;

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
