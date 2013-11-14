// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4PARTICLEGUN_H
#define DD4HEP_DDG4_GEANT4PARTICLEGUN_H

// Framework include files
#include "DDG4/Geant4GeneratorAction.h"

// Forward declarations
class G4ParticleDefinition;
class G4ParticleGun;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {
    /** @class Geant4ParticleGun Geant4ParticleGun.h DDG4/Geant4ParticleGun.h
     *
     * Implementation wrapper of the Geant4 particle gun
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4ParticleGun: public Geant4GeneratorAction {
    protected:
      /// Position and shooting direction of the gun
      struct {
        double x, y, z;
      } m_position, m_direction;
      /// Particle energy
      double m_energy;
      /// Desired multiplicity of the particles to be shot
      int m_multiplicity;
      /// Particle name
      std::string m_particleName;
      /// Pointer to geant4 particle definition
      G4ParticleDefinition* m_particle;
      /// Pointer to the particle gun itself
      G4ParticleGun* m_gun;
    public:
      /// Standard constructor
      Geant4ParticleGun(Geant4Context* context, const std::string& name);
      /// Default destructor
      virtual ~Geant4ParticleGun();
      /// Callback to generate primary particles
      virtual void operator()(G4Event* event);
    };
  }    // End namespace Simulation
}      // End namespace DD4hep
#endif /* DD4HEP_DDG4_GEANT4PARTICLEGUN_H  */

