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

// Framework include files
#include <DD4hep/Printout.h>
#include <DD4hep/InstanceCount.h>
#include <DDG4/Geant4Random.h>
#include <DDG4/Geant4IsotropeGenerator.h>

using namespace dd4hep::sim;

/// Standard constructor
Geant4IsotropeGenerator::Geant4IsotropeGenerator(Geant4Context* ctxt, const std::string& nam)
  : Geant4ParticleGenerator(ctxt, nam)
{
  InstanceCount::increment(this);
  declareProperty("PhiMin", m_phiMin = 0.0);
  declareProperty("PhiMax", m_phiMax = 2.0*M_PI);
  declareProperty("ThetaMin", m_thetaMin = 0.0);
  declareProperty("ThetaMax", m_thetaMax = M_PI);
  declareProperty("Distribution", m_distribution = "uniform" );
  declareProperty("Halton", m_halton = false);
  declareProperty("HaltonOffset", m_haltonOffset = 0ULL);
  m_haltonIndex = 0;
  m_haltonShift = {0.0, 0.0, 0.0};
}

/// Default destructor
Geant4IsotropeGenerator::~Geant4IsotropeGenerator() {
  InstanceCount::decrement(this);
}

/// Initialize Cranley-Patterson shifts using rnd; set m_haltonIndex = m_haltonOffset
void Geant4IsotropeGenerator::initHalton(Geant4Random& rnd) const  {
  for (auto& s : m_haltonShift) s = rnd.rndm();
  m_haltonIndex = m_haltonOffset;
}

/// Compute the radical inverse of index in the given base (Halton value in [0,1))
double Geant4IsotropeGenerator::haltonValue(uint64_t index, int base)  {
  uint64_t num   = 0;
  uint64_t denom = 1;
  while ( index > 0 )  {
    denom *= base;
    num    = num * base + (index % base);
    index /= base;
  }
  return static_cast<double>(num) / static_cast<double>(denom);
}

/// Cranley-Patterson scrambled Halton sample for dimension dim (0=phi, 1=theta, 2=momentum)
double Geant4IsotropeGenerator::haltonScrambled(uint64_t index, unsigned int dim) const  {
  static constexpr std::array<int,3> bases = {2, 3, 5};
  if ( dim >= bases.size() )  {
    except("haltonScrambled: dimension %u out of range [0,%zu].", dim, bases.size()-1);
  }
  return std::fmod(haltonValue(index, bases[dim]) + m_haltonShift[dim], 1.0);
}

/// Build a per-particle sampler: PRNG or Halton depending on m_halton
std::function<double(unsigned int)> Geant4IsotropeGenerator::makeSampler(Geant4Random& rnd) const  {
  if ( m_halton )  {
    std::call_once(m_haltonOnce, &Geant4IsotropeGenerator::initHalton, this, std::ref(rnd));
    uint64_t idx = m_haltonIndex++;
    return [this, idx](unsigned int dim) { return haltonScrambled(idx, dim); };
  }
  return [&rnd](unsigned int) { return rnd.rndm(); };
}

/// Sample momentum from [m_momentumMin, m_momentumMax] using a pre-computed [0,1) value h
void Geant4IsotropeGenerator::sampleMomentum(double h, double& momentum) const  {
  if ( m_energy != -1 )  {
    momentum = m_energy;
    return;
  }
  if ( m_momentumMax < m_momentumMin )
    momentum = m_momentumMin + (momentum - m_momentumMin) * h;
  else
    momentum = m_momentumMin + (m_momentumMax - m_momentumMin) * h;
}

/// Uniform particle distribution
void Geant4IsotropeGenerator::getParticleDirectionUniform(int, const std::function<double(unsigned int)>& sample,
                                                          ROOT::Math::XYZVector& direction, double& momentum) const  {
  double phi   = m_phiMin   + (m_phiMax   - m_phiMin)   * sample(0);
  double theta = m_thetaMin + (m_thetaMax - m_thetaMin) * sample(1);
  direction.SetXYZ(std::sin(theta)*std::cos(phi), std::sin(theta)*std::sin(phi), std::cos(theta));
  sampleMomentum(sample(2), momentum);
}

/// Particle distribution ~ cos(theta)
void Geant4IsotropeGenerator::getParticleDirectionCosTheta(int, const std::function<double(unsigned int)>& sample,
                                                           ROOT::Math::XYZVector& direction, double& momentum) const  {
  double phi       = m_phiMin + (m_phiMax - m_phiMin) * sample(0);
  double cos_theta = std::cos(m_thetaMin) + (std::cos(m_thetaMax) - std::cos(m_thetaMin)) * sample(1);
  double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);
  direction.SetXYZ(sin_theta*std::cos(phi), sin_theta*std::sin(phi), cos_theta);
  sampleMomentum(sample(2), momentum);
}

/// Particle distribution flat in eta (pseudo rapidity)
void Geant4IsotropeGenerator::getParticleDirectionEta(int, const std::function<double(unsigned int)>& sample,
                                                      ROOT::Math::XYZVector& direction, double& momentum) const  {
  struct Distribution {
    static double eta(double x)  { return -1.0*std::log(std::tan(x/2.0)); }
  };
  const double dmin = std::numeric_limits<double>::epsilon();
  double phi        = m_phiMin + (m_phiMax - m_phiMin) * sample(0);
  double eta_max    = Distribution::eta(m_thetaMin > dmin ? m_thetaMin : dmin);
  double eta_min    = Distribution::eta(m_thetaMax > (M_PI - dmin) ? M_PI - dmin : m_thetaMax);
  double eta        = eta_min + (eta_max - eta_min) * sample(1);
  double x1         = std::cos(phi);
  double x2         = std::sin(phi);
  double x3         = std::sinh(eta);
  double r          = std::sqrt(1.0+x3*x3);
  direction.SetXYZ(x1/r,x2/r,x3/r);
  sampleMomentum(sample(2), momentum);
}

/// e+e- --> ffbar particle distribution ~ 1 + cos^2(theta) (PRNG only; incompatible with Halton)
void Geant4IsotropeGenerator::getParticleDirectionFFbar(int, ROOT::Math::XYZVector& direction, double& momentum) const   {
  struct Distribution {
    static double ffbar(double x)  { double c = std::cos(x); return 1 + c*c; }
    //static double integral(double x)  { return 1.5*x + sin(2.*x)/4.0; }
  };
  Geant4Event&  evt = context()->event();
  Geant4Random& rnd = evt.random();
  double phi = m_phiMin+(m_phiMax-m_phiMin)*rnd.rndm();

  // 1 + cos^2(theta) cannot be integrated and then inverted.
  // We have to throw the dice until it fits.....
  double v1 = Distribution::ffbar(m_thetaMin), v2 = Distribution::ffbar(m_thetaMax);
  double vmax = std::max(v1,v2); // ffbar symmetric and monotonic in |x|.
  while(1)  {
    double dice  = rnd.rndm()*vmax;
    double theta = m_thetaMin+(m_thetaMax-m_thetaMin)*rnd.rndm();
    if ( dice <= Distribution::ffbar(theta) )  {
      direction.SetXYZ(std::sin(theta)*std::cos(phi), std::sin(theta)*std::sin(phi), std::cos(theta));
      sampleMomentum(rnd.rndm(), momentum);
      return;
    }
  }
}

/// Particle modification. Caller presets defaults to: ( direction = m_direction, momentum = [mMin, mMax])
void Geant4IsotropeGenerator::getParticleDirection(int num, ROOT::Math::XYZVector& direction, double& momentum) const   {
  if ( m_halton && ::toupper(m_distribution[0]) == 'F' )  {
    except("Halton mode is incompatible with the 'ffbar' distribution: "
           "acceptance-rejection cannot be driven by a fixed per-particle Halton point. "
           "Use distribution='uniform', 'cos(theta)', or 'eta' with Halton mode.");
  }
  Geant4Event&  evt    = context()->event();
  Geant4Random& rnd    = evt.random();
  auto          sample = makeSampler(rnd);
  switch(::toupper(m_distribution[0]))  {
  case 'C':  // cos(theta)
    return getParticleDirectionCosTheta(num, sample, direction, momentum);
  case 'F':  // ffbar: ~ 1 + cos^2(theta)
    return getParticleDirectionFFbar(num, direction, momentum);
  case 'E':  // eta, rapidity, pseudorapidity
  case 'P':
  case 'R':
    return getParticleDirectionEta(num, sample, direction, momentum);
  case 'U':  // uniform
    return getParticleDirectionUniform(num, sample, direction, momentum);
  default:
    break;
  }
  except("Unknown distribution density: %s. Cannot generate primaries.",
         m_distribution.c_str());
}
