//==========================================================================
//  AIDA Detector description implementation 
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
#ifndef DDG4_PYTHON_GEANT4PYTHONCALL_H
#define DDG4_PYTHON_GEANT4PYTHONCALL_H

// ROOT include files
#include "TPyReturn.h"

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
    class Geant4PythonCall   {
      /// Python callable reference
      PyObject* m_callable;
      /// Python arguments object
      PyObject* m_arguments;

    public:
      /// Standard constructor
      Geant4PythonCall();
      /// Default destructor
      virtual ~Geant4PythonCall();

      /// Check if call is set
      bool isValid()  const {  return m_callable != 0;  }

      /// Execute command in the python interpreter.
      template <typename RETURN> RETURN execute() const;

      /// Execute command in the python interpreter.
      template <typename RETURN> RETURN execute(PyObject* callable) const;

      /// Execute command in the python interpreter.
      template <typename RETURN> RETURN execute(PyObject* callable, PyObject* args) const;

      /// Set the callback structures for callbacks with arguments
      void set(PyObject* callable, PyObject* args);
      /// Set the callback structures for callbacks without arguments
      void set(PyObject* callable);
    };
  }    // End namespace sim
}      // End namespace dd4hep
#endif // DDG4_PYTHON_GEANT4PYTHONCALL_H
