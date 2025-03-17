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
/* 
 Plugin invocation:
 ==================
 This plugin behaves like a main program.
 Invoke the plugin with something like this:

 geoPluginRun -destroy -plugin DD4hep_TestConstantsMultiplier -opt [-opt]

*/
// Framework include files
#include "DD4hep/Printout.h"
#include "DD4hep/Factories.h"
#include "DD4hep/Detector.h"
#include "DD4hep/DetectorTools.h"
#include <fstream>
#include <cerrno>

using namespace dd4hep;

static int  print_segmentation(Detector& detector, int argc, char** argv)  {
  std::string placement_path;
  printout(ALWAYS, "PrintSegments", "+++ ------------------------------------------------------------------");
  for( int i=0; i<argc; ++i )  {
    printout(ALWAYS, "PrintSegments", "+++ Argument: %s", argv[i]);
    placement_path = argv[i];
  }
  if( placement_path.empty() )  {
    printout(ALWAYS, "PrintSegments", "+++ No sensitive placed volume name given.");
    return EINVAL;
  }
  PlacedVolume pv = detail::tools::findNode(detector.world().placement(), placement_path);
  if( !pv.isValid() )  {
    printout(ALWAYS, "PrintSegments", "+++ No sensitive placed volume with path %s found.", placement_path.c_str());
  }
  printout(ALWAYS, "PrintSegments", "+++ Got placement %s: %p", placement_path.c_str(), (void*)pv.ptr());
  /* Volume vol = */ pv.volume();
  
  return 1;
}
DECLARE_APPLY(DD4hep_PrintSegmentation,print_segmentation)
