#if 0
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author Markus Frank
//  \date   2015-11-03
//
//==========================================================================
// $Id$
#ifndef DD4HEP_DDG4_GEANT4USERACTIONINITIALIZATION_H
#define DD4HEP_DDG4_GEANT4USERACTIONINITIALIZATION_H

// Framework include files
#include "DDG4/Geant4UserInitialization.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    /// Base class to initialize a multi-threaded or single threaded Geant4 application
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4UserActionInitialization : public Geant4UserInitialization   {
    public:
      /// Standard constructor
      Geant4UserActionInitialization(Geant4Context* c, const std::string& nam);
      /// Default destructor
      virtual ~Geant4UserActionInitialization() {}
      
      /// Callback function to build setup for the MT worker thread
      virtual void build();
      /// Callback function to build setup for the MT master thread
      virtual void buildMaster();
    };
  }    // End namespace Simulation
}      // End namespace DD4hep
#endif // DD4HEP_DDG4_GEANT4USERACTIONINITIALIZATION_H


//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author Markus Frank
//  \date   2015-11-03
//
//==========================================================================
// $Id$

// Framework include files
#include "DDG4/Factories.h"
//#include "DDG4/Geant4UserActionInitialization.h"
#include "DDG4/Geant4Context.h"

// C/C++ include files

using namespace std;
using namespace DD4hep::Simulation;

//DECLARE_GEANT4ACTION(Geant4UserActionInitialization)

/// Standard constructor, initializes variables
Geant4UserActionInitialization::Geant4UserActionInitialization(Geant4Context* ctxt, const string& nam)
  : Geant4UserInitialization(ctxt,nam)
{
}

/// Callback function to build setup for the MT worker thread
void Geant4UserActionInitialization::build()   {

}

/// Callback function to build setup for the MT master thread
void Geant4UserActionInitialization::buildMaster()   {
}
#endif
