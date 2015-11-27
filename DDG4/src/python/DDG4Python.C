// $Id$
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//====================================================================
//
// Define the ROOT dictionaries for all data classes to be saved 
// which are created by the DDG4 examples.
//
//  Author     : M.Frank
//
//====================================================================
// FRamework include files
#include "DDG4Python/Geant4PythonCall.h"
#include "DDG4Python/Geant4PythonAction.h"
#include "DDG4Python/Geant4PythonInitialization.h"
#include "DDG4Python/Geant4PythonDetectorConstruction.h"
#include "PyDDG4.h"

// CINT configuration
#if defined(__MAKECINT__)
#pragma link C++ namespace DD4hep;
#pragma link C++ namespace DD4hep::Simulation;
#pragma link C++ class DD4hep::Simulation::Geant4PythonCall;
#pragma link C++ class DD4hep::Simulation::Geant4PythonAction;
#pragma link C++ class DD4hep::Simulation::Geant4PythonInitialization;
#pragma link C++ class DD4hep::Simulation::Geant4PythonDetectorConstruction;
#pragma link C++ class PyDDG4;
#endif
