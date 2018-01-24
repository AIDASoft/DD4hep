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
//
//  Define the ROOT dictionaries for all data classes to be saved
//  which are created by the DDG4 examples.
//
//====================================================================
#ifndef DD4HEP_DDG4_DDG4DICT_H
#define DD4HEP_DDG4_DDG4DICT_H

#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wdeprecated"
#pragma GCC diagnostic ignored "-Wunused"
#pragma GCC diagnostic ignored "-Woverlength-strings"

#elif defined(__llvm__) || defined(__APPLE__)

#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#pragma clang diagnostic ignored "-Wdeprecated"
#pragma clang diagnostic ignored "-Wunused"
#pragma clang diagnostic ignored "-Woverlength-strings"
#endif

// FRamework include files
#include "DD4hep/Memory.h"
#include "DDG4/Geant4Data.h"
#include "DDG4/Geant4Particle.h"

namespace dd4hep {
  namespace DDSegmentation { }
  namespace detail   { }
  namespace sim { }
}

namespace { class DDG4Dict {};   }

// CINT configuration
#if defined(__CINT__) || defined(__MAKECINT__) || defined(__CLING__) || defined(__ROOTCLING__)
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

using namespace std;

/// Define namespaces
#pragma link C++ namespace dd4hep;
#pragma link C++ namespace dd4hep::sim;
#pragma link C++ namespace dd4hep::detail;

/// Simple stuff
#pragma link C++ class dd4hep::sim::SimpleRun+;
#pragma link C++ class dd4hep::sim::SimpleEvent+;
#pragma link C++ class dd4hep::sim::DataExtension+;

/// Dictionaires for Geant4 particles
#pragma link C++ class dd4hep::sim::ParticleExtension+;

/// Auto-pointers related. ROOT cannot handle I/O!
#pragma link C++ class dd4hep::dd4hep_ptr<dd4hep::sim::DataExtension>;
#pragma link C++ class dd4hep::dd4hep_ptr<dd4hep::sim::ParticleExtension>;

#ifdef DD4HEP_DD4HEP_PTR_AUTO
#pragma link C++ class dd4hep::dd4hep_ptr<dd4hep::sim::DataExtension>::base_t;
#pragma link C++ class dd4hep::dd4hep_ptr<dd4hep::sim::ParticleExtension>::base_t;
#endif

#pragma link C++ class dd4hep::sim::Geant4Particle+;
#pragma link C++ class vector<dd4hep::sim::Geant4Particle*>+;
#pragma link C++ class map<int,dd4hep::sim::Geant4Particle*>+;
#pragma link C++ class map<int,dd4hep::sim::Geant4Particle*>::iterator;
#pragma link C++ class map<int,dd4hep::sim::Geant4Particle*>::const_iterator;

#ifdef R__MACOSX
// We only need these declarations for the clang compiler
#pragma link C++ function operator==( const map<int,dd4hep::sim::Geant4Particle*>::iterator&, const map<int,dd4hep::sim::Geant4Particle*>::iterator& );
#pragma link C++ function operator!=( const map<int,dd4hep::sim::Geant4Particle*>::iterator&, const map<int,dd4hep::sim::Geant4Particle*>::iterator& );
#endif

//#pragma link C++ class type_info;

/// Dictionaires for basic Hit data structures
#pragma link C++ class dd4hep::sim::Geant4HitData+;
#pragma link C++ class vector<dd4hep::sim::Geant4HitData*>+;
#pragma link C++ class dd4hep::sim::Geant4HitData::Contribution+;
#pragma link C++ class dd4hep::sim::Geant4HitData::Contributions+;

/// Dictionaires for Tracker Hit data structures
#pragma link C++ class dd4hep::sim::Geant4Tracker+;
#pragma link C++ class dd4hep::sim::Geant4Tracker::Hit+;
#pragma link C++ class vector<dd4hep::sim::Geant4Tracker::Hit*>+;

/// Dictionaires for Calorimeter Hit data structures
#pragma link C++ class dd4hep::sim::Geant4Calorimeter+;
#pragma link C++ class dd4hep::sim::Geant4Calorimeter::Hit+;
#pragma link C++ class vector<dd4hep::sim::Geant4Calorimeter::Hit*>+;

#endif

#ifdef __DDG4_STANDALONE_DICTIONARIES__
/*
 *   dd4hep namespace declaration
 */
namespace dd4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace sim {
#define NO_CALL {      throw "This function shoule never ever be called!";    }
    /// Default constructor
    inline SimpleRun::SimpleRun() : runID(0), numEvents(0)  {    }
    /// Default destructor
    inline SimpleRun::~SimpleRun() {    }
    /// Default constructor
    inline SimpleEvent::SimpleEvent() : runID(0), eventID(0) {    }
    /// Default destructor
    inline SimpleEvent::~SimpleEvent() {    }

    /// Default destructor
    inline DataExtension::~DataExtension()  {    }
    /// Default destructor
    inline ParticleExtension::~ParticleExtension()  {    }
    /// Default constructor
    inline Geant4Particle::Geant4Particle()   {     }
    /// Default destructor
    inline Geant4Particle::~Geant4Particle()   {     }
    /// Remove daughter from set
    inline void Geant4Particle::removeDaughter(int)   {   NO_CALL  }
    /// Default constructor
    inline Geant4HitData::Geant4HitData(): cellID(0), flag(0), g4ID(0)   {    }
    /// Default destructor
    inline  Geant4HitData::~Geant4HitData()  {    }
    /// Extract the MC contribution for a given hit from the step information
    inline Geant4HitData::Contribution Geant4HitData::extractContribution(const G4Step*) { return Contribution(); }
    /// Default constructor
    inline Geant4Tracker::Hit::Hit() : length(0), energyDeposit(0e0)  {    }
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
    inline Geant4Calorimeter::Hit::Hit() : energyDeposit(0e0)  {    }
    /// Initializing constructor
    inline Geant4Calorimeter::Hit::Hit(const Position&) : energyDeposit(0e0) {}
    /// Default destructor
    inline Geant4Calorimeter::Hit::~Hit()   {    }
  }
}
#undef NO_CALL

#endif // __DDG4_STANDALONE_DICTIONARIES__

#endif /* DD4HEP_DDG4_DDG4DICT_H */
