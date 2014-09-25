// $Id$
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DDG4_DEFS_H
#define DDG4_DEFS_H

// Framework include files
#include "DD4hep/LCDD.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    // Forward declarations;
    typedef Geometry::Position Position;
    typedef Geometry::Position Direction;
    typedef Geometry::Position Momentum;
    typedef Geometry::LCDD LCDD;
    typedef Geometry::Readout Readout;
    typedef Geometry::DetElement DetElement;

    class Geant4StepHandler;
    class Geant4Hit;
    class Geant4TrackerHit;
    class Geant4CalorimeterHit;
    class Geant4SensitiveDetector;
    template <class T> class Geant4GenericSD;

  }    // End namespace Simulation
}      // End namespace DD4hep
#endif // DDG4_DEFS_H
