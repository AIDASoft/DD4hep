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

// ROOT include files
#include "Math/Vector4D.h"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {
    /// Generate particles isotrop in space around origine (0,0,0)
    /**
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4InteractionVertexBoost: public Geant4GeneratorAction {
    protected:
      /// The constant Lorentz transformation angle
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
