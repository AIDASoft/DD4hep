// $Id: Geant4Hits.cpp 513 2013-04-05 14:31:53Z gaede $
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
#include "DDG4/Geant4Particle.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4VProcess.hh"

using namespace DD4hep;
using namespace DD4hep::Simulation;

/// Default destructor
ParticleExtension::~ParticleExtension() {
}

/// Copy constructor
Geant4Particle::Geant4Particle(const Geant4Particle& c)
  : ref(1), id(c.id), g4Parent(c.g4Parent), reason(c.reason), usermask(c.usermask),
    steps(c.steps), secondaries(c.secondaries), pdgID(c.pdgID),
    status(c.status), charge(0),
    vsx(c.vsx), vsy(c.vsy), vsz(c.vsz), 
    vex(c.vex), vey(c.vey), vez(c.vez), 
    psx(c.psx), psy(c.psy), psz(c.psz), 
    pex(c.pex), pey(c.pey), pez(c.pez), 
    mass(c.mass), time(c.time), properTime(c.properTime),
    parents(c.parents), daughters(c.daughters), extension(),
    process(c.process), definition(c.definition)
{
  InstanceCount::increment(this);
  spin[0] = c.spin[0];
  spin[1] = c.spin[1];
  spin[2] = c.spin[2];
  colorFlow[0] = c.colorFlow[0];
  colorFlow[1] = c.colorFlow[1];
}

/// Default constructor
Geant4Particle::Geant4Particle()
  : ref(1), id(0), g4Parent(0), reason(0), usermask(0), 
    steps(0), secondaries(0), pdgID(0),
    status(0), charge(0),
    vsx(0.0), vsy(0.0), vsz(0.0), 
    vex(0.0), vey(0.0), vez(0.0), 
    psx(0.0), psy(0.0), psz(0.0), 
    pex(0.0), pey(0.0), pez(0.0), 
    mass(0.0), time(0.0), properTime(0.0),
    daughters(), extension(), process(0), definition(0)
{
  InstanceCount::increment(this);
  spin[0] = spin[1] = spin[2] = 0;
  colorFlow[0] = colorFlow[1] = 0;
}

/// Constructor with ID initialization
Geant4Particle::Geant4Particle(int part_id)
  : ref(1), id(part_id), g4Parent(0), reason(0), usermask(0), 
    steps(0), secondaries(0), pdgID(0),
    status(0), charge(0),
    vsx(0.0), vsy(0.0), vsz(0.0), 
    vex(0.0), vey(0.0), vez(0.0), 
    psx(0.0), psy(0.0), psz(0.0), 
    pex(0.0), pey(0.0), pez(0.0), 
    mass(0.0), time(0.0), properTime(0.0),
    daughters(), extension(), process(0), definition(0)
{
  InstanceCount::increment(this);
  spin[0] = spin[1] = spin[2] = 0;
  colorFlow[0] = colorFlow[1] = 0;
}

/// Default destructor
Geant4Particle::~Geant4Particle()  {
  InstanceCount::decrement(this);
  //::printf("************ Delete Geant4Particle[%p]: ID:%d pdgID %d ref:%d\n",(void*)this,id,pdgID,ref);
}

void Geant4Particle::release()  {
  //::printf("************ Release Geant4Particle[%p]: ID:%d pdgID %d ref:%d\n",(void*)this,id,pdgID,ref-1);
  if ( --ref <= 0 )  {
    delete this;
  }
}

/// Assignment operator
Geant4Particle& Geant4Particle::get_data(Geant4Particle& c)   {
  if ( this != &c )  {
    id = c.id; 
    g4Parent    = c.g4Parent;
    reason      = c.reason; 
    usermask    = c.usermask;
    status      = c.status;
    charge      = c.charge;
    steps       = c.steps; 
    secondaries = c.secondaries;
    pdgID       = c.pdgID;
    vsx         = c.vsx;
    vsy         = c.vsy;
    vsz         = c.vsz; 
    vex         = c.vex;
    vey         = c.vey;
    vez         = c.vez; 
    psx         = c.psx;
    psy         = c.psy;
    psz         = c.psz; 
    pex         = c.pex;
    pey         = c.pey;
    pez         = c.pez; 
    mass        = c.mass;
    time        = c.time;
    properTime  = c.properTime;
    process     = c.process; 
    definition  = c.definition;
    daughters   = c.daughters;
    parents     = c.parents;
    extension   = c.extension;
    //auto_ptr<ParticleExtension>(c.extension.release());
  }
  return *this;
}

/// Remove daughter from set
void Geant4Particle::removeDaughter(int id_daughter)  {
  std::set<int>::iterator j = daughters.find(id_daughter);
  if ( j != daughters.end() ) daughters.erase(j);
}

/// Access to the Geant4 particle name
std::string Geant4ParticleHandle::particleName() const   {
  if ( particle->definition ) return particle->definition->GetParticleName();
  G4ParticleTable*      tab = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* def = tab->FindParticle(particle->pdgID);
  if ( def )   {
    particle->definition = def;
    return def->GetParticleName();
  }
#if 0
  TDatabasePDG* db = TDatabasePDG::Instance();
  TParticlePDG* pdef = db->GetParticle(particle->pdgID);
  if ( pdef ) return pdef->GetName();
#endif
  char text[32];
  ::snprintf(text,sizeof(text),"PDG:%d",particle->pdgID);
  return text;
}

/// Access to the Geant4 particle type
std::string Geant4ParticleHandle::particleType() const   {
  if ( particle->definition ) return particle->definition->GetParticleType();
  G4ParticleTable*      tab = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* def = tab->FindParticle(particle->pdgID);
  if ( def )   {
    particle->definition = def;
    return def->GetParticleType();
  }
#if 0
  TDatabasePDG* db = TDatabasePDG::Instance();
  TParticlePDG* pdef = db->GetParticle(particle->pdgID);
  if ( pdef ) return pdef->ParticleClass();
#endif
  char text[32];
  ::snprintf(text,sizeof(text),"PDG:%d",particle->pdgID);
  return text;
}

/// Access to the creator process name
std::string Geant4ParticleHandle::processName() const   {
  if ( particle->process ) return particle->process->GetProcessName();
  else if ( particle->reason&G4PARTICLE_PRIMARY ) return "Primary";
  else if ( particle->status&G4PARTICLE_GEN_HISTORY ) return "Gen.History";
  else if ( particle->status&G4PARTICLE_GEN_CREATED ) return "Generator";
  return "???";
}

/// Access to the creator process type name
std::string Geant4ParticleHandle::processTypeName() const   {
  if ( particle->process )   {
    return G4VProcess::GetProcessTypeName(particle->process->GetProcessType());
  }
  return "";
}

/// Offset all particle identifiers (id, parents, daughters) by a constant number
void Geant4ParticleHandle::offset(int off)  const   {
  std::set<int> temp;
  Geant4Particle* p = particle;
  p->id += off;

  temp = p->daughters;
  p->daughters.clear();
  for(std::set<int>::iterator i=temp.begin(); i != temp.end(); ++i)
    p->daughters.insert((*i)+off);

  temp = p->parents;
  p->parents.clear();
  for(std::set<int>::iterator i=temp.begin(); i != temp.end(); ++i)
    p->parents.insert((*i)+off);
}

/// Output type 1:+++ <tag>   10 def:0xde4eaa8 [gamma     ,   gamma] reason:      20 E:+1.017927e+03  #Par:  1/4    #Dau:  2
void Geant4ParticleHandle::dump1(int level, const std::string& src, const char* tag) const   {
  char text[256];
  Geant4ParticleHandle p(*this);
  text[0]=0;
  if ( p->parents.size() == 1 ) 
    ::snprintf(text,sizeof(text),"/%d",*(p->parents.begin()));
  else if ( p->parents.size() >  1 )   {
    text[0]='/';text[1]=0;
    for(std::set<int>::const_iterator i=p->parents.begin(); i!=p->parents.end(); ++i)
      ::snprintf(text+strlen(text),sizeof(text)-strlen(text),"%d ",*i);
  }
  printout((DD4hep::PrintLevel)level,src,
	   "+++ %s %4d def [%-11s,%8s] reason:%8d E:%+.2e %3s #Dau:%3d #Par:%3d%-5s",
	   tag, p->id, 
	   p.particleName().c_str(),
	   p.particleType().c_str(),
	   p->reason,
	   p.energy(),
	   p->g4Parent>0 ? "Sim" : "Gen",
	   int(p->daughters.size()),
	   int(p->parents.size()),text);
}

/// Output type 2:+++ <tag>   20 G4:   7 def:0xde4eaa8 [gamma     ,   gamma] reason:      20 E:+3.304035e+01 in record:YES  #Par:  1/18   #Dau:  0
void Geant4ParticleHandle::dump2(int level, const std::string& src, const char* tag, int g4id, bool inrec) const   {
  char text[32];
  Geant4ParticleHandle p(*this);
  if ( p->parents.size() == 0 ) text[0]=0;
  else if ( p->parents.size() == 1 ) ::snprintf(text,sizeof(text),"/%d",*(p->parents.begin()));
  else if ( p->parents.size() >  1 ) ::snprintf(text,sizeof(text),"/%d..",*(p->parents.begin()));
  printout((DD4hep::PrintLevel)level,src,
	   "+++ %s %4d G4:%4d def [%-11s,%8s] reason:%8d "
	   "E:%+.2e in record:%s  #Par:%3d%-5s #Dau:%3d",
	   tag, p->id, g4id,
	   p.particleName().c_str(),
	   p.particleType().c_str(),
	   p->reason,
	   p.energy(),
	   yes_no(inrec),
	   int(p->parents.size()),text,
	   int(p->daughters.size()));
}

/// Output type 3:+++ <tag> ID:  0 e-           status:00000014 type:       11 Vertex:(+0.00e+00,+0.00e+00,+0.00e+00) [mm] time: +0.00e+00 [ns] #Par:  0 #Dau:  4
void Geant4ParticleHandle::dump3(int level, const std::string& src, const char* tag) const  {
  char text[256];
  Geant4ParticleHandle p(*this);
  text[0]=0;
  if ( p->parents.size() == 1 ) 
    ::snprintf(text,sizeof(text),"/%d",*(p->parents.begin()));
  else if ( p->parents.size() >  1 )   {
    text[0]='/';text[1]=0;
    for(std::set<int>::const_iterator i=p->parents.begin(); i!=p->parents.end(); ++i)
      ::snprintf(text+strlen(text),sizeof(text)-strlen(text),"%d ",*i);
  }
  printout((DD4hep::PrintLevel)level,src,
	   "+++ %s ID:%3d %-12s status:%08X typ:%9d Vtx:(%+.2e,%+.2e,%+.2e)[mm] "
	   "time: %+.2e [ns] #Dau:%3d #Par:%1d%-6s",
	   tag,p->id,p.particleName().c_str(),p->status,p->pdgID,
	   p->vsx/mm,p->vsy/mm,p->vsz/mm,p->time/ns,
	   p->daughters.size(),
	   p->parents.size(),
	   text);
}

/// Default destructor
Geant4ParticleMap::~Geant4ParticleMap()    {
  clear();
}

/// Clear particle maps
void Geant4ParticleMap::clear()    {
  releaseObjects(particleMap)();
  particleMap.clear();
  equivalentTracks.clear();
}

/// Adopt particle maps
void Geant4ParticleMap::adopt(ParticleMap& pm, TrackEquivalents& equiv)    {
  clear();
  particleMap = pm;
  equivalentTracks = equiv;
  pm.clear();
  equiv.clear();
}

/// Access the equivalent track id (shortcut to the usage of TrackEquivalents)
int Geant4ParticleMap::particleID(int g4_id, bool) const   {
  int equiv_id  = g4_id;
  if ( g4_id != 0 )  {
    ParticleMap::const_iterator ipar;
    while( (ipar=particleMap.find(equiv_id)) == particleMap.end() )  {
      TrackEquivalents::const_iterator iequiv = equivalentTracks.find(equiv_id);
      equiv_id = (iequiv == equivalentTracks.end()) ? -1 : (*iequiv).second;
      if ( equiv_id < 0 ) {
	printout(INFO,"Geant4ParticleMap",
		 "+++ No Equivalent particle for track:%d last known is:%d",
		 g4_id,equiv_id);
	break;
      }
    }
  }
  return equiv_id;
}
