//==========================================================================
//  AIDA Detector description implementation 
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
/** \addtogroup Geant4GeneratorAction
 *
 @{
   \package Geant4ParticleGun
 * \brief Implementation of a particle gun using Geant4Particles.
 *
 *
@}
 */

#ifndef DD4HEP_DDG4_GEANT4PARTICLEGUN_H
#define DD4HEP_DDG4_GEANT4PARTICLEGUN_H

// Framework include files
#include "DDG4/Geant4IsotropeGenerator.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

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
    class Geant4ParticleGun: public Geant4IsotropeGenerator {
    protected:
      /// Property: Isotrope particles?
      bool m_isotrop;
      /// Property: Standalone mode: includes interaction merging and primary generation
      bool m_standalone;
      /// Property: Enable printout
      bool m_print;
      /// Shot number in sequence
      int m_shotNo;
      /// Particle modification. Caller presets defaults to: ( direction = m_direction, momentum = m_energy)
      virtual void getParticleDirection(int, ROOT::Math::XYZVector& direction, double& momentum) const  {
        direction = m_direction;
        momentum = m_energy;
      }
    public:
      /// Standard constructor
      Geant4ParticleGun(Geant4Context* context, const std::string& name);
      /// Default destructor
      virtual ~Geant4ParticleGun();
      /// Callback to generate primary particles
      virtual void operator()(G4Event* event);
    };
  }    // End namespace sim
}      // End namespace dd4hep
#endif /* DD4HEP_DDG4_GEANT4PARTICLEGUN_H  */
