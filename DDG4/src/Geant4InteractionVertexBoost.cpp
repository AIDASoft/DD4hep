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
#include "DDG4/Geant4InteractionVertexBoost.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include "G4ParticleDefinition.hh"

// C/C++ include files
#include <stdexcept>
#include <cmath>

using namespace std;
using namespace DD4hep::Simulation;
#define SQR(x)  (x*x)

/// Standard constructor
Geant4InteractionVertexBoost::Geant4InteractionVertexBoost(Geant4Context* context, const string& name)
  : Geant4GeneratorAction(context, name)
{
  InstanceCount::increment(this);
  declareProperty("Angle", m_angle = 0);
  declareProperty("Mask",  m_mask = 0);
  m_needsControl = true;
}

/// Default destructor
Geant4InteractionVertexBoost::~Geant4InteractionVertexBoost() {
  InstanceCount::decrement(this);
}

/// Callback to generate primary particles
void Geant4InteractionVertexBoost::operator()(G4Event*) {
  typedef Geant4PrimaryEvent::Interaction Interaction;
  typedef Geant4Particle Particle;
  typedef Geant4Particle Vertex;
  Geant4PrimaryEvent* evt = context()->event().extension<Geant4PrimaryEvent>();
  Interaction* inter = evt->get(m_mask);

  if ( inter )  {
    Interaction::VertexMap::iterator iv;    
    Interaction::ParticleMap::iterator ip;
    double alpha = m_angle;
    double gamma = std::sqrt(1 + SQR(tan(alpha)));
    double betagamma = std::tan(alpha);

    if ( alpha != 0.0 )  {
      // Now move begin and end-vertex of all primary vertices accordingly
      for(iv=inter->vertices.begin(); iv != inter->vertices.end(); ++iv)  {
	Geant4Vertex* v = (*iv).second;
	double t = gamma * t + betagamma * v->x / c_light;
	double x = gamma * v->x + betagamma * c_light * v->time;
	double y = v->y;
	double z = v->z;
	v->x = x;
	v->y = y;
	v->z = z;
	v->time = t;
      }

      // Now move begin and end-vertex of all primary and generator particles accordingly
      for(ip=inter->particles.begin(); ip != inter->particles.end(); ++ip)  {
	Particle* p = (*ip).second;
	double t = gamma * p->time + betagamma * p->vsx / c_light;
	double x = gamma * p->vsx + betagamma * c_light * p->time;
	double y = p->vsx;
	double z = p->vsz;

	double m  = p->definition->GetPDGMass();
	double e2 = SQR(p->psx)+SQR(p->psy)+SQR(p->psz)+SQR(m);
	double px = betagamma * std::sqrt(e2) + gamma * p->psx;
	double py = p->psy;
	double pz = p->psz;

	p->vsx = x;
	p->vsy = y;
	p->vsz = z;
	p->time = t;

	p->psx += px;
	p->psy += py;
	p->psz += pz;
      }
    }
  }
}
