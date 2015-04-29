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
#include "DD4hep/LCDD.h"
#include "DD4hep/Factories.h"
#include "DD4hep/Printout.h"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/DD4hepRootPersistency.h"
#include "../LCDDImp.h"

// ROOT includes
#include "TGeoManager.h"
#include "TGeoVolume.h"

#include <fstream>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static LCDDBuildType build_type(const char* value)   {
  if ( strcmp(value,"BUILD_DEFAULT")==0 )
    return BUILD_DEFAULT;
  else if ( strcmp(value,"BUILD_SIMU")==0 )
    return BUILD_SIMU;
  else if ( strcmp(value,"1")==0 )
    return BUILD_SIMU;
  else if ( strcmp(value,"BUILD_RECO")==0 )
    return BUILD_RECO;
  else if ( strcmp(value,"2")==0 )
    return BUILD_RECO;
  else if ( strcmp(value,"BUILD_DISPLAY")==0 )
    return BUILD_DISPLAY;
  else if ( strcmp(value,"3")==0 )
    return BUILD_DISPLAY;
  else if ( strcmp(value,"BUILD_ENVELOPE")==0 )
    return BUILD_ENVELOPE;
  else if ( strcmp(value,"4")==0 )
    return BUILD_ENVELOPE;
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
    if ( argc > 1 )  {
      type = build_type(argv[1]);
      printout(INFO,"CompactLoader","+++ Processing compact file: %s with flag %s",
               input.c_str(), argv[1]);
      lcdd.fromCompact(input,type);
      return 1;
    }
    printout(INFO,"CompactLoader","+++ Processing compact file: %s",input.c_str());
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
    if ( argc > 1 )  {
      type = build_type(argv[1]);
      printout(INFO,"XMLLoader","+++ Processing XML file: %s with flag %s",
               input.c_str(), argv[1]);
      lcdd.fromXML(input,type);
      return 1;
    }
    printout(INFO,"XMLLoader","+++ Processing compact file: %s",input.c_str());
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
      imp->imp_loadVolumeManager();
      printout(INFO,"VolumeManager","+++ Volume manager populated and loaded.");
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

static long dump_geometry2root(LCDD& lcdd, int argc, char** argv) {
  if ( argc > 0 )   {
    string output = argv[0];
    printout(INFO,"Geometry2ROOT","+++ Dump geometry to root file:%s",output.c_str());
    //lcdd.manager().Export(output.c_str()+1);
    if ( DD4hepRootPersistency::save(lcdd,output.c_str(),"Geometry") > 1 )  {
      return 1;
    }
  }
  printout(ERROR,"Geometry2ROOT","+++ No output file name given.");
  return 0;
}
DECLARE_APPLY(DD4hepGeometry2ROOT,dump_geometry2root)

static long load_geometryFromroot(LCDD& lcdd, int argc, char** argv) {
  if ( argc > 0 )   {
    string input = argv[0];
    printout(INFO,"DD4hepRootLoader","+++ Read geometry from root file:%s",input.c_str());
    if ( 1 == DD4hepRootPersistency::load(lcdd,input.c_str(),"Geometry") )  {
      return 1;
    }
  }
  printout(ERROR,"DD4hepRootLoader","+++ No input file name given.");
  return 0;
}
DECLARE_APPLY(DD4hepRootLoader,load_geometryFromroot)

/** Basic entry point to print out the volume hierarchy
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/04/2014
 */
static long dump_volume_tree(LCDD& lcdd, int , char** ) {
  struct Actor {
    static long dump(TGeoNode* ideal, TGeoNode* aligned,int level) {
      char fmt[256];
      if ( ideal == aligned )  {
	::snprintf(fmt,sizeof(fmt),"%03d %%-%ds %%s \t\tNode:%p",level+1,2*level+1,(void*)ideal);
      }
      else  {
	::snprintf(fmt,sizeof(fmt),"%03d %%-%ds %%s Ideal node:%p Aligned node:%p",
		   level+1,2*level+1,(void*)ideal,(void*)aligned);
      }
      printout(INFO,"+++",fmt,"",aligned->GetName());
      TGeoVolume* volume = ideal->GetVolume();
      for (Int_t idau = 0, ndau = aligned->GetNdaughters(); idau < ndau; ++idau)  {
	TGeoNode*   ideal_daughter   = ideal->GetDaughter(idau);
	const char* daughter_name    = ideal_daughter->GetName();
	TGeoNode*   aligned_daughter = volume->GetNode(daughter_name);
	dump(ideal_daughter,aligned_daughter,level+1);
      }
      return 1;
    }
  };
  string place = lcdd.world().placementPath();
  DetectorTools::PlacementPath path;
  DetectorTools::placementPath(lcdd.world(), path);
  PlacedVolume  pv = DetectorTools::findNode(lcdd.world().placement(),place);
  return Actor::dump(lcdd.world().placement().ptr(),pv.ptr(),0);
  //return Actor::dump(lcdd.manager().GetTopNode(),pv.ptr(),0);
}
DECLARE_APPLY(DD4hepVolumeDump,dump_volume_tree)

/** Basic entry point to print out the detector element hierarchy
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/04/2014
 */
static long dump_detelement_tree(LCDD& lcdd, int argc, char** argv) {
  struct Actor {
    static long dump(DetElement de,int level, bool sensitive_only) {
      const DetElement::Children& c = de.children();
      if ( !sensitive_only || 0 != de.volumeID() )  {
	char fmt[64];
	::sprintf(fmt,"%03d %%-%ds %%s #Dau:%%d VolID:%%p",level+1,2*level+1);
	printout(INFO,"+++",fmt,"",de.placementPath().c_str(),int(c.size()),(void*)de.volumeID());
      }
      for (DetElement::Children::const_iterator i = c.begin(); i != c.end(); ++i)
	dump((*i).second,level+1,sensitive_only);
      return 1;
    }
  };
  bool sensitive_only = false;
  for(int i=0; i<argc; ++i)  {
    if ( ::strcmp(argv[i],"--sensitive")==0 ) { sensitive_only = true; }
  }
  return Actor::dump(lcdd.world(),0,sensitive_only);
}
DECLARE_APPLY(DD4hepDetectorDump,dump_detelement_tree)

/** Basic entry point to print out the volume hierarchy
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/04/2014
 */
static long detelement_cache(LCDD& lcdd, int , char** ) {
  struct Actor {
    static long cache(DetElement de) {
      const DetElement::Children& c = de.children();
      de.worldTransformation();
      de.parentTransformation();
      de.placementPath();
      de.path();
      for (DetElement::Children::const_iterator i = c.begin(); i != c.end(); ++i)
	cache((*i).second);
      return 1;
    }
  };
  return Actor::cache(lcdd.world());
}
DECLARE_APPLY(DD4hepDetElementCache,detelement_cache)

#include "../GeometryTreeDump.h"
static long exec_GeometryTreeDump(LCDD& lcdd, int, char** ) {
  GeometryTreeDump dmp;
  dmp.create(lcdd.world());
  return 1;
}
DECLARE_APPLY(DD4hepGeometryTreeDump,exec_GeometryTreeDump)

#include "../SimpleGDMLWriter.h"
static long exec_SimpleGDMLWriter(LCDD& lcdd, int argc, char** argv) {
  if ( argc > 1 )   {
    string output = argv[1];
    ofstream out(output.c_str()+1,ios_base::out);
    SimpleGDMLWriter dmp(out);
    dmp.create(lcdd.world());
  }
  else   {
    SimpleGDMLWriter dmp(cout);
    dmp.create(lcdd.world());
  }
  return 1;
}

DECLARE_APPLY(DD4hepSimpleGDMLWriter,exec_SimpleGDMLWriter)

#include "DD4hep/SurfaceInstaller.h"
typedef SurfaceInstaller TestSurfacesPlugin;
DECLARE_SURFACE_INSTALLER(TestSurfaces,TestSurfacesPlugin)
