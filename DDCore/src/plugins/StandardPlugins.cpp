// $Id$
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
#include "../LCDDImp.h"

// ROOT includes
#include "TGeoManager.h"
#include "TGeoVolume.h"

using namespace std;
using namespace DD4hep::Geometry;

static void* create_lcdd_instance(const char* /* name */) {
  return &LCDD::getInstance();
}
DECLARE_CONSTRUCTOR(LCDD_constructor,create_lcdd_instance);

static long display(LCDD& lcdd,int argc,char** argv)    {
  TGeoManager& mgr = lcdd.manager();
  const char* opt = "ogl";
  if ( argc > 0 )   {
    opt = argv[0];
  }
  mgr.SetVisLevel(4);
  mgr.SetVisOption(1);
  TGeoVolume* vol = mgr.GetTopVolume();
  if ( vol ) {
    vol->Draw(opt);
    return 1;
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

static long load_xml(LCDD& lcdd,int argc,char** argv)    {
  string input = argv[0];
  cout << "Processing compact input file : " << input << endl;
  lcdd.fromXML(input);
  return 1;
}
DECLARE_APPLY(DD4hepXMLLoader,load_xml);

static long load_volmgr(LCDD& lcdd,int,char**)    {
  try {
    LCDDImp* imp = dynamic_cast<LCDDImp*>(&lcdd);
    imp->m_volManager = VolumeManager(lcdd, "World", imp->world(), Readout(), VolumeManager::TREE);
    cout << "++ Volume manager populated and loaded." << endl;
  }
  catch(const exception& e)  {
    throw runtime_error(string(e.what())+"\n"
			"DD4hep: while programming VolumeManager. Are your volIDs correct?");
  }
  catch(...)  {
    throw runtime_error("UNKNOWN exception while programming VolumeManager. Are your volIDs correct?");
  }
  return 1;
}
DECLARE_APPLY(DD4hepVolumeManager,load_volmgr);
