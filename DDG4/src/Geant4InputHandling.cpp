//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

// Framework include files
#include "DDG4/Geant4InputHandling.h"
#include "DDG4/Geant4Primary.h"
#include "DDG4/Geant4Context.h"
#include "DDG4/Geant4Action.h"
#include "DDG4/Geant4PrimaryHandler.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

// Geant4 include files
#include "G4ParticleDefinition.hh"
#include "G4Event.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"

// C/C++ include files
#include <stdexcept>
#include <cmath>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::sim;

typedef detail::ReferenceBitMask<int> PropertyMask;


/// Create a vertex object from the Geant4 primary vertex
Geant4Vertex* dd4hep::sim::createPrimary(const G4PrimaryVertex* g4)      {        
  Geant4Vertex* v = new Geant4Vertex();
  v->x = g4->GetX0();
  v->y = g4->GetY0();
  v->z = g4->GetZ0();
  v->time = g4->GetT0();
  return v;
}

/// Create a particle object from the Geant4 primary particle
Geant4Particle* 
dd4hep::sim::createPrimary(int particle_id, 
                                  const Geant4Vertex* v,
                                  const G4PrimaryParticle* g4p)
{
  Geant4ParticleHandle p = new Geant4Particle();
  p->id           = particle_id;
  p->reason       = 0;
  p->pdgID        = g4p->GetPDGcode();
  p->psx          = g4p->GetPx();
  p->psy          = g4p->GetPy();
  p->psz          = g4p->GetPz();
  p->time         = g4p->GetProperTime();
  p->properTime   = g4p->GetProperTime();
  p->vsx          = v->x;
  p->vsy          = v->y;
  p->vsz          = v->z;
  p->vex          = v->x;
  p->vey          = v->y;
  p->vez          = v->z;
  //p->definition   = g4p->GetG4code();
  //p->process      = 0;
  p->spin[0]      = 0;
  p->spin[1]      = 0;
  p->spin[2]      = 0;
  p->colorFlow[0] = 0;
  p->colorFlow[0] = 0;
  p->mass         = g4p->GetMass();
  p->charge       = g4p->GetCharge();
  PropertyMask status(p->status);
  status.set(G4PARTICLE_GEN_STABLE);
  return p;
}

/// Helper to recursively build a DDG4 interaction from an existing G4 interaction (primary vertex)
static void collectPrimaries(Geant4PrimaryMap*         pm,
                             Geant4PrimaryInteraction* interaction, 
                             Geant4Vertex*             particle_origine,
                             G4PrimaryParticle*        gp)
{
  int pid = int(interaction->particles.size());
  Geant4Particle* p = createPrimary(pid,particle_origine,gp);
  G4PrimaryParticle* dau = gp->GetDaughter();
  PropertyMask status(p->status);
  int mask = interaction->mask;
  
  interaction->particles.insert(make_pair(p->id,p));
  status.set(G4PARTICLE_PRIMARY);
  p->mask = mask;
  particle_origine->out.insert(p->id);
  // Insert pair in map. Do not forget to increase reference count!
  pm->insert(gp,p);

  if ( dau )   {
    Geant4Vertex* dv = new Geant4Vertex(*particle_origine);
    int vid = int(interaction->vertices.size());
    PropertyMask reason(p->reason);
    reason.set(G4PARTICLE_HAS_SECONDARIES);

    dv->mask = mask;
    dv->in.insert(p->id);

    interaction->vertices[vid].push_back(dv) ;

    for(; dau; dau = dau->GetNext())
      collectPrimaries(pm, interaction, dv, dau);
  }
}

/// Import a Geant4 interaction defined by a primary vertex into a DDG4 interaction record
Geant4PrimaryInteraction* 
dd4hep::sim::createPrimary(int mask,
                                  Geant4PrimaryMap* pm,
                                  const G4PrimaryVertex* gv)
{
  Geant4PrimaryInteraction* interaction = new Geant4PrimaryInteraction();
  Geant4Vertex* v = createPrimary(gv);
  int vid = int(interaction->vertices.size());
  interaction->locked = true;
  interaction->mask = mask;
  v->mask = mask;
  interaction->vertices[vid].push_back(v);

  for (G4PrimaryParticle *gp = gv->GetPrimary(); gp; gp = gp->GetNext() )
    collectPrimaries(pm, interaction, v, gp);
  return interaction;
}

/// Initialize the generation of one event
int dd4hep::sim::generationInitialization(const Geant4Action* /* caller */,
                                                 const Geant4Context* context)
{
  /**
   *  This action should register all event extension required for the further
   *  processing. We want to avoid that every client has to check if a given
   *  object is present or not and then later install the required data structures.
   */
  context->event().addExtension(new Geant4PrimaryMap());

  // The final set of created particles in the simulation.
  context->event().addExtension(new Geant4ParticleMap());

  //
  // The Geant4PrimaryEvent extension contains a whole set of
  // Geant4PrimaryInteraction objects each may represent a complete
  // interaction. Particles and vertices may be unbiased.
  // This is the input to the translator forming the final
  // Geant4PrimaryInteraction (see below) containing rebiased particle
  // and vertex maps.
  Geant4PrimaryEvent* evt = new Geant4PrimaryEvent();
  context->event().addExtension(evt);
  //
  // The Geant4PrimaryInteraction extension contains the final
  // vertices and particles ready to be injected to Geant4.
  Geant4PrimaryInteraction* inter = new Geant4PrimaryInteraction();
  inter->setNextPID(-1);
  context->event().addExtension(inter);
  return 1;
}

/// Append input interaction to global output
static void appendInteraction(const Geant4Action* caller,
                              Geant4PrimaryInteraction* output,
                              Geant4PrimaryInteraction* input)
{
  Geant4PrimaryInteraction::ParticleMap::iterator ip, ipend;
  for( ip=input->particles.begin(), ipend=input->particles.end(); ip != ipend; ++ip )  {
    Geant4Particle* p = (*ip).second;
    output->particles.insert(make_pair(p->id,p->addRef()));
  }
  Geant4PrimaryInteraction::VertexMap::iterator ivfnd, iv, ivend;
  for( iv=input->vertices.begin(), ivend=input->vertices.end(); iv != ivend; ++iv )   {
    ivfnd = output->vertices.find((*iv).first) ; //(*iv).second->mask);
    if ( ivfnd != output->vertices.end() )   {
      caller->abortRun("Duplicate primary interaction identifier!",
                       "Cannot handle 2 interactions with identical identifiers!");
    }
    for(Geant4Vertex* vtx :  (*iv).second )
      output->vertices[(*iv).first].push_back( vtx->addRef() );
  }
}

static void rebaseParticles(Geant4PrimaryInteraction::ParticleMap& particles, int &offset)    {
  Geant4PrimaryInteraction::ParticleMap::iterator ip, ipend;
  int mx_id = offset;
  // Now move begin and end-vertex of all primary and generator particles accordingly
  for( ip=particles.begin(), ipend=particles.end(); ip != ipend; ++ip )  {
    Geant4ParticleHandle p((*ip).second);
    p.offset(offset);
    mx_id = p->id+1 > mx_id ? p->id+1 : mx_id;
  }
  offset = mx_id;
}

static void rebaseVertices(Geant4PrimaryInteraction::VertexMap& vertices, int part_offset)    {
  Geant4PrimaryInteraction::VertexMap::iterator iv, ivend;
  set<int> in, out;
  set<int>::iterator i;
  // Now move begin and end-vertex of all primary vertices accordingly
  for(iv=vertices.begin(), ivend=vertices.end(); iv != ivend; ++iv)  {
    for( Geant4Vertex* v : (*iv).second ){ 
      in  = v->in;
      out = v->out;
      for(in=v->in, i=in.begin(), v->in.clear(); i != in.end(); ++i)
	v->in.insert((*i)+part_offset);
      for(out=v->out, i=out.begin(), v->out.clear(); i != out.end(); ++i)
	v->out.insert((*i)+part_offset);
    }
  }
}
/// Merge all interactions present in the context
int dd4hep::sim::mergeInteractions(const Geant4Action* caller,
                                          const Geant4Context* context)
{
  typedef Geant4PrimaryEvent::Interaction  Interaction;
  typedef vector<Interaction*>             Interactions;
  Geant4Event& event = context->event();
  Geant4PrimaryEvent* evt = event.extension<Geant4PrimaryEvent>();
  Interaction* output = event.extension<Interaction>();
  Interactions inter  = evt->interactions();
  int particle_offset = 0;

  for(Interactions::const_iterator i=inter.begin(); i != inter.end(); ++i)  {
    Interaction* interaction = *i;
    int vertex_offset = particle_offset;
    if ( !interaction->applyMask() )   {
      caller->abortRun("Found single interaction with multiple primary vertices!",
                       "Cannot merge individual interactions with more than one primary!");
    }
    rebaseParticles(interaction->particles,particle_offset);
    rebaseVertices(interaction->vertices,vertex_offset);
    appendInteraction(caller,output,interaction);
  }
  output->setNextPID(particle_offset);
  Geant4PrimaryInteraction::ParticleMap::iterator ip, ipend;
  caller->debug("+++ Merging MC input record from %d interactions:",(int)inter.size());
  for( ip=output->particles.begin(), ipend=output->particles.end(); ip != ipend; ++ip )
    Geant4ParticleHandle((*ip).second).dump1(DEBUG,caller->name(),"Merged particles");
  return 1;
}

/// Boost particles of one interaction identified by its mask
int dd4hep::sim::boostInteraction(const Geant4Action* caller,
                                         Geant4PrimaryEvent::Interaction* inter,
                                         double alpha)
{
#define SQR(x)  (x*x)
  Geant4PrimaryEvent::Interaction::VertexMap::iterator iv;
  Geant4PrimaryEvent::Interaction::ParticleMap::iterator ip;
  double gamma = std::sqrt(1 + SQR(tan(alpha)));
  double betagamma = std::tan(alpha);

  if ( inter->locked )  {
    caller->abortRun("Locked interactions may not be boosted!",
                     "Cannot boost interactions with a native G4 primary record!");
  }
  else if ( alpha != 0.0 )  {
    // Now move begin and end-vertex of all primary vertices accordingly
    for(iv=inter->vertices.begin(); iv != inter->vertices.end(); ++iv){  
      for( Geant4Vertex* v :  (*iv).second ) {
	double t = gamma * v->time + betagamma * v->x / CLHEP::c_light;
	double x = gamma * v->x + betagamma * CLHEP::c_light * v->time;
	double y = v->y;
	double z = v->z;
	v->x = x;
	v->y = y;
	v->z = z;
	v->time = t;
      }
    }
    // Now move begin and end-vertex of all primary and generator particles accordingly
    for(ip=inter->particles.begin(); ip != inter->particles.end(); ++ip)  {
      Geant4ParticleHandle p = (*ip).second;
      double t = gamma * p->time + betagamma * p->vsx / CLHEP::c_light;
      double x = gamma * p->vsx + betagamma * CLHEP::c_light * p->time;
      double y = p->vsy;
      double z = p->vsz;
      
      double m  = p->mass;
      double e2 = SQR(p->psx)+SQR(p->psy)+SQR(p->psz)+SQR(m);
      double px = betagamma * std::sqrt(e2) + gamma * p->psx;
      double py = p->psy;
      double pz = p->psz;
      
      p->vsx = x;
      p->vsy = y;
      p->vsz = z;
      p->time = t;
      
      p->psx = px;
      p->psy = py;
      p->psz = pz;
    }
  }
  return 1;
}

/// Smear the primary vertex of an interaction
int dd4hep::sim::smearInteraction(const Geant4Action* caller,
                                         Geant4PrimaryEvent::Interaction* inter,
                                         double dx, double dy, double dz, double dt)
{
  Geant4PrimaryEvent::Interaction::VertexMap::iterator iv;
  Geant4PrimaryEvent::Interaction::ParticleMap::iterator ip;

  if ( inter->locked )  {
    caller->abortRun("Locked interactions may not be smeared!",
                     "Cannot smear interactions with a native G4 primary record!");
  }
  
  // Now move begin and end-vertex of all primary vertices accordingly
  for(iv=inter->vertices.begin(); iv != inter->vertices.end(); ++iv)  {
    for( Geant4Vertex* v : (*iv).second ){
      v->x += dx;
      v->y += dy;
      v->z += dz;
      v->time += dt;
    }
  }
  // Now move begin and end-vertex of all primary and generator particles accordingly
  for(ip=inter->particles.begin(); ip != inter->particles.end(); ++ip)  {
    Geant4Particle* p = (*ip).second;
    p->vsx += dx;
    p->vsy += dy;
    p->vsz += dz;
    p->vex += dx;
    p->vey += dy;
    p->vez += dz;
    p->time += dt;
  }
  return 1;
}

static G4PrimaryParticle* createG4Primary(const Geant4ParticleHandle p)  {
  G4PrimaryParticle* g4 = 0;
  if ( 0 != p->pdgID )   {
    g4 = new G4PrimaryParticle(p->pdgID, p->psx, p->psy, p->psz);
  }
  else   {
    const G4ParticleDefinition* def = p.definition();
    g4 = new G4PrimaryParticle(def, p->psx, p->psy, p->psz);
    g4->SetCharge(p.charge());
  }
  g4->SetMass(p->mass);
  return g4;
}

static vector< pair<Geant4Particle*,G4PrimaryParticle*> >
getRelevant(set<int>& visited,
            map<int,G4PrimaryParticle*>& prim,
            Geant4PrimaryInteraction::ParticleMap& pm,
            const set<int>& rejectPDGs,
            const Geant4ParticleHandle p)
{
  typedef vector< pair<Geant4Particle*,G4PrimaryParticle*> > Primaries;
  Primaries res;
  visited.insert(p->id);
  PropertyMask status(p->status);
  if ( status.isSet(G4PARTICLE_GEN_STABLE) )  {
    if ( prim.find(p->id) == prim.end() )  {
      G4PrimaryParticle* p4 = createG4Primary(p);
      prim[p->id] = p4;
      res.push_back(make_pair(p,p4));
    }
  }
  else if ( p->daughters.size() > 0 )  {
    const Geant4Particle::Particles& dau = p->daughters;
    int first_daughter = *(dau.begin());
    Geant4ParticleHandle dp = pm[first_daughter];
    double en = p.energy();
    double me = en > std::numeric_limits<double>::epsilon() ? p->mass / en : 0.0;
    //  fix by S.Morozov for real != 0
    double proper_time = fabs(dp->time-p->time) * me;
    double proper_time_Precision = pow(10.,-DBL_DIG)*me*fmax(fabs(p->time),fabs(dp->time));
    bool isProperTimeZero = (proper_time <= proper_time_Precision);

    const std::set<int> leptonPDGs{11,13,15,17};
    // -- remove original if ---
    bool rejectParticle = not p.definition() // completely unknown to geant4
      or (rejectPDGs.count(abs(p->pdgID)) != 0) // quarks, gluon, "strings", W, Z etc.
      or (isProperTimeZero and p.definition()->GetPDGStable() ) // initial state electrons, etc.
      or (isProperTimeZero and leptonPDGs.count(abs(p->pdgID)) != 0 ) ; // charged 'documentation' leptons, e.g. in lepton pairs w/ FSR
    if (not rejectParticle) {
      map<int,G4PrimaryParticle*>::iterator ip4 = prim.find(p->id);
      G4PrimaryParticle* p4 = (ip4 == prim.end()) ? 0 : (*ip4).second;
      if ( !p4 )  {
        p4 = createG4Primary(p);
        p4->SetProperTime(proper_time);
        prim[p->id] = p4;
        Primaries daughters;
        for(Geant4Particle::Particles::const_iterator i=dau.begin(); i!=dau.end(); ++i)  {
          if ( visited.find(*i) == visited.end() )  {
            Primaries tmp = getRelevant(visited,prim,pm,rejectPDGs,pm[*i]);
            daughters.insert(daughters.end(), tmp.begin(),tmp.end());
          }
        }
        for(Primaries::iterator i=daughters.begin(); i!=daughters.end(); ++i)
          p4->SetDaughter((*i).second);
      }
      res.push_back(make_pair(p,p4));
    }
    else  {
      for(Geant4Particle::Particles::const_iterator i=dau.begin(); i!=dau.end(); ++i)  {
        if ( visited.find(*i) == visited.end() )  {
          Primaries tmp = getRelevant(visited,prim,pm,rejectPDGs,pm[*i]);
          res.insert(res.end(), tmp.begin(),tmp.end());
        }
      }
    }
  }
  return res;
}

/// Generate all primary vertices corresponding to the merged interaction
int dd4hep::sim::generatePrimaries(const Geant4Action* caller,
                                          const Geant4Context* context,
                                          G4Event* event)
{
  typedef vector< pair<Geant4Particle*,G4PrimaryParticle*> > Primaries;
  typedef Geant4PrimaryInteraction Interaction;
  Geant4PrimaryMap* primaries   = context->event().extension<Geant4PrimaryMap>();
  Interaction*      interaction = context->event().extension<Interaction>();
  Interaction::ParticleMap& pm  = interaction->particles;
  Interaction::VertexMap&   vm  = interaction->vertices;
  map<int,G4PrimaryParticle*> prim;
  set<int> visited;

  auto const* primHandler = dynamic_cast<const Geant4PrimaryHandler*>(caller);
  auto const& rejectPDGs = primHandler ? primHandler->m_rejectPDGs : std::set<int>();

  caller->debug("Rejecting PDGs: %s", [&rejectPDGs]{ std::stringstream str; for (int i: rejectPDGs) { str << i << ", "; } return str.str();}().c_str());

  if ( interaction->locked )  {
    caller->abortRun("Locked interactions may not be used to generate primaries!",
                     "Cannot handle a native G4 primary record!");
    return 0;
  }
  else   {
    Geant4PrimaryInteraction::VertexMap::iterator ivfnd, iv, ivend;
    for(Interaction::VertexMap::const_iterator iend=vm.end(),i=vm.begin(); i!=iend; ++i)  {
      for( Geant4Vertex* v : (*i).second ){

	int num_part = 0;
	G4PrimaryVertex* v4 = new G4PrimaryVertex(v->x,v->y,v->z,v->time);
	event->AddPrimaryVertex(v4);
	caller->print("+++++ G4PrimaryVertex at (%+.2e,%+.2e,%+.2e) [mm] %+.2e [ns]",
		      v->x/CLHEP::mm,v->y/CLHEP::mm,v->z/CLHEP::mm,v->time/CLHEP::ns);
	for(Geant4Vertex::Particles::const_iterator ip=v->out.begin(); ip!=v->out.end(); ++ip)  {
	  Geant4ParticleHandle p = pm[*ip];
	  if ( p->daughters.size() > 0 )  {
	    PropertyMask mask(p->reason);
	    mask.set(G4PARTICLE_HAS_SECONDARIES);
	  }
	  if ( p->parents.size() == 0 )  {
	    Primaries relevant = getRelevant(visited,prim,pm,rejectPDGs,p);
	    for(Primaries::const_iterator j=relevant.begin(); j!= relevant.end(); ++j)  {
	      Geant4ParticleHandle r = (*j).first;
	      G4PrimaryParticle* p4 = (*j).second;
	      PropertyMask reason(r->reason);
	      char text[64];
	      
	      reason.set(G4PARTICLE_PRIMARY);
	      v4->SetPrimary(p4);
	      ::snprintf(text,sizeof(text),"-> G4Primary[%3d]",num_part);
	      r.dumpWithMomentum(caller->outputLevel()-1,caller->name(),text);
	      ++num_part;
	    }
	  }
	}
        if(caller->outputLevel() <= VERBOSE){
          v4->Print();
        }
      }
    }
    for(map<int,G4PrimaryParticle*>::iterator i=prim.begin(); i!=prim.end(); ++i)  {
      Geant4ParticleHandle p = pm[(*i).first];
      primaries->insert((*i).second,p);
    }
  }
  return 1;
}
