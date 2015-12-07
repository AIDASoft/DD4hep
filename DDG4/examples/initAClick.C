// $Id: $
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

// ROOT include files
#include "TInterpreter.h"
#include "TSystem.h"
#include "RVersion.h"

// C/C++ include files
#include <iostream>
#include <string>

std::string make_str(const char* data)  {
  if ( !data )   {
    std::cout << "make_str:  '" << (data ? data : "Bad-Pointer") << "'" << std::endl;
    return std::string("");
  }
  return std::string(data);
}

/// Process a single command in the ROOT interpreter
int processCommand(const char* command, bool end_process)   {
  int status;
  // Disabling auto-parse is a hack required by a bug in ROOT
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,0,0)
  gInterpreter->SetClassAutoparsing(false);
  status = gInterpreter->ProcessLine(command);
  gInterpreter->SetClassAutoparsing(true);
#else
  status = gInterpreter->ProcessLine(command);
#endif
  ::printf("+++ Status(%s) = %d\n",command,status);
  if ( end_process )  {
    gInterpreter->ProcessLine("gSystem->Exit(0)");
  }
  return status;
}

/// Process a ROOT AClick given a file
int processMacro(const char* macro, bool end_process)   {
  std::string cmd = ".X ";
  cmd += macro;
  cmd += ".C+()";
  return processCommand(cmd.c_str(), end_process);
}

/// Initialize the ROOT environment to compile and execute a ROOT AClick
int initAClick(const char* command=0)  {
  std::string rootsys = make_str(gSystem->Getenv("ROOTSYS"));
  std::string g4_base = make_str(gSystem->Getenv("G4INSTALL"));
  std::string dd4hep  = make_str(gSystem->Getenv("DD4hepINSTALL"));
  std::string libs = (" -L"+rootsys+"/lib");
  std::string inc  = " -I"+dd4hep+"/examples/DDG4/examples" +
    " -I"+dd4hep +
    " -I"+dd4hep+"/include" +
    " -I"+g4_base+"/include/Geant4" +
    " -Wno-shadow -g -O0";
  if ( ROOT_VERSION_CODE >= ROOT_VERSION(6,0,0) )
    libs += " -lCore -lMathCore";
  else
    libs += " -lCore -lCint -lMathCore";
  libs += " -pthread -lm -ldl -rdynamic";
  libs += " -L"+dd4hep+"/lib -lDDCore -lDDG4 -lDDSegmentation";
  libs += (" -L"+g4_base+"/lib -L"+g4_base+"/lib64 -lG4event -lG4tracking -lG4particles");
  gSystem->AddIncludePath(inc.c_str());
  gSystem->AddLinkedLibs(libs.c_str());
  std::cout << "+++ Includes:   " << gSystem->GetIncludePath() << std::endl;
  std::cout << "+++ Linked libs:" << gSystem->GetLinkedLibs()  << std::endl;
  int ret = gSystem->Load("libDDG4Plugins");
  if ( 0 == ret )   {
    if ( command )  {
      processCommand(command, true);
    }
  }
  return ret;
}
