// $Id: Geant4Field.cpp 888 2013-11-14 15:54:56Z markus.frank@cern.ch $
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
#include "DDG4/Geant4PrimaryHandler.h"

#include "G4Event.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"

#include <stdexcept>

using namespace DD4hep;
using namespace DD4hep::Simulation;

/// Standard constructor
Geant4PrimaryHandler::Geant4PrimaryHandler(Geant4Context* context, const std::string& nam)
  : Geant4GeneratorAction(context,nam)
{
  InstanceCount::increment(this);
}

/// Default destructor
Geant4PrimaryHandler::~Geant4PrimaryHandler()  {
  InstanceCount::decrement(this);
}

/// Event generation action callback
void Geant4PrimaryHandler::operator()(G4Event* event)  {
  typedef Geant4PrimaryInteraction Interaction;
  Geant4PrimaryMap* primaries   = context()->event().extension<Geant4PrimaryMap>();
  Interaction*      interaction = context()->event().extension<Interaction>();
  Interaction::ParticleMap& pm  = interaction->particles;
  Interaction::VertexMap&   vm  = interaction->vertices;
  Interaction::VertexMap::const_iterator iv, ivend;
  int num_vtx = 0, num_part = 0;

  for(iv=vm.begin(), ivend=vm.end(); iv != ivend; ++iv, ++num_vtx, num_part=0)  {
    Geant4Vertex* v = (*iv).second;
    G4PrimaryVertex* g4 = new G4PrimaryVertex(v->x,v->y,v->z,v->time);
    print("+++++ G4PrimaryVertex %3d  at (%+.2e,%+.2e,%+.2e) [mm] %+.2e [ns]  %d particles",
	  num_vtx,v->x/mm,v->y/mm,v->z/mm,v->time/ns,int(v->out.size()));
    // Generate Geant4 primaries coming from this vertex
    for(Geant4Vertex::Particles::const_iterator j=v->out.begin(); j!=v->out.end(); ++j, ++num_part)  {
      // Same particle cannot come from 2 vertices! Hence it must ALWAYS be recreated
      Interaction::ParticleMap::const_iterator ip = pm.find(*j);
      if ( ip == pm.end() )  { // ERROR. may not happen. Something went wrong in the gathering.
	const char* text = "+++ Fatal inconsistency in the Geant4PrimaryInteraction record.";
	printout(ERROR,name(),text);
	throw std::runtime_error(name()+std::string("  ")+text);
      }
      Geant4Particle* p = (*ip).second;
      G4PrimaryParticle* g4part = new G4PrimaryParticle(p->pdgID,p->psx,p->psy,p->psz);
      g4part->SetMass(p->mass);
      g4->SetPrimary(g4part);
      printM1("+++ +-> G4Primary[%3d] ID:%3d type:%9d Momentum:(%+.2e,%+.2e,%+.2e) [GeV] time:%+.2e [ns] #Par:%3d #Dau:%3d",
	       num_part,p->id,p->pdgID,p->psx/GeV,p->psy/GeV,p->psz/GeV,p->time/ns,
	       int(p->parents.size()),int(p->daughters.size()));
      primaries->primaryMap[g4part] = p->addRef();
    }
    event->AddPrimaryVertex(g4);
  }
}
