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
#include "DDG4/Factories.h"
#include "DDG4/Python/Geant4PythonAction.h"
#include "DDG4/Python/Geant4PythonCall.h"
#include "DDG4/Python/DDPython.h"

// C/C++ include files
#include <stdexcept>
#include <fstream>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::sim;

DECLARE_GEANT4ACTION(Geant4PythonAction)

/// Standard constructor, initializes variables
Geant4PythonAction::Geant4PythonAction(Geant4Context* ctxt, const string& nam)
  : Geant4Action(ctxt,nam)
{
  m_needsControl = true;
}

/// Execute command in the python interpreter. Directly forwarded to TPython.
int Geant4PythonAction::exec(const std::string& cmd)   {
  return DDPython::instance().execute(cmd);
}

/// Execute command in the python interpreter.
int Geant4PythonAction::eval(const std::string& cmd)   {
  return DDPython::instance().evaluate(cmd);
}
 
/// Execute command in the python interpreter.
int Geant4PythonAction::runFile(const std::string& cmd)   {
  return DDPython::instance().runFile(cmd);
}

/// Execute command in the python interpreter.
int Geant4PythonAction::call(PyObject* method, PyObject* args)  {
  return Geant4PythonCall().execute<int>(method,args);
}

/// Invoke command prompt
void Geant4PythonAction::prompt()   {
  DDPython::instance().prompt();
}
