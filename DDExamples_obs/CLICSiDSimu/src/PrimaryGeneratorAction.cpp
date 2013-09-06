#include "PrimaryGeneratorAction.h"
#include "DD4hep/Volumes.h"
#include "TGeoBBox.h"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "Randomize.hh"

using namespace DD4hep::Geometry;

PrimaryGeneratorAction::PrimaryGeneratorAction(const LCDD& DC)
  : m_detector(DC),m_rndmFlag("off")
{
  G4int n_particle = 1;
  m_gun  = new G4ParticleGun(n_particle);
  
  // default particle kinematic

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName;
  G4ParticleDefinition* part = particleTable->FindParticle(particleName="e-");
  m_gun->SetParticleDefinition(part);
  m_gun->SetParticleMomentumDirection(G4ThreeVector(1.,0.,0.));
  m_gun->SetParticleEnergy(50.*MeV);
  Box worldbox = m_detector.worldVolume().solid();
  G4double position = -0.5*(worldbox->GetDX());
  m_gun->SetParticlePosition(G4ThreeVector(position,0.*cm,0.*cm));
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()  {
  delete m_gun;
  m_gun = 0;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* evt) {
  //this function is called at the begining of event
  // 
  G4double x0 = 0.*cm;
  G4double y0 = 0.*cm;
  G4double z0 = 0.*cm;
  m_gun->SetParticlePosition(G4ThreeVector(x0,y0,z0));
  m_gun->GeneratePrimaryVertex(evt);
}
