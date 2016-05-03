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
#include "DDG4/Python/DDPython.h"

struct DD4hepPython {
  static void setMainThread()   {
    DD4hep::DDPython::setMainThread();
  }
};

// CINT configuration
#if defined(__MAKECINT__)
#pragma link C++ namespace DD4hep;
#pragma link C++ class DD4hep::DDPython;
#pragma link C++ class DD4hepPython;
#endif
