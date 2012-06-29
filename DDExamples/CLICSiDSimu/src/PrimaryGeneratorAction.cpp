#include "PrimaryGeneratorAction.h"
#include "DD4hep/Volumes.h"
#include "TGeoBBox.h"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "Randomize.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction(const DD4hep::Geometry::LCDD& DC)
  : Detector(DC),rndmFlag("off")
{
  G4int n_particle = 1;
  particleGun  = new G4ParticleGun(n_particle);
  
  // default particle kinematic

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName;
  G4ParticleDefinition* particle
                    = particleTable->FindParticle(particleName="e-");
  particleGun->SetParticleDefinition(particle);
  particleGun->SetParticleMomentumDirection(G4ThreeVector(1.,0.,0.));
  particleGun->SetParticleEnergy(50.*MeV);
  DD4hep::Geometry::Box worldbox = Detector.worldVolume().solid();
  G4double position = -0.5*(worldbox->GetDX());
  particleGun->SetParticlePosition(G4ThreeVector(position,0.*cm,0.*cm));

}

PrimaryGeneratorAction::~PrimaryGeneratorAction()  {
  delete particleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
  //this function is called at the begining of event
  // 
  G4double x0 = 0.*cm;
  G4double y0 = 0.*cm;
  G4double z0 = 0.*cm;
  particleGun->SetParticlePosition(G4ThreeVector(x0,y0,z0));
  particleGun->GeneratePrimaryVertex(anEvent);
}
