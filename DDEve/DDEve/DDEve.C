//==========================================================================
//  AIDA Detector description implementation 
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

// C/C++ include files
#include <string>
#include <cerrno>

// ROOT include files
#include "TError.h"
#include "TSystem.h"
#include "TInterpreter.h"

void DDEve(const char* xmlConfig=0, const char* evtData=0)  {
  Long_t result;
  char text[1024];
  const char* dd4hep = gSystem->Getenv("DD4hepINSTALL");
  if ( 0 == dd4hep )   {
    Error("DDEve","++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    Error("DDEve","+++ Your dd4hep installation seems incomplete. The environment DD4hepINSTALL is not defined!   +++");
    Error("DDEve","++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    gSystem->Exit(EINVAL);
  }
  ::snprintf(text,sizeof(text)," -I%s/include -D__DD4HEP_DDEVE_EXCLUSIVE__ -Wno-shadow -g -O0",dd4hep);
  gSystem->AddIncludePath(text);
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,0,0)
  TString fname = "libDDG4IO";
  const char* io_lib = gSystem->FindDynamicLibrary(fname,kTRUE);
#else
  const char* io_lib = "libDDG4IO";
#endif
  if ( io_lib )  {
    result = gSystem->Load("libDDG4IO");
    if ( 0 != result )  {
      Error("DDEve","++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
      Error("DDEve","+++ FAILED to load the DDG4 IO library 'libDDG4IO'!                                            +++");
      Error("DDEve","+++ %s",io_lib);
      Error("DDEve","++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
      gSystem->Exit(EINVAL);
    }
    Info("DDEve","+++ Loaded DDG4IO library .....");
  }
  else  {
    Error("DDEve","+++ No DDG4 IO library 'libDDG4IO'  present!");
  }
  result = gSystem->Load("libDDEve");
  if ( 0 != result )  {
    Error("DDEve","++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    Error("DDEve","+++ Your dd4hep installation seems incomplete. FAILED to load the library 'libdd4hepEve'!      +++");
    Error("DDEve","++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    gSystem->Exit(EINVAL);
  }
  Info("DDEve","+++ Loaded DDEve library .....");
#if 0
  Info("DDEve","Has to be run in compiled mode to support DDG4 input ... doing this for you now....");
  ::snprintf(text,sizeof(text),".L %s/examples/DDEve/DDG4IO.C+",dd4hep);
  gSystem->ResetErrno();
  result = gInterpreter->ProcessLine(text);
  result = gSystem->Load("DDG4Support_C");
  if ( result != 1 )   {
    Error("DDEve","++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    Error("DDEve","+++ Result:%ld %s",result,text);
    Error("DDEve","+++ errno=%d %s",gSystem->GetErrno(),gSystem->GetError());
    Error("DDEve","++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    gSystem->Exit(gSystem->GetErrno());
  }
#endif
  if ( xmlConfig && evtData )
    ::snprintf(text,sizeof(text),"dd4hep::DDEve::run(\"%s\",\"%s\")", xmlConfig, evtData);
  else if ( xmlConfig )
    ::snprintf(text,sizeof(text),"dd4hep::DDEve::run(\"%s\")", xmlConfig);
  else 
    ::snprintf(text,sizeof(text),"dd4hep::DDEve::run(0)");
  gInterpreter->ProcessLine(text);
}
