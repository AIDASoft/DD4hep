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

// Framework include files
#include "run_plugin.h"

//______________________________________________________________________________
int main(int argc,char** argv)  {
  std::vector<const char*> av;
  std::string level, visopt, opt, detector;
  bool dry = false;
  for(int i=0; i<argc; ++i)  {
    if ( i==1 && argv[i][0] != '-' ) av.emplace_back("-input");
    if      ( strncmp(argv[i],"-load-only",4) == 0 ) dry = true, av.emplace_back(argv[i]);
    else if ( strncmp(argv[i],"-dry-run",4)   == 0 ) dry = true, av.emplace_back(argv[i]);
    else if ( strncmp(argv[i],"-visopt",4)    == 0 ) visopt   = argv[++i];
    else if ( strncmp(argv[i],"-level", 4)    == 0 ) level    = argv[++i];
    else if ( strncmp(argv[i],"-option",4)    == 0 ) opt      = argv[++i];
    else if ( strncmp(argv[i],"-detector",4)  == 0 ) detector = argv[++i];
    else av.emplace_back(argv[i]);
  }
  if ( !dry )   {
    av.emplace_back("-interactive");
    av.emplace_back("-plugin");
    av.emplace_back("DD4hep_GeometryWebDisplay");
    if ( !opt.empty()      ) av.emplace_back("-opt"),      av.emplace_back(opt.c_str());
    if ( !level.empty()    ) av.emplace_back("-level"),    av.emplace_back(level.c_str());
    if ( !visopt.empty()   ) av.emplace_back("-visopt"),   av.emplace_back(visopt.c_str());
    if ( !detector.empty() ) av.emplace_back("-detector"), av.emplace_back(detector.c_str());
  }
  return dd4hep::execute::main_plugins("DD4hep_GeometryWebDisplay", av.size(), (char**)&av[0]);
}
