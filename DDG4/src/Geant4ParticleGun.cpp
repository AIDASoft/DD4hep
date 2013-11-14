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

using namespace DD4hep::Simulation;

/// Standard constructor
Geant4ParticleGun::Geant4ParticleGun(Geant4Context* context, const std::string& name)
    : Geant4GeneratorAction(context, name), m_particle(0), m_gun(0) {
  InstanceCount::increment(this);
  m_needsControl = true;
  declareProperty("energy", m_energy = 50 * MeV);
  declareProperty("multiplicity", m_multiplicity = 1);
  declareProperty("pos_x", m_position.x = 0);
  declareProperty("pos_y", m_position.y = 0);
  declareProperty("pos_z", m_position.z = 0);
  declareProperty("direction_x", m_direction.x = 1);
  declareProperty("direction_y", m_direction.y = 1);
  declareProperty("direction_z", m_direction.z = 0.3);
  declareProperty("particle", m_particleName = "e-");
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
      throw std::runtime_error("Bad particle type!");
    }
  }
  m_gun->SetParticleDefinition(m_particle);
  m_gun->SetParticleEnergy(m_energy);
  m_gun->SetParticleMomentumDirection(G4ThreeVector(m_direction.x, m_direction.y, m_direction.z));
  m_gun->SetParticlePosition(G4ThreeVector(m_position.x, m_position.y, m_position.z));
  m_gun->GeneratePrimaryVertex(event);
}
