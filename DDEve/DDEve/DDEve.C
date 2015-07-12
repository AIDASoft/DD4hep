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

#include <string>
#include <cerrno>
#include "TError.h"
#include "TSystem.h"

void DDEve(const char* xmlConfig=0)  {
  char text[1024];
  const char* dd4hep = gSystem->Getenv("DD4hepINSTALL");
  if ( 0 == dd4hep )   {
    Error("DDEve","++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    Error("DDEve","+++ Your DD4hep installation seems incomplete. The environment DD4hepINSTALL is not defined!   +++");
    Error("DDEve","++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    gSystem->Exit(EINVAL);
  }
  ::snprintf(text,sizeof(text)," -I%s/include -D__DD4HEP_DDEVE_EXCLUSIVE__ -Wno-shadow -g -O0",dd4hep);
  gSystem->AddIncludePath(text);
  Long_t result =  = gSystem->Load("libDDEve");
  if ( 0 != result )  {
    Error("DDEve","++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    Error("DDEve","+++ Your DD4hep installation seems incomplete. FAILED to load the library 'libDD4hepEve'!      +++");
    Error("DDEve","++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    gSystem->Exit(EINVAL);
  }
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
  DD4hep::EveDisplay(xmlConfig);
}
