// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
// Framework include files
#include "DDG4/Geant4SensitiveDetector_inline.h"
#include "DDG4/Factories.h"

/*
 *   DD4hep::Simulation namespace declaration
 */
namespace DD4hep {  namespace Simulation {
  /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  ///               Geant4GenericSD<Tracker>
  /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  struct Tracker {};
  /// Method for generating hit(s) using the information of G4Step object.
  template <> bool Geant4GenericSD<Tracker>::buildHits(G4Step* step,G4TouchableHistory* ) {
    /// Add a new hit to the collection
    Geant4Hit* hit = Geant4Hit::createTrackerHit(step);
    if ( hit ) collection(0)->insert(hit);
    return hit != 0;
  }
  typedef Geant4GenericSD<Tracker> Geant4TrackerSD;
}}    // End namespace DD4hep::Simulation

DECLARE_GEANT4SENSITIVEDETECTOR(Geant4TrackerSD);
