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

/** \addtogroup Geant4PhysicsConstructor
 *
 * @{
 * \package Geant4RangeCut
 *
 * \brief PhysicsConstructor to change the global range cut

 This plugin allows one to change the global geant4 range cut for producing secondaries.

 *
 * @}
 */

#ifndef DDG4_Geant4DefaultRangeCut_h
#define DDG4_Geant4DefaultRangeCut_h 1

// Framework include files
#include "DDG4/Geant4PhysicsList.h"

// geant4
#include "G4VUserPhysicsList.hh"


/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    /// Geant4 physics list action to set global range cuts for secondary productions
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4DefaultRangeCut : public Geant4PhysicsList    {
    public:
      Geant4DefaultRangeCut(Geant4Context* ctxt, const std::string& nam)
        : Geant4PhysicsList(ctxt, nam), m_rangeCut(0.7*CLHEP::mm)
      {
        declareProperty("RangeCut", m_rangeCut);
      }
      virtual ~Geant4DefaultRangeCut()   {
      }
      /// Callback to construct processes (uses the G4 particle table)
      virtual void constructProcesses(G4VUserPhysicsList* physics_list)   { 
        if ( physics_list )  {
          info("+++ Set PhysicsList RangeCut to %f mm",m_rangeCut/CLHEP::mm);
          physics_list->SetDefaultCutValue(m_rangeCut);
          physics_list->SetCuts();
          if( DEBUG == printLevel() ) physics_list->DumpCutValuesTable();
          return;
        }
        except("No valid reference to a G4VUserPhysicsList instance!");
      }

    private:
      /// global range cut for secondary productions
      double m_rangeCut;
    };
  }
}
#endif   // DDG4_Geant4DefaultRangeCut_h

#include "DDG4/Factories.h"
using namespace DD4hep::Simulation;
DECLARE_GEANT4ACTION(Geant4DefaultRangeCut)
