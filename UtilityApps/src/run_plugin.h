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

// C/C++ include files
#include <iostream>
#include <cstdlib>
#include <vector>
#include <cerrno>
#include <string>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

//________________________________________________________________________________
#include "TRint.h"
//________________________________________________________________________________
namespace {

  //______________________________________________________________________________
  Detector& dd4hep_instance(const char* /* name */ ="") {
#if 0
#include "DD4hep/PluginService.h"
    try {
      union { void* p; Detector* l; } v;
      v.p = ::dd4hep::PluginService::Create<void*>("Detector_constructor",name);
      if ( v.p )  {
        return *v.l;
      }
      except("RunPlugin","++ Failed to locate plugin to create Detector instance");
    }
    catch(const exception& e)  {
      except("RunPlugin","++ Exception: %s", e.what());
    }
    catch(...)  {
      except("RunPlugin","++ UNKNOWN Exception while creating Detector instance.");
    }
    except("RunPlugin","++ UNKNOWN Exception while creating Detector instance.");
#endif
    return Detector::getInstance();
  }

  //______________________________________________________________________________
  long run_plugin(Detector& description, const char* name, int argc, char** argv) {
    try {
      description.apply(name,argc,argv);
      return 0;
    }
    catch(const exception& e)  {
      except("RunPlugin","++ Exception while executing plugin %s:\n\t\t%s",
             name ? name : "<unknown>", e.what());
    }
    catch(...)  {
      except("RunPlugin","++ UNKNOWN Exception while executing plugin %s.",name ? name : "<unknown>");
    }
    ::exit(EINVAL);
    return EINVAL;
  }

  //______________________________________________________________________________
  ostream& print_default_args()  {
    cout << 
      "        -build_type <number/string> Specify the build type                         \n"
      "                     [OPTIONAL]     MUST come immediately after the -compact input.\n"
      "                                    Default for each file is: BUILD_DEFAULT [=1]   \n"
      "                                    Allowed: BUILD_SIMU [=1], BUILD_RECO [=2],     \n"
      "                                    BUILD_DISPLAY [=3] or BUILD_ENVELOPE [=4]      \n"
      "        -destroy     [OPTIONAL]     Force destruction of the Detector instance         \n"
      "                                    before exiting the application                 \n"
      "        -volmgr      [OPTIONAL]     Load and populate phys.volume manager to       \n"
      "                                    check the volume ids for duplicates etc.       \n"
      "        -print      <number/string> Specify output level. Default: INFO(=3)        \n"
      "                     [OPTIONAL]     Allowed values: VERBOSE(=1), DEBUG(=2),        \n"
      "                                    INFO(=3), WARNING(=4), ERROR(=5), FATAL(=6)    \n"
      "                                    The lower the level, the more printout...      \n"
      "        -plugin <name> <args>       Execute plugin <name> after loading geometry.  \n"
      "                                    All arguments following until the next '-'     \n"
      "                                    are considered as arguments to the plugin.     \n"
      "        -ui                         Install ROOT interpreter UI for dd4hep         \n"
      "                                    Will show up in the global interpreter variable\n"
      "                                    'dd4hep::ROOTUI* gdd4hepUI' and allows the user\n"
      "                                    to interact with the the Detector instance from the\n"
      "                                    ROOT interactive prompt.                       \n";
    return cout;
  }

  //______________________________________________________________________________
  void usage_default(const char* name) {
    cout << " " << name << " -opt [-opt]                                                  \n"
      "        -compact       <file>       Specify the compact geometry file              \n"
      "                     [REQUIRED]     At least one compact geo file is required!     \n";
    print_default_args() <<
      "        -load_only   [OPTIONAL]     Dry-run to only load geometry without     \n"
      "                                    starting the dispay.                      \n"
                         << endl;
    exit(EINVAL);
  }

  //______________________________________________________________________________
  struct Args  {
    bool        volmgr, dry_run, destroy, interpreter, ui;
    int         print;
    vector<const char*> geo_files, build_types;
    vector<vector<const char*> > plugins;

    //____________________________________________________________________________
    Args() {
      ui = false;
      volmgr  = false;
      dry_run = false;
      destroy = false;
      interpreter = true;
      print   = dd4hep::INFO;
    }

    //____________________________________________________________________________
    int handle(int& i, int argc, char** argv)    {
      if ( strncmp(argv[i],"-compact",5)==0 || strncmp(argv[i],"-input",4)==0 )  {
        geo_files.push_back(argv[++i]);
        if ( argc>i+2 && strncmp(argv[i+1],"-build_type",6)==0 )  {
          build_types.push_back(argv[i+2]);
          i += 2;
        }
        else  {
          build_types.push_back("BUILD_DEFAULT");
        }
      }
      else if ( strncmp(argv[i],"-load_only",5)==0 )
        dry_run = true;
      else if ( strncmp(argv[i],"-dry-run",5)==0 )
        dry_run = true;
      else if ( strncmp(argv[i],"-print",4)==0 )
        dd4hep::setPrintLevel(dd4hep::PrintLevel(print = decodePrintLevel(argv[++i])));
      else if ( strncmp(argv[i],"-destroy",5)==0 )
        destroy = true;
      else if ( strncmp(argv[i],"-no-destroy",8)==0 )
        destroy = false;
      else if ( strncmp(argv[i],"-volmgr",4)==0 )
        volmgr = true;
      else if ( strncmp(argv[i],"-no-volmgr",7)==0 )
        volmgr = false;
      else if ( strncmp(argv[i],"-interpreter",6)==0 )
        interpreter = true;
      else if ( strncmp(argv[i],"-no-interpreter",7)==0 )
        interpreter = false;
      else if ( strncmp(argv[i],"-ui",3)==0 )
        ui = true;
      else if ( strncmp(argv[i],"-plugin",5)==0 )   {
        // Need to interprete plugin args here locally.....
        plugins.push_back(vector<const char*>());
        plugins.back().push_back(argv[++i]);
        for( ++i; i < argc; ++i )   {
          if ( strncmp(argv[i],"-plugin",5)==0 ) { --i; break; }
          if ( strncmp(argv[i],"-end-plugin",4)==0 )  { break; }
          plugins.back().push_back(argv[i]);
        }
        plugins.back().push_back(0);
      }
      else 
        return 0;
      return 1;
    }

    //____________________________________________________________________________
    long run(Detector& description, const char* name)  {
      pair<int, char**> a(0,0);
      long result;
      for(size_t i=0; i<plugins.size(); ++i)   {
        vector<const char*>& plug=plugins[i];
        // Remove plugin name and trailing 0x0 from args.
        size_t num_args = plug.size()>2 ? plug.size()-2 : 0;

        result = run_plugin(description,plug[0],plug.size()-1,(char**)(num_args>0 ? &plug[1] : 0));
        if ( result == EINVAL )   {
          cout << "FAILED to execute dd4hep plugin: '" << plug[0] 
               << "' with args (" << num_args << ") :[ ";
          for(size_t j=1; j<plug.size(); ++j)   {
            if ( plug[j] ) cout << plug[j] << " ";
          }
          cout << "]" << endl;
          usage_default(name);
        }
        cout << "Executed dd4hep plugin: '" << plug[0]
             << "' with args (" << num_args << ") :[ ";
        for(size_t j=1; j<plug.size(); ++j)   {
          if ( plug[j] ) cout << plug[j] << " ";
        }
        cout << "]" << endl;
      }
      result = run_plugin(description,name,a.first,a.second);
      return result;
    }

    //____________________________________________________________________________
    int decodePrintLevel(const string& val)   {
      switch(::toupper(val[0]))  {
      case '1':
      case 'V':
        return dd4hep::VERBOSE;
      case '2':
      case 'D':
        return dd4hep::DEBUG;
      case '3':
      case 'I':
        return dd4hep::INFO;
      case '4':
      case 'W':
        return dd4hep::WARNING;
      case '5':
      case 'E':
        return dd4hep::ERROR;
      case '6':
      case 'F':
        return dd4hep::FATAL;
      case '7':
      case 'A':
        return dd4hep::FATAL;
      default:
        cout << "Unknown print level supplied:'" << val << "'. Argument ignored." << endl;
        throw runtime_error("Invalid printLevel:"+val);
      }
    }
  };

  //______________________________________________________________________________
  void load_compact(Detector& description, Args& args)   {
    // Load all compact files
    for(size_t i=0; i<args.geo_files.size(); ++i)  {
      const char* argv[] = {args.geo_files[i], args.build_types[i], 0};
      run_plugin(description,"dd4hepCompactLoader",2,(char**)argv);
    }
  }

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
        args.geo_files.push_back(argv[i]);
        args.build_types.push_back("BUILD_DEFAULT");
      }
    }
    if ( args.geo_files.empty() )
      usage_default(name);

    Detector& description = dd4hep_instance();
    // Load all compact files
    load_compact(description, args);
    if ( args.ui ) run_plugin(description,"dd4hepInteractiveUI",0,0);
    // Create volume manager and populate it required
    if ( args.volmgr ) run_plugin(description,"dd4hepVolumeManager",0,0);

    // Create an interactive ROOT application
    if ( !args.dry_run ) {
      long result = 0;
      pair<int, char**> a(0,0);
      if ( args.interpreter )   {
        TRint app(name, &a.first, a.second);
        result = args.run(description,name);
        if ( result != EINVAL ) app.Run();
      }
      else
        result = args.run(description,name);
      if ( result == EINVAL ) usage_default(name);
    }
    else {
      cout << "The geometry was loaded. Application now exiting." << endl;
    }
    if ( args.destroy ) delete &description;
    return 0;
  }

  //______________________________________________________________________________
  int main_default(const char* name, int argc, char** argv)  {
    try {
      return main_wrapper(name,argc,argv);
    }
    catch(const exception& e)  {
      cout << "Got uncaught exception: " << e.what() << endl;
    }
    catch (...)  {
      cout << "Got UNKNOWN uncaught exception." << endl;
    }
    return EINVAL;    
  }
}
