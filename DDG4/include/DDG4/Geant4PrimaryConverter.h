//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4PRIMARYCONVERTER_H
#define DD4HEP_DDG4_GEANT4PRIMARYCONVERTER_H

// Framework include files
#include "DDG4/Geant4GeneratorAction.h"
#include "Math/Vector3D.h"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {

    /** @class Geant4Primaryconverter Geant4Primaryconverter.h DDG4/Geant4Primaryconverter.h
     *
     * Convert G4PrimaryVertix and G4PrimaryParticle constructs into 
     * Geant4Primary records. Required for native G4 particle guns.
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4PrimaryConverter: public Geant4GeneratorAction {
    protected:
      /// Interaction mask
      int m_mask;
    public:
      /// Standard constructor
      Geant4PrimaryConverter(Geant4Context* context, const std::string& name);
      /// Default destructor
      virtual ~Geant4PrimaryConverter();
      /// Callback to generate primary particles
      virtual void operator()(G4Event* event);
    };
  }    // End namespace Simulation
}      // End namespace DD4hep
#endif /* DD4HEP_DDG4_GEANT4PRIMARYCONVERTER_H  */
