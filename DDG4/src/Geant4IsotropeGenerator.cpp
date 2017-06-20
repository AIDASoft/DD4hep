// $Id: $
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
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4Random.h"
#include "DDG4/Geant4IsotropeGenerator.h"

using namespace std;
using namespace dd4hep::sim;

/// Standard constructor
Geant4IsotropeGenerator::Geant4IsotropeGenerator(Geant4Context* ctxt, const string& nam)
  : Geant4ParticleGenerator(ctxt, nam)
{
  InstanceCount::increment(this);
  declareProperty("PhiMin", m_phiMin = 0.0);
  declareProperty("PhiMax", m_phiMax = 2.0*M_PI);
  declareProperty("ThetaMin", m_thetaMin = 0.0);
  declareProperty("ThetaMax", m_thetaMax = M_PI);
  declareProperty("Distribution", m_distribution = "uniform" );
}

/// Default destructor
Geant4IsotropeGenerator::~Geant4IsotropeGenerator() {
  InstanceCount::decrement(this);
}

/// Uniform particle distribution
void Geant4IsotropeGenerator::getParticleDirectionUniform(int, ROOT::Math::XYZVector& direction, double& momentum) const   {
  Geant4Event&  evt = context()->event();
  Geant4Random& rnd = evt.random();
  double phi   = m_phiMin+(m_phiMax-m_phiMin)*rnd.rndm();
  double theta = m_thetaMin+(m_thetaMax-m_thetaMin)*rnd.rndm();
  double x1 = std::sin(theta)*std::cos(phi);
  double x2 = std::sin(theta)*std::sin(phi);
  double x3 = std::cos(theta);

  direction.SetXYZ(x1,x2,x3);
  momentum = rnd.rndm()*momentum;
}

/// Particle distribution ~ cos(theta)
void Geant4IsotropeGenerator::getParticleDirectionCosTheta(int, ROOT::Math::XYZVector& direction, double& momentum) const   {
  Geant4Event&  evt = context()->event();
  Geant4Random& rnd = evt.random();
  double phi       = m_phiMin+(m_phiMax-m_phiMin)*rnd.rndm();
  double cos_theta = std::cos(m_thetaMin)+(std::cos(m_thetaMax)-std::cos(m_thetaMin))*rnd.rndm();
  double sin_theta = std::sqrt(1.0-cos_theta*cos_theta);
  double x1 = sin_theta*std::cos(phi);
  double x2 = sin_theta*std::sin(phi);
  double x3 = cos_theta;

  direction.SetXYZ(x1,x2,x3);
  momentum = rnd.rndm()*momentum;
}

/// Particle distribution flat in eta (pseudo rapidity)
void Geant4IsotropeGenerator::getParticleDirectionEta(int, ROOT::Math::XYZVector& direction, double& momentum) const   {
  struct Distribution {
    static double eta(double x)  { return -1.0*std::log(std::tan(x/2.0)); }
  };
  Geant4Event&  evt = context()->event();
  Geant4Random& rnd = evt.random();
  // See https://en.wikipedia.org/wiki/Pseudorapidity
  const double dmin = std::numeric_limits<double>::epsilon();
  double phi        = m_phiMin+(m_phiMax-m_phiMin)*rnd.rndm();
  double eta_min    = Distribution::eta(m_thetaMin>dmin ? m_thetaMin : dmin);
  double eta_max    = Distribution::eta(m_thetaMax>(M_PI-dmin) ? m_thetaMax : M_PI-dmin);
  double eta        = eta_min + (eta_max-eta_min)*rnd.rndm();
  double x1         = std::cos(phi);
  double x2         = std::sin(phi);
  double x3         = std::sinh(eta);
  double r          = std::sqrt(1.0+x3*x3);
  direction.SetXYZ(x1/r,x2/r,x3/r);
  momentum = rnd.rndm()*momentum;
}

/// e+e- --> ffbar particle distribution ~ 1 + cos^2(theta)
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
    double dist  = Distribution::ffbar(theta);
    if ( dice <= dist )   {
      double x1 = std::sin(theta)*std::cos(phi);
      double x2 = std::sin(theta)*std::sin(phi);
      double x3 = std::cos(theta);
      direction.SetXYZ(x1,x2,x3);
      momentum = rnd.rndm()*momentum;
      return;
    }
  }
}

/// Particle modification. Caller presets defaults to: ( direction = m_direction, momentum = m_energy)
void Geant4IsotropeGenerator::getParticleDirection(int num, ROOT::Math::XYZVector& direction, double& momentum) const   {
  switch(::toupper(m_distribution[0]))  {
  case 'C':  // cos(theta)
    return getParticleDirectionCosTheta(num, direction, momentum);
  case 'F':  // ffbar: ~ 1 + cos^2(theta)
    return getParticleDirectionFFbar(num, direction, momentum);
  case 'E':  // eta, rapidity, pseudorapidity
  case 'P':
  case 'R':
    return getParticleDirectionEta(num, direction, momentum);
  case 'U':  // uniform
    return getParticleDirectionUniform(num, direction, momentum);
  default:
    break;
  }
  except("Unknown distribution densitiy: %s. Cannot generate primaries.",
         m_distribution.c_str());
}
