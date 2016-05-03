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
#ifndef DD4HEP_DDG4_GEANT4PYTHONDETECTORCONSTRUCTIONLAST_H
#define DD4HEP_DDG4_GEANT4PYTHONDETECTORCONSTRUCTIONLAST_H

// Framework include files
#include "DDG4/Geant4DetectorConstruction.h"
#include "DDG4/Python/DDPython.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    /// Base class to initialize a multi-threaded or single threaded Geant4 application
    /**
     *  All python callbacks are supposed to return the integer '1' on success.
     *  Any other return code is assumed to be failure.
     * 
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4PythonDetectorConstructionLast : public Geant4DetectorConstruction  {
    public:
      /// Standard constructor
      Geant4PythonDetectorConstructionLast(Geant4Context* ctxt, const std::string& nam)
        : Geant4DetectorConstruction(ctxt,nam) {}
      /// Default destructor
      virtual ~Geant4PythonDetectorConstructionLast() {}
      /// Geometry construction callback. Called at "Construct()"
      virtual void constructGeo(Geant4DetectorConstructionContext*)      {
        info("+++ Python setup finished. From now on THREADS ARE ALLOWED!");
        DDPython::allowThreads();
      }
    };
  }    // End namespace Simulation
}      // End namespace DD4hep
#endif // DD4HEP_DDG4_GEANT4PYTHONDETECTORCONSTRUCTIONLAST_H

using namespace DD4hep::Simulation;

#include "DDG4/Factories.h"
DECLARE_GEANT4ACTION(Geant4PythonDetectorConstructionLast)
