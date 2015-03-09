// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4Random.h"
#include "DDG4/Geant4IsotropeGenerator.h"

using namespace std;
using namespace DD4hep::Simulation;

/// Standard constructor
Geant4IsotropeGenerator::Geant4IsotropeGenerator(Geant4Context* context, const string& name)
: Geant4ParticleGenerator(context, name)
{
  InstanceCount::increment(this);
}

/// Default destructor
Geant4IsotropeGenerator::~Geant4IsotropeGenerator() {
  InstanceCount::decrement(this);
}

/// Particle modification. Caller presets defaults to: ( direction = m_direction, momentum = m_energy)
void Geant4IsotropeGenerator::getParticleDirection(int, ROOT::Math::XYZVector& direction, double& momentum) const   {
  Geant4Event& evt = context()->event();
  Geant4Random& rnd = evt.random();
  double phi   = 2*M_PI*rnd.rndm();
  double theta = M_PI*rnd.rndm();
  double x1 = std::sin(theta)*std::cos(phi);
  double x2 = std::sin(theta)*std::sin(phi);
  double x3 = std::cos(theta);

  direction.SetXYZ(x1,x2,x3);
  momentum = rnd.rndm()*momentum;
}
