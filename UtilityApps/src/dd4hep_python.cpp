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
// Author     : M.Frank
//
//==========================================================================
#include "TSystem.h"
#include "TInterpreter.h"
#include "DDG4/Python/DDPython.h"
#include <vector>

static int load_libs(const std::vector<char*>& libs)  {
  for(size_t i=0; i<libs.size(); ++i)   {
    int ret = gSystem->Load(libs[i]);
    if ( 0 != ret )   {
      ::printf("+++ Failed to load library: %s [ignored]\n",libs[i]);
      return ret;
    }
    else  {
      ::printf("+++ Successfully loaded library: %s\n",libs[i]);
    }
  }
  return 0;
}

int main(int argc, char** argv)   {
  std::vector<char*> args;
  std::vector<char*> libs;
  int first_arg = 0;
  int ret;

  for(int i=first_arg; i<argc; ++i)  {
    if ( 0 == ::strcmp(argv[i],"-L") )
      libs.push_back(argv[++i]);
    else
      args.push_back(argv[i]);
  }
  if ( 0 == (ret=load_libs(libs)) )   {
    ::printf("+++ Calling now Py_Main...\n");
    ret = DD4hep::DDPython::run_interpreter(args.size(), &args[0]);
    //::printf("+++ Return code Py_Main=%d\n",ret);
  }
  return ret;
}
