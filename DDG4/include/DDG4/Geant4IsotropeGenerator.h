// $Id: $
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

#ifndef DD4HEP_DDG4_GEANT4ISOTROPEGENERATOR_H
#define DD4HEP_DDG4_GEANT4ISOTROPEGENERATOR_H

// Framework include files
#include "DDG4/Geant4ParticleGenerator.h"

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
    class Geant4IsotropeGenerator: public Geant4ParticleGenerator {
    protected:
      /// Particle modification. Caller presets defaults to: ( direction = m_direction, momentum = m_energy)
      /** Use this function to implement isotrop guns, multiple guns etc. 
          User must return a UNIT vector, which gets scaled with momentum.
      */
      virtual void getParticleDirection(int num, ROOT::Math::XYZVector& direction, double& momentum) const;

    public:
      /// Standard constructor
      Geant4IsotropeGenerator(Geant4Context* context, const std::string& name);
      /// Default destructor
      virtual ~Geant4IsotropeGenerator();
    };
  }    // End namespace Simulation
}      // End namespace DD4hep
#endif /* DD4HEP_DDG4_GEANT4ISOTROPEGENERATOR_H  */
