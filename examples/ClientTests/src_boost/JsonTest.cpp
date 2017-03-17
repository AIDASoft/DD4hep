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
#ifdef DD4HEP_USE_BOOST

// Framework include files
#include "JSON/Elements.h"
#include "JSON/DocumentHandler.h"
#include "DD4hep/Factories.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::JSON;
using namespace DD4hep::Geometry;

static long json_dump(LCDD& /* lcdd */, int argc, char** argv)   {
  if ( argc < 1 )  {
    ::printf("DD4hep_JsonDumper <file>                               \n");
    exit(EINVAL);
  }
  DocumentHolder doc(JSON::DocumentHandler().load(argv[0]));
  dumpTree(doc.root());
  return 1;
}

DECLARE_APPLY(DD4hep_JsonDumper,json_dump)

#endif
