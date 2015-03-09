// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4PARTICLEGENERATOR_H
#define DD4HEP_DDG4_GEANT4PARTICLEGENERATOR_H

// Framework include files
#include "DDG4/Geant4GeneratorAction.h"
#include "Math/Vector3D.h"

// Forward declarations
class G4ParticleDefinition;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {
    /// Generate particles isotrop in space around origine (0,0,0)
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4ParticleGenerator: public Geant4GeneratorAction {
    protected:
      /// Property: Shooting direction of the gun
      ROOT::Math::XYZVector m_direction;
      /// Property: Position of the gun in space
      ROOT::Math::XYZVector m_position;
      /// Property: Particle name
      std::string m_particleName;
      /// Pointer to geant4 particle definition
      G4ParticleDefinition* m_particle;
      /// Property: Particle energy
      double m_energy;
      /// Property: Desired multiplicity of the particles to be shot
      int m_multiplicity;
      /// Property: User mask passed to all particles in the generated interaction
      int m_mask;
      
      /// Particle modification. Caller presets defaults to: (multiplicity=m_multiplicity)
      virtual void getParticleMultiplicity(int& multiplicity) const;

      /// Particle's vertex modification. Caller presets defaults to: (multiplicity=m_multiplicity)
      virtual void getVertexPosition(ROOT::Math::XYZVector& position) const;

      /// Particle modification. Caller presets defaults to: ( direction = m_direction, momentum = m_energy)
      /** Use this function to implement isotrop guns, multiple guns etc. 
	  User must return a UNIT vector, which gets scaled with momentum.
       */
      virtual void getParticleDirection(int num, ROOT::Math::XYZVector& direction, double& momentum) const;

    public:
      /// Standard constructor
      Geant4ParticleGenerator(Geant4Context* context, const std::string& name);
      /// Default destructor
      virtual ~Geant4ParticleGenerator();
      /// Callback to generate primary particles
      virtual void operator()(G4Event* event);
    };
  }    // End namespace Simulation
}      // End namespace DD4hep
#endif /* DD4HEP_DDG4_GEANT4PARTICLEGENERATOR_H  */
