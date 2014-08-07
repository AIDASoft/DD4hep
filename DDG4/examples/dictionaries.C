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
// Framework include files
#include "DDG4/Geant4Data.h"
#include <vector>
#include <set>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;

// CINT configuration
#if defined(__MAKECINT__)
#pragma link C++ class Position+;
//#pragma link C++ class Direction+;
#pragma link C++ class SimpleRun+;
#pragma link C++ class SimpleEvent+;
//#pragma link C++ class SimpleEvent::Seeds+;
#pragma link C++ class SimpleHit+;
#pragma link C++ class std::vector<SimpleHit*>+;
#pragma link C++ class SimpleHit::Contribution+;
#pragma link C++ class SimpleHit::Contributions+;
#pragma link C++ class SimpleTracker+;
#pragma link C++ class SimpleTracker::Hit+;
#pragma link C++ class std::vector<SimpleTracker::Hit*>+;
#pragma link C++ class SimpleCalorimeter+;
#pragma link C++ class SimpleCalorimeter::Hit+;
#pragma link C++ class std::vector<SimpleCalorimeter::Hit*>+;
#pragma link C++ class Particle+;
#pragma link C++ class std::vector<Particle*>+;
#pragma link C++ class std::set<int>+;
//#pragma link C++ class ;
#endif

int dictionaries()  {
  return 0;
}
