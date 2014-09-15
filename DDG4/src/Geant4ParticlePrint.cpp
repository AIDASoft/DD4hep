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

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;

typedef ReferenceBitMask<const int> PropertyMask;

/// Standard constructor
Geant4ParticlePrint::Geant4ParticlePrint(Geant4Context* context, const std::string& nam)
  : Geant4EventAction(context,nam)
{
  declareProperty("OutputType",m_outputType=3);
  declareProperty("PrintBegin",m_printBegin=false);
  declareProperty("PrintEnd",  m_printEnd=true);
  declareProperty("PrintGen",  m_printGeneration=true);
  InstanceCount::increment(this);
}

/// Default destructor
Geant4ParticlePrint::~Geant4ParticlePrint()  {
  InstanceCount::decrement(this);
}

/// Print particle table
void Geant4ParticlePrint::makePrintout() const  {
  Geant4ParticleMap* parts = context()->event().extension<Geant4ParticleMap>();
  if ( parts )   {
    const ParticleMap& particles = parts->particles();
    if ( (m_outputType&2) != 0 ) printParticleTree(particles);  // Tree printout....
    if ( (m_outputType&1) != 0 ) printParticles(particles);     // Table printout....
    return;
  }
  except("No Geant4MonteCarloTruth object present in the event structure to access the particle information!", c_name());
}

/// Generation action callback
void Geant4ParticlePrint::operator()(G4Event* )  {
  if ( m_printGeneration ) makePrintout();
}

/// Pre-event action callback
void Geant4ParticlePrint::begin(const G4Event* )  {
  if ( m_printBegin ) makePrintout();
}

/// Post-event action callback
void Geant4ParticlePrint::end(const G4Event* )  {
  if ( m_printEnd ) makePrintout();
}

void Geant4ParticlePrint::printParticle(const std::string& prefix, Geant4ParticleHandle p) const   {
  char equiv[32];
  PropertyMask mask(p->reason);
  string proc_name = p.processName();
  string proc_type = p.processTypeName();
  int parent_id = p->parents.empty() ? -1 : *(p->parents.begin());

  equiv[0] = 0;
  if ( p->parents.end() == p->parents.find(p->g4Parent) )  {
    ::snprintf(equiv,sizeof(equiv),"/%d",p->g4Parent);
  }
  print("+++ %s ID:%7d %12s %6d%-7s %7s %3s %5d %3s %+.3e  %-4s %-7s %-3s %-3s %2d  [%s%s%s]",
	prefix.c_str(),
	p->id,
	p.particleName().c_str(),
	parent_id,equiv,
	yes_no(mask.isSet(G4PARTICLE_PRIMARY)),
	yes_no(mask.isSet(G4PARTICLE_HAS_SECONDARIES)),
	int(p->daughters.size()),
	yes_no(mask.isSet(G4PARTICLE_ABOVE_ENERGY_THRESHOLD)),
	p.energy(),
	yes_no(mask.isSet(G4PARTICLE_CREATED_CALORIMETER_HIT)),
	yes_no(mask.isSet(G4PARTICLE_CREATED_TRACKER_HIT)),
	yes_no(mask.isSet(G4PARTICLE_KEEP_PROCESS)),
	mask.isSet(G4PARTICLE_KEEP_PARENT) ? "YES" : "",
	p.numParent(),
	proc_name.c_str(),
	p->process ? "/" : "",
	proc_type.c_str()
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

  print("+++ MC Particles #Tracks:%7d ParticleType Parent/Geant4 "
	"Primary Secondary Energy in [MeV] Calo Tracker Process/Par Details",
	int(particles.size()));
  for(ParticleMap::const_iterator i=particles.begin(); i!=particles.end(); ++i)  {
    Geant4ParticleHandle p = (*i).second;
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
  print("+++ MC Particles #Tracks:%7d ParticleType Parent/Geant4 "
	"Primary Secondary Energy          Calo Tracker Process/Par",
	int(particles.size()));
  print("+++ MC Particle Summary:                       %7d %10d %7d  %7d      %9d %5d %6d",
	num_primary, num_secondaries, num_energy,
	num_calo_hits,num_tracker_hits,num_process,num_parent);
}

void Geant4ParticlePrint::printParticleTree(const ParticleMap& particles, int level, Geant4ParticleHandle p)  const  {
  char txt[32];
  size_t len = sizeof(txt)-1;
  // Ensure we do not overwrite the array
  if ( level>int(len)-3 ) level=len-3;
  
  ::snprintf(txt,sizeof(txt),"%5d ",level);
  ::memset(txt+6,' ',len-6);
  txt[len-1] = 0;
  txt[len-2] = '>';
  txt[level+6]='+';
  ::memset(txt+level+6+1,'-',len-level-3-6);

  printParticle(txt, p);
  const set<int>& daughters = p->daughters;
  // For all particles, the set of daughters must be contained in the record.
  for(set<int>::const_iterator id=daughters.begin(); id!=daughters.end(); ++id)   {
    int id_dau = *id;
    Geant4ParticleHandle dau = (*particles.find(id_dau)).second;
    printParticleTree(particles,level+1,dau);
  }
}

/// Print tree of kept particles
void Geant4ParticlePrint::printParticleTree(const ParticleMap& particles)  const  {
  print("+++ MC Particle Parent daughter relationships. [%d particles]",int(particles.size()));
  print("+++ MC Particles %12s #Tracks:%7d %-12s Parent%-7s "
	"Primary Secondary Energy %-8s Calo Tracker Process/Par  Details",
	"",int(particles.size()),"ParticleType","","in [MeV]");
  for(ParticleMap::const_iterator i=particles.begin(); i!=particles.end(); ++i)  {
    Geant4ParticleHandle p = (*i).second;
    PropertyMask mask(p->reason);
    if ( mask.isSet(G4PARTICLE_PRIMARY) ) printParticleTree(particles,0,p);
  }
}
