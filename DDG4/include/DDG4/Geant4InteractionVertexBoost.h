// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4INTERACTIONVERTEXBOOST_H
#define DD4HEP_DDG4_GEANT4INTERACTIONVERTEXBOOST_H

// Framework include files
#include "DDG4/Geant4GeneratorAction.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    /// Action class to boost the primary vertex (and all outgoing particles) of a single interaction
    /**
     * The vertex smearing is steered by the Lorentz transformation angle.
     * The interaction to be modified is identified by the interaction's unique mask.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4InteractionVertexBoost: public Geant4GeneratorAction {
    protected:
      /// Property: The constant Lorentz transformation angle
      double m_angle;
      /// Property: Unique identifier of the interaction to be modified
      int m_mask;
    public:
      /// Standard constructor
      Geant4InteractionVertexBoost(Geant4Context* context, const std::string& name);
      /// Default destructor
      virtual ~Geant4InteractionVertexBoost();
      /// Callback to generate primary particles
      virtual void operator()(G4Event* event);
    };
  }    // End namespace Simulation
}      // End namespace DD4hep
#endif /* DD4HEP_DDG4_GEANT4INTERACTIONVERTEXBOOST_H  */
