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
#ifndef DDG4_GEANT4RUNMANAGER_H
#define DDG4_GEANT4RUNMANAGER_H 1

/// Framework include files
#include "DDG4/Geant4Action.h"

/// Geant4 include files
#include "G4RunManager.hh"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    /// Geant4 run manager plugin class. 
    /**
     *  The plugin acts as a normal Geant4Action. However, it must support a 
     *  dynamic_cast to G4RunManager.
     *  The templated class may be specialized by any user defined class
     *  which has G4RunManager as a super class.
     *
     *  Current specializations are:
     *  - G4RunManager for single threaded applications
     *  - G4MTRunManager for multi threaded applications
     *
     *  For convenience we name the factory instances according to the G4 classes.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    template <typename RUNMANAGER>
    class Geant4RunManager : public Geant4Action, public RUNMANAGER    {
    public:
      Geant4RunManager(Geant4Context* ctxt, const std::string& nam)
        : Geant4Action(ctxt, nam), RUNMANAGER()
      {
        declareProperty("NumberOfThreads", m_numThreads);
      }
      virtual ~Geant4RunManager()   { }
      /// Enable and install UI messenger
      virtual void enableUI();
    private:
      /// global range cut for secondary productions
      int m_numThreads;
    };
    template <> void Geant4RunManager<G4RunManager>::enableUI()  {
      Geant4Action::enableUI();
      printout(WARNING,"Geant4RunManager","+++ Configured run manager of type: %s.",
               typeName(typeid(G4RunManager)).c_str());
      printout(WARNING,"Geant4Kernel","+++ Multi-threaded mode requested, "
               "but not supported by this compilation of Geant4.");
      printout(WARNING,"Geant4Kernel","+++ Falling back to single threaded mode.");
      m_numThreads = 0;
    }
    typedef Geant4RunManager<G4RunManager>   Geant4STRunManager;
  }
}
#endif   // DDG4_GEANT4RUNMANAGER_H

#include "DDG4/Factories.h"
using namespace dd4hep::sim;
DD4HEP_PLUGINSVC_FACTORY(Geant4STRunManager,G4RunManager,dd4hep::sim::Geant4Action*(_ns::CT*,std::string),__LINE__)

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {
  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {
    template <> void Geant4RunManager<G4MTRunManager>::enableUI()  {
      Geant4Action::enableUI();
      this->G4MTRunManager::SetNumberOfThreads(m_numThreads);
      printout(WARNING,"Geant4RunManager","+++ Configured run manager of type: %s with %d threads.",
               typeName(typeid(G4MTRunManager)).c_str(), m_numThreads);
    }
    typedef Geant4RunManager<G4MTRunManager> Geant4MTRunManager;
  }
}
DD4HEP_PLUGINSVC_FACTORY(Geant4MTRunManager,G4MTRunManager,dd4hep::sim::Geant4Action*(_ns::CT*,std::string),__LINE__)
#endif


