// $Id: $
//==========================================================================
//  AIDA Detector description implementation for LCD
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
#include "DDG4/Geant4UserPhysicsList.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Plugins.h"

// Geant4 include files
#include "G4VPhysicsConstructor.hh"
#include "G4PhysListFactory.hh"
#include "G4ProcessManager.hh"
#include "G4ParticleTable.hh"
#include "G4VProcess.hh"
#include "G4Decay.hh"

// C/C++ include files
#include <stdexcept>
#include <regex.h>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;

namespace {

  struct EmptyPhysics : public G4VModularPhysicsList {
    EmptyPhysics() {}
    virtual ~EmptyPhysics() {}
    virtual void ConstructProcess()      {}
    virtual void ConstructParticle()      {}
  };

  void _findDef(const string& expression, vector<G4ParticleDefinition*>& results) {
    string exp = expression;   //'^'+expression+"$";
    G4ParticleTable* pt = G4ParticleTable::GetParticleTable();
    G4ParticleTable::G4PTblDicIterator* iter = pt->GetIterator();
    char msgbuf[128];
    regex_t reg;
    int ret = ::regcomp(&reg, exp.c_str(), 0);
    if (ret) {
      throw runtime_error(format("Geant4PhysicsList", "REGEX: Failed to compile particle name %s", exp.c_str()));
    }
    results.clear();
    iter->reset();
    while ((*iter)()) {
      G4ParticleDefinition* p = iter->value();
      ret = ::regexec(&reg, p->GetParticleName().c_str(), 0, NULL, 0);
      if (!ret)
        results.push_back(p);
      else if (ret == REG_NOMATCH)
        continue;
      else {
        ::regerror(ret, &reg, msgbuf, sizeof(msgbuf));
        ::regfree(&reg);
        throw runtime_error(format("Geant4PhysicsList", "REGEX: Failed to match particle name %s err=%s", exp.c_str(), msgbuf));
      }
    }
    ::regfree(&reg);
  }
}

/// Standard constructor with initailization parameters
Geant4UserPhysics::Geant4UserPhysics() {
  InstanceCount::increment(this);
}

/// Default destructor
Geant4UserPhysics::~Geant4UserPhysics() {
  InstanceCount::decrement(this);
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
    ordAtRestDoIt = p.ordAtRestDoIt;
    ordAlongSteptDoIt = p.ordAlongSteptDoIt;
    ordPostStepDoIt = p.ordPostStepDoIt;
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
  printout(ALWAYS,name(),"+++ Geant4PhysicsList Dump");
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
  throw runtime_error(format("Geant4PhysicsList", "Failed to access the physics process '%s' [Unknown-Process]", nam.c_str()));
}

/// Callback to construct the physics list
void Geant4PhysicsList::constructProcess(Geant4UserPhysics* physics_pointer) {
  printout(INFO, "Geant4PhysicsList", "%s> constructProcess %p", name().c_str(), physics_pointer);
  constructPhysics(physics_pointer);
  constructProcesses(physics_pointer);
}

/// Callback to construct particle decays
void Geant4PhysicsList::constructPhysics(Geant4UserPhysics* physics_pointer) {
  for (PhysicsConstructors::const_iterator i = m_physics.begin(); i != m_physics.end(); ++i) {
    const PhysicsConstructors::value_type& ctor = *i;
    G4VPhysicsConstructor* p = PluginService::Create<G4VPhysicsConstructor*>(ctor);
    if (!p) {
      throw runtime_error(format("Geant4PhysicsList", "Failed to create the physics entities "
                                 "for the G4VPhysicsConstructor '%s'", ctor.c_str()));
    }
    physics_pointer->RegisterPhysics(p);
    printout(INFO, "Geant4PhysicsList", "%s> registered Geant4 physics %s", name().c_str(), ctor.c_str());
  }
}

/// constructParticle callback
void Geant4PhysicsList::constructParticles(Geant4UserPhysics* physics_pointer) {
  printout(INFO, "Geant4PhysicsList", "%s> constructParticles %p", name().c_str(), physics_pointer);
  /// Now define all particles
  for (ParticleConstructors::const_iterator i = m_particles.begin(); i != m_particles.end(); ++i) {
    const ParticleConstructors::value_type& ctor = *i;
    G4ParticleDefinition* def = PluginService::Create<G4ParticleDefinition*>(ctor);
    if (!def) {
      /// Check if we have here a particle group constructor
      long* result = (long*) PluginService::Create<long>(ctor);
      if (!result || *result != 1L) {
        throw runtime_error(format("Geant4PhysicsList", "Failed to create particle type '%s' result=%d", ctor.c_str(), result));
      }
    }
    printout(INFO, "Geant4PhysicsList", "%s> constructed Geant4 particle %s", name().c_str(), ctor.c_str());
  }
}

/// Callback to construct processes (uses the G4 particle table)
void Geant4PhysicsList::constructProcesses(Geant4UserPhysics* physics_pointer) {
  printout(INFO, "Geant4PhysicsList", "%s> constructProcesses %p", name().c_str(), physics_pointer);
  for (PhysicsProcesses::const_iterator i = m_processes.begin(); i != m_processes.end(); ++i) {
    const string& part_name = (*i).first;
    const ParticleProcesses& procs = (*i).second;
    vector<G4ParticleDefinition*> defs;
    _findDef(part_name, defs);
    if (defs.empty()) {
      throw runtime_error(format("Geant4PhysicsList", "Particle:%s "
                                 "Cannot find the corresponding entry in the particle table.", part_name.c_str()));
    }
    for (vector<G4ParticleDefinition*>::const_iterator id = defs.begin(); id != defs.end(); ++id) {
      G4ParticleDefinition* particle = *id;
      G4ProcessManager* mgr = particle->GetProcessManager();
      for (ParticleProcesses::const_iterator ip = procs.begin(); ip != procs.end(); ++ip) {
        const Process& p = (*ip);
        G4VProcess* g4 = PluginService::Create<G4VProcess*>(p.name);
        if (!g4) {   // Error no factory for this process
          throw runtime_error(format("Geant4PhysicsList", "Particle:%s -> [%s] "
                                     "Cannot create physics process %s", part_name.c_str(), particle->GetParticleName().c_str(), p.name.c_str()));
        }
        mgr->AddProcess(g4, p.ordAtRestDoIt, p.ordAlongSteptDoIt, p.ordPostStepDoIt);
        printout(INFO, "Geant4PhysicsList", "Particle:%s -> [%s] "
                 "added process %s with flags (%d,%d,%d)", part_name.c_str(), particle->GetParticleName().c_str(), p.name.c_str(),
                 p.ordAtRestDoIt, p.ordAlongSteptDoIt, p.ordPostStepDoIt);
      }
    }
  }
}

/// Standard constructor
Geant4PhysicsListActionSequence::Geant4PhysicsListActionSequence(Geant4Context* ctxt, const string& nam)
  : Geant4Action(ctxt, nam), m_transportation(false), m_decays(false) {
  declareProperty("transportation", m_transportation);
  declareProperty("extends", m_extends);
  declareProperty("decays", m_decays);
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
G4VUserPhysicsList* Geant4PhysicsListActionSequence::extensionList()  const  {
  G4VModularPhysicsList* physics = ( m_extends.empty() )
    ? new EmptyPhysics()
    : G4PhysListFactory().GetReferencePhysList(m_extends);
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
  m_actors(&Geant4PhysicsList::dump);
}

/// Add an actor responding to all callbacks. Sequence takes ownership.
void Geant4PhysicsListActionSequence::adopt(Geant4PhysicsList* action) {
  if (action) {
    action->addRef();
    m_actors.add(action);
    return;
  }
  throw runtime_error("Geant4EventActionSequence: Attempt to add invalid actor!");
}

/// begin-of-event callback
void Geant4PhysicsListActionSequence::constructParticles(Geant4UserPhysics* physics_pointer) {
  m_particle(physics_pointer);
  m_actors(&Geant4PhysicsList::constructParticles, physics_pointer);
}

/// constructProcess callback
void Geant4PhysicsListActionSequence::constructProcess(Geant4UserPhysics* physics_pointer) {
  m_actors(&Geant4PhysicsList::constructProcess, physics_pointer);
  m_process(physics_pointer);
  if (m_decays) {
    constructDecays(physics_pointer);
  }
  if (m_transportation) {
    physics_pointer->AddTransportation();
  }
}

/// Callback to construct particle decays
void Geant4PhysicsListActionSequence::constructDecays(Geant4UserPhysics* physics_pointer) {
  G4ParticleTable* pt = G4ParticleTable::GetParticleTable();
  G4ParticleTable::G4PTblDicIterator* iter = pt->GetIterator();
  // Add Decay Process
  G4Decay* decay = new G4Decay();
  printout(INFO, "Geant4PhysicsList", "%s> constructDecays %p", name().c_str(), physics_pointer);
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

