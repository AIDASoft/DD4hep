//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_Geant4GDMLDetector_H
#define DD4HEP_Geant4GDMLDetector_H

#include "G4VUserDetectorConstruction.hh"
#include <string>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {
  
  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {
    
    class Geant4GDMLDetector : public G4VUserDetectorConstruction  {
    public:
      
      Geant4GDMLDetector(const std::string& gdmlFile );
      virtual ~Geant4GDMLDetector() {}
      G4VPhysicalVolume* Construct();
    private:
      std::string m_fileName ; 
      G4VPhysicalVolume* m_world;
    };
  }
}

#endif
