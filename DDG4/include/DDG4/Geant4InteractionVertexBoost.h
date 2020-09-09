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
   \package Geant4InteractionVertexBoost
 * \brief Action class to boost the primary vertex (and all outgoing particles) of a single interaction
 *
 *
@}
 */

#ifndef DDG4_GEANT4INTERACTIONVERTEXBOOST_H
#define DDG4_GEANT4INTERACTIONVERTEXBOOST_H

// Framework include files
#include "DDG4/Geant4GeneratorAction.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    /// Action class to boost the primary vertex (and all outgoing particles) of a single interaction
    /**
     * The vertex boost is steered by the Lorentz transformation angle.
     * The interaction to be modified is identified by the interaction's unique mask.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4InteractionVertexBoost: public Geant4GeneratorAction {
    public:
      /// Interaction definition
      typedef Geant4PrimaryInteraction Interaction;

    protected:
      /// Property: The constant Lorentz transformation angle
      double m_angle;
      /// Property: Unique identifier of the interaction to be modified
      int m_mask;

      /// Action routine to boost one single interaction according to the properties
      void boost(Interaction* interaction)  const;

    public:
      /// Inhibit default constructor
      Geant4InteractionVertexBoost() = delete;
      /// Standard constructor
      Geant4InteractionVertexBoost(Geant4Context* context, const std::string& name);
      /// Default destructor
      virtual ~Geant4InteractionVertexBoost();
      /// Callback to generate primary particles
      virtual void operator()(G4Event* event);
    };
  }    // End namespace sim
}      // End namespace dd4hep
#endif // DDG4_GEANT4INTERACTIONVERTEXBOOST_H
