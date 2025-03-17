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
#include <DD4hep/Printout.h>
#include <DDG4/Geant4Kernel.h>
#include <DDG4/Geant4Interrupts.h>

/// Default SIGINT handler: trigger end-of-event-loop in Geant4Kernel object
bool dd4hep::sim::Geant4Interrupts::default_sigint_handler(void* user_context, int)   {
  Geant4Kernel* krnl = (Geant4Kernel*)user_context;
  if ( krnl )  {
    krnl->triggerStop();
    return true;
  }
  except("Geant4Interrupts", "+++ Internal error: no user context in default SIGINT handler!");
  return true;
}

/// Install specialized handler for SIGINT
bool dd4hep::sim::Geant4Interrupts::registerHandler_SIGINT()  {
  return this->registerHandler(SIGINT, &m_kernel, default_sigint_handler);
}
