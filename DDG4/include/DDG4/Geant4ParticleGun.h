// $Id$
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

#ifndef DD4HEP_DDG4_GEANT4PARTICLEGUN_H
#define DD4HEP_DDG4_GEANT4PARTICLEGUN_H

// Framework include files
#include "DDG4/Geant4GeneratorAction.h"
#include "Math/Vector3D.h"

// Forward declarations
class G4ParticleDefinition;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    /// Implementation of a particle gun using Geant4Particles.
    /**
     *  The {\tt{Geant4ParticleGun}} is a tool to shoot a number of
     *  particles with identical properties into a given region of the
     *  detector to be simulated.
     *
     *  The particle gun is a input source like any other and participates
     *  in the general input stage merging process like any other input
     *  e.g. from file. Hence, there may be several particle guns present
     *  each generating it's own primary vertex. Use the mask property to
     *  ensure each gun generates it's own, well identified primary vertex.
     *
     *  There is one 'user lazyness' support though:
     *  If there is only one particle gun in use, the property 'Standalone',
     *  which by default is set to true invokes the interaction merging and he
     *  Geant4 primary generation directly.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
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
