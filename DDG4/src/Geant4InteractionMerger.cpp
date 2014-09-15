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
#include "DDG4/Geant4InteractionMerger.h"

#include <stdexcept>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;

/// Standard constructor
Geant4InteractionMerger::Geant4InteractionMerger(Geant4Context* context, const std::string& nam)
  : Geant4GeneratorAction(context,nam)
{
  InstanceCount::increment(this);
}

/// Default destructor
Geant4InteractionMerger::~Geant4InteractionMerger()  {
  InstanceCount::decrement(this);
}

void rebaseParticles(Geant4PrimaryInteraction::ParticleMap& particles, int &offset)    {
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

void rebaseVertices(Geant4PrimaryInteraction::VertexMap& vertices, int part_offset)    {
  Geant4PrimaryInteraction::VertexMap::iterator iv, ivend;    
  set<int> in, out;
  set<int>::iterator i;
  // Now move begin and end-vertex of all primary vertices accordingly
  for(iv=vertices.begin(), ivend=vertices.end(); iv != ivend; ++iv)  {
    Geant4Vertex* v = (*iv).second;
    in  = v->in;
    out = v->out;
    for(in=v->in, i=in.begin(), v->in.clear(); i != in.end(); ++i)
      v->in.insert((*i)+part_offset);
    for(out=v->out, i=out.begin(), v->out.clear(); i != out.end(); ++i)
      v->out.insert((*i)+part_offset);
  }
}

void appendInteraction(Geant4PrimaryInteraction* output, Geant4PrimaryInteraction* input)   {
  Geant4PrimaryInteraction::ParticleMap::iterator ip, ipend;
  for( ip=input->particles.begin(), ipend=input->particles.end(); ip != ipend; ++ip )  {
    Geant4Particle* p = (*ip).second;
    output->particles.insert(make_pair(p->id,p->addRef()));
  }
  Geant4PrimaryInteraction::VertexMap::iterator iv, ivend;
  for( iv=input->vertices.begin(), ivend=input->vertices.end(); iv != ivend; ++iv )
    output->vertices.insert(make_pair(output->vertices.size(),(*iv).second->addRef()));
}

/// Event generation action callback
void Geant4InteractionMerger::operator()(G4Event* /* event */)  {
  typedef Geant4PrimaryEvent::Interaction  Interaction;
  typedef vector<Interaction*>             Interactions;
  Geant4PrimaryEvent* evt = context()->event().extension<Geant4PrimaryEvent>();
  Interaction* output = context()->event().extension<Interaction>();
  Interactions inter  = evt->interactions();
  int particle_offset = 0, vertex_offset = 0;

  for(Interactions::const_iterator i=inter.begin(); i != inter.end(); ++i)  {
    Interaction* interaction = *i;
    vertex_offset = particle_offset;
    rebaseParticles(interaction->particles,particle_offset);
    rebaseVertices(interaction->vertices,vertex_offset);
    appendInteraction(output,interaction);
  }
  output->setNextPID(particle_offset);
  Geant4PrimaryInteraction::ParticleMap::iterator ip, ipend;
  debug("+++ Merged MC input record from %d interactions:",(int)inter.size());
  for( ip=output->particles.begin(), ipend=output->particles.end(); ip != ipend; ++ip )
    Geant4ParticleHandle((*ip).second).dump1(DEBUG,name(),"Merged particles");
}
