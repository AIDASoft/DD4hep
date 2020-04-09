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
   \package Geant4InteractionVertexSmear
 * \brief Action class to smear the primary vertex (and all outgoing particles) of a single interaction
 *
 *
@}
 */

#ifndef DD4HEP_DDG4_GEANT4INTERACTIONVERTEXSMEAR_H
#define DD4HEP_DDG4_GEANT4INTERACTIONVERTEXSMEAR_H

// Framework include files
#include "DDG4/Geant4GeneratorAction.h"

// ROOT include files
#include "Math/Vector4D.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    /// Forward declarations
    class Geant4PrimaryInteraction;

    /// Action class to smear the primary vertex (and all outgoing particles) of a single interaction
    /**
     * The vertex smearing is steered by a 3D gaussian given by a constant offset and
     * the corresponding errors. The interaction to be modified is identified by the
     * interaction's unique mask.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4InteractionVertexSmear: public Geant4GeneratorAction {
    public:
      /// Interaction definition
      typedef Geant4PrimaryInteraction Interaction;

    protected:
      /// Property: The constant smearing offset
      ROOT::Math::PxPyPzEVector m_offset;
      /// Property: The gaussian sigmas to the offset
      ROOT::Math::PxPyPzEVector m_sigma;
      /// Property: Unique identifier of the interaction created
      int m_mask;

      /// Action routine to smear one single interaction according to the properties
      void smear(Interaction* interaction)  const;
      
    public:
      /// Inhibit default constructor
      Geant4InteractionVertexSmear() = delete;
      /// Inhibit copy constructor
      Geant4InteractionVertexSmear(const Geant4InteractionVertexSmear& copy) = delete;
      /// Standard constructor
      Geant4InteractionVertexSmear(Geant4Context* context, const std::string& name);
      /// Default destructor
      virtual ~Geant4InteractionVertexSmear();
      /// Callback to generate primary particles
      virtual void operator()(G4Event* event);
    };
  }    // End namespace sim
}      // End namespace dd4hep
#endif /* DD4HEP_DDG4_GEANT4INTERACTIONVERTEXSMEAR_H  */
