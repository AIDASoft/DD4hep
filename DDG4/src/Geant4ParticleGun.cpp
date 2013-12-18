// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4ParticleGun.h"

#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"

// C/C++ include files
#include <stdexcept>
#include <limits>
#include <cmath>

using namespace std;
using namespace DD4hep::Simulation;

/// Standard constructor
Geant4ParticleGun::Geant4ParticleGun(Geant4Context* context, const string& name)
  : Geant4GeneratorAction(context, name), m_position(0,0,0), m_direction(1,1,0.3),
    m_particle(0), m_gun(0) 
{
  InstanceCount::increment(this);
  m_needsControl = true;
  declareProperty("particle", m_particleName = "e-");
  declareProperty("energy", m_energy = 50 * MeV);
  declareProperty("multiplicity", m_multiplicity = 1);
  declareProperty("position", m_position);
  declareProperty("direction", m_direction);
}

/// Default destructor
Geant4ParticleGun::~Geant4ParticleGun() {
  if (m_gun)
    delete m_gun;
  InstanceCount::decrement(this);
}

/// Callback to generate primary particles
void Geant4ParticleGun::operator()(G4Event* event) {
  if (0 == m_gun) {
    m_gun = new G4ParticleGun(m_multiplicity);
  }
  if (0 == m_particle || m_particle->GetParticleName() != m_particleName.c_str()) {
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    m_particle = particleTable->FindParticle(m_particleName);
    if (0 == m_particle) {
      throw runtime_error("Bad particle type!");
    }
  }
  double r = m_direction.R(), eps = numeric_limits<float>::epsilon();
  if ( r > eps && std::fabs(r-1.0) > eps )  {
    m_direction.SetXYZ(m_direction.X()/r, m_direction.Y()/r, m_direction.Z()/r);
  }
  m_gun->SetParticleDefinition(m_particle);
  m_gun->SetParticleEnergy(m_energy);
  m_gun->SetParticleMomentumDirection(G4ThreeVector(m_direction.X(), m_direction.Y(), m_direction.Z()));
  m_gun->SetParticlePosition(G4ThreeVector(m_position.X(), m_position.Y(), m_position.Z()));
  m_gun->GeneratePrimaryVertex(event);
}
