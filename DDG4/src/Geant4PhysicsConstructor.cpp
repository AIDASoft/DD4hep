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
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4PhysicsConstructor.h"
#include "G4VModularPhysicsList.hh"
#include "G4Version.hh"

using namespace DD4hep::Simulation;

namespace  {

  /// Concrete implementation
  class ConstructorImp : virtual public Geant4PhysicsConstructor::Constructor   {
  public:
    Geant4PhysicsConstructor* ctor;
    /// Initializing constructor dealing with the callbacks
    ConstructorImp(Geant4PhysicsConstructor* ct, int typ)
      : G4VPhysicsConstructor(ct->name(),typ), Geant4PhysicsConstructor::Constructor(), ctor(ct)
    {}
    /// Default destructor
    virtual ~ConstructorImp() {}
    /// Access to a fresh (resetted) instance of the particle table iterator
    G4ParticleTable::G4PTblDicIterator* particleIterator()  const   {
      G4ParticleTable::G4PTblDicIterator* iter;

#if G4VERSION_NUMBER >= 1030
      iter = GetParticleIterator();
#elif G4VERSION_NUMBER >= 1000
      iter = aParticleIterator;
#else  
      iter = theParticleIterator;
#endif 

      iter->reset();
      return iter;
    }
    /// G4VPhysicsConstructor overload: This method will be invoked in the Construct() method. 
    virtual void ConstructParticle()   {
      ctor->constructParticle(*this);
    }
    /// G4VPhysicsConstructor overload: This method will be invoked in the Construct() method.
    virtual void ConstructProcess()   {
      ctor->constructProcess(*this);
    }
  };
}



/// Standard action constructor
Geant4PhysicsConstructor::Constructor::Constructor()  
  : G4VPhysicsConstructor() 
{
}

/// Default destructor
Geant4PhysicsConstructor::Constructor::~Constructor()  {
}

/// Standard action constructor
Geant4PhysicsConstructor::Geant4PhysicsConstructor(Geant4Context* ctxt, const std::string& nam)
  : Geant4PhysicsList(ctxt, nam)
{
  InstanceCount::increment(this);
  declareProperty("PhysicsType", m_type = 0);
}

/// Default destructor
Geant4PhysicsConstructor::~Geant4PhysicsConstructor() {
  InstanceCount::decrement(this);
  // Do NOT delete the constructor !
}

/// constructPhysics callback
void Geant4PhysicsConstructor::constructPhysics(G4VModularPhysicsList* physics_list)   {
  // Register try constructor providing the callbacks
  physics_list->RegisterPhysics(new ConstructorImp(this, m_type));
}

/// Callback to construct processes (uses the G4 particle table)
void Geant4PhysicsConstructor::constructProcess(Constructor& /* ctor */)   {
}

/// Callback to construct particles
void Geant4PhysicsConstructor::constructParticle(Constructor& /* ctor */)   {
}
