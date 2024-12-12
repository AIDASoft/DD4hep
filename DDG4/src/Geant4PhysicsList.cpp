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
#include <DDG4/Geant4PhysicsList.h>
#include <DDG4/Geant4UIMessenger.h>
#include <DDG4/Geant4Particle.h>
#include <DDG4/Geant4Kernel.h>
#include <DD4hep/InstanceCount.h>
#include <DD4hep/Printout.h>
#include <DD4hep/Plugins.h>
#include <DDG4/Celeritas.h>

// Geant4 include files
#include <G4VPhysicsConstructor.hh>
#include <G4PhysListFactory.hh>
#include <G4ProcessManager.hh>
#include <G4ParticleTable.hh>
#include <G4RunManager.hh>
#include <G4VProcess.hh>
#include <G4Decay.hh>
#include <G4EmParameters.hh>
#include <G4HadronicParameters.hh>

// C/C++ include files
#include <stdexcept>
#include <regex.h>

using namespace dd4hep::sim;

namespace {

  struct MyPhysics : G4VUserPhysicsList  {
    void AddTransportation()   {  this->G4VUserPhysicsList::AddTransportation(); }
  };

  struct EmptyPhysics : public G4VModularPhysicsList {
    EmptyPhysics() = default;
    virtual ~EmptyPhysics() = default;
  };
  struct ParticlePhysics : public G4VPhysicsConstructor {
    Geant4PhysicsListActionSequence* seq;
    G4VUserPhysicsList*              phys;
    ParticlePhysics(Geant4PhysicsListActionSequence* s, G4VUserPhysicsList* p) : seq(s), phys(p)  { }
    virtual ~ParticlePhysics() = default;
    virtual void ConstructProcess()  {
      seq->constructProcesses(phys);
      if ( seq->transportation() )   {
        MyPhysics* ph = (MyPhysics*)phys;
        ph->AddTransportation();
      }
    }
    virtual void ConstructParticle()  {
      seq->constructParticles(phys);
    }
  };
}

/// Default constructor
Geant4PhysicsList::Process::Process()
  : ordAtRestDoIt(-1), ordAlongSteptDoIt(-1), ordPostStepDoIt(-1)  {
}
/// Copy constructor
Geant4PhysicsList::Process::Process(const Process& p)
  : name(p.name), ordAtRestDoIt(p.ordAtRestDoIt), ordAlongSteptDoIt(p.ordAlongSteptDoIt), ordPostStepDoIt(p.ordPostStepDoIt)  {
}

/// Assignment operator
Geant4PhysicsList::Process& Geant4PhysicsList::Process::operator=(const Process& p)  {
  if ( this != &p )  {
    name = p.name;
    ordAtRestDoIt     = p.ordAtRestDoIt;
    ordAlongSteptDoIt = p.ordAlongSteptDoIt;
    ordPostStepDoIt   = p.ordPostStepDoIt;
  }
  return *this;
}

/// Standard constructor
Geant4PhysicsList::Geant4PhysicsList(Geant4Context* ctxt, const std::string& nam)
  : Geant4Action(ctxt, nam)  {
  InstanceCount::increment(this);
}

/// Default destructor
Geant4PhysicsList::~Geant4PhysicsList()  {
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
  for ( const auto& p : m_physics)
    printout(ALWAYS,name(),"+++ PhysicsConstructor:           %s",p.c_str());
  for ( const auto& p : m_particles )
    printout(ALWAYS,name(),"+++ ParticleConstructor:          %s",p.c_str());
  for ( const auto& p : m_particlegroups )
    printout(ALWAYS,name(),"+++ ParticleGroupConstructor:     %s",p.c_str());
  for ( const auto& [part_name,procs] : m_discreteProcesses )   {
    printout(ALWAYS,name(),"+++ DiscretePhysicsProcesses of particle  %s",part_name.c_str());
    for (ParticleProcesses::const_iterator ip = procs.begin(); ip != procs.end(); ++ip)  {
      printout(ALWAYS,name(),"+++        Process    %s", (*ip).name.c_str());
    }
  }
  for ( const auto& [part_name, procs] : m_processes )  {
    printout(ALWAYS,name(),"+++ PhysicsProcesses of particle  %s",part_name.c_str());
    for ( const Process& p : procs )    {
      printout(ALWAYS,name(),"+++        Process    %s  ordAtRestDoIt=%d ordAlongSteptDoIt=%d ordPostStepDoIt=%d",
               p.name.c_str(),p.ordAtRestDoIt,p.ordAlongSteptDoIt,p.ordPostStepDoIt);
    }
  }
}

/// Add physics particle constructor by name
void Geant4PhysicsList::addParticleConstructor(const std::string& part_name)   {
  particles().emplace_back(part_name);
}

/// Add physics particle constructor by name
void Geant4PhysicsList::addParticleGroup(const std::string& part_name)   {
  particlegroups().emplace_back(part_name);
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
  processes(part_name).emplace_back(p);
}

/// Add discrete particle process by name with arguments
void Geant4PhysicsList::addDiscreteParticleProcess(const std::string& part_name,
                                                   const std::string& proc_name)
{
  Process p;
  p.name = proc_name;
  discreteProcesses(part_name).emplace_back(p);
}

/// Add PhysicsConstructor by name
void Geant4PhysicsList::addPhysicsConstructor(const std::string& phys_name)  {
  physics().emplace_back(phys_name);
}

/// Access processes for one particle type
Geant4PhysicsList::ParticleProcesses& Geant4PhysicsList::processes(const std::string& nam)  {
  if (auto i = m_processes.find(nam); i != m_processes.end())
    return (*i).second;
  auto ret = m_processes.emplace(nam, ParticleProcesses());
  return (*(ret.first)).second;
}

/// Access processes for one particle type (CONST)
const Geant4PhysicsList::ParticleProcesses& Geant4PhysicsList::processes(const std::string& nam) const {
  if (auto i = m_processes.find(nam); i != m_processes.end())
    return (*i).second;
  except("Failed to access the physics process '%s' [Unknown-Process]", nam.c_str());
  throw std::runtime_error("Failed to access the physics process"); // never called anyway
}

/// Access discrete processes for one particle type
Geant4PhysicsList::ParticleProcesses& Geant4PhysicsList::discreteProcesses(const std::string& nam)  {
  if (auto i = m_discreteProcesses.find(nam); i != m_discreteProcesses.end())
    return (*i).second;
  auto ret = m_discreteProcesses.emplace(nam, ParticleProcesses());
  return (*(ret.first)).second;
}

/// Access discrete processes for one particle type (CONST)
const Geant4PhysicsList::ParticleProcesses& Geant4PhysicsList::discreteProcesses(const std::string& nam) const {
  if (auto i = m_discreteProcesses.find(nam); i != m_discreteProcesses.end())
    return (*i).second;
  except("Failed to access the physics process '%s' [Unknown-Process]", nam.c_str());
  throw std::runtime_error("Failed to access the physics process"); // never called anyway
}

/// Access physics constructor by name (CONST)
Geant4PhysicsList::PhysicsConstructor Geant4PhysicsList::physics(const std::string& nam)  const    {
  for ( const auto& ctor : m_physics )   {
    if ( ctor == nam )  {
      if ( nullptr == ctor.pointer )
        except("Failed to instaniate the physics for constructor '%s'", nam.c_str());
      return ctor;
    }
  }
  except("Failed to access the physics for constructor '%s' [Unknown physics]", nam.c_str());
  throw std::runtime_error("Failed to access the physics process"); // never called anyway
}

/// Add PhysicsConstructor by name
void Geant4PhysicsList::adoptPhysicsConstructor(Geant4Action* action)  {
  if ( 0 != action )   {
    if ( G4VPhysicsConstructor* p = dynamic_cast<G4VPhysicsConstructor*>(action) )  {
      PhysicsConstructor ctor(action->name());
      ctor.pointer = p;
      action->addRef();
      m_physics.emplace_back(ctor);
      return;
    }
    except("Failed to adopt action object %s as physics constructor. [Invalid-Base]",action->c_name());
  }
  except("Failed to adopt invalid Geant4Action as PhysicsConstructor. [Invalid-object]");
}

/// Callback to construct particle decays
void Geant4PhysicsList::constructPhysics(G4VModularPhysicsList* physics_pointer)  {
  debug("constructPhysics %p", physics_pointer);
  for ( auto& ctor : m_physics )   {
    if ( 0 == ctor.pointer )   {
      if ( G4VPhysicsConstructor* p = PluginService::Create<G4VPhysicsConstructor*>(ctor) )
        ctor.pointer = p;
      else
        except("Failed to create the physics for G4VPhysicsConstructor '%s'", ctor.c_str());
    }
    physics_pointer->RegisterPhysics(ctor.pointer);
    info("Registered Geant4 physics constructor %s to physics list", ctor.c_str());
  }
}

/// constructParticle callback
void Geant4PhysicsList::constructParticles(G4VUserPhysicsList* physics_pointer)   {
  debug("constructParticles %p", physics_pointer);
  /// Now define all particles
  for ( const auto& ctor : m_particles )   {
    G4ParticleDefinition* def = PluginService::Create<G4ParticleDefinition*>(ctor);
    if ( !def )  {
      /// Check if we have here a particle group constructor
      long* result = (long*) PluginService::Create<long>(ctor);
      if ( !result || *result != 1L )   {
        except("Failed to create particle type '%s' result=%d", ctor.c_str(), result);
      }
      info("Constructed Geant4 particle %s [using signature long (*)()]",ctor.c_str());
    }
  }
  /// Now define all particles
  for ( const auto& ctor : m_particlegroups )  {
    /// Check if we have here a particle group constructor
    long* result = (long*) PluginService::Create<long>(ctor);
    if ( !result || *result != 1L )  {
      except("Failed to create particle type '%s' result=%d", ctor.c_str(), result);
    }
    info("Constructed Geant4 particle group %s [using signature long (*)()]",ctor.c_str());
  }
}

/// Callback to construct processes (uses the G4 particle table)
void Geant4PhysicsList::constructProcesses(G4VUserPhysicsList* physics_pointer)   {
  debug("constructProcesses %p", physics_pointer);
  for ( const auto& [part_name, procs] : m_discreteProcesses )  {
    std::vector<G4ParticleDefinition*> defs(Geant4ParticleHandle::g4DefinitionsRegEx(part_name));
    if ( defs.empty() )  {
      except("Particle:%s Cannot find the corresponding entry in the particle table.", part_name.c_str());
    }
    for ( const Process& p : procs )  {
      if ( G4VProcess* g4 = PluginService::Create<G4VProcess*>(p.name) )   {
        for ( G4ParticleDefinition* particle : defs )   {
          G4ProcessManager* mgr = particle->GetProcessManager();
          mgr->AddDiscreteProcess(g4);
          info("Particle:%s -> [%s] added discrete process %s", 
               part_name.c_str(), particle->GetParticleName().c_str(), p.name.c_str());
        }
        continue;
      }
      except("Cannot create discrete physics process %s", p.name.c_str());
    }
  }
  for ( const auto& [part_name, procs] : m_processes )   {
    std::vector<G4ParticleDefinition*> defs(Geant4ParticleHandle::g4DefinitionsRegEx(part_name));
    if (defs.empty())  {
      except("Particle:%s Cannot find the corresponding entry in the particle table.", part_name.c_str());
    }
    for ( const Process& p : procs )  {
      if ( G4VProcess* g4 = PluginService::Create<G4VProcess*>(p.name) )   {
        for ( G4ParticleDefinition* particle : defs )   {
          G4ProcessManager* mgr = particle->GetProcessManager();
          mgr->AddProcess(g4, p.ordAtRestDoIt, p.ordAlongSteptDoIt, p.ordPostStepDoIt);
          info("Particle:%s -> [%s] added process %s with flags (%d,%d,%d)", 
               part_name.c_str(), particle->GetParticleName().c_str(), p.name.c_str(),
               p.ordAtRestDoIt, p.ordAlongSteptDoIt, p.ordPostStepDoIt);
        }
        continue;
      }
      except("Cannot create physics process %s", p.name.c_str());
    }
  }
}

/// Enable physics list: actions necessary to be propagated to Geant4.
void Geant4PhysicsList::enable(G4VUserPhysicsList* /* physics */)  {
}

/// Standard constructor
Geant4PhysicsListActionSequence::Geant4PhysicsListActionSequence(Geant4Context* ctxt, const std::string& nam)
  : Geant4Action(ctxt, nam), m_rangecut(0.7*CLHEP::mm)
{
  declareProperty("transportation", m_transportation);
  declareProperty("extends",  m_extends);
  declareProperty("decays",   m_decays);
  declareProperty("rangecut", m_rangecut);
  declareProperty("verbosity", m_verbosity);
  m_needsControl = true;
  InstanceCount::increment(this);
}

/// Default destructor
Geant4PhysicsListActionSequence::~Geant4PhysicsListActionSequence()  {
  m_actors(&Geant4Action::release);
  m_actors.clear();
  m_process.clear();
  InstanceCount::decrement(this);
}

#include <G4FastSimulationPhysics.hh>

/// Extend physics list from factory:
G4VUserPhysicsList* Geant4PhysicsListActionSequence::extensionList()    {
  G4VModularPhysicsList* physics = ( m_extends.empty() )
    ? new EmptyPhysics()
    : G4PhysListFactory().GetReferencePhysList(m_extends);

#if 0
  G4FastSimulationPhysics* fastSimulationPhysics = new G4FastSimulationPhysics();
  // -- We now configure the fastSimulationPhysics object.
  // -- The gflash model (GFlashShowerModel, see ExGflashDetectorConstruction.cc)
  // -- is applicable to e+ and e- : we augment the physics list for these
  // -- particles (by adding a G4FastSimulationManagerProcess with below's
  // -- calls), this will make the fast simulation to be activated:
  fastSimulationPhysics->ActivateFastSimulation("e-");
  fastSimulationPhysics->ActivateFastSimulation("e+");
  // -- Register this fastSimulationPhysics to the physicsList,
  // -- when the physics list will be called by the run manager
  // -- (will happen at initialization of the run manager)
  // -- for physics process construction, the fast simulation
  // -- configuration will be applied as well.
  physics->RegisterPhysics( fastSimulationPhysics );
#endif
  // Register all physics constructors with the physics list
  constructPhysics(physics);
  // Ensure the particles and processes declared are also invoked.
  // Hence: Create a special physics constructor doing so.
  // Ownership is transferred to the physics list.
  // Do not delete this pointer afterwards....
  physics->RegisterPhysics(new ParticlePhysics(this,physics));

  //Setting verbosity for pieces of the physics
  physics->SetVerboseLevel(m_verbosity);
  G4EmParameters::Instance()->SetVerbose(m_verbosity);
  G4HadronicParameters::Instance()->SetVerboseLevel(m_verbosity);

  return physics;
}

G4VUserPhysicsList* Geant4PhysicsListActionSequence::activateCeleritas()    {
  G4VModularPhysicsList* physics = ( m_extends.empty() )
    ? new EmptyPhysics()
    : G4PhysListFactory().GetReferencePhysList(m_extends);

physics->ReplacePhysics(new EMPhysicsConstructor);

return physics;
}


/// Install command control messenger if wanted
void Geant4PhysicsListActionSequence::installCommandMessenger()   {
  control()->addCall("dump", "Dump content of " + name(), Callback(this).make(&Geant4PhysicsListActionSequence::dump));
}

/// Dump content to stdout
void Geant4PhysicsListActionSequence::dump()    {
  printout(ALWAYS,name(),"+++ Dump of physics list component(s)");
  printout(ALWAYS,name(),"+++ Extension name       %s",m_extends.c_str());
  printout(ALWAYS,name(),"+++ Transportation flag: %d",m_transportation);
  printout(ALWAYS,name(),"+++ Program decays:      %d",m_decays);
  printout(ALWAYS,name(),"+++ RangeCut:            %f",m_rangecut);
  printout(ALWAYS,name(),"+++ Verbosity:           %i",m_verbosity);
  m_actors(&Geant4PhysicsList::dump);
}

/// Add an actor responding to all callbacks. Sequence takes ownership.
void Geant4PhysicsListActionSequence::adopt(Geant4PhysicsList* action)  {
  if (action)  {
    action->addRef();
    m_actors.add(action);
    return;
  }
  except("Geant4EventActionSequence: Attempt to add invalid actor!");
}

/// Callback to construct particles
void Geant4PhysicsListActionSequence::constructParticles(G4VUserPhysicsList* physics_pointer)  {
  m_particle(physics_pointer);
  m_actors(&Geant4PhysicsList::constructParticles, physics_pointer);
}

/// Callback to execute physics constructors
void Geant4PhysicsListActionSequence::constructPhysics(G4VModularPhysicsList* physics_pointer)  {
  m_physics(physics_pointer);
  m_actors(&Geant4PhysicsList::constructPhysics, physics_pointer);
}

/// constructProcess callback
void Geant4PhysicsListActionSequence::constructProcesses(G4VUserPhysicsList* physics_pointer)  {
  m_actors(&Geant4PhysicsList::constructProcesses, physics_pointer);
  m_process(physics_pointer);
  if (m_decays)  {
    constructDecays(physics_pointer);
  }
}

/// Callback to construct particle decays
void Geant4PhysicsListActionSequence::constructDecays(G4VUserPhysicsList* physics_pointer)  {
  G4ParticleTable* pt = G4ParticleTable::GetParticleTable();
  G4ParticleTable::G4PTblDicIterator* iter = pt->GetIterator();
  // Add Decay Process
  G4Decay* decay = new G4Decay();
  info("ConstructDecays %p",physics_pointer);
  iter->reset();
  while ((*iter)())  {
    G4ParticleDefinition* p = iter->value();
    G4ProcessManager* mgr = p->GetProcessManager();
    if (decay->IsApplicable(*p))  {
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

