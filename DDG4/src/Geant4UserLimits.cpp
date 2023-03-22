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
#include "DDG4/Geant4UserLimits.h"
#include "DDG4/Geant4Particle.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/DD4hepUnits.h"
#include "DD4hep/Primitives.h"
#include "DD4hep/Printout.h"

// Geant 4 include files
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "CLHEP/Units/SystemOfUnits.h"

// C/C++ include files
#include <stdexcept>

using namespace std;
using namespace dd4hep::sim;

namespace  {
  bool user_limit_debug = false;
}

      /// Allow for debugging user limits (very verbose)
bool Geant4UserLimits::enable_debug(bool value)   {
  bool tmp = user_limit_debug;
  user_limit_debug = value;
  return tmp;
}

/// Access value according to track
double Geant4UserLimits::Handler::value(const G4Track& track) const    {
  auto* def = track.GetParticleDefinition();
  if ( !particleLimits.empty() )  {
    auto i = particleLimits.find(track.GetDefinition());
    if ( i != particleLimits.end() )  {
      if ( user_limit_debug )   {
	dd4hep::printout(dd4hep::INFO,"Geant4UserLimits", "Apply explicit limit %f to track: %s",
			 def->GetParticleName().c_str());
      }
      return (*i).second;
    }
  }
  if ( user_limit_debug )   {
    dd4hep::printout(dd4hep::INFO,"Geant4UserLimits", "Apply default limit %f to track: %s",
		     def->GetParticleName().c_str());
  }
  return defaultValue;
}

/// Set the handler value(s)
void Geant4UserLimits::Handler::set(const string& particles, double val)   {
  if ( particles == "*" || particles == ".(.*)" )   {
    defaultValue = val;
    return;
  }
  auto defs = Geant4ParticleHandle::g4DefinitionsRegEx(particles);
  for( auto* d : defs )  {
    particleLimits[d] = val;
  }
}

/// Initializing Constructor
Geant4UserLimits::Geant4UserLimits(LimitSet limitset)
  : G4UserLimits(limitset.name()), limits(limitset)
{
  this->update(limitset);
  InstanceCount::increment(this);
}

/// Standard destructor
Geant4UserLimits::~Geant4UserLimits()  {
  InstanceCount::decrement(this);
}

/// Update the object
void Geant4UserLimits::update(LimitSet limitset)    {
  const auto& lim = limitset.limits();
  /// Set defaults
  maxStepLength.defaultValue  = fMaxStep;
  maxTrackLength.defaultValue = fMaxTrack;
  maxTime.defaultValue        = fMaxTime;
  minEKine.defaultValue       = fMinEkine;
  minRange.defaultValue       = fMinRange;
  /// Overwrite with values if present:
  for(const Limit& l : lim)   {
    if (l.name == "step_length_max")
      maxStepLength.set(l.particles, l.value*CLHEP::mm/dd4hep::mm);
    else if (l.name == "track_length_max")
      maxTrackLength.set(l.particles, l.value*CLHEP::mm/dd4hep::mm);
    else if (l.name == "time_max")
      maxTime.set(l.particles, l.value*CLHEP::ns/dd4hep::ns);
    else if (l.name == "ekin_min")
      minEKine.set(l.particles, l.value*CLHEP::MeV/dd4hep::MeV);
    else if (l.name == "range_min")
      minRange.set(l.particles, l.value);
    else
      throw runtime_error("Unknown Geant4 user limit: " + l.toString());
  }
}

/// Setters may not be called!
void Geant4UserLimits::SetMaxAllowedStep(G4double /* ustepMax */)  {
  dd4hep::notImplemented(string(__PRETTY_FUNCTION__)+" May not be called!");
}

void Geant4UserLimits::SetUserMaxTrackLength(G4double /* utrakMax */)  {
  dd4hep::notImplemented(string(__PRETTY_FUNCTION__)+" May not be called!");
}

void Geant4UserLimits::SetUserMaxTime(G4double /* utimeMax */)  {
  dd4hep::notImplemented(string(__PRETTY_FUNCTION__)+" May not be called!");
}

void Geant4UserLimits::SetUserMinEkine(G4double /* uekinMin */)  {
  dd4hep::notImplemented(string(__PRETTY_FUNCTION__)+" May not be called!");
}

void Geant4UserLimits::SetUserMinRange(G4double /* urangMin */)  {
  dd4hep::notImplemented(string(__PRETTY_FUNCTION__)+" May not be called!");
}

