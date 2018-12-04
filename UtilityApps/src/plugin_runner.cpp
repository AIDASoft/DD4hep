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
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

// Framework include files
#include "run_plugin.h"
#include <cerrno>

/// Main entry point as a program
int main(int argc, char** argv)   {
  try  {
    return dd4hep::execute::invoke_plugin_runner("", argc, argv);
  }
  catch(const std::exception& e)  {
    std::cout << "geoPluginRun: Got uncaught exception: " << e.what() << std::endl;
  }
  catch (...)  {
    std::cout << "geoPluginRun: Got UNKNOWN uncaught exception." << std::endl;
  }
  return EINVAL;    
}
