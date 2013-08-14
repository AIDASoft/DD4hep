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

