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
#include "Math/Vector3D.h"

// Forward declarations
class G4ParticleDefinition;

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
      /// Property: Position and shooting direction of the gun
      ROOT::Math::XYZVector m_position, m_direction;
      /// Property: Particle energy
      double m_energy;
      /// Property: Particle name
      std::string m_particleName;
      /// Property: Desired multiplicity of the particles to be shot
      int m_multiplicity;
      /// Property: Interaction mask indentifier
      int m_mask;
      /// Property: Isotrope particles?
      bool m_isotrop;
      /// Property: Standalone mode: includes interaction merging and primary generation
      bool m_standalone;

      /// Pointer to geant4 particle definition
      G4ParticleDefinition* m_particle;
      /// Shot number in sequence
      int m_shotNo;
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
