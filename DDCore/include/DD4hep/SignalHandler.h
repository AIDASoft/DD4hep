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
#ifndef DD4HEP_DDCORE_SIGNALHANDLER_H
#define DD4HEP_DDCORE_SIGNALHANDLER_H

/// System include files
#include <csignal>
#include <memory>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Interruptsback interface class with argument
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CORE
   */
  class  SignalHandler  {
  public:
    /// User signal handler definition
    typedef bool (*signal_handler_t)(void* user_context, int signal);
    /// Internal implementation class
    class implementation;
      
  public:
    /// Default constructor
    SignalHandler();
    /// Default destructor
    virtual ~SignalHandler();
    /// (Re-)apply registered interrupt handlers to override potentially later registrations by other libraries
    void applyHandlers();
    /// Specialized handler for any signal
    bool registerHandler(int sig_num, void* param, signal_handler_t handler);
  };
}      // End namespace dd4hep
#endif // DD4HEP_DDCORE_SIGNALHANDLER_H
