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

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  // Forward declarations
  namespace Geometry {
    class LCDD;
  }

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    // Forward declarations
    class Geant4Kernel;

    /// Class to create Geant4 detector geometry from TGeo representation in memory
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
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
