// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Standard plugins necessary for nearly everything.
// 
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/Factories.h"
#include "DD4hep/LCDD.h"
// ROOT includes
#include "TGeoManager.h"
#include "TGeoVolume.h"

using namespace std;
using namespace DD4hep::Geometry;

static void* create_lcdd_instance(const char* /* name */) {
  return &LCDD::getInstance();
}
DECLARE_CONSTRUCTOR(LCDD_constructor,create_lcdd_instance);

static long display(LCDD& /* lcdd */,int argc,char** argv)    {
  TGeoManager* mgr = gGeoManager;
  const char* opt = "ogl";
  if ( argc > 0 )   {
    opt = argv[0];
  }
  if ( mgr ) {
    mgr->SetVisLevel(4);
    mgr->SetVisOption(1);
    TGeoVolume* vol = mgr->GetTopVolume();
    if ( vol ) {
      vol->Draw(opt);
      return 1;
    }
  }
  return 0;
}
DECLARE_APPLY(DD4hepGeometryDisplay,display);

static long load_compact(LCDD& lcdd,int argc,char** argv)    {
  for(size_t j=0; j<argc; ++j) {
    string input = argv[j];
    cout << "Processing compact input file : " << input << endl;
    lcdd.fromCompact(input);
  }
  return 1;
}
DECLARE_APPLY(DD4hepCompactLoader,load_compact);

