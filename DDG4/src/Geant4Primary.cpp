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
#include "DD4hep/Primitives.h"
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4Primary.h"

// C/C++ include files
#include <stdexcept>
#include <cstdio>

using namespace DD4hep;
using namespace DD4hep::Simulation;

/// Default destructor
PrimaryExtension::~PrimaryExtension() {
}

/// Default destructor
Geant4PrimaryMap::~Geant4PrimaryMap()   {
  releaseObjects(primaryMap)();
}

/// Default constructor
Geant4PrimaryInteraction::Geant4PrimaryInteraction()
  : mask(0), next_particle_identifier(-1)
{
}

/// Copy constructor
Geant4PrimaryInteraction::Geant4PrimaryInteraction(const Geant4PrimaryInteraction&)
  : mask(0), next_particle_identifier(-1)
{
}

/// Assignment operator
Geant4PrimaryInteraction& Geant4PrimaryInteraction::operator=(const Geant4PrimaryInteraction& c)  {
  if ( &c == this ) {}
  return *this;
}

/// Default destructor
Geant4PrimaryInteraction::~Geant4PrimaryInteraction()   {
  releaseObjects(vertices)();
  releaseObjects(particles)();
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
  if ( vertices.size() <= 1 )  {
    Geant4PrimaryInteraction::ParticleMap::iterator ip, ipend;
    for( ip=particles.begin(), ipend=particles.end(); ip != ipend; ++ip )
      (*ip).second->mask = mask;

    Geant4PrimaryInteraction::VertexMap::iterator iv, ivend;
    for( iv=vertices.begin(), ivend=vertices.end(); iv != ivend; ++iv )
      (*iv).second->mask = mask;
    return true;
  }
  return false;
}

/// Default constructor
Geant4PrimaryEvent::Geant4PrimaryEvent()
{
}

/// Copy constructor
Geant4PrimaryEvent::Geant4PrimaryEvent(const Geant4PrimaryEvent&)
{
}

/// Assignment operator
Geant4PrimaryEvent& Geant4PrimaryEvent::operator=(const Geant4PrimaryEvent& c)  {
  if ( &c == this ) {}
  return *this;
}

/// Default destructor
Geant4PrimaryEvent::~Geant4PrimaryEvent()   {
  destroyObjects(m_interactions)();
}

/// Add a new interaction object to the event
void Geant4PrimaryEvent::add(int id, Geant4PrimaryInteraction* interaction)   {
  if ( interaction )  {
    Interactions::iterator i = m_interactions.find(id);
    if ( i == m_interactions.end() )  {
      interaction->mask = id;
      m_interactions.insert(std::make_pair(id,interaction));
      return;
    }
    char text[132];
    ::snprintf(text,sizeof(text),"Geant4PrimaryEvent: Interaction with ID '%d' "
               "exists and cannot be added twice!",id);
    throw std::runtime_error(text);
  }
  throw std::runtime_error("Geant4PrimaryEvent: CANNOT add invalid Interaction!");
}

/// Retrieve an interaction by it's ID
Geant4PrimaryEvent::Interaction* Geant4PrimaryEvent::get(int mask) const   {
  Interactions::const_iterator i = m_interactions.find(mask);
  if ( i != m_interactions.end() )  {
    return (*i).second;
  }
  return 0;
}

/// Retrieve all intractions
std::vector<Geant4PrimaryEvent::Interaction*> Geant4PrimaryEvent::interactions() const   {
  std::vector<Interaction*> v;
  for(Interactions::const_iterator i=m_interactions.begin(); i!=m_interactions.end(); ++i)
    v.push_back((*i).second);
  return v;
}

