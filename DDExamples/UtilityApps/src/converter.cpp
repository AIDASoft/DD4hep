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
    cout << "geoConverter -opt [-opt]                                                \n"
      "        Action flags:               Usage is exclusive, 1 required!           \n"
      "        -compact2lcdd               Convert compact xml geometry to lcdd.     \n"
      "        -compact2gdml               Convert compact xml geometry to gdml.     \n"
      "        -compact2pandora            Convert compact xml to pandora xml      \n\n"
      "        -input  <file>  [REQUIRED]  Specify input file.                       \n"
      "        -output <file>  [OPTIONAL]  Specify output file.                      \n"
      "                                    if no output file is specified, the output\n"
      "                                    device is stdout.                         \n"
	 << endl;
    exit(EINVAL);
  }
}

//______________________________________________________________________________
int main(int argc,char** argv)  {
  bool compact2lcdd = false;
  bool compact2gdml = false;
  bool compact2pand = false;
  int output = 0;
  vector<char*> geo_files;
  for(int i=1; i<argc;++i) {
    if ( argv[i][0]=='-' ) {
      if ( strncmp(argv[i],"-compact2lcdd",12)==0 )
	compact2lcdd = true;
      else if ( strncmp(argv[i],"-compact2gdml",12)==0 )
	compact2gdml = true;
      else if ( strncmp(argv[i],"-compact2pandora",12)==0 )
	compact2pand = true;
      else if ( strncmp(argv[i],"-input",2)==0 )
	geo_files.push_back(argv[++i]);
      else if ( strncmp(argv[i],"-output",2)==0 )
        output = ++i;
      else
	usage();
    }
    else {
      usage();
    }
  }
  if ( geo_files.empty() || (!compact2lcdd && !compact2gdml && !compact2pand))
    usage();

  try {
    LCDD& lcdd = LCDD::getInstance();  
    // Load compact files
    lcdd.apply("DD4hepCompactLoader",int(geo_files.size()),&geo_files[0]);
    if ( compact2lcdd )
      lcdd.apply("DD4hepGeometry2LCDD",output,&argv[output]);
    else if ( compact2gdml )
      lcdd.apply("DD4hepGeometry2GDML",output,&argv[output]);
    else if ( compact2pand )
      lcdd.apply("DD4hepGeometry2PANDORA",output,&argv[output]);
    return 0;
  }
  catch(const exception& e)  {
    cout << e.what() << endl;
  }
  catch(...)  {
    cout << "UNKNOWN Exception" << endl;
  }
  return EINVAL;
}
