// $Id:$
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

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation   {

    // Forward declarations;
    typedef Geometry::Position             Position;
    typedef Geometry::Direction            Direction;
    typedef Geometry::Momentum             Momentum;
    typedef Geometry::LCDD                 LCDD;
    typedef Geometry::Readout              Readout;
    typedef Geometry::DetElement           DetElement;
    typedef Geometry::SensitiveDetector    SensitiveDetector;

    template<class HIT> struct HitCompare;
    template<class HIT> struct HitPositionCompare;
    class Geant4StepHandler;
    class Geant4Hit;
    class Geant4TrackerHit;
    class Geant4CalorimeterHit;

    class Geant4SensitiveDetector;
    template <class T> class Geant4GenericSD;



  }    // End namespace Simulation
}      // End namespace DD4hep
#endif // DDG4_DEFS_H
