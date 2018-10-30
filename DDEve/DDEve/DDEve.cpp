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
#include <iostream>

// ROOT include files
#include "TRint.h"
#include "DDEve.C"

int main(int argc, char** argv)    {
  std::string config, data;
  for(int i=1; i<argc; ++i)  {
    if ( 0 == ::strncmp(argv[i],"-config",4) )
      config = argv[++i];
    else if ( 0 == ::strncmp(argv[i],"--config",5) )
      config = argv[++i];
    else if ( 0 == ::strncmp(argv[i],"-data",4) )
      data = argv[++i];
    else if ( 0 == ::strncmp(argv[i],"--data",5) )
      data = argv[++i];
    else  {
      std::cout <<
        "ddeve -opt [-opt] \n"
        "   -config <file>    XML configuration file   \n"
        "   -data   <file>    Event data file   file   \n"
           << std::endl;
      exit(0);
    }
  }
  std::pair<int, char**> a(0,0);
  const char* config_xml = config.empty() ? 0 : config.c_str();
  gApplication = new TRint("DDEve", &a.first, a.second);
  DDEve(config_xml);
  gApplication->Run();
  return 0;
}
