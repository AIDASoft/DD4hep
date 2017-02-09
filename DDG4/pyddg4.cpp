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
  bool have_prompt = false;
  bool do_execute = false;
  std::vector<char*> args;
  std::vector<char*> libs;
  int first_arg = 1;
  int ret;

  if ( argc>first_arg && strncmp(argv[first_arg],"-p",2)==0 )  {
    have_prompt = true;
    args.push_back(argv[0]);
    ++first_arg;
  }
  else if ( argc>first_arg && strncmp(argv[first_arg],"-e",2)==0 )  {
    do_execute = true;
    ++first_arg;
  }
  for(int i=first_arg; i<argc; ++i)  {
    if ( 0 == ::strcmp(argv[i],"-L") )
      libs.push_back(argv[++i]);
    else
      args.push_back(argv[i]);
  }
  if ( !have_prompt && args.size()>0 )  {
    libs.push_back((char*)"libDDG4Python");
    if ( 0 == (ret=load_libs(libs)) )   {
      DD4hep::DDPython::instance().setArgs(args.size(), &args[0]);
      DD4hep::DDPython::instance().setMainThread();
      DD4hep::DDPython::instance().runFile(args[0]);
      if ( do_execute )
        return gInterpreter->ProcessLine("PyDDG4::execute()");
      else 
        return 0;
    }
    return ret;
  }
  if ( 0 == (ret=load_libs(libs)) )   {
    ::printf("+++ Calling now Py_Main...\n");
    ret = DD4hep::DDPython::run_interpreter(args.size(), &args[0]);
    //::printf("+++ Return code Py_Main=%d\n",ret);
  }
  return ret;
}
