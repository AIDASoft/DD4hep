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
#include "DDG4/Geant4InteractionVertexSmear.h"
#include "CLHEP/Units/SystemOfUnits.h"

// C/C++ include files
#include <stdexcept>
#include <cmath>

using namespace std;
using namespace DD4hep::Simulation;

/// Standard constructor
Geant4InteractionVertexSmear::Geant4InteractionVertexSmear(Geant4Context* context, const string& name)
  : Geant4GeneratorAction(context, name)
{
  InstanceCount::increment(this);
  declareProperty("Offset", m_offset);
  declareProperty("Sigma",  m_sigma);
  declareProperty("Mask",   m_mask = 0);
  m_needsControl = true;
}

/// Default destructor
Geant4InteractionVertexSmear::~Geant4InteractionVertexSmear() {
  InstanceCount::decrement(this);
}

/// Callback to generate primary particles
void Geant4InteractionVertexSmear::operator()(G4Event*) {
  typedef Geant4PrimaryEvent::Interaction Interaction;
  typedef Geant4Particle Particle;
  typedef Geant4Particle Vertex;
  Geant4Random& rndm = context()->event().random();
  Geant4PrimaryEvent* evt = context()->event().extension<Geant4PrimaryEvent>();
  Interaction* inter = evt->get(m_mask);

  if ( inter )  {
    Interaction::VertexMap::iterator iv;    
    Interaction::ParticleMap::iterator ip;
    double dx = rndm.gauss(m_offset.x(),m_sigma.x());
    double dy = rndm.gauss(m_offset.y(),m_sigma.y());
    double dz = rndm.gauss(m_offset.z(),m_sigma.z());
    double dt = rndm.gauss(m_offset.t(),m_sigma.t());

    print("+++ Smearing primary vertex for interaction type %d by (%+.2e mm, %+.2e mm, %+.2e mm, %+.2e ns)",m_mask,dx,dy,dz,dt);

    // Now move begin and end-vertex of all primary vertices accordingly
    for(iv=inter->vertices.begin(); iv != inter->vertices.end(); ++iv)  {
      Geant4Vertex* v = (*iv).second;
      v->x += dx;
      v->y += dy;
      v->z += dz;
      v->time += dt;
    }
    // Now move begin and end-vertex of all primary and generator particles accordingly
    for(ip=inter->particles.begin(); ip != inter->particles.end(); ++ip)  {
      Particle* p = (*ip).second;
      p->vsx += dx;
      p->vsy += dy;
      p->vsz += dz;
      p->vex += dx;
      p->vey += dy;
      p->vez += dz;
      p->time += dt;
    }
  }
  else  {
    print("+++ No interaction of type %d present.",m_mask);
  }
}
