// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4ISOTROPEGENERATOR_H
#define DD4HEP_DDG4_GEANT4ISOTROPEGENERATOR_H

// Framework include files
#include "DDG4/Geant4GeneratorAction.h"

// Forward declarations
class G4ParticleDefinition;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {
    /// Generate particles isotrop in space around origine (0,0,0)
    /**
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4IsotropeGenerator: public Geant4GeneratorAction {
    protected:
      /// Pointer to geant4 particle definition
      G4ParticleDefinition* m_particle;
      /// Property: Particle energy
      double m_energy;
      /// Property: Particle name
      std::string m_particleName;
      /// Property: Desired multiplicity of the particles to be shot
      int m_multiplicity;
      /// Property: User mask passed to all particles in the generated interaction
      int m_mask;
    public:
      /// Standard constructor
      Geant4IsotropeGenerator(Geant4Context* context, const std::string& name);
      /// Default destructor
      virtual ~Geant4IsotropeGenerator();
      /// Callback to generate primary particles
      virtual void operator()(G4Event* event);
    };
  }    // End namespace Simulation
}      // End namespace DD4hep
#endif /* DD4HEP_DDG4_GEANT4ISOTROPEGENERATOR_H  */
