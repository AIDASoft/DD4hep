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
#include "DDG4/Geant4EventAction.h"

#include <csignal>
#include <unistd.h>

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
    class TestSignalAction : public Geant4EventAction {
      long num_calls { 0 };
      int  signal_event { 10000 };

    public:
      /// Standard constructor
      TestSignalAction(Geant4Context* context, const std::string& nam)
	: Geant4EventAction(context, nam) 
      {
	declareProperty("signal_event", signal_event);
      }
      /// Default destructor
      virtual ~TestSignalAction() = default;
      /// Begin-of-event callback
      virtual void begin(const G4Event* /* event */)  {
	if ( ++num_calls == signal_event )  {
	  always("Sending interrupt signal to self at call %d", ++num_calls);
	  ::kill(::getpid(), SIGINT);
	}
      }
    };
  }    // End namespace sim
}      // End namespace dd4hep

#include "DDG4/Factories.h"
DECLARE_GEANT4ACTION_NS(dd4hep::sim,TestSignalAction)
