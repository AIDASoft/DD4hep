// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Generic ROOT based geometry display program
// 
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/LCDD.h"
#include "DD4hep/Printout.h"

// C/C++ include files
#include <iostream>
#include <cstdlib>
#include <vector>
#include <cerrno>
#include <string>

using namespace std;
using namespace DD4hep::Geometry;

//______________________________________________________________________________
#include "TRint.h"
//______________________________________________________________________________
namespace {

  LCDD& dd4hep_instance(const char* name="") {
#if 0
#include "Reflex/PluginService.h"
    try {
      union { void* p; LCDD* l; } v;
      v.p = ROOT::Reflex::PluginService::Create<void*>("LCDD_constructor",name);
      if ( v.p )  {
	return *v.l;
      }
      throw runtime_error("Failed to locate plugin to create LCDD instance");
    }
    catch(const exception& e)  {
      cout << "Exception:" << e.what() << endl;
      throw runtime_error("Exception:\""+string(e.what()));
    }
    catch(...)  {
      cout << "UNKNOWN Exception while creating LCDD instance." << endl;
    }
    throw runtime_error("UNKNOWN Exception while creating LCDD instance.");
#endif
    return LCDD::getInstance();
  }

  int run_plugin(LCDD& lcdd, const char* name, int argc, char** argv) {
    try {
      lcdd.apply(name,argc,argv);
      return 0;
    }
    catch(const exception& e)  {
      cout << e.what() << endl;
    }
    catch(...)  {
      cout << "UNKNOWN Exception" << endl;
    }
    ::exit(EINVAL);
    return EINVAL;
  }

  std::ostream& print_default_args()  {
    cout << 
      "        -build_type <number/string> Specify the build type                         \n"
      "                     [OPTIONAL]     MUST come immediately after the -compact input.\n"
      "                                    Default for each file is: BUILD_DEFAULT [=1]   \n"
      "                                    Allowed values: BUILD_SIMU [=1], BUILD_RECO [=2] or BUILD_DISPLAY [=3]\n"
      "        -destroy     [OPTIONAL]     Force destruction of the LCDD instance         \n"
      "                                    before exiting the application                 \n"
      "        -volmgr      [OPTIONAL]     Load and populate phys.volume manager to       \n"
      "                                    check the volume ids for duplicates etc.       \n"
      "        -print      <number/string> Specify output level. Default: INFO(=3)        \n"
      "                     [OPTIONAL]     Allowed values: VERBOSE(=1), DEBUG(=2),        \n"
      "                                    INFO(=3), WARNING(=4), ERROR(=5), FATAL(=6)    \n"
      "                                    The lover the level, the more printout...      \n";
    return cout;
  }

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

  struct Args  {
    bool        volmgr, dry_run, destroy;
    int         print;
    std::vector<const char*> geo_files, build_types;
    
    Args() {
      volmgr  = false;
      dry_run = false;
      destroy = false;
      print   = DD4hep::INFO;
    }
    int handle(int& i, int argc, char** argv)    {
      if ( strncmp(argv[i],"-compact",2)==0 || strncmp(argv[i],"-input",2)==0 )  {
	geo_files.push_back(argv[++i]);
	if ( argc>i+2 && strncmp(argv[i+1],"-build_type",6)==0 )  {
	  build_types.push_back(argv[i+2]);
	  i += 2;
	}
	else  {
	  build_types.push_back("BUILD_DEFAULT");
	}
      }
      else if ( strncmp(argv[i],"-load_only",2)==0 )
	dry_run = true;
      else if ( strncmp(argv[i],"-print",2)==0 )
	DD4hep::setPrintLevel(DD4hep::PrintLevel(print = decodePrintLevel(argv[++i])));
      else if ( strncmp(argv[i],"-destroy",2)==0 )
	destroy = true;
      else if ( strncmp(argv[i],"-volmgr",2)==0 )
	volmgr = true;
      else 
	return 0;
      return 1;
    }
    int decodePrintLevel(const std::string& val)   {
      switch(::toupper(val[0]))  {
      case '1':
      case 'V':
	return DD4hep::VERBOSE;
      case '2':
      case 'D':
	return DD4hep::DEBUG;
      case '3':
      case 'I':
	return DD4hep::INFO;
      case '4':
      case 'W':
	return DD4hep::WARNING;
      case '5':
      case 'E':
	return DD4hep::ERROR;
      case '6':
      case 'F':
	return DD4hep::FATAL;
      case '7':
      case 'A':
	return DD4hep::FATAL;
      default:
	cout << "Unknown print level supplied:'" << val << "'. Argument ignored." << endl;
	throw std::runtime_error("Invalid printLevel:"+val);
      }
    }
  };

  void load_compact(LCDD& lcdd, Args& args)   {
    // Load all compact files
    for(size_t i=0; i<args.geo_files.size(); ++i)  {
      const char* argv[] = {args.geo_files[i], args.build_types[i], 0};
      run_plugin(lcdd,"DD4hepCompactLoader",2,(char**)argv);
    }
  }
  //______________________________________________________________________________
  int main_default(const char* name, int argc,char** argv)  {
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

    LCDD& lcdd = dd4hep_instance();
    // Load all compact files
    load_compact(lcdd, args);
    // Create volume manager and populate it required
    if ( args.volmgr  ) run_plugin(lcdd,"DD4hepVolumeManager",0,0);

    // Create an interactive ROOT application
    if ( !args.dry_run ) {
      pair<int, char**> a(0,0);
      TRint app(name, &a.first, a.second);
      run_plugin(lcdd,name,a.first,a.second);
      app.Run();
    }
    else {
      cout << "The geometry was loaded. Application now exiting." << endl;
    }
    if ( args.destroy ) delete &lcdd;
    return 0;
  }
}
