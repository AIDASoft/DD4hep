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
#include "DD4hep/Printout.h"
#include "DD4hep/Primitives.h"
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4Primary.h"

// Geant4 include files
#include "G4PrimaryParticle.hh"

// C/C++ include files
#include <stdexcept>
#include <cstdio>

using namespace dd4hep;
using namespace dd4hep::sim;

/// Default destructor
PrimaryExtension::~PrimaryExtension() {
}

/// Default destructor
Geant4PrimaryMap::~Geant4PrimaryMap()   {
  detail::releaseObjects(m_primaryMap);
}

/// Add a new object pair (G4 primary particle, DDG4 particle) into the maps
void Geant4PrimaryMap::insert(G4PrimaryParticle* g4,Geant4Particle* p)   {
  m_primaryMap.insert(std::make_pair(g4,p->addRef()));
}

/// Access DDG4 particle by G4 primary particle
Geant4Particle* Geant4PrimaryMap::get(const G4PrimaryParticle* particle)   {
  Primaries::iterator i=m_primaryMap.find(particle);
  return i != m_primaryMap.end() ? (*i).second : 0;
}

/// Access DDG4 particle by G4 primary particle (const)
const Geant4Particle* Geant4PrimaryMap::get(const G4PrimaryParticle* particle) const   {
  Primaries::const_iterator i=m_primaryMap.find(particle);
  return i != m_primaryMap.end() ? (*i).second : 0;
}

/// Default destructor
Geant4PrimaryInteraction::~Geant4PrimaryInteraction()   {
  for(const auto& iv : vertices)  {
    for( Geant4Vertex* vtx : iv.second ) 
      detail::ReleaseObject<Geant4Vertex*>()( vtx ); 
  } 
  detail::releaseObjects(particles);
}

/// Access a new particle identifier within the interaction
int Geant4PrimaryInteraction::nextPID()   {
  return ++next_particle_identifier;
}

/// Access a new particle identifier within the interaction
void Geant4PrimaryInteraction::setNextPID(int new_value)   {
  next_particle_identifier = new_value-1;
}

/// Apply mask to all contained vertices and particles
bool Geant4PrimaryInteraction::applyMask()   {
  for(auto& ip : particles)
    ip.second->mask = mask;
  
  for(auto& iv : vertices )  {
    for(auto* vtx : iv.second )
      vtx->mask = mask;
  }
  return true;
}

/// Default destructor
Geant4PrimaryEvent::~Geant4PrimaryEvent()   {
  detail::destroyObjects(m_interactions);
}

/// Add a new interaction object to the event
void Geant4PrimaryEvent::add(int id, Geant4PrimaryInteraction* interaction)   {
  if ( interaction )  {
    Interactions::iterator i = m_interactions.find(id);
    if ( i == m_interactions.end() )  {
      interaction->mask = id;
      m_interactions.emplace(id,interaction);
      return;
    }
    except("Geant4PrimaryEvent","+++ Interaction with ID '%d' "
           "exists and cannot be added twice!",id);
  }
  except("Geant4PrimaryEvent","+++ CANNOT add invalid Interaction!");
}

/// Retrieve an interaction by it's ID
Geant4PrimaryEvent::Interaction* Geant4PrimaryEvent::get(int mask) const   {
  Interactions::const_iterator i = m_interactions.find(mask);
  return (i != m_interactions.end()) ? (*i).second : 0;
}

/// Retrieve all intractions
std::vector<Geant4PrimaryEvent::Interaction*> Geant4PrimaryEvent::interactions() const   {
  std::vector<Interaction*> v;
  v.reserve(m_interactions.size());
  for(const auto& i : m_interactions)
    v.emplace_back(i.second);
  return v;
}

