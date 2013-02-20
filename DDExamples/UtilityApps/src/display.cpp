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
#include "TRint.h"

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
    cout << "<exe> -opt [-opt]                                                       \n"
      "        -compact       <file>       Specify the compact geometry file         \n"
      "                                    At least one compact geo file is required!\n"
      "        -load_only                  Dry-run to only load geometry without     \n"
      "                                    starting the dispay.                      \n"
	 << endl;
    exit(EINVAL);
  }
}

//______________________________________________________________________________
int main(int argc,char** argv)  {
  bool dry_run = false;
  vector<char*> geo_files;
  for(int i=1; i<argc;++i) {
    if ( argv[i][0]=='-' ) {
      if ( strncmp(argv[i],"-compact",2)==0 )
	geo_files.push_back(argv[++i]);
      else if ( strncmp(argv[i],"-load_only",2)==0 )
        dry_run = true;
      else
	usage();
    }
    else {  // This is the default
      geo_files.push_back(argv[i]);
    }
  }
  if ( geo_files.empty() )
    usage();

  try {
    pair<int, char**> args(0,0);
    LCDD& lcdd = LCDD::getInstance();  
    // Load all compact files
    lcdd.apply("DD4hepCompactLoader",int(geo_files.size()),&geo_files[0]);
    // Create an interactive ROOT application
    if ( !dry_run ) {
      TRint app("DD4hepGeometryDisplay", &args.first, args.second);
      lcdd.apply("DD4hepGeometryDisplay",args.first,args.second);
      app.Run();
    }
    else {
      cout << "The geometry was loaded. Application now exiting." << endl;
    }
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
