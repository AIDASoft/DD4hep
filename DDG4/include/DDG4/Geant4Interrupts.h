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
#ifndef DDG4_GEANT4INTERRUPTS_H
#define DDG4_GEANT4INTERRUPTS_H

/// Framework include files
#include <DD4hep/SignalHandler.h>

/// System include files

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    /// Forward declarations
    class Geant4Kernel;
    
    /// Interruptsback interface class with argument
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class  Geant4Interrupts : public SignalHandler {
    public:
      /// Reference to simulation kernel
      Geant4Kernel& m_kernel;
      Geant4Kernel& kernel()  {  return m_kernel;  }

      /// Default SIGINT handler: trigger end-of-event-loop in Geant4Kernel object
      static bool default_sigint_handler(void* user_context, int signum);

    public:
      /// Default constructor
      Geant4Interrupts(Geant4Kernel& krnl) : m_kernel(krnl) { }
      /// Default destructor
      virtual ~Geant4Interrupts() = default;
      /// Specialized handler for SIGINT
      bool registerHandler_SIGINT();
    };
  }    // End namespace sim
}      // End namespace dd4hep
#endif // DDG4_GEANT4INTERRUPTS_H
