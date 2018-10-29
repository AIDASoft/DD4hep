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
#include "DDG4/Geant4PhysicsList.h"
#include "DDG4/Geant4UIMessenger.h"
#include "DDG4/Geant4Particle.h"
#include "DDG4/Geant4Kernel.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Plugins.h"

// Geant4 include files
#include "G4VPhysicsConstructor.hh"
#include "G4PhysListFactory.hh"
#include "G4ProcessManager.hh"
#include "G4ParticleTable.hh"
#include "G4RunManager.hh"
#include "G4VProcess.hh"
#include "G4Decay.hh"

// C/C++ include files
#include <stdexcept>
#include <regex.h>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::sim;

namespace {

  struct MyPhysics : G4VUserPhysicsList  {
    void AddTransportation()   {  this->G4VUserPhysicsList::AddTransportation(); }
  };

  struct EmptyPhysics : public G4VModularPhysicsList {
    EmptyPhysics() {}
    virtual ~EmptyPhysics() {}
    virtual void ConstructProcess()      {}
    virtual void ConstructParticle()      {}
  };
  struct ParticlePhysics : public G4VPhysicsConstructor {
    Geant4PhysicsListActionSequence* seq;
    G4VUserPhysicsList*              phys;
    ParticlePhysics(Geant4PhysicsListActionSequence* s, G4VUserPhysicsList* p)
      : seq(s), phys(p) {}
    virtual ~ParticlePhysics() {}
    virtual void ConstructProcess()  {
      seq->constructProcesses(phys);
      if ( seq->transportation() )   {
        MyPhysics* ph = (MyPhysics*)phys;
        ph->AddTransportation();
      }
    }
    virtual void ConstructParticle()     {  seq->constructParticles(phys);     }
  };
}

/// Default constructor
Geant4PhysicsList::Process::Process()
  : ordAtRestDoIt(-1), ordAlongSteptDoIt(-1), ordPostStepDoIt(-1) {
}
/// Copy constructor
Geant4PhysicsList::Process::Process(const Process& p)
  : name(p.name), ordAtRestDoIt(p.ordAtRestDoIt), ordAlongSteptDoIt(p.ordAlongSteptDoIt), ordPostStepDoIt(p.ordPostStepDoIt) {
}

/// Assignment operator
Geant4PhysicsList::Process& Geant4PhysicsList::Process::operator=(const Process& p) {
  if ( this != &p )  {
    name = p.name;
    ordAtRestDoIt     = p.ordAtRestDoIt;
    ordAlongSteptDoIt = p.ordAlongSteptDoIt;
    ordPostStepDoIt   = p.ordPostStepDoIt;
  }
  return *this;
}

/// Standard constructor
Geant4PhysicsList::Geant4PhysicsList(Geant4Context* ctxt, const string& nam)
  : Geant4Action(ctxt, nam) {
  InstanceCount::increment(this);
}

/// Default destructor
Geant4PhysicsList::~Geant4PhysicsList() {
  InstanceCount::decrement(this);
}

/// Install command control messenger if wanted
void Geant4PhysicsList::installCommandMessenger()   {
  control()->addCall("dump", "Dump content of " + name(), Callback(this).make(&Geant4PhysicsList::dump));
}

/// Dump content to stdout
void Geant4PhysicsList::dump()    {
  if ( !m_physics.empty() && !m_particles.empty() && !m_processes.empty() )  {
    printout(ALWAYS,name(),"+++ Geant4PhysicsList Dump");
  }
  for (PhysicsConstructors::const_iterator i = m_physics.begin(); i != m_physics.end(); ++i)
    printout(ALWAYS,name(),"+++ PhysicsConstructor:           %s",(*i).c_str());
  for (ParticleConstructors::const_iterator i = m_particles.begin(); i != m_particles.end(); ++i)
    printout(ALWAYS,name(),"+++ ParticleConstructor:          %s",(*i).c_str());
  for (PhysicsProcesses::const_iterator i = m_processes.begin(); i != m_processes.end(); ++i) {
    const string& part_name = (*i).first;
    const ParticleProcesses& procs = (*i).second;
    printout(ALWAYS,name(),"+++ PhysicsProcesses of particle  %s",part_name.c_str());
    for (ParticleProcesses::const_iterator ip = procs.begin(); ip != procs.end(); ++ip) {
      const Process& p = (*ip);
      printout(ALWAYS,name(),"+++        Process    %s  ordAtRestDoIt=%d ordAlongSteptDoIt=%d ordPostStepDoIt=%d",
               p.name.c_str(),p.ordAtRestDoIt,p.ordAlongSteptDoIt,p.ordPostStepDoIt);
    }
  }
}

/// Add physics particle constructor by name
void Geant4PhysicsList::addParticleConstructor(const std::string& part_name)   {
  particles().push_back(part_name);
}

/// Add physics particle constructor by name
void Geant4PhysicsList::addParticleGroup(const std::string& part_name)   {
  particlegroups().push_back(part_name);
}

/// Add particle process by name with arguments
void Geant4PhysicsList::addParticleProcess(const std::string& part_name,
                                           const std::string& proc_name,
                                           int ordAtRestDoIt,
                                           int ordAlongSteptDoIt,
                                           int ordPostStepDoIt)
{
  Process p;
  p.name = proc_name;
  p.ordAtRestDoIt     = ordAtRestDoIt;
  p.ordAlongSteptDoIt = ordAlongSteptDoIt;
  p.ordPostStepDoIt   = ordPostStepDoIt;
  processes(part_name).push_back(p);
}

/// Add PhysicsConstructor by name
void Geant4PhysicsList::addPhysicsConstructor(const std::string& phys_name)  {
  physics().push_back(phys_name);
}

/// Access processes for one particle type
Geant4PhysicsList::ParticleProcesses& Geant4PhysicsList::processes(const string& nam) {
  PhysicsProcesses::iterator i = m_processes.find(nam);
  if (i != m_processes.end()) {
    return (*i).second;
  }
  pair<PhysicsProcesses::iterator, bool> ret = m_processes.insert(make_pair(nam, ParticleProcesses()));
  return (*(ret.first)).second;
}

/// Access processes for one particle type (CONST)
const Geant4PhysicsList::ParticleProcesses& Geant4PhysicsList::processes(const string& nam) const {
  PhysicsProcesses::const_iterator i = m_processes.find(nam);
  if (i != m_processes.end()) {
    return (*i).second;
  }
  except("Failed to access the physics process '%s' [Unknown-Process]", nam.c_str());
  throw runtime_error("Failed to access the physics process"); // never called anyway
}

/// Add PhysicsConstructor by name
void Geant4PhysicsList::adoptPhysicsConstructor(Geant4Action* action)  {
  if ( 0 != action )   {
    G4VPhysicsConstructor* p = dynamic_cast<G4VPhysicsConstructor*>(action);
    if ( p )   {
      PhysicsConstructor ctor(action->name());
      ctor.pointer = p;
      action->addRef();
      m_physics.push_back(ctor);
      return;
    }
    except("Failed to adopt action object %s as physics constructor. [Invalid-Base]",action->c_name());
  }
  except("Failed to adopt invalid Geant4Action as PhysicsConstructor. [Invalid-object]");
}

/// Callback to construct particle decays
void Geant4PhysicsList::constructPhysics(G4VModularPhysicsList* physics_pointer) {
  debug("constructPhysics %p", physics_pointer);
  for (PhysicsConstructors::iterator i=m_physics.begin(); i != m_physics.end(); ++i) {
    PhysicsConstructors::value_type& ctor = *i;
    if ( 0 == ctor.pointer )   {
      G4VPhysicsConstructor* p = PluginService::Create<G4VPhysicsConstructor*>(ctor);
      if (!p) {
        except("Failed to create the physics entities "
               "for the G4VPhysicsConstructor '%s'", ctor.c_str());
      }
      ctor.pointer = p;
    }
    physics_pointer->RegisterPhysics(ctor.pointer);
    info("Registered Geant4 physics constructor %s to physics list", ctor.c_str());
  }
}

/// constructParticle callback
void Geant4PhysicsList::constructParticles(G4VUserPhysicsList* physics_pointer) {
  debug("constructParticles %p", physics_pointer);
  /// Now define all particles
  for (ParticleConstructors::const_iterator i = m_particles.begin(); i != m_particles.end(); ++i) {
    const ParticleConstructors::value_type& ctor = *i;
    G4ParticleDefinition* def = PluginService::Create<G4ParticleDefinition*>(ctor);
    if (!def) {
      /// Check if we have here a particle group constructor
      long* result = (long*) PluginService::Create<long>(ctor);
      if (!result || *result != 1L) {
        except("Failed to create particle type '%s' result=%d", ctor.c_str(), result);
      }
      info("Constructed Geant4 particle %s [using signature long (*)()]",ctor.c_str());
    }
  }
  /// Now define all particles
  for (ParticleConstructors::const_iterator i = m_particlegroups.begin(); i != m_particlegroups.end(); ++i) {
    const ParticleConstructors::value_type& ctor = *i;
    /// Check if we have here a particle group constructor
    long* result = (long*) PluginService::Create<long>(ctor);
    if (!result || *result != 1L) {
      except("Failed to create particle type '%s' result=%d", ctor.c_str(), result);
    }
  }
}

/// Callback to construct processes (uses the G4 particle table)
void Geant4PhysicsList::constructProcesses(G4VUserPhysicsList* physics_pointer) {
  debug("constructProcesses %p", physics_pointer);
  for (PhysicsProcesses::const_iterator i = m_processes.begin(); i != m_processes.end(); ++i) {
    const string& part_name = (*i).first;
    const ParticleProcesses& procs = (*i).second;
    vector<G4ParticleDefinition*> defs(Geant4ParticleHandle::g4DefinitionsRegEx(part_name));
    if (defs.empty()) {
      except("Particle:%s Cannot find the corresponding entry in the particle table.", part_name.c_str());
    }
    for (vector<G4ParticleDefinition*>::const_iterator id = defs.begin(); id != defs.end(); ++id) {
      G4ParticleDefinition* particle = *id;
      G4ProcessManager* mgr = particle->GetProcessManager();
      for (ParticleProcesses::const_iterator ip = procs.begin(); ip != procs.end(); ++ip) {
        const Process& p = (*ip);
        G4VProcess* g4 = PluginService::Create<G4VProcess*>(p.name);
        if (!g4) {   // Error no factory for this process
          except("Particle:%s -> [%s] Cannot create physics process %s", 
                 part_name.c_str(), particle->GetParticleName().c_str(), p.name.c_str());
        }
        mgr->AddProcess(g4, p.ordAtRestDoIt, p.ordAlongSteptDoIt, p.ordPostStepDoIt);
        info("Particle:%s -> [%s] added process %s with flags (%d,%d,%d)", 
             part_name.c_str(), particle->GetParticleName().c_str(), p.name.c_str(),
             p.ordAtRestDoIt, p.ordAlongSteptDoIt, p.ordPostStepDoIt);
      }
    }
  }
}

/// Enable physics list: actions necessary to be propagated to Geant4.
void Geant4PhysicsList::enable(G4VUserPhysicsList* /* physics */)  {
}

/// Standard constructor
Geant4PhysicsListActionSequence::Geant4PhysicsListActionSequence(Geant4Context* ctxt, const string& nam)
  : Geant4Action(ctxt, nam), m_transportation(false), m_decays(false), m_rangecut(0.7*CLHEP::mm) {
  declareProperty("transportation", m_transportation);
  declareProperty("extends",  m_extends);
  declareProperty("decays",   m_decays);
  declareProperty("rangecut", m_rangecut);
  m_needsControl = true;
  InstanceCount::increment(this);
}

/// Default destructor
Geant4PhysicsListActionSequence::~Geant4PhysicsListActionSequence() {
  m_actors(&Geant4Action::release);
  m_actors.clear();
  m_process.clear();
  InstanceCount::decrement(this);
}

/// Extend physics list from factory:
G4VUserPhysicsList* Geant4PhysicsListActionSequence::extensionList()    {
  G4VModularPhysicsList* physics = ( m_extends.empty() )
    ? new EmptyPhysics()
    : G4PhysListFactory().GetReferencePhysList(m_extends);
  // Register all physics constructors with the physics list
  constructPhysics(physics);
  // Ensure the particles and processes declared are also invoked.
  // Hence: Create a special physics constructor doing so.
  // Ownership is transferred to the physics list.
  // Do not delete this pointer afterwards....
  physics->RegisterPhysics(new ParticlePhysics(this,physics));
  return physics;
}

/// Install command control messenger if wanted
void Geant4PhysicsListActionSequence::installCommandMessenger()   {
  control()->addCall("dump", "Dump content of " + name(), Callback(this).make(&Geant4PhysicsListActionSequence::dump));
}

/// Dump content to stdout
void Geant4PhysicsListActionSequence::dump()    {
  printout(ALWAYS,name(),"+++ Dump");
  printout(ALWAYS,name(),"+++ Extension name       %s",m_extends.c_str());
  printout(ALWAYS,name(),"+++ Transportation flag: %d",m_transportation);
  printout(ALWAYS,name(),"+++ Program decays:      %d",m_decays);
  printout(ALWAYS,name(),"+++ RangeCut:            %f",m_rangecut);
  m_actors(&Geant4PhysicsList::dump);
}

/// Add an actor responding to all callbacks. Sequence takes ownership.
void Geant4PhysicsListActionSequence::adopt(Geant4PhysicsList* action) {
  if (action) {
    action->addRef();
    m_actors.add(action);
    return;
  }
  except("Geant4EventActionSequence: Attempt to add invalid actor!");
}

/// Callback to construct particles
void Geant4PhysicsListActionSequence::constructParticles(G4VUserPhysicsList* physics_pointer) {
  m_particle(physics_pointer);
  m_actors(&Geant4PhysicsList::constructParticles, physics_pointer);
}

/// Callback to execute physics constructors
void Geant4PhysicsListActionSequence::constructPhysics(G4VModularPhysicsList* physics_pointer) {
  m_physics(physics_pointer);
  m_actors(&Geant4PhysicsList::constructPhysics, physics_pointer);
}

/// constructProcess callback
void Geant4PhysicsListActionSequence::constructProcesses(G4VUserPhysicsList* physics_pointer) {
  m_actors(&Geant4PhysicsList::constructProcesses, physics_pointer);
  m_process(physics_pointer);
  if (m_decays) {
    constructDecays(physics_pointer);
  }
}

/// Callback to construct particle decays
void Geant4PhysicsListActionSequence::constructDecays(G4VUserPhysicsList* physics_pointer) {
  G4ParticleTable* pt = G4ParticleTable::GetParticleTable();
  G4ParticleTable::G4PTblDicIterator* iter = pt->GetIterator();
  // Add Decay Process
  G4Decay* decay = new G4Decay();
  info("ConstructDecays %p",physics_pointer);
  iter->reset();
  while ((*iter)()) {
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

/// Enable physics list: actions necessary to be propagated to Geant4.
void Geant4PhysicsListActionSequence::enable(G4VUserPhysicsList* physics_pointer)   {
  m_actors(&Geant4PhysicsList::enable, physics_pointer);
}

