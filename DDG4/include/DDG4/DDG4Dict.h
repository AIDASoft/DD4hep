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
//
//  Define the ROOT dictionaries for all data classes to be saved
//  which are created by the DDG4 examples.
//
//====================================================================
#ifndef DD4HEP_DDG4_DDG4DICT_H
#define DD4HEP_DDG4_DDG4DICT_H

// FRamework include files
#include "DDG4/Geant4Data.h"
#include "DDG4/Geant4Particle.h"
namespace DD4hep {
  namespace DDSegmentation { }
  namespace Simulation { }
  namespace Geometry   { }
}

// CINT configuration
#if defined(__MAKECINT__)
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

/// Define namespaces
#pragma link C++ namespace DD4hep;
#pragma link C++ namespace DD4hep::Geometry;
#pragma link C++ namespace DD4hep::Simulation;

/// Simple stuff
#pragma link C++ class DD4hep::Simulation::SimpleRun+;
#pragma link C++ class DD4hep::Simulation::SimpleEvent+;
#pragma link C++ class DD4hep::Simulation::DataExtension+;

#pragma link C++ class DD4hep::dd4hep_ptr<DD4hep::Simulation::DataExtension>+;
#pragma link C++ class DD4hep::dd4hep_ptr<DD4hep::Simulation::DataExtension>::base_t+;

/// Dictionaires for Geant4 particles
#pragma link C++ class DD4hep::Simulation::ParticleExtension+;
#pragma link C++ class DD4hep::dd4hep_ptr<DD4hep::Simulation::ParticleExtension>+;
#pragma link C++ class DD4hep::dd4hep_ptr<DD4hep::Simulation::ParticleExtension>::base_t+;
#pragma link C++ class DD4hep::Simulation::Geant4Particle+;
#pragma link C++ class std::vector<DD4hep::Simulation::Geant4Particle*>+;
#pragma link C++ class std::map<int,DD4hep::Simulation::Geant4Particle*>+;
#pragma link C++ class map<int,DD4hep::Simulation::Geant4Particle*>::iterator;
#pragma link C++ class map<int,DD4hep::Simulation::Geant4Particle*>::const_iterator;

#ifdef R__MACOSX
// We only need these declarations for the clang compiler
#pragma link C++ function operator==( const map<int,DD4hep::Simulation::Geant4Particle*>::iterator&, const map<int,DD4hep::Simulation::Geant4Particle*>::iterator& );
#pragma link C++ function operator!=( const map<int,DD4hep::Simulation::Geant4Particle*>::iterator&, const map<int,DD4hep::Simulation::Geant4Particle*>::iterator& );
#endif

/// Dictionaires for basic Hit data structures
#pragma link C++ class DD4hep::Simulation::Geant4HitData+;
#pragma link C++ class std::vector<DD4hep::Simulation::Geant4HitData*>+;
#pragma link C++ class DD4hep::Simulation::Geant4HitData::Contribution+;
#pragma link C++ class DD4hep::Simulation::Geant4HitData::Contributions+;

/// Dictionaires for Tracker Hit data structures
#pragma link C++ class DD4hep::Simulation::Geant4Tracker+;
#pragma link C++ class DD4hep::Simulation::Geant4Tracker::Hit+;
#pragma link C++ class std::vector<DD4hep::Simulation::Geant4Tracker::Hit*>+;

/// Dictionaires for Calorimeter Hit data structures
#pragma link C++ class DD4hep::Simulation::Geant4Calorimeter+;
#pragma link C++ class DD4hep::Simulation::Geant4Calorimeter::Hit+;
#pragma link C++ class std::vector<DD4hep::Simulation::Geant4Calorimeter::Hit*>+;

#endif

#ifdef __DDG4_STANDALONE_DICTIONARIES__
/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {
#define NO_CALL {      throw "This function shoule never ever be called!";    }
    /// Default constructor
    inline SimpleRun::SimpleRun() {    }
    /// Default destructor
    inline SimpleRun::~SimpleRun() {    }
    /// Default constructor
    inline SimpleEvent::SimpleEvent() {    }
    /// Default destructor
    inline SimpleEvent::~SimpleEvent() {    }

    /// Default destructor
    inline DataExtension::~DataExtension()  {    }
    /// Default destructor
    inline ParticleExtension::~ParticleExtension()  {    }
    /// Default constructor
    inline Geant4Particle::Geant4Particle()   {     }
    /// Copy constructor
    inline Geant4Particle::Geant4Particle(const Geant4Particle&)   {  NO_CALL   }
    /// Default destructor
    inline Geant4Particle::~Geant4Particle()   {     }
    /// Remove daughter from set
    inline void Geant4Particle::removeDaughter(int)   {   NO_CALL  }
    /// Default constructor
    inline Geant4HitData::Geant4HitData()   {    }
    /// Default destructor
    inline  Geant4HitData::~Geant4HitData()  {    }
    /// Extract the MC contribution for a given hit from the step information
    inline Geant4HitData::Contribution Geant4HitData::extractContribution(const G4Step*) { return Contribution(); }
    /// Default constructor
    inline Geant4Tracker::Hit::Hit()   {    }
    /// Initializing constructor
    inline Geant4Tracker::Hit::Hit(int, int, double, double)   {}
    /// Default destructor
    inline Geant4Tracker::Hit::~Hit()  {    }
    /// Assignment operator
    inline Geant4Tracker::Hit& Geant4Tracker::Hit::operator=(const Hit&)   { return *this; }
    /// Clear hit content
    inline Geant4Tracker::Hit& Geant4Tracker::Hit::clear()    { return *this; }
    /// Store Geant4 point and step information into tracker hit structure.
    inline Geant4Tracker::Hit& Geant4Tracker::Hit::storePoint(const G4Step*, const G4StepPoint*)  { return *this;}
    /// Default constructor
    inline Geant4Calorimeter::Hit::Hit()   {    }
    /// Initializing constructor
    inline Geant4Calorimeter::Hit::Hit(const Position&)  {}
    /// Default destructor
    inline Geant4Calorimeter::Hit::~Hit()   {    }
  }
}
#undef NO_CALL

#endif // __DDG4_STANDALONE_DICTIONARIES__

#endif /* DD4HEP_DDG4_DDG4DICT_H */
