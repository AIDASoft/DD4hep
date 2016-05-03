// Framework include files
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
// $Id$
#ifndef DD4HEP_DDG4_PYDDG4_H
#define DD4HEP_DDG4_PYDDG4_H

// Framework include files
#include "DDG4/Geant4Kernel.h"

struct PyDDG4  {
  typedef DD4hep::Simulation::Geant4Kernel Kernel;

  static int execute();
  static int process(const char* fname);
  static int run(Kernel& kernel);
  static int run(const char* fname);
};

#endif // DD4HEP_DDG4_PYDDG4_H
