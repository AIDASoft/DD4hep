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
#include <memory>

using namespace std;

//______________________________________________________________________________
namespace {
  void usage() {
    cout <<
      "geoPluginRun -opt [-opt]                                                \n"
      "        -input  <file>  [OPTIONAL]  Specify geometry input file.        \n"
      "        -plugin <name>  <args> [args] [-end-plugin]                     \n"
      "                        [REQUIRED]  Plugin to be executed and applied.  \n"
      "        -plugin <name>  <args> [args] -end-plugin                       \n"
      "                        [OPTIONAL]  Next plugin with arguments.         \n";
    print_default_args() << endl;
    ::exit(EINVAL);
  }

  //______________________________________________________________________________
  int invoke_plugin_runner(int argc,char** argv)  {
    Args arguments;
    arguments.interpreter = false;
    
    for(int i=1; i<argc;++i) {
      if ( argv[i][0]=='-' ) {
        if ( arguments.handle(i,argc,argv) )
          continue;
      }
      else {
        usage();
      }
    }
    if ( !arguments.ui && !arguments.interpreter && arguments.plugins.empty() )  {
      usage();
    }
    unique_ptr<TRint> interpreter;
    dd4hep::Detector& description = dd4hep_instance();
    // Load compact files if required by plugin
    if ( !arguments.geo_files.empty() )   {
      load_compact(description, arguments);
    }
    else  {
      cout << "geoPluginRun: No geometry input supplied. "
           << "No geometry will be loaded." << endl;
    }
    // Attach UI instance if requested to ease interaction from the ROOT prompt
    if ( arguments.ui )  {
      run_plugin(description,"DD4hep_InteractiveUI",0,0);
    }
    // Create volume manager and populate it required
    if ( arguments.volmgr  )   {
      run_plugin(description,"DD4hep_VolumeManager",0,0);
    }
    if ( arguments.interpreter )  {
      pair<int, char**> a(0,0);
      interpreter.reset(new TRint("geoPluginRun", &a.first, a.second));
    }
    // Execute plugin
    for(size_t i=0; i<arguments.plugins.size(); ++i)   {
      vector<const char*>& plug = arguments.plugins[i];
      int num_args = int(plug.size())-2;
      long result = run_plugin(description,plug[0], num_args,(char**)&plug[1]);
      if ( result == EINVAL )   {
        cout << "geoPluginRun: FAILED to execute dd4hep plugin: '" << plug[0] 
                  << "' with args (" << num_args << ") :[ ";
        for(size_t j = 1; j < plug.size(); ++j)   {
          if ( plug[j] ) cout << plug[j] << " ";
        }
        cout << "]" << endl;
        usage();
      }
      cout << "geoPluginRun: Executed dd4hep plugin: '" << plug[0]
                << "' with args (" << num_args << ") :[ ";
      for(size_t j=1; j<plug.size(); ++j)   {
        if ( plug[j] ) cout << plug[j] << " ";
      }
      cout << "]" << endl << flush;
    }
    if ( interpreter )  {
      interpreter->Run();
    }
    if ( arguments.destroy ) delete &description;
    return 0;
  }
}

/// Main entry point as a program
int main(int argc, char** argv)   {
  try  {
    return invoke_plugin_runner(argc, argv);
  }
  catch(const exception& e)  {
    cout << "geoPluginRun: Got uncaught exception: " << e.what() << endl;
  }
  catch (...)  {
    cout << "geoPluginRun: Got UNKNOWN uncaught exception." << endl;
  }
  return EINVAL;    
}
