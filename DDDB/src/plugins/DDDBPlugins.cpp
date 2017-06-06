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
//
// DDDB is a detector description convention developed by the LHCb experiment.
// For further information concerning the DTD, please see:
// http://lhcb-comp.web.cern.ch/lhcb-comp/Frameworks/DetDesc/Documents/lhcbDtd.pdf
//
//==========================================================================

// Framework includes
#include "DD4hep/LCDD.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Factories.h"
#include "DD4hep/ConditionsProcessor.h"
#include "DDDB/DDDBConditionPrinter.h"
#include "DDDB/DDDBConditionsLoader.h"
#include "DD4hep/PluginTester.h"
using namespace DD4hep;

//==========================================================================
/// Plugin function
static long dddb_dump_conditions(Geometry::LCDD& lcdd, int argc, char** argv) {
  const char* prt_level = "INFO";
  for(int i=0; i<argc; ++i)  {
    if ( ::strcmp(argv[i],"--print")==0 )  {
      prt_level = argv[++i];
      printout(INFO,"DDDB","Setting conditions print level to %s",prt_level);
    }
  }
  const void* args[] = { "-processor", "DDDB_ConditionsPrinter", "-prefix", "DDDB", "-printlevel", prt_level, "-end-processor", 0};
  lcdd.apply("DD4hep_ConditionsDump", 7, (char**)args);
  return 1;
}
DECLARE_APPLY(DDDB_ConditionsDump,dddb_dump_conditions)

//==========================================================================
/// Plugin function
static long dddb_dump_conditions_summary(Geometry::LCDD& lcdd, int , char** ) {
  const void* args[] = { "-processor", "DDDB_ConditionsPrinter", "-prefix", "DDDB", "-printlevel", "DEBUG", "-end-processor", 0};
  lcdd.apply("DD4hep_ConditionsDump", 7, (char**)args);
  return 1;
}
DECLARE_APPLY(DDDB_ConditionsSummary,dddb_dump_conditions_summary)

//==========================================================================
/// Plugin function
static void* create_dddb_conditions_printer(Geometry::LCDD& lcdd, int argc, char** argv)  {
  using namespace Conditions;
  typedef ConditionsProcessorWrapper<DDDB::ConditionPrinter> Wrapper;
  int        flags = 0, have_pool = 0;
  string     prefix = "";
  PrintLevel prtLevel = INFO;
  for(int i=0; i<argc && argv[i]; ++i)  {
    if ( 0 == ::strncmp("-prefix",argv[i],6) )
      prefix = argv[++i];
    else if ( 0 == ::strncmp("-flags",argv[i],6) )
      flags = ::atol(argv[++i]);
    else if ( 0 == ::strncmp("-pool",argv[i],5) )
      have_pool = 1;
    else if ( 0 == ::strncmp("-printlevel",argv[i],6) )
      prtLevel = DD4hep::printLevel(argv[++i]);
  }
  ConditionsSlice* slice = 0;
  if ( have_pool )   {
    PluginTester* test = lcdd.extension<PluginTester>();
    slice = test->extension<ConditionsSlice>("ConditionsTestSlice");
  }
  DDDB::ConditionPrinter* p(flags
                            ? new DDDB::ConditionPrinter(slice,prefix,flags)
                            : new DDDB::ConditionPrinter(slice,prefix));
  p->printLevel = prtLevel;
  Wrapper* wrap = createProcessorWrapper(p);
  return (void*)dynamic_cast<Condition::Processor*>(wrap);
}
DECLARE_LCDD_CONSTRUCTOR(DDDB_ConditionsPrinter,create_dddb_conditions_printer)

//==========================================================================
/// Plugin function
static void* create_dddb_loader(Geometry::LCDD& lcdd, int argc, char** argv)   {
  using Conditions::ConditionsManagerObject;
  const char* name = argc>0 ? argv[0] : "DDDBLoader";
  ConditionsManagerObject* m = (ConditionsManagerObject*)(argc>0 ? argv[1] : 0);
  return new DDDB::DDDBConditionsLoader(lcdd,m,name);
}
DECLARE_LCDD_CONSTRUCTOR(DD4hep_Conditions_dddb_Loader,create_dddb_loader)
