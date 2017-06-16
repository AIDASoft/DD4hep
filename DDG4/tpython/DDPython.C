// $Id$
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
// FRamework include files
#include "DDG4/Python/DDPython.h"

struct dd4hepPython {
  static void setMainThread()   {
    dd4hep::DDPython::setMainThread();
  }
};

// CINT configuration
#if defined(__MAKECINT__)
#pragma link C++ namespace dd4hep;
#pragma link C++ class dd4hep::DDPython;
#pragma link C++ class dd4hepPython;
#endif
