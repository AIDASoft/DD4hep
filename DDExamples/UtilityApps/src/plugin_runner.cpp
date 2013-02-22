// $Id:$
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
namespace {
  void usage() {
    cout << "geoPlugin -opt [-opt]                                                   \n"
      "        -plugin <name>  [REQUIRED]  Plugin to be executed and applied.        \n"
      "        -input  <file>  [REQUIRED]  Specify input file.                       \n"
	 << endl;
    exit(EINVAL);
  }
}

//______________________________________________________________________________
int main(int argc,char** argv)  {
  string plugin;
  vector<char*> geo_files;
  for(int i=1; i<argc;++i) {
    if ( argv[i][0]=='-' ) {
      if ( strncmp(argv[i],"-input",2)==0 )
	geo_files.push_back(argv[++i]);
      else if ( strncmp(argv[i],"-plugin",2)==0 )
        plugin = argv[++i];
      else
	usage();
    }
    else {
      usage();
    }
  }
  if ( geo_files.empty() || plugin.empty() )
    usage();

  try {
    LCDD& lcdd = LCDD::getInstance();  
    // Load compact files
    lcdd.apply("DD4hepCompactLoader",int(geo_files.size()),&geo_files[0]);
    // Execute plugin
    lcdd.apply(plugin.c_str(),0,0);
    return 0;
  }
  catch(const exception& e)  {
    cout << "Exception:" << e.what() << endl;
  }
  catch(...)  {
    cout << "UNKNOWN Exception" << endl;
  }
  return EINVAL;
}
