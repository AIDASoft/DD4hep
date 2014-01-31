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

  void usage_default(const char* name) {
    cout << " " << name << " -opt [-opt]                                             \n"
      "        -compact       <file>       Specify the compact geometry file         \n"
      "                     [REQUIRED]     At least one compact geo file is required!\n"
      "        -load_only   [OPTIONAL]     Dry-run to only load geometry without     \n"
      "                                    starting the dispay.                      \n"
      "        -destroy     [OPTIONAL]     Force destruction of the LCDD instance    \n"
      "                                    before exiting the application            \n"
      "        -volmgr      [OPTIONAL]     Load and populate phys.volume manager to  \n"
      "                                    check the volume ids for duplicates etc.  \n"
	 << endl;
    exit(EINVAL);
  }

  //______________________________________________________________________________
  int main_default(const char* name, int argc,char** argv)  {
    bool volmgr = false;
    bool dry_run = false, destroy = false;
    vector<char*> geo_files;
    for(int i=1; i<argc;++i) {
      if ( argv[i][0]=='-' ) {
	if ( strncmp(argv[i],"-compact",2)==0 )
	  geo_files.push_back(argv[++i]);
	else if ( strncmp(argv[i],"-load_only",2)==0 )
	  dry_run = true;
	else if ( strncmp(argv[i],"-destroy",2)==0 )
	  destroy = true;
	else if ( strncmp(argv[i],"-volmgr",2)==0 )
	  volmgr = true;
	else
	  usage_default(name);
      }
      else {  // This is the default
	geo_files.push_back(argv[i]);
      }
    }
    if ( geo_files.empty() )
      usage_default(name);

    LCDD& lcdd = dd4hep_instance();
    // Load all compact files
    run_plugin(lcdd,"DD4hepCompactLoader",int(geo_files.size()),&geo_files[0]);
    // Create volume manager and populate it required
    if ( volmgr  ) run_plugin(lcdd,"DD4hepVolumeManager",0,0);

    // Create an interactive ROOT application
    if ( !dry_run ) {
      pair<int, char**> args(0,0);
      TRint app(name, &args.first, args.second);
      run_plugin(lcdd,name,args.first,args.second);
      app.Run();
    }
    else {
      cout << "The geometry was loaded. Application now exiting." << endl;
    }
    if ( destroy ) delete &lcdd;
    return 0;
  }
}
