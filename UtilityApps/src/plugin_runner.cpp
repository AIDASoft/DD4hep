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
#include "run_plugin.h"

//______________________________________________________________________________
namespace {
  void usage() {
    cout << "geoPlugin -opt [-opt]                                                   \n"
      "        -plugin <name>  [REQUIRED]  Plugin to be executed and applied.        \n"
      "        -input  <file>  [REQUIRED]  Specify input file.                       \n";
    print_default_args() << endl;
    exit(EINVAL);
  }
}

//______________________________________________________________________________
int main(int argc,char** argv)  {
  char plugin_runner[64] = "plugin_runner";
  string plugin;
  Args arguments;
  vector<const char*> options;
  options.push_back(plugin_runner);
  for(int i=1; i<argc;++i) {
    if ( argv[i][0]=='-' ) {
      if ( arguments.handle(i,argc,argv) )
	continue;
      else if ( strncmp(argv[i],"-plugin",2)==0 )
        plugin = argv[++i];
      else
	options.push_back(argv[i]);
    }
    else {
      usage();
    }
  }
  if ( arguments.geo_files.empty() || plugin.empty() )
    usage();
  options.push_back(0);
  LCDD& lcdd = dd4hep_instance();
  // Load compact files
  load_compact(lcdd, arguments);
  // Create volume manager and populate it required
  if ( arguments.volmgr  ) run_plugin(lcdd,"DD4hepVolumeManager",0,0);
  // Execute plugin
  run_plugin(lcdd,plugin.c_str(),(int)(options.size()-1),(char**)&options[0]);
  if ( arguments.destroy ) delete &lcdd;
  return 0;
}
