//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

// Framework include files
#include "DD4hep/LCDD.h"
#include "DD4hep/DD4hepUI.h"
#include "DD4hep/Factories.h"
#include "DD4hep/Printout.h"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/DD4hepRootPersistency.h"
#include "../LCDDImp.h"

// ROOT includes
#include "TInterpreter.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TClass.h"
#include "TRint.h"

// C/C++ include files
#include <fstream>
#include <sstream>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

/// Basic entry point to create a LCDD instance
/**
 *  Factory: LCDD_constructor
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static void* create_lcdd_instance(const char* /* name */) {
  return &LCDD::getInstance();
}
DECLARE_CONSTRUCTOR(LCDD_constructor,create_lcdd_instance)

/// Basic entry point to display the currently loaded geometry using the ROOT OpenGL viewer
/**
 *  Factory: DD4hepGeometryDisplay
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
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

/// Basic entry point to start the ROOT interpreter.
/**
 *  Factory: DD4hepRint
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long run_interpreter(LCDD& /* lcdd */, int argc, char** argv) {
  if ( argc > 0 )   {
    pair<int, char**> a(argc,argv);
  }
  else   {
    pair<int, char**> a(0,0);
    TRint app("DD4hep", &a.first, a.second);
    app.Run();
  }
  return 1;
}
DECLARE_APPLY(DD4hepRint,run_interpreter)

/// Basic entry point to start the ROOT interpreter.
/**
 *  The UI will show up in the ROOT prompt and is accessible
 *  in the interpreter with the global variable 
 *  DD4hep::DD4hepUI* gDD4hepUI;
 *
 *  Factory: DD4hepInteractiveUI
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long root_ui(LCDD& lcdd, int /* argc */, char** /* argv */) {
  char cmd[256];
  DD4hepUI* ui = new DD4hepUI(lcdd);
  ::snprintf(cmd,sizeof(cmd),"DD4hep::DD4hepUI* gDD4hepUI = (DD4hep::DD4hepUI*)%p;",(void*)ui);
  gInterpreter->ProcessLine(cmd);
  printout(ALWAYS,"DD4hepUI",
           "Use the ROOT interpreter variable gDD4hepUI "
           "to interact with the detector description.");
  return 1;
}
DECLARE_APPLY(DD4hepInteractiveUI,root_ui)

/// Basic entry point to interprete an XML document
/**
 *  - The file URI to be opened 
 *    is passed as first argument to the call.
 *  - The processing hint (build type) is passed as optional 
 *    second argument.
 *
 *  Factory: DD4hepCompactLoader
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long load_compact(LCDD& lcdd, int argc, char** argv) {
  if ( argc > 0 )   {
    LCDDBuildType type = BUILD_DEFAULT;
    string input = argv[0];
    if ( argc > 1 )  {
      type = buildType(argv[1]);
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

/// Basic entry point to process any XML document.
/**
 *  - The file URI to be opened 
 *    is passed as first argument to the call.
 *  - The processing hint (build type) is passed as optional 
 *    second argument.
 *
 *  The root tag defines the plugin to interprete it.
 *
 *  Factory: DD4hepXMLLoader
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long load_xml(LCDD& lcdd, int argc, char** argv) {
  if ( argc > 0 )   {
    LCDDBuildType type = BUILD_DEFAULT;
    string input = argv[0];
    if ( argc > 1 )  {
      type = buildType(argv[1]);
      printout(INFO,"XMLLoader","+++ Processing XML file: %s with flag %s",
               input.c_str(), argv[1]);
      lcdd.fromXML(input,type);
      return 1;
    }
    printout(INFO,"XMLLoader","+++ Processing XML file: %s",input.c_str());
    lcdd.fromXML(input);
    return 1;
  }
  return 0;
}
DECLARE_APPLY(DD4hepXMLLoader,load_xml)

/// Basic entry point to process any pre-parsed XML document.
/**
 *  - The handle to the XML element (XercesC DOMNode) 
 *    is passed as first argument to the call.
 *  - The processing hint (build type) is passed as optional 
 *    second argument.
 *
 *  The root tag defines the plugin to interprete it.
 *
 *  Factory: DD4hepXMLProcessor
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long process_xml_doc(LCDD& lcdd, int argc, char** argv) {
  if ( argc > 0 )   {
    LCDDBuildType type = BUILD_DEFAULT;
    LCDDImp* imp = dynamic_cast<LCDDImp*>(&lcdd);
    if ( imp )  {
      XML::XmlElement* h = (XML::XmlElement*)argv[0];
      XML::Handle_t input(h);
      if ( input.ptr() )   {
        if ( argc > 1 )  {
          type = buildType(argv[1]);
          printout(INFO,"XMLLoader","+++ Processing XML element: %s with flag %s",
                   input.tag().c_str(), argv[1]);
        }
        imp->processXMLElement(input, type);
        return 1;
      }
      except("DD4hepXMLProcessor",
             "++ The passed reference to the parsed XML document is invalid.");
    }
  }
  return 0;
}
DECLARE_APPLY(DD4hepXMLProcessor,process_xml_doc)

/// Basic entry point to load the volume manager object
/**
 *  Factory: DD4hepVolumeManager
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
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

/// Basic entry point to dump a DD4hep geometry to a ROOT file
/**
 *  Factory: DD4hepGeometry2ROOT
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
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

/// Basic entry point to load a DD4hep geometry directly from the ROOT file
/**
 *  Factory: DD4hepRootLoader
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
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

/// Basic entry point to print out the volume hierarchy
/**
 *  Factory: DD4hepVolumeDump
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long dump_volume_tree(LCDD& lcdd, int argc, char** argv) {
  struct Actor {
    typedef PlacedVolume::VolID  VID;
    typedef PlacedVolume::VolIDs VIDs;
    bool m_printVolIDs;
    bool m_printPositions;
    bool m_printSensitivesOnly;
    Actor(int ac, char** av) 
      : m_printVolIDs(false), m_printPositions(false), m_printSensitivesOnly(false)
    {
      for(int i=0; i<ac; ++i)  {
        char c = ::tolower(av[i][0]);
        if ( c == 'v' ) m_printVolIDs = true;
        else if ( c == 'p' ) m_printPositions = true;
        else if ( c == 's' ) m_printSensitivesOnly = true;
      }
    }

    long dump(TGeoNode* ideal, TGeoNode* aligned,int level, VIDs volids) const {
      char fmt[128];
      string opt_info;
      PlacedVolume pv(ideal);
      bool sensitive = false;
      if ( m_printPositions || m_printVolIDs )  {
        stringstream log;
        // Top level volume! have no volume ids
        if ( m_printVolIDs && ideal && ideal->GetMotherVolume() )  {
          VIDs vid = pv.volIDs();
          if ( !vid.empty() )  {
            sensitive = true;
            log << " VolID: ";
            volids.std::vector<VID>::insert(volids.end(),vid.begin(),vid.end());
            for(VIDs::const_iterator i=volids.begin(); i!=volids.end(); ++i)  {
              ::snprintf(fmt, sizeof(fmt), "%s:%2d ",(*i).first.c_str(), (*i).second);
              log << fmt;
            }
            if ( !vid.empty() || pv.volume().isSensitive() )  {
              SensitiveDetector sd = pv.volume().sensitiveDetector();
              if ( sd.isValid() )  {
                IDDescriptor dsc = sd.readout().idSpec();
                if ( dsc.isValid() )  {
                  log << hex << " (0x" << setfill('0') << setw(8)
                      << dsc.encode(volids)
                      << setfill(' ') << dec << ") ";
                }
              }
            }
          }
        }
        if ( m_printPositions )  {
          const double* trans = ideal->GetMatrix()->GetTranslation();
          ::snprintf(fmt, sizeof(fmt), "Pos: (%f,%f,%f) ",trans[0],trans[1],trans[2]);
          log << fmt;
        }
        opt_info = log.str();
      }
      TGeoVolume* volume = ideal->GetVolume();
      if ( !m_printSensitivesOnly || (m_printSensitivesOnly && sensitive) )  {
        char sens = pv.volume().isSensitive() ? 'S' : ' ';
        if ( ideal == aligned )  {
          ::snprintf(fmt,sizeof(fmt),"%03d %%-%ds %%-16s (%%s) \t[%p] %c %%s",
                     level+1,2*level+1,(void*)ideal, sens);
        }
        else  {
          ::snprintf(fmt,sizeof(fmt),"%03d %%-%ds %%-16s (%%s) Ideal:%p Aligned:%p %c %%s",
                     level+1,2*level+1,(void*)ideal,(void*)aligned, sens);
        }
        printout(INFO,"VolumeDump",fmt,"",
                 aligned->GetName(),
                 volume->GetShape()->IsA()->GetName(),
                 opt_info.c_str());
      }
      for (Int_t idau = 0, ndau = aligned->GetNdaughters(); idau < ndau; ++idau)  {
        TGeoNode*   ideal_daughter   = ideal->GetDaughter(idau);
        const char* daughter_name    = ideal_daughter->GetName();
        TGeoNode*   aligned_daughter = volume->GetNode(daughter_name);
        dump(ideal_daughter, aligned_daughter, level+1, volids);
      }
      return 1;
    }
  };
  string place = lcdd.world().placementPath();
  DetectorTools::PlacementPath path;
  DetectorTools::placementPath(lcdd.world(), path);
  PlacedVolume  pv = DetectorTools::findNode(lcdd.world().placement(),place);
  Actor actor(argc,argv);
  return actor.dump(lcdd.world().placement().ptr(),pv.ptr(),0,PlacedVolume::VolIDs());
}
DECLARE_APPLY(DD4hepVolumeDump,dump_volume_tree)

/// Basic entry point to print out the detector element hierarchy
/**
 *  Factory: DD4hepDetectorDump, DD4hepDetectorVolumeDump
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
template <int flag> long dump_detelement_tree(LCDD& lcdd, int argc, char** argv) {
  struct Actor {
    static long dump(DetElement de,int level, bool sensitive_only) {
      const DetElement::Children& c = de.children();
      if ( !sensitive_only || 0 != de.volumeID() )  {
        PlacedVolume place = de.placement();
        const TGeoNode* node = place.ptr();
        char sens = place.volume().isSensitive() ? 'S' : ' ';
        int value = flag;
        char fmt[128];
        switch(value)  {
        case 0:
          ::snprintf(fmt,sizeof(fmt),"%03d %%-%ds %%s #Dau:%%d VolID:%%08X Place:%%p  %%c",level+1,2*level+1);
          printout(INFO,"DetectorDump",fmt,"",de.path().c_str(),int(c.size()),
                   (unsigned long)de.volumeID(), (void*)node, sens);
          break;
        case 1:
          ::snprintf(fmt,sizeof(fmt),"%03d %%-%ds Detector: %%s #Dau:%%d VolID:%%p",level+1,2*level+1);
          printout(INFO,"DetectorDump", fmt, "", de.path().c_str(),
                   int(c.size()), (void*)de.volumeID());
          ::snprintf(fmt,sizeof(fmt),"%03d %%-%ds Placement: %%s   %%c",level+1,2*level+3);
          printout(INFO,"DetectorDump",fmt,"", de.placementPath().c_str(), sens);
          break;
        default:
          break;
        }
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
DECLARE_APPLY(DD4hepDetectorDump,dump_detelement_tree<0>)
DECLARE_APPLY(DD4hepDetectorVolumeDump,dump_detelement_tree<1>)

/// Basic entry point to print out the volume hierarchy
/**
 *  Factory: DD4hepDetElementCache
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
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

/// Basic entry point to dump the geometry tree of the lcdd instance
/**
 *  Factory: DD4hepGeometryTreeDump
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
#include "../GeometryTreeDump.h"
static long exec_GeometryTreeDump(LCDD& lcdd, int, char** ) {
  GeometryTreeDump dmp;
  dmp.create(lcdd.world());
  return 1;
}
DECLARE_APPLY(DD4hepGeometryTreeDump,exec_GeometryTreeDump)

/// Basic entry point to dump the geometry in GDML format
/**
 *  Factory: DD4hepSimpleGDMLWriter
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
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

/// Basic entry point to print out detector type map
/**
 *  Factory: DD4hepDetectorTypes
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long detectortype_cache(LCDD& lcdd, int , char** ) {
  vector<string> v = lcdd.detectorTypes();
  printout(INFO,"DetectorTypes","Detector type dump:  %ld types:",long(v.size()));
  for(vector<string>::const_iterator i=v.begin(); i!=v.end(); ++i)   {
    const vector<DetElement>& vv=lcdd.detectors(*i);
    printout(INFO,"DetectorTypes","\t --> %ld %s detectors:",long(vv.size()),(*i).c_str());
    for(vector<DetElement>::const_iterator j=vv.begin(); j!=vv.end(); ++j)
      printout(INFO,"DetectorTypes","\t\t %-16s --> %s  [%s]",(*i).c_str(),(*j).name(),(*j).type().c_str());
  }
  return 1;
}
DECLARE_APPLY(DD4hepDetectorTypes,detectortype_cache)

#include "DD4hep/SurfaceInstaller.h"
typedef SurfaceInstaller TestSurfacesPlugin;
DECLARE_SURFACE_INSTALLER(TestSurfaces,TestSurfacesPlugin)

#include "DD4hep/ConditionsPrinter.h"
/// Basic entry point to instantiate the basic DD4hep conditions printer
/**
 *  Factory: DD4hepConditionsPrinter
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    17/11/2016
 */
static void* create_conditions_printer(Geometry::LCDD& /* lcdd */, int argc,char** argv)  {
  string prefix = "";
  int flags = 0;
  for(int i=0; i<argc && argv[i]; ++i)  {
    if ( 0 == ::strncmp("-prefix",argv[i],4) )
      prefix = argv[++i];
    else if ( 0 == ::strncmp("-flags",argv[i],2) )
      flags = ::atol(argv[++i]);
  }
  if ( flags )
    return new Conditions::ConditionsPrinter(prefix,flags);
  return new Conditions::ConditionsPrinter(prefix);
}
DECLARE_LCDD_CONSTRUCTOR(DD4hepConditionsPrinter,create_conditions_printer)
