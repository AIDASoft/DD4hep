//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_GEANT4DETECTORCONSTRUCTION_H
#define DD4HEP_GEANT4DETECTORCONSTRUCTION_H

#include "G4VUserDetectorConstruction.hh"

namespace DD4hep {

  namespace Geometry { class LCDD; }
  namespace Simulation {
    
    class Geant4DetectorConstruction : public G4VUserDetectorConstruction  {
    public:
      
      Geant4DetectorConstruction(Geometry::LCDD& lcdd);
      virtual ~Geant4DetectorConstruction() {}
      G4VPhysicalVolume* Construct();
    private:
      Geometry::LCDD&    m_lcdd;
      G4VPhysicalVolume* m_world;
    };
  }
}

#endif
