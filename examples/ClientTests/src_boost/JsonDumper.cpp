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
#include "JSON/Elements.h"
#include "JSON/DocumentHandler.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Factories.h"

using namespace dd4hep;

static long json_dump(Detector& /* description */, int argc, char** argv)   {
  if ( argc < 1 )  {
    ::printf("DD4hep_JsonDumper <file>                               \n");
    exit(EINVAL);
  }
  std::string fname = argv[0];
  json::DocumentHolder doc(json::DocumentHandler().load(fname));
  dumpTree(doc.root());
  printout(INFO,"JsonDumper","+++ Successfully dumped json input: %s.",fname.c_str());
  return 1;
}
DECLARE_APPLY(DD4hep_JsonDumper,json_dump)
