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
#include "DDG4/Geant4Primary.h"
#include "DDG4/Geant4Context.h"
#include "DDG4/Geant4PrimaryConverter.h"

#include "G4Event.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"

// C/C++ include files
#include <limits>

using namespace std;
using namespace DD4hep::Simulation;

/// Standard constructor
Geant4PrimaryConverter::Geant4PrimaryConverter(Geant4Context* context, const string& name)
  : Geant4GeneratorAction(context, name)
{
  InstanceCount::increment(this);
  declareProperty("Mask", m_mask = 1);
  m_needsControl = true;
}

/// Default destructor
Geant4PrimaryConverter::~Geant4PrimaryConverter() {
  InstanceCount::decrement(this);
}

/// Callback to generate primary particles
void Geant4PrimaryConverter::operator()(G4Event* event) {
  typedef DD4hep::ReferenceBitMask<int> PropertyMask;
  Geant4Event& evt = context()->event();
  Geant4PrimaryMap* primaries = evt.extension<Geant4PrimaryMap>();
  Geant4PrimaryEvent* prim = evt.extension<Geant4PrimaryEvent>();
  Geant4PrimaryInteraction* inter = new Geant4PrimaryInteraction();
  Geant4PrimaryInteraction* output = evt.extension<Geant4PrimaryInteraction>();
  const G4Event& e = *event;

  prim->add(m_mask, inter);
  print("+++ Particle interaction converted from Geant4 primaries:");

  for(int cnt=0, i=0, ni=e.GetNumberOfPrimaryVertex(); i<ni; ++i)  {
    G4PrimaryVertex* v4 = e.GetPrimaryVertex(i);
    Geant4Vertex* vtx = new Geant4Vertex();
    vtx->x = v4->GetX0()*mm;
    vtx->y = v4->GetY0()*mm;
    vtx->z = v4->GetZ0()*mm;
    vtx->time = v4->GetT0()*ns;
    inter->vertices.insert(make_pair(inter->vertices.size(),vtx));
    output->vertices.insert(make_pair(output->vertices.size(),vtx->addRef()));
    for(int j=0, nj=v4->GetNumberOfParticle(); j<nj; ++j)  {
      const G4PrimaryParticle* p4 = v4->GetPrimary(j);
      Geant4ParticleHandle p = new Geant4Particle(cnt);
      p->reason       = 0;
      p->usermask     = 0;
      p->pdgID        = p4->GetPDGcode();
      p->psx          = p4->GetPx()*GeV;
      p->psy          = p4->GetPy()*GeV;
      p->psz          = p4->GetPz()*GeV;
      p->time         = v4->GetT0()*ns;
      p->properTime   = v4->GetT0()*ns;
      p->vsx          = v4->GetX0()*mm;
      p->vsy          = v4->GetY0()*mm;
      p->vsz          = v4->GetZ0()*mm;
      p->vex          = v4->GetX0()*mm;
      p->vey          = v4->GetY0()*mm;
      p->vez          = v4->GetZ0()*mm;
      p->definition   = p4->GetParticleDefinition();
      p->process      = 0;
      p->spin[0]      = 0;
      p->spin[1]      = 0;
      p->spin[2]      = 0;
      p->colorFlow[0] = 0;
      p->colorFlow[0] = 0;
      p->mass         = p4->GetMass();
      p->charge       = p4->GetCharge();
      PropertyMask status(p->status);
      status.set(G4PARTICLE_GEN_CREATED);
      status.set(G4PARTICLE_GEN_STABLE);
      p.dump3(outputLevel()-1,name(),"+->");

      vtx->out.insert(p->id);
      inter->particles.insert(make_pair(p->id,p));
      output->particles.insert(make_pair(p->id,p->addRef()));
      primaries->primaryMap.insert(make_pair(p4,p->addRef()));
      cnt++;
    }
  }
}
