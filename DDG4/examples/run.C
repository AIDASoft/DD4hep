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

// ROOT include files
#include "TInterpreter.h"

// C/C++ include files
#include <string>


// Run a ROOT macro
void run(const char* macro)  {
  int status;
  char cmd[1024];
  const char* dd4hep_install = getenv("DD4hepINSTALL");
  if ( dd4hep_install )  {
    ::sprintf(cmd,".L %s/examples/DDG4/examples/initAClick.C+",dd4hep_install);
  }
  else  {
    ::sprintf(cmd,".L examples/DDG4/examples/initAClick.C+");
  }
  status = gInterpreter->ProcessLine(cmd); 
  ::printf("Status(%s) = %d\n",cmd,status);
  status = gInterpreter->ProcessLine("initAClick()");
  ::printf("Status(%s) = %d\n",cmd,status);
  ::sprintf(cmd,"processMacro(\"%s\",true)",macro);
  status = gInterpreter->ProcessLine(cmd);
  ::printf("Status(%s) = %d\n",cmd,status);
}
