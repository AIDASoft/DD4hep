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

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;

typedef Geant4PrimaryInteraction Interaction;
typedef ReferenceBitMask<int> PropertyMask;

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

static G4PrimaryParticle* createG4Primary(const Geant4ParticleHandle p)  {
  G4PrimaryParticle* g4 = new G4PrimaryParticle(p->pdgID, p->psx, p->psy, p->psz);
  g4->SetMass(p->mass);
  return g4;
}

typedef map<Geant4Particle*,G4PrimaryParticle*> Primaries;
Primaries getRelevant(set<int>& visited,
						    map<int,G4PrimaryParticle*>& prim,
						    Interaction::ParticleMap& pm, 
						    const Geant4ParticleHandle p)  
{
  Primaries res;
  visited.insert(p->id);
  PropertyMask status(p->status);
  if ( status.isSet(G4PARTICLE_GEN_STABLE) )  {
    if ( prim.find(p->id) == prim.end() )  {
      G4PrimaryParticle* p4 = createG4Primary(p);
      prim[p->id] = p4;
      res.insert(make_pair(p,p4));
    }
  }
  else if ( p->daughters.size() > 0 )  {
    const Geant4Particle::Particles& dau = p->daughters;
    int first_daughter = *(dau.begin());
    Geant4ParticleHandle dp = pm[first_daughter];
    double me = p->mass / p.energy();
    //  fix by S.Morozov for real != 0
    double proper_time = fabs(dp->time-p->time) * me;
    double proper_time_Precision =  pow(10.,-DBL_DIG)*me*fmax(fabs(p->time),fabs(dp->time));
    bool isProperTimeZero = ( proper_time <= proper_time_Precision ) ;
    // -- remove original --- if (proper_time != 0) {
    if ( !isProperTimeZero ) {
      map<int,G4PrimaryParticle*>::iterator ip4 = prim.find(p->id);
      G4PrimaryParticle* p4 = (ip4 == prim.end()) ? 0 : (*ip4).second;
      if ( !p4 )  {
	p4 = createG4Primary(p);
	p4->SetProperTime(proper_time);
	prim[p->id] = p4;
	Primaries daughters;
	for(Geant4Particle::Particles::const_iterator i=dau.begin(); i!=dau.end(); ++i)  {
	  if ( visited.find(*i) == visited.end() )  {
	    Primaries tmp = getRelevant(visited,prim,pm,pm[*i]);
	    daughters.insert(tmp.begin(),tmp.end());
	  }
	}
	for(Primaries::iterator i=daughters.begin(); i!=daughters.end(); ++i)
	  p4->SetDaughter((*i).second);
      }
      res.insert(make_pair(p,p4));
    }
    else  {
      for(Geant4Particle::Particles::const_iterator i=dau.begin(); i!=dau.end(); ++i)  {
	if ( visited.find(*i) == visited.end() )  {
	  Primaries tmp = getRelevant(visited,prim,pm,pm[*i]);
	  res.insert(tmp.begin(),tmp.end());
	}
      }
    }
  }
  return res;
}

/// Event generation action callback
void Geant4PrimaryHandler::operator()(G4Event* event)  {
  Geant4PrimaryMap* primaries   = context()->event().extension<Geant4PrimaryMap>();
  Interaction*      interaction = context()->event().extension<Interaction>();
  Interaction::ParticleMap& pm  = interaction->particles;
  Interaction::VertexMap&   vm  = interaction->vertices;
  map<int,G4PrimaryParticle*> prim;
  set<int> visited;

  Geant4PrimaryInteraction::VertexMap::iterator ivfnd, iv, ivend;
  for(Interaction::VertexMap::const_iterator iend=vm.end(),i=vm.begin(); i!=iend; ++i)  {
    int num_part = 0;
    Geant4Vertex* v = (*i).second;
    G4PrimaryVertex* v4 = new G4PrimaryVertex(v->x,v->y,v->z,v->time);
    event->AddPrimaryVertex(v4);
    print("+++++ G4PrimaryVertex at (%+.2e,%+.2e,%+.2e) [mm] %+.2e [ns]",
	  v->x/mm,v->y/mm,v->z/mm,v->time/ns);    
    for(Geant4Vertex::Particles::const_iterator ip=v->out.begin(); ip!=v->out.end(); ++ip)  {
      Geant4ParticleHandle p = pm[*ip];
      if ( p->parents.size() == 0 )  {
	Primaries relevant = getRelevant(visited,prim,pm,p);
	for(Primaries::const_iterator j=relevant.begin(); j!= relevant.end(); ++j)  {
	  Geant4ParticleHandle r = (*j).first;
	  G4PrimaryParticle* p4 = (*j).second;
	  PropertyMask reason(r->reason);
	  reason.set(G4PARTICLE_PRIMARY);
	  v4->SetPrimary(p4);
	  printM1("+++ +-> G4Primary[%3d] ID:%3d type:%9d/%-12s "
		  "Momentum:(%+.2e,%+.2e,%+.2e) [GeV] time:%+.2e [ns] #Par:%3d #Dau:%3d",
		  num_part,r->id,r->pdgID,r.particleName().c_str(),
		  r->psx/GeV,r->psy/GeV,r->psz/GeV,r->time/ns,
		  int(r->parents.size()),int(r->daughters.size()));
	  ++num_part;
	}
      }
    }
  }
  for(map<int,G4PrimaryParticle*>::iterator i=prim.begin(); i!=prim.end(); ++i)  {
    Geant4ParticleHandle p = pm[(*i).first];
    primaries->primaryMap[(*i).second] = p->addRef();
  }
}
