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
      "        -input  <file>  [REQUIRED]  Specify input file.                       \n"
      "        -destroy        [OPTIONAL]  Force destruction of the LCDD instance    \n"
      "                                    before exiting the application            \n"
      "        -volmgr         [OPTIONAL]  Load and populate phys.volume manager to  \n"
      "                                    check the volume ids for duplicates etc.  \n"
	 << endl;
    exit(EINVAL);
  }
}

//______________________________________________________________________________
int main(int argc,char** argv)  {
  string plugin;
  bool volmgr = false;
  bool destroy = false;
  vector<char*> geo_files;
  for(int i=1; i<argc;++i) {
    if ( argv[i][0]=='-' ) {
      if ( strncmp(argv[i],"-input",2)==0 )
	geo_files.push_back(argv[++i]);
      else if ( strncmp(argv[i],"-plugin",2)==0 )
        plugin = argv[++i];
      else if ( strncmp(argv[i],"-destroy",2)==0 )
        destroy = true;
      else if ( strncmp(argv[i],"-volmgr",2)==0 )
        volmgr = true;
      else
	usage();
    }
    else {
      usage();
    }
  }
  if ( geo_files.empty() || plugin.empty() )
    usage();

  LCDD& lcdd = dd4hep_instance();
  // Load compact files
  run_plugin(lcdd,"DD4hepCompactLoader",int(geo_files.size()),&geo_files[0]);
  // Execute plugin
  run_plugin(lcdd,plugin.c_str(),0,0);
  if ( volmgr  ) run_plugin(lcdd,"DD4hepVolumeManager",0,0);
  if ( destroy ) delete &lcdd;
  return 0;
}
