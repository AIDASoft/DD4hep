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
#include "DDG4/Geant4Context.h"
#include "DDG4/Geant4Random.h"
#include "DDG4/Geant4Primary.h"
#include "DDG4/Geant4ParticleGun.h"
#include "DDG4/Geant4InputHandling.h"
#include "CLHEP/Units/SystemOfUnits.h"

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
    m_particle(0), m_shotNo(0)
{
  InstanceCount::increment(this);
  m_needsControl = true;
  declareProperty("particle", m_particleName = "e-");
  declareProperty("energy", m_energy = 50 * MeV);
  declareProperty("multiplicity", m_multiplicity = 1);
  declareProperty("position", m_position);
  declareProperty("direction", m_direction);
  declareProperty("isotrop", m_isotrop = false);
  declareProperty("Mask", m_mask = 1);
  declareProperty("Standalone", m_standalone = true);
}

/// Default destructor
Geant4ParticleGun::~Geant4ParticleGun() {
  InstanceCount::decrement(this);
}

/// Callback to generate primary particles
void Geant4ParticleGun::operator()(G4Event* event)   {
  typedef DD4hep::ReferenceBitMask<int> PropertyMask;
  if ( m_standalone )  {
    generationInitialization(this,context());
  }

  Geant4Event& evt = context()->event();
  Geant4PrimaryEvent* prim = evt.extension<Geant4PrimaryEvent>();
  if (0 == m_particle || m_particle->GetParticleName() != m_particleName.c_str()) {
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    m_particle = particleTable->FindParticle(m_particleName);
    if (0 == m_particle) {
      throw runtime_error("Geant4ParticleGun: Bad particle type:"+m_particleName+"!");
    }
  }
  if ( m_isotrop )   {
    Geant4Random& rnd = context()->event().random();
    double phi = 2*M_PI*rnd.rndm();
    double theta = M_PI*rnd.rndm();
    double x1 = std::sin(theta)*std::cos(phi);
    double x2 = std::sin(theta)*std::sin(phi);
    double x3 = std::cos(theta);
    m_direction.SetXYZ(x1,x2,x3);
  }
  else  {
    double r = m_direction.R(), eps = numeric_limits<float>::epsilon();
    if ( r > eps )  {
      m_direction.SetXYZ(m_direction.X()/r, m_direction.Y()/r, m_direction.Z()/r);
    }
  }
  print("Shoot [%d] %.3f GeV %s pos:(%.3f %.3f %.3f)[mm] dir:(%6.3f %6.3f %6.3f)",
	m_shotNo, m_energy/GeV, m_particleName.c_str(),
	m_position.X(), m_position.Y(), m_position.Z(),
	m_direction.X(),m_direction.Y(), m_direction.Z());

  Geant4PrimaryInteraction* inter = new Geant4PrimaryInteraction();
  Geant4Vertex* vtx = new Geant4Vertex();
  inter->vertices.insert(make_pair(m_mask,vtx));
  prim->add(m_mask, inter);

  vtx->x = m_position.X();
  vtx->y = m_position.Y();
  vtx->z = m_position.Z();
  for(int i=0; i<m_multiplicity; ++i)    {
    Geant4ParticleHandle p = new Geant4Particle(i);
    p->reason       = 0;
    p->pdgID        = m_particle->GetPDGEncoding();
    p->psx          = m_direction.X()*m_energy;
    p->psy          = m_direction.Y()*m_energy;
    p->psz          = m_direction.Z()*m_energy;
    p->time         = 0;
    p->properTime   = 0;
    p->vsx          = vtx->x;
    p->vsy          = vtx->y;
    p->vsz          = vtx->z;
    p->vex          = vtx->x;
    p->vey          = vtx->y;
    p->vez          = vtx->z;
    //p->definition   = m_particle;
    //p->process      = 0;
    p->spin[0]      = 0;
    p->spin[1]      = 0;
    p->spin[2]      = 0;
    p->colorFlow[0] = 0;
    p->colorFlow[0] = 0;
    p->mass         = m_particle->GetPDGMass();
    p->charge       = m_particle->GetPDGCharge();
    PropertyMask status(p->status);
    status.set(G4PARTICLE_GEN_STABLE);
    vtx->out.insert(p->id);
    inter->particles.insert(make_pair(p->id,p));
    p.dumpWithVertex(outputLevel()-1,name(),"+->");
  }
  ++m_shotNo;

  if ( m_standalone ) {
    mergeInteractions(this,context());
    generatePrimaries(this,context(),event);
  }
}
