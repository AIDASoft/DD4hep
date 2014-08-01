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
#include "DD4hep/Primitives.h"
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4ParticlePrint.h"
#include "DDG4/Geant4Data.h"

#include "G4ParticleDefinition.hh"
#include "G4VProcess.hh"
#include "G4Event.hh"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;

typedef ReferenceBitMask<const int> PropertyMask;

/// Standard constructor
Geant4ParticlePrint::Geant4ParticlePrint(Geant4Context* context, const std::string& nam)
  : Geant4EventAction(context,nam)
{
  declareProperty("OutputType",m_outputType=3);
  InstanceCount::increment(this);
}

/// Default destructor
Geant4ParticlePrint::~Geant4ParticlePrint()  {
  InstanceCount::decrement(this);
}

/// Pre-event action callback
void Geant4ParticlePrint::begin(const G4Event* )  {
}

/// Post-event action callback
void Geant4ParticlePrint::end(const G4Event* )  {
  Geant4MonteCarloTruth* truth = context()->event().extension<Geant4MonteCarloTruth>();
  const ParticleMap& particles = truth->particles();
  // Table printout....
  if ( (m_outputType&1) != 0 ) printParticles(particles);
  // Tree printout....
  if ( (m_outputType&2) != 0 ) printParticleTree(particles);
}

void Geant4ParticlePrint::printParticle(const std::string& prefix, const Particle* p) const   {
  char equiv[32];
  PropertyMask mask(p->reason);
  const char* proc_name = "???";
  const char* proc_type = p->process ? G4VProcess::GetProcessTypeName(p->process->GetProcessType()).c_str() : "";

  if ( p->process ) proc_name = p->process->GetProcessName().c_str();
  else if ( mask.isSet(G4PARTICLE_PRIMARY) ) proc_name = "Primary";

  equiv[0] = 0;
  if ( p->g4Parent != p->parent )  {
    ::snprintf(equiv,sizeof(equiv),"/%d",p->g4Parent);
  }
  printout(INFO,name(),"+++ %sID: %6d %12s %6d%-7s %7s %3s %5d %6s %8.3g %4s %7s %7s %3s [%s%s%s]",
	   prefix.c_str(),
	   p->id,
	   p->definition->GetParticleName().c_str(),
	   p->parent,equiv,
	   yes_no(mask.isSet(G4PARTICLE_PRIMARY)),
	   yes_no(mask.isSet(G4PARTICLE_HAS_SECONDARIES)),
	   int(p->daughters.size()),
	   yes_no(mask.isSet(G4PARTICLE_ABOVE_ENERGY_THRESHOLD)),
	   p->energy,
	   yes_no(mask.isSet(G4PARTICLE_CREATED_CALORIMETER_HIT)),
	   yes_no(mask.isSet(G4PARTICLE_CREATED_TRACKER_HIT)),
	   yes_no(mask.isSet(G4PARTICLE_KEEP_PROCESS)),
	   mask.isSet(G4PARTICLE_KEEP_PARENT) ? "YES" : "",
	   proc_name,
	   p->process ? "/" : "",
	   proc_type
	   );
}

/// Print record of kept particles
void Geant4ParticlePrint::printParticles(const ParticleMap& particles) const  {
  int num_energy = 0;
  int num_parent = 0;
  int num_process = 0;
  int num_primary = 0;
  int num_secondaries = 0;
  int num_calo_hits = 0;
  int num_tracker_hits = 0;

  printout(INFO,name(),"+++ MC Particles #Tracks:%6d %-12s Parent%-7s "
	   "Primary Secondary Energy %-9s Calo Tracker Process/Par Details",
	   int(particles.size()),"ParticleType","","in [MeV]");
  for(ParticleMap::const_iterator i=particles.begin(); i!=particles.end(); ++i)  {
    const Particle* p = (*i).second;
    PropertyMask mask(p->reason);
    printParticle("MC Particle Track",p);
    num_secondaries += int(p->daughters.size());
    if ( mask.isSet(G4PARTICLE_ABOVE_ENERGY_THRESHOLD) ) ++num_energy;
    if ( mask.isSet(G4PARTICLE_PRIMARY) ) ++num_primary;
    if ( mask.isSet(G4PARTICLE_CREATED_TRACKER_HIT) ) ++num_tracker_hits;
    if ( mask.isSet(G4PARTICLE_CREATED_CALORIMETER_HIT) ) ++num_calo_hits;
    if ( mask.isSet(G4PARTICLE_KEEP_PARENT) ) ++num_parent;
    else if ( mask.isSet(G4PARTICLE_KEEP_PROCESS) ) ++num_process;
  }
  printout(INFO,name(),"+++ MC Particles #Tracks:%6d %-12s Parent%-7s "
	   "Primary Secondary Energy %-9s Calo Tracker Process/Parent",
	   int(particles.size()),"ParticleType","","");
  printout(INFO,name(),"+++ MC Particles #Tracks:%6d %-12s Parent%-7s "
	   "%7d %9d %6d %-9s %4d %7d %7d %6d",
	   int(particles.size()),"ParticleType","",
	   num_primary, num_secondaries, num_energy,"",
	   num_calo_hits,num_tracker_hits,num_process,num_parent);
}

void Geant4ParticlePrint::printParticleTree(const ParticleMap& particles, int level, const Particle* p)  const  {
  char txt[32];
  size_t len = sizeof(txt)-1;
  // Ensure we do not overwrite the array
  if ( level>int(len)-3 ) level=len-3;
  
  ::snprintf(txt,sizeof(txt),"%5d ",level);
  ::memset(txt+6,' ',len-6);
  txt[len] = 0;
  txt[len-2] = '>';
  txt[level+6]='+';
  ::memset(txt+level+6+1,'-',len-level-3-6);

  printParticle(txt, p);
  const set<int>& daughters = p->daughters;
  // For all particles, the set of daughters must be contained in the record.
  for(set<int>::const_iterator id=daughters.begin(); id!=daughters.end(); ++id)   {
    int id_dau = *id;
    const Particle* dau = (*particles.find(id_dau)).second;
    printParticleTree(particles,level+1,dau);
  }
}

/// Print tree of kept particles
void Geant4ParticlePrint::printParticleTree(const ParticleMap& particles)  const  {
  printout(INFO,name(),"+++ MC Particle Parent daughter relationships. [%d particles]",int(particles.size()));
  for(ParticleMap::const_iterator i=particles.begin(); i!=particles.end(); ++i)  {
    const Particle* p = (*i).second;
    PropertyMask mask(p->reason);
    if ( mask.isSet(G4PARTICLE_PRIMARY) ) printParticleTree(particles,0,p);
  }
}
