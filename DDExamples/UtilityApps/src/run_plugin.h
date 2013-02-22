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

