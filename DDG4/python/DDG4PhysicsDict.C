//==========================================================================
//  AIDA Detector description implementation 
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

// Framework include files
#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wdeprecated"
#pragma GCC diagnostic ignored "-Wunused"
#pragma GCC diagnostic ignored "-Woverlength-strings"

#elif defined(__llvm__) || defined(__APPLE__)

#pragma clang diagnostic ignored "-Wdefaulted-function-deleted"
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#pragma clang diagnostic ignored "-Wdeprecated"
#pragma clang diagnostic ignored "-Wunused"
#pragma clang diagnostic ignored "-Woverlength-strings"
#endif

#include "G4VPhysicsConstructor.hh"
#include "G4StepLimiterPhysics.hh"

#if defined(__CINT__) || defined(__MAKECINT__) || defined(__CLING__) || defined(__ROOTCLING__)
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class G4VPhysicsConstructor;
#pragma link C++ class G4StepLimiterPhysics;
#endif
