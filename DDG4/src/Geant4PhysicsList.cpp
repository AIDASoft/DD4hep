// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DDG4/Geant4PhysicsList.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Plugins.h"

#include "G4VPhysicsConstructor.hh"
#include "G4ParticleTable.hh"
#include "G4ProcessManager.hh"
#include "G4VProcess.hh"
#include "G4Decay.hh"

// C/C++ include files
#include <stdexcept>
#include <regex.h>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;

namespace {
  void _findDef(const string& expression, vector<G4ParticleDefinition*>& results)  {
    string exp = expression; //'^'+expression+"$";
    G4ParticleTable* pt = G4ParticleTable::GetParticleTable();
    G4ParticleTable::G4PTblDicIterator* iter = pt->GetIterator();
    char msgbuf[128];
    regex_t reg;
    int ret = ::regcomp(&reg,exp.c_str(),0);
    if ( ret )   {
      throw runtime_error(format("Geant4PhysicsList",
				 "REGEX: Failed to compile particle name %s",exp.c_str()));
    }
    results.clear();
    iter->reset();
    while( (*iter)() ){
      G4ParticleDefinition* p = iter->value();
      ret = ::regexec(&reg,p->GetParticleName().c_str(), 0, NULL, 0);
      if( !ret ) 
	results.push_back(p);
      else if( ret == REG_NOMATCH )
	continue;
      else  {
	::regerror(ret, &reg, msgbuf, sizeof(msgbuf));
	::regfree(&reg);
	throw runtime_error(format("Geant4PhysicsList",
				   "REGEX: Failed to match particle name %s err=%s",
				   exp.c_str(),msgbuf));
      }
    }
    ::regfree(&reg);
  }
}

/// Standard constructor with initailization parameters
Geant4UserPhysics::Geant4UserPhysics()
{
  InstanceCount::increment(this);
}

/// Default destructor
Geant4UserPhysics::~Geant4UserPhysics()   {
  InstanceCount::decrement(this);
}

/// Default constructor
Geant4PhysicsList::Process::Process() : ordAtRestDoIt(-1), ordAlongSteptDoIt(-1), ordPostStepDoIt(-1) 
{
}
/// Copy constructor
Geant4PhysicsList::Process::Process(const Process& p) 
: name(p.name), ordAtRestDoIt(p.ordAtRestDoIt), 
  ordAlongSteptDoIt(p.ordAlongSteptDoIt), ordPostStepDoIt(p.ordPostStepDoIt) 
{
}

/// Assignment operator
Geant4PhysicsList::Process& Geant4PhysicsList::Process::operator=(const Process& p)  {
  name              = p.name;
  ordAtRestDoIt     = p.ordAtRestDoIt;
  ordAlongSteptDoIt = p.ordAlongSteptDoIt;
  ordPostStepDoIt   = p.ordPostStepDoIt;
  return *this;
}

/// Standard constructor
Geant4PhysicsList::Geant4PhysicsList(Geant4Context* context, const string& nam)
: Geant4Action(context,nam)
{
  InstanceCount::increment(this);
}

/// Default destructor
Geant4PhysicsList::~Geant4PhysicsList()  {
  InstanceCount::decrement(this);
}

/// Access processes for one particle type
Geant4PhysicsList::ParticleProcesses& Geant4PhysicsList::processes(const string& nam)
{
  PhysicsProcesses::iterator i = m_processes.find(nam);
  if ( i != m_processes.end() )  {
    return (*i).second;
  }
  pair<PhysicsProcesses::iterator,bool> ret = 
    m_processes.insert(make_pair(nam,ParticleProcesses()));
  return (*(ret.first)).second;
}

/// Access processes for one particle type (CONST)
const Geant4PhysicsList::ParticleProcesses& Geant4PhysicsList::processes(const string& nam) const
{
  PhysicsProcesses::const_iterator i = m_processes.find(nam);
  if ( i != m_processes.end() )  {
    return (*i).second;
  }
  throw runtime_error(format("Geant4PhysicsList",
			     "Failed to access the physics process '%s' [Unknown-Process]",
			     nam.c_str()));
}

/// Callback to construct particle decays
void Geant4PhysicsList::constructPhysics(Geant4UserPhysics* physics)   {
  for(PhysicsConstructors::const_iterator i=m_physics.begin(); i != m_physics.end(); ++i)   {
    const PhysicsConstructors::value_type& ctor = *i;
    G4VPhysicsConstructor* p = PluginService::Create<G4VPhysicsConstructor*>(ctor);
    if ( !p )    {
      throw runtime_error(format("Geant4PhysicsList","Failed to create the physics entities "
				 "for the G4VPhysicsConstructor '%s'",ctor.c_str()));
    }
    physics->RegisterPhysics(p);
    printout(INFO,"Geant4PhysicsList","%s> registered Geant4 physics %s",name().c_str(),ctor.c_str());
  }
}

/// constructParticle callback
void Geant4PhysicsList::constructParticles(Geant4UserPhysics* physics)   {
  printout(INFO,"Geant4PhysicsList","%s> constructParticles %p",name().c_str(),physics);
  for(ParticleConstructors::const_iterator i=m_particles.begin(); i!=m_particles.end(); ++i)   {
    const ParticleConstructors::value_type& ctor = *i;
    G4ParticleDefinition* def = PluginService::Create<G4ParticleDefinition*>(ctor);
    if ( !def )    {
      /// Check if we have here a particle group constructor
      long* result =  (long*)PluginService::Create<long>(ctor);
      if ( !result || *result != 1L )  { 
	throw runtime_error(format("Geant4PhysicsList","Failed to create particle type '%s' result=%d",
				   ctor.c_str(),result));
      }
    }
    printout(INFO,"Geant4PhysicsList","%s> constructed Geant4 particle %s",name().c_str(),ctor.c_str());
  }
}

#if 0
#include "G4BosonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4OpticalPhoton.hh"
#include "G4DecayTable.hh"
#include "G4PhaseSpaceDecayChannel.hh"

  G4BosonConstructor::ConstructParticle();
  G4LeptonConstructor::ConstructParticle();
  G4BaryonConstructor::ConstructParticle();
  G4MesonConstructor::ConstructParticle();
  G4OpticalPhoton::Definition();

  G4ParticleTable* pt = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* d;
  G4DecayTable* table;
  G4VDecayChannel* mode;
  // Bohr Magnetron
  G4double muB =  0.5*eplus*hbar_Planck/(electron_mass_c2/c_squared) ;   
  d=new G4ParticleDefinition(
			   "gamma",        0.0*MeV,       0.0*MeV,         0.0, 
			   2,              -1,            -1,          
			   0,               0,             0,             
			   "gamma",               0,             0,          22,
			   true,                0.0,          NULL,
			   false,           "photon",          22
			   );
  d=new G4ParticleDefinition(
			   "geantino",           0.0*MeV,       0.0*MeV,         0.0, 
			   0,                    0,             0,          
			   0,                    0,             0,             
			   "geantino",           0,             0,           0,
			   true,               0.0,          NULL,
			   false,        "geantino",            0
			   );
  //-----------------------------------------------------------------------------------
  d=new G4ParticleDefinition(
			   "e-",  electron_mass_c2,       0.0*MeV,    -1.*eplus, 
			   1,                 0,             0,          
			   0,                 0,             0,             
			   "lepton",                 1,             0,          11,
			   true,              -1.0,          NULL,
			   false,                  "e"
			   );
  d->SetPDGMagneticMoment( muB * 2.* 1.0011596521859 );
  //-----------------------------------------------------------------------------------
  d=new G4ParticleDefinition(
			   "e+", electron_mass_c2,       0.0*MeV,    +1.*eplus, 
			   1,                0,             0,          
			   0,                0,             0,             
			   "lepton",               -1,             0,          -11,
			   true,             -1.0,          NULL,
			   false,              "e"
			   );
  d->SetPDGMagneticMoment( muB * 2.* 1.0011596521859 );
  //-----------------------------------------------------------------------------------
  d = new G4ParticleDefinition(
			       "mu+", 0.105658367*GeV, 2.995912e-16*MeV,  +1.*eplus, 
			       1,               0,                0,          
			       0,               0,                0,             
			       "lepton",              -1,                0,        -13,
			       false,      2197.03*ns,             NULL,
			       false,           "mu"
			       );
  d->SetPDGMagneticMoment( muB * 2.* 1.0011659208);
  //-----------------------------------------------------------------------------------
  d = new G4ParticleDefinition(
			       "mu-", 0.105658367*GeV, 2.995912e-16*MeV,  -1.*eplus, 
			       1,               0,                0,          
			       0,               0,                0,             
			       "lepton",               1,                0,          13,
			       false,      2197.03*ns,             NULL,
			       false,           "mu"
			       );
  d->SetPDGMagneticMoment( muB * 2. * 1.0011659208);
  //-----------------------------------------------------------------------------------
  d = new G4ParticleDefinition(
			       "pi+",    0.1395701*GeV, 2.5284e-14*MeV,    +1.*eplus,
			       0,              -1,             0,
			       2,              +2,            -1,
			       "meson",               0,             0,         211,
			       false,       26.033*ns,          NULL,
			       false,       "pi");
  table = new G4DecayTable();
  // create a decay channel
  // pi+ -> mu+ + nu_mu
  mode = new G4PhaseSpaceDecayChannel("pi+",1.00,2,"mu+","nu_mu");
  table->Insert(mode);
  d->SetDecayTable(table);
  //-----------------------------------------------------------------------------------
  d = new G4ParticleDefinition(
			       "pi-",    0.1395701*GeV, 2.5284e-14*MeV,    -1.*eplus,
			       0,              -1,             0,
			       2,              -2,            -1,
			       "meson",               0,             0,        -211,
			       false,       26.033*ns,          NULL,
			       false,       "pi");
  table = new G4DecayTable();
  // create a decay channel
  // pi+ -> mu+ + nu_mu
  mode = new G4PhaseSpaceDecayChannel("pi-",1.00,2,"mu-","anti_nu_mu");
  table->Insert(mode);
  d->SetDecayTable(table);
  //-----------------------------------------------------------------------------------
#endif

/// Callback to construct the physics list
void Geant4PhysicsList::constructProcess(Geant4UserPhysics* physics)   {
  printout(INFO,"Geant4PhysicsList","%s> constructProcess %p",name().c_str(),physics);
  constructPhysics(physics);
  constructProcesses(physics);
}

/// Callback to construct processes (uses the G4 particle table)
void Geant4PhysicsList::constructProcesses(Geant4UserPhysics* physics)   {
  printout(INFO,"Geant4PhysicsList","%s> constructProcesses %p",name().c_str(),physics);
  for(PhysicsProcesses::const_iterator i = m_processes.begin(); i!=m_processes.end(); ++i)  {
    const string& part_name = (*i).first;
    const ParticleProcesses& procs = (*i).second;
    vector<G4ParticleDefinition*> defs;
    _findDef(part_name, defs);
    if ( defs.empty() )   {
      throw runtime_error(format("Geant4PhysicsList", "Particle:%s "
				 "Cannot find the corresponding entry in the particle table.",
				 part_name.c_str()));
    }
    for(vector<G4ParticleDefinition*>::const_iterator id=defs.begin(); id!=defs.end(); ++id)  {
      G4ParticleDefinition* particle = *id;
      G4ProcessManager* mgr = particle->GetProcessManager();
      for (ParticleProcesses::const_iterator ip=procs.begin(); ip != procs.end(); ++ip)  {
	const Process& p = (*ip);
	G4VProcess* g4 = PluginService::Create<G4VProcess*>(p.name);
	if ( !g4 )  {  // Error no factory for this process
	  throw runtime_error(format("Geant4PhysicsList","Particle:%s -> [%s] "
				     "Cannot create physics process %s",
				     part_name.c_str(),particle->GetParticleName().c_str(),
				     p.name.c_str()));
	}
	mgr->AddProcess(g4,p.ordAtRestDoIt,p.ordAlongSteptDoIt,p.ordPostStepDoIt);
	printout(INFO,"Geant4PhysicsList","Particle:%s -> [%s] "
		 "added process %s with flags (%d,%d,%d)",
		 part_name.c_str(),particle->GetParticleName().c_str(),
		 p.name.c_str(),p.ordAtRestDoIt,p.ordAlongSteptDoIt,
		 p.ordPostStepDoIt);
      }
    }
  }
}

/// Standard constructor
Geant4PhysicsListActionSequence::Geant4PhysicsListActionSequence(Geant4Context* context, const string& nam)
: Geant4Action(context,nam), m_transportation(false), m_decays(false) 
{
  InstanceCount::increment(this);
  declareProperty("transportation",m_transportation);
  declareProperty("decays",m_decays);
}

/// Default destructor
Geant4PhysicsListActionSequence::~Geant4PhysicsListActionSequence()   {
  m_actors(&Geant4Action::release);
  m_actors.clear();
  m_process.clear();
  InstanceCount::decrement(this);
}

/// Add an actor responding to all callbacks. Sequence takes ownership.
void Geant4PhysicsListActionSequence::adopt(Geant4PhysicsList* action)  {
  if ( action )  {
    action->addRef();
    m_actors.add(action);
    return;
  }
  throw runtime_error("Geant4EventActionSequence: Attempt to add invalid actor!");
}

/// begin-of-event callback
void Geant4PhysicsListActionSequence::constructParticles(Geant4UserPhysics* physics)  {
  m_particle(physics);
  m_actors(&Geant4PhysicsList::constructParticles,physics);
}

/// constructProcess callback
void Geant4PhysicsListActionSequence::constructProcess(Geant4UserPhysics* physics)   {
  m_actors(&Geant4PhysicsList::constructProcess,physics);
  m_process(physics);
  if ( m_decays )  {
    constructDecays(physics);
  }
  if ( m_transportation )  {
    physics->AddTransportation();
  }
}

/// Callback to construct particle decays
void Geant4PhysicsListActionSequence::constructDecays(Geant4UserPhysics* physics)   {
  G4ParticleTable* pt = G4ParticleTable::GetParticleTable();
  G4ParticleTable::G4PTblDicIterator* iter = pt->GetIterator();
  // Add Decay Process
  G4Decay* decay = new G4Decay();
  printout(INFO,"Geant4PhysicsList","%s> constructDecays %p",name().c_str(),physics);
  iter->reset();
  while( (*iter)() )  {
    G4ParticleDefinition* p = iter->value();
    G4ProcessManager* mgr = p->GetProcessManager();
    if (decay->IsApplicable(*p)) { 
      mgr->AddProcess(decay);
      // set ordering for PostStepDoIt and AtRestDoIt
      mgr->SetProcessOrdering(decay, idxPostStep);
      mgr->SetProcessOrdering(decay, idxAtRest);
    }
  }
}

