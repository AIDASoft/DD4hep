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
//
// DDDB is a detector description convention developed by the LHCb experiment.
// For further information concerning the DTD, please see:
// http://lhcb-comp.web.cern.ch/lhcb-comp/Frameworks/DetDesc/Documents/lhcbDtd.pdf
//
//==========================================================================

// Framework includes
#include "DD4hep/Detector.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Factories.h"
#include "DDDB/DDDBConditionsLoader.h"

using namespace dd4hep;

//==========================================================================
/// Plugin function
static long dddb_dump_conditions(Detector& description, int argc, char** argv) {
  const char* prt_level = "INFO";
  for(int i=0; i<argc; ++i)  {
    if ( ::strcmp(argv[i],"--print")==0 )  {
      prt_level = argv[++i];
      printout(INFO,"DDDB","Setting conditions print level to %s",prt_level);
    }
  }
  const void* args[] = { "-processor", "dd4hep_ConditionsPrinter",
                         "-name", "DDDB",
                         "-prefix", "DDDB",
                         "-print", prt_level,
                         "-end-processor", 0};
  description.apply("dd4hep_ConditionsDump", 9, (char**)args);
  return 1;
}
DECLARE_APPLY(DDDB_ConditionsDump,dddb_dump_conditions)

//==========================================================================
/// Plugin function
static long dddb_dump_conditions_summary(Detector& description, int , char** ) {
  const void* args[] = { "-processor", "dd4hep_ConditionsPrinter",
                         "-name", "DDDB",
                         "-prefix", "DDDB",
                         "-print", "DEBUG",
                         "-end-processor", 0};
  description.apply("dd4hep_ConditionsDump", 9, (char**)args);
  return 1;
}
DECLARE_APPLY(DDDB_ConditionsSummary,dddb_dump_conditions_summary)

//==========================================================================
/// Plugin function
static void* create_dddb_loader(Detector& description, int argc, char** argv)   {
  const char* name = argc>0 ? argv[0] : "DDDBLoader";
  cond::ConditionsManagerObject* m = (cond::ConditionsManagerObject*)(argc>0 ? argv[1] : 0);
  return new DDDB::DDDBConditionsLoader(description,m,name);
}
DECLARE_Detector_CONSTRUCTOR(dd4hep_Conditions_dddb_Loader,create_dddb_loader)
