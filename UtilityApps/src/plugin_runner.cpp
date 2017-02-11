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
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

// Framework include files
#include "run_plugin.h"

//______________________________________________________________________________
namespace {
  void usage() {
    cout << "geoPluginRun -opt [-opt]                                                \n"
      "        -input  <file>  [OPTIONAL]  Specify geometry input file.              \n"
      "        -plugin <name>  <args> [args]                                         \n"
      "                        [REQUIRED]  Plugin to be executed and applied.        \n"
      "        -plugin <name>  <args> [args]                                         \n"
      "                        [OPTIONAL]  Next plugin with arguments.               \n";
    print_default_args() << endl;
    exit(EINVAL);
  }

  //______________________________________________________________________________
  int invoke_plugin_runner(int argc,char** argv)  {
    Args arguments;
    for(int i=1; i<argc;++i) {
      if ( argv[i][0]=='-' ) {
        if ( arguments.handle(i,argc,argv) )
          continue;
      }
      else {
        usage();
      }
    }
    if ( arguments.plugins.empty() )
      usage();

    LCDD& lcdd = dd4hep_instance();
    // Load compact files if required by plugin
    if ( !arguments.geo_files.empty() )   {
      load_compact(lcdd, arguments);
    }
    else  {
      cout << "geoPluginRun: No geometry input supplied. No geometry will be loaded." << endl;
    }
    // Create volume manager and populate it required
    if ( arguments.volmgr  ) run_plugin(lcdd,"DD4hepVolumeManager",0,0);
    // Execute plugin
    for(size_t i=0; i<arguments.plugins.size(); ++i)   {
      std::vector<const char*>& plug = arguments.plugins[i];
      int num_arg = int(plug.size())-2;
      run_plugin(lcdd,plug[0], num_arg,(char**)&plug[1]);
    }
    if ( arguments.destroy ) delete &lcdd;
    return 0;
  }
}


/// Main entry point as a program
int main(int argc, char** argv)   {
  try  {
    return invoke_plugin_runner(argc, argv);
  }
  catch(const std::exception& e)  {
    std::cout << "Got uncaught exception: " << e.what() << std::endl;
  }
  catch (...)  {
    std::cout << "Got UNKNOWN uncaught exception." << std::endl;
  }
  return EINVAL;    
}
