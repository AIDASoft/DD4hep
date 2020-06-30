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
#include "DD4hep/Detector.h"
#include "DD4hep/Printout.h"
#include "TTimeStamp.h"

// C/C++ include files
#include <iostream>
#include <cstdlib>
#include <vector>
#include <cerrno>
#include <string>
#include <memory>

//________________________________________________________________________________
#include "TRint.h"
//________________________________________________________________________________
namespace {

  //______________________________________________________________________________
  dd4hep::Detector& dd4hep_instance(const char* /* name */ ="") {
#if 0
#include "DD4hep/PluginService.h"
    try {
      union { void* p; Detector* l; } v;
      v.p = ::dd4hep::PluginService::Create<void*>("Detector_constructor",name);
      if ( v.p )  {
        return *v.l;
      }
      dd4hep::except("RunPlugin","++ Failed to locate plugin to create Detector instance");
    }
    catch(const std::exception& e)  {
      dd4hep::except("RunPlugin","++ Exception: %s", e.what());
    }
    catch(...)  {
      dd4hep::except("RunPlugin","++ UNKNOWN Exception while creating Detector instance.");
    }
    dd4hep::except("RunPlugin","++ UNKNOWN Exception while creating Detector instance.");
#endif
    return dd4hep::Detector::getInstance();
  }

  //______________________________________________________________________________
  long run_plugin(dd4hep::Detector& description, const char* name, int argc, char** argv) {
    try {
      description.apply(name,argc,argv);
      return 0;
    }
    catch(const std::exception& e)  {
      dd4hep::except("RunPlugin","++ Exception while executing plugin %s:\n\t\t%s",
                     name ? name : "<unknown>", e.what());
    }
    catch(...)  {
      dd4hep::except("RunPlugin","++ UNKNOWN Exception while executing plugin %s.",name ? name : "<unknown>");
    }
    ::exit(EINVAL);
    return EINVAL;
  }

  //______________________________________________________________________________
  std::ostream& print_default_args()  {
    std::cout << 
      "        -build_type <number/string> Specify the build type                         \n"
      "                        [OPTIONAL]  MUST come immediately after the -compact input.\n"
      "                                    Default for each file is: BUILD_DEFAULT [=1]   \n"
      "                                    Allowed: BUILD_SIMU [=1], BUILD_RECO [=2],     \n"
      "                                    BUILD_DISPLAY [=3] or BUILD_ENVELOPE [=4]      \n"
      "        -destroy        [OPTIONAL]  Force destruction of the Detector instance     \n"
      "                                    before exiting the application                 \n"
      "        -no-destroy     [OPTIONAL]  Inhibit destruction of the Detector instance   \n"
      "        -volmgr         [OPTIONAL]  Load and populate phys.volume manager to       \n"
      "                                    check the volume ids for duplicates etc.       \n"
      "        -no-volmgr      [OPTIONAL]  Inhibit loading phys.volume manager            \n"
      "        -interpreter    [OPTIONAL]  Start ROOT C++ interpreter after execution.    \n"
      "        -interactive    [OPTIONAL]  Alias for -interpreter argument.               \n"
      "        -no-interpreter [OPTIONAL]  Inhibit ROOT C++ interpreter.                  \n"
      "        -print     <number/string>  Specify output level. Default: INFO(=3)        \n"
      "                        [OPTIONAL]  Allowed values: VERBOSE(=1), DEBUG(=2),        \n"
      "                                    INFO(=3), WARNING(=4), ERROR(=5), FATAL(=6)    \n"
      "                                    The lower the level, the more printout...      \n"
      "        -dry-run        [OPTIONAL]  Only load description. No execution.           \n"
      "        -ui             [OPTIONAL]  Install ROOT interpreter UI for dd4hep         \n"
      "                                    Will show up in the global interpreter variable\n"
      "                                    'dd4hep::ROOTUI* gdd4hepUI' and allows the user\n"
      "                                    to interact with the the Detector instance from the\n"
      "                                    ROOT interactive prompt.                       \n"
      "        -plugin <name> <args>       Execute plugin <name> after loading geometry.  \n"
      "                                    All arguments following until the next         \n"
      "                                    '-plugin' tag are considered as arguments      \n"
      "                                    to the current plugin.                         \n"
      " ";
    return std::cout;
  }

  //______________________________________________________________________________
  void usage_default(const char* name) {
    std::cout << " " << name << " -opt [-opt]                                             \n"
      "        -compact       <file>       Specify the compact geometry file              \n"
      "                     [REQUIRED]     At least one compact geo file is required!     \n";
    print_default_args() <<
      "        -load_only   [OPTIONAL]     Dry-run to only load geometry without          \n"
      "                                    starting the dispay.                           \n"
                         << std::endl;
    std::exit(EINVAL);
  }

  //______________________________________________________________________________
  struct Args  {
    bool        volmgr, dry_run, destroy, interpreter, ui;
    dd4hep::PrintLevel  print;
    std::vector<const char*> geo_files, build_types;
    std::vector<std::vector<const char*> > plugins;

    //____________________________________________________________________________
    Args() {
      ui          = false;
      volmgr      = false;
      dry_run     = false;
      destroy     = false;
      interpreter = true;
      print       = dd4hep::INFO;
    }

    //____________________________________________________________________________
    int handle(int& i, int argc, char** argv)    {
      if ( ::strncmp(argv[i],"-compact",5)==0 || ::strncmp(argv[i],"-input",4)==0 )  {
        geo_files.emplace_back(argv[++i]);
        if ( argc>i+2 && ::strncmp(argv[i+1],"-build_type",6)==0 )  {
          build_types.emplace_back(argv[i+2]);
          i += 2;
        }
        else  {
          build_types.emplace_back("BUILD_DEFAULT");
        }
      }
      else if ( ::strncmp(argv[i],"-load_only",5)==0 )
        dry_run = true;
      else if ( ::strncmp(argv[i],"-dry-run",5)==0 )
        dry_run = true;
      else if ( ::strncmp(argv[i],"-print",4)==0 )
        dd4hep::setPrintLevel(dd4hep::PrintLevel(print = dd4hep::decodePrintLevel(argv[++i])));
      else if ( ::strncmp(argv[i],"-destroy",5)==0 )
        destroy = true;
      else if ( ::strncmp(argv[i],"-no-destroy",8)==0 )
        destroy = false;
      else if ( ::strncmp(argv[i],"-volmgr",4)==0 )
        volmgr = true;
      else if ( ::strncmp(argv[i],"-no-volmgr",7)==0 )
        volmgr = false;
      else if ( ::strncmp(argv[i],"-interpreter",6)==0 )
        interpreter = true;
      else if ( ::strncmp(argv[i],"-no-interpreter",7)==0 )
        interpreter = false;
      else if ( ::strncmp(argv[i],"-interactive",6)==0 )
        interpreter = true;
      else if ( ::strncmp(argv[i],"-no-interactive",7)==0 )
        interpreter = false;
      else if ( ::strncmp(argv[i],"-ui",3)==0 )
        ui = true;
      else if ( ::strncmp(argv[i],"-no-ui",6)==0 )
        ui = false;
      else if ( ::strncmp(argv[i],"-plugin",5)==0 )   {
        // Need to interprete plugin args here locally.....
        plugins.emplace_back(std::vector<const char*>());
        plugins.back().emplace_back(argv[++i]);
        for( ++i; i < argc; ++i )   {
          if ( ::strncmp(argv[i],"-plugin",5)==0 ) { --i; break; }
          if ( ::strncmp(argv[i],"-end-plugin",4)==0 )  { break; }
          plugins.back().emplace_back(argv[i]);
        }
        plugins.back().emplace_back(nullptr);
      }
      else 
        return 0;
      return 1;
    }

    //____________________________________________________________________________
    long run(dd4hep::Detector& description, const char* name)  {
      std::pair<int, char**> a(0,0);
      long result;
      for(size_t i=0; i<plugins.size(); ++i)   {
        std::vector<const char*>& plug = plugins[i];
        // Remove plugin name and trailing 0x0 from args.
        size_t num_args = plug.size()>2 ? plug.size()-2 : 0;
        TTimeStamp start;
        char text[32];
        result = run_plugin(description,plug[0],plug.size()-1,(char**)(num_args>0 ? &plug[1] : 0));
        TTimeStamp stop;
        ::snprintf(text,sizeof(text),"[%8.3f sec]",stop.AsDouble()-start.AsDouble());
        if ( result == EINVAL )   {
          std::cout << "FAILED to execute dd4hep plugin: '" << plug[0] 
                    << "' with args (" << num_args << ") :[ ";
          for(size_t j = 1; j < plug.size(); ++j)   {
            if ( plug[j] ) std::cout << plug[j] << " ";
          }
          std::cout << "]" << std::endl;
          usage_default(name);
        }
        std::cout << "run_plugin: " << text << " Executed dd4hep plugin: '" << plug[0]
                  << "' with args (" << num_args << ") :[ ";
        for(size_t j=1; j<plug.size(); ++j)   {
          if ( plug[j] ) std::cout << plug[j] << " ";
        }
        std::cout << "]" << std::endl << std::flush;
      }
      result = run_plugin(description,name,a.first,a.second);
      return result;
    }
  };

  //______________________________________________________________________________
  void load_compact(dd4hep::Detector& description, Args& args)   {
    // Load all compact files
    for(size_t i=0; i<args.geo_files.size(); ++i)  {
      const char* argv[] = {args.geo_files[i], args.build_types[i], 0};
      run_plugin(description,"DD4hep_CompactLoader",2,(char**)argv);
    }
  }
}


namespace dd4hep  {
  namespace execute  {

    //______________________________________________________________________________
    int main_wrapper(const char* name, int argc, char** argv)  {
      Args args;
      for(int i=1; i<argc;++i) {
        if ( argv[i][0]=='-' ) {
          if ( args.handle(i,argc,argv) )
            continue;
          else
            usage_default(name);
        }
        else {  // This is the default
          args.geo_files.emplace_back(argv[i]);
          args.build_types.emplace_back("BUILD_DEFAULT");
        }
      }
      if ( args.geo_files.empty() )
        usage_default(name);

      dd4hep::Detector& description = dd4hep_instance();
      // Load all compact files
      load_compact(description, args);
      if ( args.ui ) run_plugin(description,"DD4hep_InteractiveUI",0,0);
      // Create volume manager and populate it required
      if ( args.volmgr ) run_plugin(description,"DD4hep_VolumeManager",0,0);

      // Create an interactive ROOT application
      if ( !args.dry_run ) {
        long result = 0;
        std::pair<int, char**> a(0,0);
        if ( args.interpreter )   {
          TRint app(name, &a.first, a.second);
          result = args.run(description, name);
          if ( result != EINVAL ) app.Run();
        }
        else
          result = args.run(description, name);
        if ( result == EINVAL ) usage_default(name);
      }
      else {
        std::cout << "The geometry was loaded. Application now exiting." << std::endl;
      }
      try   {
        if ( args.destroy ) description.destroyInstance();
      }
      catch(const std::exception& e)  {
        std::cout << "destroyInstance: Got uncaught exception: " << e.what() << std::endl;
      }
      catch (...)  {
        std::cout << "destroyInstance: Got UNKNOWN uncaught exception." << std::endl;
      }
      return 0;
    }

    void usage_plugin_runner() {
      std::cout <<
        "geoPluginRun -opt [-opt]                                                \n"
        "        -input  <file>  [OPTIONAL]  Specify geometry input file.        \n"
        "        -plugin <name>  <args> [args] [-end-plugin]                     \n"
        "                        [REQUIRED]  Plugin to be executed and applied.  \n"
        "        -plugin <name>  <args> [args] -end-plugin                       \n"
        "                        [OPTIONAL]  Next plugin with arguments.         \n";
      print_default_args() << std::endl;
      ::exit(EINVAL);
    }

    //______________________________________________________________________________
    int invoke_plugin_runner(const char* name, int argc,char** argv)  {
      Args arguments;
      arguments.interpreter = false;
    
      for(int i=1; i<argc;++i) {
        if ( argv[i][0]=='-' ) {
          if ( arguments.handle(i,argc,argv) )
            continue;
        }
        else {
          usage_plugin_runner();
        }
      }
      if ( !arguments.dry_run &&
           !arguments.ui      &&
           !arguments.interpreter &&
           arguments.plugins.empty() )
      {
        usage_plugin_runner();
      }
      std::unique_ptr<TRint> interpreter;
      dd4hep::Detector& description = dd4hep_instance();
      // Load compact files if required by plugin
      if ( !arguments.geo_files.empty() )   {
        load_compact(description, arguments);
      }
      else  {
        std::cout << "geoPluginRun: No geometry input supplied. "
                  << "No geometry will be loaded." << std::endl;
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
        std::pair<int, char**> a(0,0);
        interpreter.reset(new TRint("geoPluginRun", &a.first, a.second));
      }
      // Execute plugin
      for(size_t i=0; i<arguments.plugins.size(); ++i)   {
        std::vector<const char*>& plug = arguments.plugins[i];
        int num_args = int(plug.size())-2;
        TTimeStamp start;
        char text[32];
        long result = run_plugin(description,plug[0], num_args,(char**)&plug[1]);
        TTimeStamp stop;
        ::snprintf(text,sizeof(text),"[%8.3f sec]",stop.AsDouble()-start.AsDouble());
        if ( result == EINVAL )   {
          std::cout << "geoPluginRun: FAILED to execute dd4hep plugin: '" << plug[0] 
                    << "' with args (" << num_args << ") :[ ";
          for(size_t j = 1; j < plug.size(); ++j)   {
            if ( plug[j] ) std::cout << plug[j] << " ";
          }
          std::cout << "]" << std::endl;
          usage_plugin_runner();
        }
        std::cout << "geoPluginRun: " << text <<" Executed dd4hep plugin: '" << plug[0]
                  << "' with args (" << num_args << ") :[ ";
        for(size_t j=1; j<plug.size(); ++j)   {
          if ( plug[j] ) std::cout << plug[j] << " ";
        }
        std::cout << "]" << std::endl << std::flush;
      }
      if ( arguments.plugins.empty() )    {
        // Create an interactive ROOT application
        if ( !arguments.dry_run ) {
          long result = 0;
          std::pair<int, char**> a(0,0);
          if ( arguments.interpreter )   {
            TRint app(name, &a.first, a.second);
            result = arguments.run(description, name);
            if ( result != EINVAL ) app.Run();
          }
          else
            result = arguments.run(description, name);
          if ( result == EINVAL ) usage_default(name);
        }
        else {
          std::cout << "The geometry was loaded. Application now exiting." << std::endl;
        }
      }
      if ( !arguments.dry_run && interpreter.get() )  {
        interpreter->Run();
      }
      if ( arguments.destroy ) description.destroyInstance();
      return 0;
    }

    /// Main entry point as a program
    int main_plugins(const char* name, int argc, char** argv)   {
      try  {
        return invoke_plugin_runner(name, argc, argv);
      }
      catch(const std::exception& e)  {
        std::cout << "geoPluginRun: Got uncaught exception: " << e.what() << std::endl;
      }
      catch (...)  {
        std::cout << "geoPluginRun: Got UNKNOWN uncaught exception." << std::endl;
      }
      return EINVAL;    
    }

    //______________________________________________________________________________
    int main_default(const char* name, int argc, char** argv)  {
      try {
        return main_wrapper(name,argc,argv);
      }
      catch(const std::exception& e)  {
        std::cout << "Got uncaught exception: " << e.what() << std::endl;
      }
      catch (...)  {
        std::cout << "Got UNKNOWN uncaught exception." << std::endl;
      }
      return EINVAL;    
    }
  }
}
