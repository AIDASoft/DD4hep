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
#include "DD4hep/Printout.h"
#include "../LCDDImp.h"

// ROOT includes
#include "TGeoManager.h"
#include "TGeoVolume.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static LCDDBuildType build_type(const char* value)   {
  if ( strcmp(value,"BUILD_DEFAULT")==0 )
    return BUILD_DEFAULT;
  else if ( strcmp(value,"BUILD_SIMU")==0 )
    return BUILD_SIMU;
  else if ( strcmp(value,"1")==0 )
    return BUILD_DEFAULT;
  else if ( strcmp(value,"BUILD_RECO")==0 )
    return BUILD_RECO;
  else if ( strcmp(value,"2")==0 )
    return BUILD_RECO;
  else if ( strcmp(value,"BUILD_DISPLAY")==0 )
    return BUILD_DISPLAY;
  else if ( strcmp(value,"3")==0 )
    return BUILD_DISPLAY;
  throw runtime_error(string("Invalid build type value: ")+value);
}
 
static void* create_lcdd_instance(const char* /* name */) {
  return &LCDD::getInstance();
}
DECLARE_CONSTRUCTOR(LCDD_constructor,create_lcdd_instance)

static long display(LCDD& lcdd, int argc, char** argv) {
  TGeoManager& mgr = lcdd.manager();
  const char* opt = "ogl";
  if (argc > 0) {
    opt = argv[0];
  }
  mgr.SetVisLevel(4);
  mgr.SetVisOption(1);
  TGeoVolume* vol = mgr.GetTopVolume();
  if (vol) {
    vol->Draw(opt);
    return 1;
  }
  return 0;
}
DECLARE_APPLY(DD4hepGeometryDisplay,display)

static long load_compact(LCDD& lcdd, int argc, char** argv) {
  if ( argc > 0 )   {
    LCDDBuildType type = BUILD_DEFAULT;
    string input = argv[0];
    cout << "Processing compact input file : " << input;
    if ( argc > 1 )  {
      type = build_type(argv[1]);
      cout << " with flag " << argv[1] << endl;
      lcdd.fromCompact(input,type);
      return 1;
    }
    cout << endl;
    lcdd.fromCompact(input);
    return 1;
  }
  return 0;
}
DECLARE_APPLY(DD4hepCompactLoader,load_compact)

static long load_xml(LCDD& lcdd, int argc, char** argv) {
  if ( argc > 0 )   {
    LCDDBuildType type = BUILD_DEFAULT;
    string input = argv[0];
    cout << "Processing compact input file : " << input;
    if ( argc > 1 )  {
      type = build_type(argv[1]);
      cout << " with flag " << argv[1] << endl;
      lcdd.fromXML(input,type);
      return 1;
    }
    cout << endl;
    lcdd.fromXML(input);
    return 1;
  }
  return 0;
}
DECLARE_APPLY(DD4hepXMLLoader,load_xml)

static long load_volmgr(LCDD& lcdd, int, char**) {
  try {
    LCDDImp* imp = dynamic_cast<LCDDImp*>(&lcdd);
    if ( imp )  {
      imp->m_volManager = VolumeManager(lcdd, "World", imp->world(), Readout(), VolumeManager::TREE);
      cout << "++ Volume manager populated and loaded." << endl;
      return 1;
    }
  }
  catch (const exception& e) {
    throw runtime_error(string(e.what()) + "\n"
			"DD4hep: while programming VolumeManager. Are your volIDs correct?");
  }
  catch (...) {
    throw runtime_error("UNKNOWN exception while programming VolumeManager. Are your volIDs correct?");
  }
  return 0;
}
DECLARE_APPLY(DD4hepVolumeManager,load_volmgr)

static long dump_geometry(LCDD& lcdd, int argc, char** argv) {
  if ( argc > 1 )   {
    string output = argv[1];
    cout << "Dump geometry to root file : " << output << endl;
    lcdd.manager().Export(output.c_str()+1);
    return 1;
  }
  return 0;
}
DECLARE_APPLY(DD4hepGeometry2Root,dump_geometry)

static long dump_geometry(PlacedVolume pv,int level) {
  char fmt[64];
  const TGeoNode* node = pv.ptr();
  ::sprintf(fmt,"%03d %%-%ds %%s",level+1,2*level+1);
  printout(INFO,"+++",fmt,"",node->GetName());
  for (Int_t idau = 0, ndau = node->GetNdaughters(); idau < ndau; ++idau) {
    TGeoNode* daughter = node->GetDaughter(idau);
    PlacedVolume placement(daughter);
    if ( placement.data() )   {
      PlacedVolume pv_dau = Ref_t(daughter);
      dump_geometry(pv_dau,level+1);
    }
  }
  return 1;
}
static long dump_geometry(DetElement de,int level) {
  const DetElement::Children& c = de.children();
  char fmt[64];
  for (DetElement::Children::const_iterator i = c.begin(); i != c.end(); ++i)   {
    ::sprintf(fmt,"%03d %%-%ds %%s",level+1,2*level+1);
    printout(INFO,"+++",fmt,"",(*i).second.placementPath().c_str());
    dump_geometry((*i).second,level+1);
  }
  return 1;
}

/** Basic entry point to print otu the detector element hierarchy
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/04/2014
 */
static long dump_detelement_tree(LCDD& lcdd, int, char**) {
  return dump_geometry(lcdd.world(),0);
}
DECLARE_APPLY(DD4hepDetectorDump,dump_detelement_tree)

/** Basic entry point to print otu the volume hierarchy
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/04/2014
 */
static long dump_volume_tree(LCDD& lcdd, int, char**) {
  return dump_geometry(lcdd.world().placement(),0);
}
DECLARE_APPLY(DD4hepVolumeDump,dump_volume_tree)
