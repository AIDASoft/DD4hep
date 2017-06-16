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
// Framework include files
#include "DDG4/Geant4Context.h"
#include "DDG4/Geant4Kernel.h"

#include "DDG4/Python/DDPython.h"
#include "DDG4/Python/Geant4PythonAction.h"
#include "DDG4/Python/Geant4PythonDetectorConstruction.h"

using namespace std;
using namespace dd4hep::sim;

#include "DDG4/Factories.h"
DECLARE_GEANT4ACTION(Geant4PythonDetectorConstruction)

/// Standard constructor, initializes variables
Geant4PythonDetectorConstruction::Geant4PythonDetectorConstruction(Geant4Context* ctxt, const string& nam)
: Geant4DetectorConstruction(ctxt,nam), 
  m_constructSD(), m_constructFLD(), m_constructGEO()
{
  m_needsControl = true;
}

/// Set the Detector initialization command
void Geant4PythonDetectorConstruction::setConstructGeo(PyObject* callable, PyObject* args)   {
  m_constructGEO.set(callable, args); 
}

/// Set the field initialization command
void Geant4PythonDetectorConstruction::setConstructField(PyObject* callable, PyObject* args)   {
  m_constructFLD.set(callable, args); 
}

/// Set the sensitive detector initialization command
void Geant4PythonDetectorConstruction::setConstructSensitives(PyObject* callable, PyObject* args)   {  
  m_constructSD.set(callable, args); 
}

/// Execute command in the python interpreter
void Geant4PythonDetectorConstruction::exec(const string& desc, const Geant4PythonCall& cmd)  const   {
  if ( cmd.isValid() )  {
    int ret = cmd.execute<int>();
    if ( ret != 1 )  {
      except("+++ %s returned %d, not SUCCESS (1). Terminating setup",desc.c_str(), ret);
    }
  }
}

/// Callback function to build setup for the MT worker thread
void Geant4PythonDetectorConstruction::constructGeo(Geant4DetectorConstructionContext* )    {
  info("+++ Worker:%ld Execute PYTHON constructGeo %s....",
       context()->kernel().id(), m_constructGEO.isValid() ? "" : "[empty]");
  DDPython::BlockThreads blocker(0);
  exec("constructGeo", m_constructGEO);
}

/// Callback function to build setup for the MT master thread
void Geant4PythonDetectorConstruction::constructField(Geant4DetectorConstructionContext* )    {
  info("+++ Worker:%ld Execute PYTHON constructField %s....",
       context()->kernel().id(), m_constructFLD.isValid() ? "" : "[empty]");
  exec("constructField", m_constructFLD);
}

/// Callback function to build setup for the MT master thread
void Geant4PythonDetectorConstruction::constructSensitives(Geant4DetectorConstructionContext* )    {
  info("+++ Worker:%ld Execute PYTHON constructSensitives %s....",
       context()->kernel().id(), m_constructSD.isValid() ? "" : "[empty]");
  exec("constructSensitives", m_constructSD);
}
