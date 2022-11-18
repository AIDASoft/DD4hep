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

// Framework include files
#include "DDG4/Geant4Action.h"
#include "DDG4/Geant4UIMessenger.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {
    
    /// Class to print message for debugging
    /** Class to print message for debugging
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class TestPrintAction : public Geant4Action {
      int num_calls { 0 };
      std::string param  { "Uninitalized" };
    public:
      /// Standard constructor
      TestPrintAction(Geant4Context* context, const std::string& nam)
	: Geant4Action(context, nam) 
      {
	declareProperty("param", param);
      }
      /// Default destructor
      virtual ~TestPrintAction()   {
      }
      /// Messenger callback
      void print_par()   {
	always("Parameter value at call %d is '%s'", ++num_calls, param.c_str());
      }
      /// Install command control messenger to write GDML file from command prompt.
      virtual void installCommandMessenger()  override {
	m_control->addCall("print_param", "Printing some increasing parameter",
			   Callback(this).make(&TestPrintAction::print_par),0);
      }
    };
  }    // End namespace sim
}      // End namespace dd4hep

#include "DDG4/Factories.h"
DECLARE_GEANT4ACTION_NS(dd4hep::sim,TestPrintAction)
