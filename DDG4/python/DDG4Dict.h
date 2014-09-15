// $Id: Geant4Data.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Define the ROOT dictionaries for all data classes to be saved 
//  which are created by the DDG4 examples.
//
//  Author     : M.Frank
//
//====================================================================
// FRamework include files
#include "DDG4/Geant4Data.h"
#include "DDG4/Geant4Particle.h"

// CINT configuration
#if defined(__MAKECINT__)
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ namespace DD4hep;
#pragma link C++ namespace DD4hep::Geometry;
#pragma link C++ namespace DD4hep::Simulation;

#pragma link C++ class DD4hep::Simulation::SimpleRun+;
#pragma link C++ class DD4hep::Simulation::SimpleEvent+;
//#pragma link C++ class DD4hep::SimpleEvent::Seeds+;
#pragma link C++ class DD4hep::Simulation::DataExtension+;
#pragma link C++ class DD4hep::Simulation::ParticleExtension+;

#pragma link C++ class std::auto_ptr<DD4hep::Simulation::DataExtension>+;
#pragma link C++ class std::auto_ptr<DD4hep::Simulation::ParticleExtension>+;

#pragma link C++ class DD4hep::Simulation::Geant4HitData+;
#pragma link C++ class DD4hep::Simulation::Geant4Particle+;
//#pragma link C++ class DD4hep::Simulation::Geant4ParticleHandle; // No streamer here!
#pragma link C++ class std::vector<DD4hep::Simulation::Geant4HitData*>+;
#pragma link C++ class DD4hep::Simulation::Geant4HitData::Contribution+;
#pragma link C++ class DD4hep::Simulation::Geant4HitData::Contributions+;
#pragma link C++ class DD4hep::Simulation::Geant4Tracker+;
#pragma link C++ class DD4hep::Simulation::Geant4Tracker::Hit+;
#pragma link C++ class std::vector<DD4hep::Simulation::Geant4Tracker::Hit*>+;
#pragma link C++ class DD4hep::Simulation::Geant4Calorimeter+;
#pragma link C++ class DD4hep::Simulation::Geant4Calorimeter::Hit+;
#pragma link C++ class std::vector<DD4hep::Simulation::Geant4Calorimeter::Hit*>+;
#pragma link C++ class std::vector<DD4hep::Simulation::Geant4Vertex*>+;
#pragma link C++ class std::vector<DD4hep::Simulation::Geant4Particle*>+;
//#pragma link C++ class std::set<int>+;  // Note: Link requested for already precompiled class 
#pragma link C++ class std::map<int,DD4hep::Simulation::Geant4Particle*>+;

//#pragma link C++ class ;
#endif
