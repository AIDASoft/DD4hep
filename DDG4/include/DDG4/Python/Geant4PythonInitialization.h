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
#ifndef DDG4_PYTHON_GEANT4PYTHONINITIALIZATION_H
#define DDG4_PYTHON_GEANT4PYTHONINITIALIZATION_H

// Framework include files
#include "DDG4/Geant4UserInitialization.h"
#include "DDG4/Python/Geant4PythonCall.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    /// Base class to initialize a multi-threaded or single threaded Geant4 application
    /**
     *  Geant4PythonInitialization allows to configure python callbacks
     *  for the master and the worker thread setup using the calls:
     *
     *     // Set the Detector initialization command
     *     void setMasterSetup(PyObject* callable, PyObject* args);
     *     // Set the field initialization command
     *     void setWorkerSetup(PyObject* callable, PyObject* args);
     *
     *  or in python as a call sequence within the master thread:
     *
     *     init_seq = kernel.userInitialization(True)
     *     init_action = UserInitialization(kernel,'Geant4PythonInitialization/PyG4Init')
     *     init_action.setWorkerSetup(worker_setup_call, < worker_args > )
     *     init_action.setMasterSetup(master_setup_call, < master_args > )
     *     init_seq.adopt(init_action)
     *
     *  The callback argument list worker_args and master_args
     *  are python tuples containing all arguments expected by the callbacks
     *  worker_setup_call and master_setup_call respecyively.
     *  The class Geant4PythonInitialization is a subclass of
     *  Geant4UserInitialization and will call the provided functions
     *  according to the protocol explained earlier in this section.
     *
     *  All python callbacks are supposed to return the integer '1' on success.
     *  Any other return code is assumed to be failure.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4PythonInitialization : public Geant4UserInitialization  {
    protected:
      /// Master initialization command. Default: empty
      Geant4PythonCall m_masterSetup;
      /// Worker initialization command. Default: empty
      Geant4PythonCall m_workerSetup;

      /// Execute command in the python interpreter
      void exec(const std::string& desc, const Geant4PythonCall& cmd)  const;

    public:
      /// Standard constructor
      Geant4PythonInitialization(Geant4Context* c, const std::string& nam);
      /// Default destructor
      virtual ~Geant4PythonInitialization() {}
      
      /// Set the Detector initialization command
      void setMasterSetup(PyObject* callable, PyObject* args);
      /// Set the field initialization command
      void setWorkerSetup(PyObject* callable, PyObject* args);
      /// Callback function to build setup for the MT worker thread
      virtual void build()  const;
      /// Callback function to build setup for the MT master thread
      virtual void buildMaster()  const;
    };
  }    // End namespace sim
}      // End namespace dd4hep
#endif // DDG4_PYTHON_GEANT4PYTHONINITIALIZATION_H
