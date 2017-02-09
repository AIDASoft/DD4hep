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

// Framework include files
#include "DDG4/Factories.h"
#include "DDG4/Geant4Kernel.h"
#include "DDG4/Python/DDPython.h"
#include "DDG4/Python/Geant4PythonInitialization.h"

using namespace std;
using namespace DD4hep::Simulation;

DECLARE_GEANT4ACTION(Geant4PythonInitialization)

/// Standard constructor, initializes variables
Geant4PythonInitialization::Geant4PythonInitialization(Geant4Context* ctxt, const string& nam)
: Geant4UserInitialization(ctxt,nam), m_masterSetup(), m_workerSetup()
{
  m_needsControl = true;
}

/// Set the Detector initialization command
void Geant4PythonInitialization::setMasterSetup(PyObject* callable, PyObject* args)  {
  m_masterSetup.set(callable, args); 
}

/// Set the field initialization command
void Geant4PythonInitialization::setWorkerSetup(PyObject* callable, PyObject* args)  {
  m_workerSetup.set(callable, args); 
}

/// Execute command in the python interpreter
void Geant4PythonInitialization::exec(const string& desc, const Geant4PythonCall& cmd)  const   {
  if ( cmd.isValid() )  { 
    int ret = cmd.execute<int>();
    if ( ret != 1 )  {
      except("+++ %s returned %d, not SUCCESS (1). Terminating setup",desc.c_str(), ret);
    }
  }
}

/// Callback function to build setup for the MT worker thread
void Geant4PythonInitialization::build() const   {
  info("+++ Worker:%ld Build PYTHON Worker %s....",
       context()->kernel().id(), m_workerSetup.isValid() ? "[empty]" : "");
  exec("Worker setup command",m_workerSetup);
}

/// Callback function to build setup for the MT master thread
void Geant4PythonInitialization::buildMaster()  const  {
  DDPython::BlockThreads blocker(0);
  info("+++ Build PYTHON Master [id:%ld] %s ....",
       context()->kernel().id(), m_masterSetup.isValid() ? "[empty]" : "");
  exec("Master setup command",m_masterSetup);
}
