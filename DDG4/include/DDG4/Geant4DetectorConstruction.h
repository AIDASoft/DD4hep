//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_GEANT4DETECTORCONSTRUCTION_H
#define DD4HEP_GEANT4DETECTORCONSTRUCTION_H

#include "DD4hep/Printout.h"
#include "G4VUserDetectorConstruction.hh"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  // Forward declarations
  namespace Geometry {
    class LCDD;
  }

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {
    class Geant4Kernel;

    class Geant4DetectorConstruction : public G4VUserDetectorConstruction {
    public:
      /// Initializing constructor for DDG4
      Geant4DetectorConstruction(Geant4Kernel& kernel);
      /// Initializing constructor for other clients
      Geant4DetectorConstruction(Geometry::LCDD& lcdd);
      /// Default destructor
      virtual ~Geant4DetectorConstruction();
      /// Geometry construction callback
      G4VPhysicalVolume* Construct();

    private:
      /// Printlevel used for the geometry conversion
      PrintLevel m_outputLevel;
      /// Reference to geometry object
      Geometry::LCDD& m_lcdd;
      /// Reference to the world after construction
      G4VPhysicalVolume* m_world;
    };
  }
}

#endif
