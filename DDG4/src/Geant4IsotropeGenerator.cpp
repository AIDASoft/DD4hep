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
#include "DDG4/Geant4Context.h"
#include "DDG4/Geant4Primary.h"
#include "DDG4/Geant4IsotropeGenerator.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"

// C/C++ include files
#include <stdexcept>
#include <cmath>

using namespace std;
using namespace DD4hep::Simulation;

/// Standard constructor
Geant4IsotropeGenerator::Geant4IsotropeGenerator(Geant4Context* context, const string& name)
  : Geant4GeneratorAction(context, name), m_particle(0)
{
  InstanceCount::increment(this);
  m_needsControl = true;
  declareProperty("particle",      m_particleName = "e-");
  declareProperty("energy",        m_energy = 50 * MeV);
  declareProperty("multiplicity",  m_multiplicity = 1);
  declareProperty("mask",          m_mask = 0);
}

/// Default destructor
Geant4IsotropeGenerator::~Geant4IsotropeGenerator() {
  InstanceCount::decrement(this);
}

/// Callback to generate primary particles
void Geant4IsotropeGenerator::operator()(G4Event*) {
  typedef Geant4Particle Particle;

  if (0 == m_particle || m_particle->GetParticleName() != m_particleName.c_str()) {
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    m_particle = particleTable->FindParticle(m_particleName);
    if (0 == m_particle) {
      throw runtime_error("Geant4IsotropeGenerator: Bad particle type:"+m_particleName+"!");
    }
  }
  Geant4Event& evt = context()->event();
  Geant4Random& rnd = evt.random();
  Geant4PrimaryEvent* prim = evt.extension<Geant4PrimaryEvent>();
  Geant4PrimaryInteraction* inter = new Geant4PrimaryInteraction();
  prim->add(m_mask, inter);

  Geant4Vertex* vtx = new Geant4Vertex();
  inter->vertices.insert(make_pair(inter->vertices.size(),vtx));
  for(int i=0; i<m_multiplicity; ++i)   {
    double phi = 2*M_PI*rnd.rndm();
    double theta = M_PI*rnd.rndm();
    double x1 = std::sin(theta)*std::cos(phi);
    double x2 = std::sin(theta)*std::sin(phi);
    double x3 = std::cos(theta);
    double momentum = rnd.rndm()*m_energy;

    Particle* p = new Particle();
    p->id         = inter->nextPID();
    p->reason    |= G4PARTICLE_PRIMARY;
    p->status    |= G4PARTICLE_GEN_CREATED;
    p->status    |= G4PARTICLE_GEN_STABLE;
    p->usermask   = m_mask;
    p->reason     = G4PARTICLE_PRIMARY;
    p->pdgID      = m_particle->GetPDGEncoding();
    p->definition = m_particle;
    p->psx        = x1*momentum;
    p->psy        = x2*momentum;
    p->psz        = x3*momentum;
    p->mass       = m_particle->GetPDGMass();
    inter->particles.insert(make_pair(p->id,p));
    vtx->out.insert(p->id);
    printout(INFO,name(),"Particle [%d] %s %.3f GeV direction:(%6.3f %6.3f %6.3f)",
	     p->id, m_particleName.c_str(), momentum/GeV, x1, x2, x3);

  }
}
