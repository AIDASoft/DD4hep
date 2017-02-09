//==========================================================================
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
#ifndef DD4HEP_DDG4_GEANT4PYTHONACTION_H
#define DD4HEP_DDG4_GEANT4PYTHONACTION_H

// Framework include files
#include "DDG4/Geant4Action.h"
#include "TPyReturn.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    /// Base class to initialize a multi-threaded or single threaded Geant4 application
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP
     */
    class Geant4PythonAction : public Geant4Action   {
    public:
      /// Standard constructor
      Geant4PythonAction(Geant4Context* ctx, const std::string& nam);
      /// Default destructor
      virtual ~Geant4PythonAction() {}

      /** Public class methods  */
      /// Execute command in the python interpreter.
      static  int call(PyObject* method, PyObject* args);
      /// Execute command in the python interpreter.
      static  int exec(const std::string& cmd);
      /// Execute command in the python interpreter.
      static  int eval(const std::string& cmd);
      /// Execute command in the python interpreter.
      static  int runFile(const std::string& cmd);
      /// Invoke command prompt
      static  void prompt();
    };
  }    // End namespace Simulation
}      // End namespace DD4hep
#endif // DD4HEP_DDG4_GEANT4PYTHONACTION_H
