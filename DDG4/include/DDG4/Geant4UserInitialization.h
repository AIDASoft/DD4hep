// $Id$
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
#ifndef DD4HEP_DDG4_GEANT4USERINITIALIZATION_H
#define DD4HEP_DDG4_GEANT4USERINITIALIZATION_H

// Framework include files
#include "DDG4/Geant4Action.h"

// Forward declarations

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    /// Base class to initialize a multi-threaded or single threaded Geant4 application
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4UserInitialization : public Geant4Action  {
    protected:
    public:
      /// Standard constructor
      Geant4UserInitialization(Geant4Context* c, const std::string& nam);
      /// Default destructor
      virtual ~Geant4UserInitialization();
      /// Callback function to build setup for the MT worker thread
      virtual void build()  const;
      /// Callback function to build setup for the MT master thread
      virtual void buildMaster()  const;
    };

    /// Class to orchestrate a modular initializion of a multi- or single threaded Geant4 application
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4UserInitializationSequence : public Geant4UserInitialization  {
    protected:
      /// Callback sequence to initialize the thread master
      CallbackSequence m_masterCalls;
      /// Callback sequence to initialize worker elements
      CallbackSequence m_workerCalls;
      /// The list of action objects to be called
      Actors<Geant4UserInitialization> m_actors;

    public:
      /// Standard constructor
      Geant4UserInitializationSequence(Geant4Context* c, const std::string& nam);
      /// Default destructor
      virtual ~Geant4UserInitializationSequence();
      /// Register callback to setup worker. Types Q and T must be polymorph!
      template <typename Q, typename T> void build(Q* p, void (T::*f)()) {
        m_workerCalls.add(p, f);
      }
      /// Register callback to setup master. Types Q and T must be polymorph!
      template <typename Q, typename T> void buildMaster(Q* p, void (T::*f)()) {
        m_masterCalls.add(p, f);
      }
      /// Add an actor responding to all callbacks. Sequence takes ownership.
      void adopt(Geant4UserInitialization* action);

      /// Set client context
      virtual void updateContext(Geant4Context* ctxt);
      /// Callback function to build setup for the MT worker thread
      virtual void build()  const;
      /// Callback function to build setup for the MT master thread
      virtual void buildMaster()  const;
    };

  }    // End namespace sim
}      // End namespace dd4hep
#endif // DD4HEP_DDG4_GEANT4USERINITIALIZATION_H
