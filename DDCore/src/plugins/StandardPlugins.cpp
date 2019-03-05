//==========================================================================
//  AIDA Detector description implementation 
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
#include "DD4hep/Detector.h"
#include "DD4hep/Memory.h"
#include "DD4hep/DD4hepUI.h"
#include "DD4hep/Factories.h"
#include "DD4hep/Printout.h"
#include "DD4hep/DD4hepUnits.h"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/PluginCreators.h"
#include "DD4hep/VolumeProcessor.h"
#include "DD4hep/DetectorProcessor.h"
#include "DD4hep/DD4hepRootPersistency.h"
#include "XML/DocumentHandler.h"
#include "XML/XMLElements.h"
#include "XML/XMLTags.h"
#include "../DetectorImp.h"

// ROOT includes
#include "TInterpreter.h"
#include "TGeoElement.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TSystem.h"
#include "TClass.h"
#include "TRint.h"
#if ROOT_VERSION_CODE > ROOT_VERSION(6,16,0)
#include "TGDMLMatrix.h"
#endif

// C/C++ include files
#include <cerrno>
#include <cstdlib>
#include <fstream>
#include <sstream>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;


namespace  {
  struct ProcessorArgs   {
    bool use = false;
    int  start = 0, end = 0, argc = 0, count=0;
    std::vector<char*> argv;
    ProcessorArgs(int ac, char** av)    {
      for(int i=0; i<ac; ++i)  {
        if ( 0 == ::strncmp(av[i],"-processor",6) )   {
          use = true;
          start = i;
        }
        if ( use )   {
          ++argc; ++count; end = i;
          if ( 0 == ::strncmp(av[i],"-end-processor",6) )  {
            argv.push_back(av[i]);
            return;
          }
          else if ( 0 == ::strncmp(av[i],"-end-plugin",4) )  { // End of current plugin
            argv.push_back((char*)"-end-processor");
            return;
          }
          else if ( 0 == ::strncmp(av[i],"-plugin",4) )  {     // Start of next plugin
            argv.push_back((char*)"-end-processor");
            return;
          }
          argv.push_back(av[i]);
        }
      }
    }
  };
}

/// Dummy plugin to be able to invoke the plugin runner and e.g. only test the geometry
/**
 *  Factory: DD4hep_DummyPlugin
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long dummy_plugin(Detector& , int, char**) {
  return 1;
}
DECLARE_APPLY(DD4hep_DummyPlugin,dummy_plugin)

/// Basic entry point to create a Detector instance
/**
 *  Factory: Detector_constructor
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static void* create_description_instance(const char* /* name */) {
  return &Detector::getInstance();
}
DECLARE_CONSTRUCTOR(Detector_constructor,create_description_instance)

/// Basic entry point to display the currently loaded geometry using the ROOT OpenGL viewer
/**
 *  Factory: DD4hep_GeometryDisplay
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long display(Detector& description, int argc, char** argv) {
  TGeoManager& mgr = description.manager();
  int vislevel = 6, visopt = 1;
  string detector = "/world";
  const char* opt = "ogl";
  for(int i = 0; i < argc && argv[i]; ++i)  {
    if ( 0 == ::strncmp("-option",argv[i],4) )
      opt = argv[++i];
    else if ( 0 == ::strncmp("-level",argv[i],4) )
      vislevel = ::atol(argv[++i]);
    else if ( 0 == ::strncmp("-visopt",argv[i],4) )
      visopt = ::atol(argv[++i]);
    else if ( 0 == ::strncmp("-detector",argv[i],4) )
      detector = argv[++i];
    else  {
      cout <<
        "Usage: -plugin <name> -arg [-arg]                                                   \n"
        "     -detector <string> Top level DetElement path. Default: '/world'                \n"
        "     -option   <string> ROOT Draw option.    Default: 'ogl'                         \n"
        "     -level    <number> Visualization level  [TGeoManager::SetVisLevel]  Default: 4 \n"
        "     -visopt   <number> Visualization option [TGeoManager::SetVisOption] Default: 1 \n"       
        "\tArguments given: " << arguments(argc,argv) << endl << flush;
      ::exit(EINVAL);
    }
  }
  mgr.SetVisLevel(vislevel);
  mgr.SetVisOption(visopt);
  TGeoVolume* vol = mgr.GetTopVolume();
  if ( detector != "/world" )   {
    DetElement elt = detail::tools::findElement(description,detector);
    if ( !elt.isValid() )  {
      except("DD4hep_GeometryDisplay","+++ Invalid DetElement path: %s",detector.c_str());
    }
    if ( !elt.placement().isValid() )   {
      except("DD4hep_GeometryDisplay","+++ Invalid DetElement placement: %s",detector.c_str());
    }
    vol = elt.placement().volume();
  }
  if ( vol )   {
    vol->Draw(opt);
    return 1;
  }
  return 0;
}
DECLARE_APPLY(DD4hep_GeometryDisplay,display)

/// Basic entry point to execute a public function from a library
/**
 *  Factory: DD4hep_Function
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long run_function(Detector&, int argc, char** argv) {
  string lib, func;
  std::vector<char*> args;
  for(int i = 0; i < argc && argv[i]; ++i)  {
    if ( 0 == ::strncmp("-library",argv[i],4) )
      lib = argv[++i];
    else if ( 0 == ::strncmp("-function",argv[i],4) )
      func = argv[++i];
    else
      args.push_back(argv[i]);
  }
  if ( lib.empty() || func.empty() )  {
    cout <<
      "Usage: -plugin <name> -arg [-arg]                                \n"
      "     -library   <string> Library to be loaded                    \n"
      "     -function  <string> name of the entry point to be executed. \n"
      "\tArguments given: " << arguments(argc,argv) << endl << flush;
    ::exit(EINVAL);
  }
  Func_t f = gSystem->DynFindSymbol("*",func.c_str());
  int ret;
  if ( !f )  {
    ret = gSystem->Load(lib.c_str());
    if ( ret != 0 )  {
      except("DD4hep_Function","+++ Failed to load library: %s",lib.c_str());
    }
    f = gSystem->DynFindSymbol("*",func.c_str());
    if ( !f )  {
      except("DD4hep_Function","+++ Failed to find function %s in library: %s",
             func.c_str(),lib.c_str());
    }
  }
  typedef int (*call_t)(int, char**);
  call_t ff  = (call_t)f;
  ret = (*ff)(args.size(),&args[0]);
  return ret;
}
DECLARE_APPLY(DD4hep_Function,run_function)

/// Basic entry point to start the ROOT interpreter.
/**
 *  Factory: DD4hep_Rint
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long run_interpreter(Detector& /* description */, int argc, char** argv) {
  if ( 0 == gApplication )  {
    pair<int, char**> a(argc,argv);
    gApplication = new TRint("DD4hepRint", &a.first, a.second);
    printout(INFO,"DD4hepRint","++ Created ROOT interpreter instance for DD4hepUI.");
  }
  for(int i=0; i<argc; ++i)   {
    printout(INFO,"DD4hepRint","Excecute[%d]: %s",i,argv[i]);
    gInterpreter->ProcessLine(argv[i]);
  }
  if ( !gApplication->IsRunning() )  {
    gApplication->Run();
  }
  return 1;
}
DECLARE_APPLY(DD4hep_Rint,run_interpreter)

/// Basic entry point to start the ROOT interpreter.
/**
 *  The UI will show up in the ROOT prompt and is accessible
 *  in the interpreter with the global variable 
 *  dd4hep::DD4hepUI* gdd4hepUI;
 *
 *  Factory: DD4hep_InteractiveUI
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long root_ui(Detector& description, int /* argc */, char** /* argv */) {
  char cmd[256];
  //DD4hepUI* ui = new DD4hepUI(description);
  //::snprintf(cmd,sizeof(cmd),"dd4hep::detail::DD4hepUI* gDD4hepUI = (dd4hep::detail::DD4hepUI*)%p;",(void*)ui);
  ::snprintf(cmd,sizeof(cmd),
             "dd4hep::detail::DD4hepUI* gDD4hepUI = new "
             "dd4hep::detail::DD4hepUI(*(dd4hep::Detector*)%p);",
             (void*)&description);
  gInterpreter->ProcessLine(cmd);
  printout(ALWAYS,"DD4hepUI",
           "Use the ROOT interpreter variable gDD4hepUI "
           "to interact with the detector description.");
  return 1;
}
DECLARE_APPLY(DD4hep_InteractiveUI,root_ui)

/// Basic entry point to dump all known GDML tables
/**
 *
 *  Factory: DD4hep_Dump_GDMLTables
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long root_gdml_tables(Detector& description, int /* argc */, char** /* argv */) {
  size_t num_tables = 0;
  size_t num_elements = 0;
#if ROOT_VERSION_CODE > ROOT_VERSION(6,16,0)
  const TObjArray* c = description.manager().GetListOfGDMLMatrices();
  TObjArrayIter arr(c);
  for(TObject* i = arr.Next(); i; i=arr.Next())   {
    TGDMLMatrix* m = (TGDMLMatrix*)i;
    num_elements += (m->GetRows()*m->GetCols());
    ++num_tables;
    m->Print();
  }
#endif
  printout(INFO,"Dump_GDMLTables",
           "+++ Successfully dumped %ld GDML tables with %ld elements.",
           num_tables, num_elements);
  return 1;
}
DECLARE_APPLY(DD4hep_Dump_GDMLTables,root_gdml_tables)

/// Basic entry point to dump the ROOT TGeoElementTable object
/**
 *  Dump the elment table to stdout or file.
 *
 *  Factory: DD4hep_ElementTable -format xml/text(default) -output <file-name>
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long root_elements(Detector& description, int argc, char** argv) {
  struct ElementPrint {
    ElementPrint() = default;
    virtual ~ElementPrint() = default;
    virtual void operator()(TGeoElement* elt)  { elt->Print();  }
    virtual void operator()(TGeoIsotope* iso)  { iso->Print();  }
  };
  struct ElementPrintXML : public ElementPrint  {
    typedef xml::Element elt_h;
    elt_h root;
    ElementPrintXML(elt_h r) : root(r) {}
    virtual ~ElementPrintXML() {}
    virtual void operator()(TGeoElement* element)  {
      elt_h elt = root.addChild(_U(element));
      elt.setAttr(_U(Z),element->Z());
      elt.setAttr(_U(N),element->N());
      elt.setAttr(_U(formula),element->GetName());
      elt.setAttr(_U(name),element->GetName());
      elt_h atom = elt.addChild(_U(atom));
      atom.setAttr(_U(type),"A");
      atom.setAttr(_U(unit),"g/mole");
      atom.setAttr(_U(value),element->A());
    }
    virtual void operator()(TGeoIsotope* isotope)  {
      elt_h iso = root.addChild(_U(isotope));
      iso.setAttr(_U(Z),isotope->GetZ());
      iso.setAttr(_U(N),isotope->GetN());
      iso.setAttr(_U(formula),isotope->GetName());
      iso.setAttr(_U(name),isotope->GetName());
      elt_h atom = iso.addChild(_U(atom));
      atom.setAttr(_U(type),"A");
      atom.setAttr(_U(unit),"g/mole");
      atom.setAttr(_U(value),isotope->GetA());
    }
  };

  string type = "text", output = "";
  for(int i=0; i<argc; ++i)  {
    if ( argv[i][0] == '-' )  {
      char c = ::tolower(argv[i][1]);
      if ( c == 't' && i+1<argc ) type = argv[++i];
      else if ( c == 'o' && i+1<argc ) output = argv[++i];
      else  {
        ::printf("DD4hep_ElementTable -opt [-opt]                        \n"
                 "  -type   <string>    Output format: text or xml       \n"
                 "  -output <file-name> Output file specifier (xml only) \n"
                 "\n");
        exit(EINVAL);
      }
    }
  }

  xml::Document doc(0);
  xml::DocumentHandler docH;
  xml::Element  element(0);
  if ( type == "xml" )  {
     const char comment[] = "\n"
    "      +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"
    "      ++++   Generic detector description in C++               ++++\n"
    "      ++++   dd4hep Detector description generator.            ++++\n"
    "      ++++                                                     ++++\n"
    "      ++++   Parser:"
    XML_IMPLEMENTATION_TYPE
    "                ++++\n"
    "      ++++                                                     ++++\n"
    "      ++++   Table of elements as defined in ROOT: " ROOT_RELEASE  "     ++++\n"
    "      ++++                                                     ++++\n"
    "      ++++                              M.Frank CERN/LHCb      ++++\n"
    "      +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n  ";
     doc = docH.create("materials", comment);
     element = doc.root();
  }
  dd4hep_ptr<ElementPrint> printer(element
                                   ? new ElementPrintXML(element)
                                   : new ElementPrint());
  TGeoElementTable* table = description.manager().GetElementTable();
  for(Int_t i=0, n=table->GetNelements(); i < n; ++i)
    (*printer)(table->GetElement(i));

  for(Int_t i=0, n=table->GetNelements(); i < n; ++i)  {
    TGeoElement* elt = table->GetElement(i);
    Int_t niso = elt->GetNisotopes();
    if ( niso > 0 )  {
      for(Int_t j=0; j < niso; ++j)
        (*printer)(elt->GetIsotope(j));
    }
  }
  if ( element )   {
    xml::DocumentHandler dH;
    dH.output(doc, output);
  }
  return 1;
}
DECLARE_APPLY(DD4hep_ElementTable,root_elements)

/// Basic entry point to dump the ROOT TGeoElementTable object
/**
 *  Dump the elment table to stdout or file.
 *
 *  Factory: DD4hepElementTable -format xml/text(default) -output <file-name>
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long root_materials(Detector& description, int argc, char** argv) {
  struct MaterialPrint {
    typedef xml::Element elt_h;
    MaterialPrint() = default;
    virtual ~MaterialPrint() = default;
    virtual elt_h print(TGeoMaterial* mat)  {
      ::printf("%-8s %-32s  Aeff=%7.3f Zeff=%7.4f rho=%8.3f [g/mole] radlen=%8.3g [cm] intlen=%8.3g [cm] index=%3d\n",
               mat->IsMixture() ? "Mixture" : "Material",
               mat->GetName(), mat->GetA(), mat->GetZ(), mat->GetDensity(),
               mat->GetRadLen(), mat->GetIntLen(), mat->GetIndex());
      return elt_h(0);
    }
    virtual void print(elt_h, TGeoElement* elt, double frac)   {
      ::printf("  %-6s Fraction: %7.3f Z=%3d A=%6.2f N=%3d Neff=%6.2f\n",
               elt->GetName(), frac, elt->Z(), elt->A(), elt->N(), elt->Neff());
    }
    virtual void operator()(TGeoMaterial* mat)  {
      Double_t* mix = mat->IsMixture() ? ((TGeoMixture*)mat)->GetWmixt() : 0;
      elt_h  mh = print(mat);
      for(int n=mat->GetNelements(), i=0; i<n; ++i)   {
        TGeoElement* elt = mat->GetElement(i);
        print(mh, elt, mix ? mix[i] : 1);
      }
    }
  };
  struct MaterialPrintXML : public MaterialPrint  {
    elt_h root;
    MaterialPrintXML(elt_h r) : root(r) {}
    virtual ~MaterialPrintXML() {}
    virtual elt_h print(TGeoMaterial* mat)  {
      elt_h elt = root.addChild(_U(material));
      elt.setAttr(_U(name),mat->GetName());
      if ( ::strlen(mat->GetTitle())>0 )   {
        elt.setAttr(_U(formula),mat->GetTitle());
      }
      else if ( mat->GetNelements() == 1 )   {
        elt.setAttr(_U(formula),mat->GetElement(0)->GetName());
      }
      elt_h D = elt.addChild(_U(D));
      D.setAttr(_U(type),"density");
      D.setAttr(_U(value),mat->GetDensity());
      D.setAttr(_U(unit),"g/cm3");
      return elt;
    }
    virtual void print(elt_h mat, TGeoElement* element, double frac)  {
      elt_h elt = mat.addChild(_U(composite));
      elt.setAttr(_U(n),frac);
      elt.setAttr(_U(ref),element->GetName());
    }
  };

  string type = "text", output = "";
  for(int i=0; i<argc; ++i)  {
    if ( argv[i][0] == '-' )  {
      char c = ::tolower(argv[i][1]);
      if ( c == 't' && i+1<argc ) type = argv[++i];
      else if ( c == 'o' && i+1<argc ) output = argv[++i];
      else  {
        ::printf("DD4hep_MaterialTable -opt [-opt]                       \n"
                 "  -type   <string>    Output format: text or xml       \n"
                 "  -output <file-name> Output file specifier (xml only) \n"
                 "\n");
        exit(EINVAL);
      }
    }
  }

  xml::Document doc(0);
  xml::DocumentHandler docH;
  xml::Element  element(0);
  if ( type == "xml" )  {
     const char comment[] = "\n"
    "      +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"
    "      ++++   Generic detector description in C++               ++++\n"
    "      ++++   dd4hep Detector description generator.            ++++\n"
    "      ++++                                                     ++++\n"
    "      ++++   Parser:"
    XML_IMPLEMENTATION_TYPE
    "                ++++\n"
    "      ++++                                                     ++++\n"
    "      ++++   Table of elements as defined in ROOT: " ROOT_RELEASE "     ++++\n"
    "      ++++                                                     ++++\n"
    "      ++++                              M.Frank CERN/LHCb      ++++\n"
    "      +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n  ";
     doc = docH.create("materials", comment);
     element = doc.root();
  }
  dd4hep_ptr<MaterialPrint> printer(element
                                   ? new MaterialPrintXML(element)
                                   : new MaterialPrint());
  TObject* obj = 0;
  TList* mats = description.manager().GetListOfMaterials();
  dd4hep_ptr<TIterator> iter(mats->MakeIterator());
  while( (obj=iter->Next()) != 0 )  {
    TGeoMaterial* mat = (TGeoMaterial*)obj;
    (*printer)(mat);
  }
  if ( element )   {
    xml::DocumentHandler dH;
    dH.output(doc, output);
  }
  return 1;
}
DECLARE_APPLY(DD4hep_MaterialTable,root_materials)

/// Basic entry point to interprete an XML document
/**
 *  - The file URI to be opened 
 *    is passed as first argument to the call.
 *  - The processing hint (build type) is passed as optional 
 *    second argument.
 *
 *  Factory: DD4hep_CompactLoader
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long load_compact(Detector& description, int argc, char** argv) {
  if ( argc > 0 )   {
    DetectorBuildType type = BUILD_DEFAULT;
    string input = argv[0];
    if ( argc > 1 )  {
      type = buildType(argv[1]);
      printout(INFO,"CompactLoader","+++ Processing compact file: %s with flag %s",
               input.c_str(), argv[1]);
      description.fromCompact(input,type);
      return 1;
    }
    printout(INFO,"CompactLoader","+++ Processing compact file: %s",input.c_str());
    description.fromCompact(input);
    return 1;
  }
  return 0;
}
DECLARE_APPLY(DD4hep_CompactLoader,load_compact)

/// Basic entry point to process any XML document.
/**
 *  - The file URI to be opened 
 *    is passed as first argument to the call.
 *  - The processing hint (build type) is passed as optional 
 *    second argument.
 *
 *  The root tag defines the plugin to interprete it.
 *
 *  Factory: DD4hep_XMLLoader
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long load_xml(Detector& description, int argc, char** argv) {
  if ( argc > 0 )   {
    DetectorBuildType type = BUILD_DEFAULT;
    string input = argv[0];
    if ( argc > 1 )  {
      type = buildType(argv[1]);
      printout(INFO,"XMLLoader","+++ Processing XML file: %s with flag %s",
               input.c_str(), argv[1]);
      description.fromXML(input,type);
      return 1;
    }
    printout(INFO,"XMLLoader","+++ Processing XML file: %s",input.c_str());
    description.fromXML(input);
    return 1;
  }
  return 0;
}
DECLARE_APPLY(DD4hep_XMLLoader,load_xml)

/// Basic entry point to process any pre-parsed XML document.
/**
 *  - The handle to the XML element (XercesC DOMNode) 
 *    is passed as first argument to the call.
 *  - The processing hint (build type) is passed as optional 
 *    second argument.
 *
 *  The root tag defines the plugin to interprete it.
 *
 *  Factory: DD4hep_XMLProcessor
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long process_xml_doc(Detector& description, int argc, char** argv) {
  if ( argc > 0 )   {
    DetectorBuildType type = BUILD_DEFAULT;
    DetectorImp* imp = dynamic_cast<DetectorImp*>(&description);
    if ( imp )  {
      xml::XmlElement* h = (xml::XmlElement*)argv[0];
      xml::Handle_t input(h);
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
DECLARE_APPLY(DD4hep_XMLProcessor,process_xml_doc)

/// Basic entry point to load the volume manager object
/**
 *  Factory: DD4hep_VolumeManager
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long load_volmgr(Detector& description, int, char**) {
  printout(INFO,"DD4hepVolumeManager","**** running plugin DD4hepVolumeManager ! " );
  try {
    DetectorImp* imp = dynamic_cast<DetectorImp*>(&description);
    if ( imp )  {
      imp->imp_loadVolumeManager();
      printout(INFO,"VolumeManager","+++ Volume manager populated and loaded.");
      return 1;
    }
  }
  catch (const exception& e) {
    throw runtime_error(string(e.what()) + "\n"
                        "dd4hep: while programming VolumeManager. Are your volIDs correct?");
  }
  catch (...) {
    throw runtime_error("UNKNOWN exception while programming VolumeManager. Are your volIDs correct?");
  }
  return 0;
}
DECLARE_APPLY(DD4hep_VolumeManager,load_volmgr)
DECLARE_APPLY(DD4hepVolumeManager,load_volmgr)

/// Basic entry point to dump a dd4hep geometry to a ROOT file
/**
 *  Factory: DD4hep_Geometry2ROOT
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long dump_geometry2root(Detector& description, int argc, char** argv) {
  if ( argc > 0 )   {
    string output;
    for(int i = 0; i < argc && argv[i]; ++i)  {
      if ( 0 == ::strncmp("-output",argv[i],4) )
        output = argv[++i];
    }
    if ( output.empty() )   {
      cout <<
        "Usage: -plugin <name> -arg [-arg]                                             \n"
        "     name:   factory name     DD4hepGeometry2ROOT                             \n"
        "     -output <string>         Output file name.                               \n"
        "\tArguments given: " << arguments(argc,argv) << endl << flush;
      ::exit(EINVAL);
    }
    printout(INFO,"Geometry2ROOT","+++ Dump geometry to root file:%s",output.c_str());
    //description.manager().Export(output.c_str()+1);
    if ( DD4hepRootPersistency::save(description,output.c_str(),"Geometry") > 1 )  {
      return 1;
    }
  }
  printout(ERROR,"Geometry2ROOT","+++ No output file name given.");
  return 0;
}
DECLARE_APPLY(DD4hep_Geometry2ROOT,dump_geometry2root)

/// Basic entry point to load a dd4hep geometry directly from the ROOT file
/**
 *  Factory: DD4hep_RootLoader
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long load_geometryFromroot(Detector& description, int argc, char** argv) {
  if ( argc > 0 )   {
    string input = argv[0];  // <-- for backwards compatibility
    for(int i = 0; i < argc && argv[i]; ++i)  {
      if ( 0 == ::strncmp("-input",argv[i],4) )
        input = argv[++i];
    }
    if ( input.empty() )   {
      cout <<
        "Usage: DD4hep_RootLoader -arg [-arg]                                          \n"
        "     name:   factory name     DD4hepGeometry2ROOT                             \n"
        "     -input  <string>         Input file name.                                \n"
        "\tArguments given: " << arguments(argc,argv) << endl << flush;
      ::exit(EINVAL);
    }
    printout(INFO,"DD4hepRootLoader","+++ Read geometry from root file:%s",input.c_str());
    if ( 1 == DD4hepRootPersistency::load(description,input.c_str(),"Geometry") )  {
      return 1;
    }
  }
  printout(ERROR,"DD4hep_RootLoader","+++ No input file name given.");
  return 0;
}
DECLARE_APPLY(DD4hep_RootLoader,load_geometryFromroot)

/// Basic entry point to check sensitive detector strictures
/**
 *  Factory: DD4hep_CheckDetectors
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long check_detectors(Detector& description, int /* argc */, char** /* argv */) {
  DD4hepRootCheck check(&description);
  return check.checkDetectors();
}
DECLARE_APPLY(DD4hep_CheckDetectors,check_detectors)

/// Basic entry point to check sensitive detector strictures
/**
 *  Factory: DD4hepCheckSensitives
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long check_sensitives(Detector& description, int /* argc */, char** /* argv */) {
  DD4hepRootCheck check(&description);
  return check.checkSensitives();
}
DECLARE_APPLY(DD4hep_CheckSensitives,check_sensitives)

/// Basic entry point to check sensitive detector strictures
/**
 *  Factory: DD4hep_CheckSegmentations
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long check_segmentations(Detector& description, int /* argc */, char** /* argv */) {
  DD4hepRootCheck check(&description);
  return check.checkSegmentations();
}
DECLARE_APPLY(DD4hep_CheckSegmentations,check_segmentations)

/// Basic entry point to check sensitive detector strictures
/**
 *  Factory: DD4hep_CheckReadouts
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long check_readouts(Detector& description, int /* argc */, char** /* argv */) {
  DD4hepRootCheck check(&description);
  return check.checkReadouts();
}
DECLARE_APPLY(DD4hep_CheckReadouts,check_readouts)

/// Basic entry point to check IDDescriptors of the detector object
/**
 *  Factory: DD4hep_CheckIdspecs
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long check_idspecs(Detector& description, int /* argc */, char** /* argv */) {
  DD4hepRootCheck check(&description);
  return check.checkIdSpecs();
}
DECLARE_APPLY(DD4hep_CheckIdspecs,check_idspecs)

/// Basic entry point to check IDDescriptors of the detector object
/**
 *  Factory: DD4hep_CheckVolumeManager
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long check_volumemanager(Detector& description, int /* argc */, char** /* argv */) {
  DD4hepRootCheck check(&description);
  return check.checkVolManager();
}
DECLARE_APPLY(DD4hep_CheckVolumeManager,check_volumemanager)

/// Basic entry point to check IDDescriptors of the detector object
/**
 *  Factory: DD4hep_CheckNominals
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long check_nominals(Detector& description, int /* argc */, char** /* argv */) {
  DD4hepRootCheck check(&description);
  return check.checkNominals();
}
DECLARE_APPLY(DD4hep_CheckNominals,check_nominals)

/// Basic entry point to print out the volume hierarchy
/**
 *  Factory: DD4hep_VolumeDump
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long dump_volume_tree(Detector& description, int argc, char** argv) {
  struct Actor {
    Detector&        description;
    bool             m_printPathes         = false;
    bool             m_printVolIDs         = false;
    bool             m_printShapes         = false;
    bool             m_printPointers       = false;
    bool             m_printPositions      = false;
    bool             m_printMaterials      = false;
    bool             m_printSensitivesOnly = false;
    long             m_printMaxLevel       = 999999;
    long             m_numNodes            = 0;
    long             m_numShapes           = 0;
    long             m_numSensitive        = 0;
    long             m_numMaterial         = 0;
    long             m_numMaterialERR      = 0;
    bool             m_topStat             = false;
    std::string      m_detector;
    map<string,long> top_counts;
    string           currTop;
    
    Actor(Detector& desc, int ac, char** av) : description(desc)  {
      m_detector = "/world";
      for(int i=0; i<ac; ++i)  {
        char c = ::tolower(av[i][0]);
        char* p = av[i];
        if ( c == '-' ) { ++p; c = ::tolower(av[i][1]); }
        if ( c == '-' ) { ++p; c = ::tolower(av[i][1]); }
        if      ( ::strncmp(p,"volume_ids",3) == 0  ) m_printVolIDs         = true;
        else if ( ::strncmp(p,"level",3)      == 0  ) m_printMaxLevel       = ::atol(av[++i]);
        else if ( ::strncmp(p,"materials",3)  == 0  ) m_printMaterials      = true;
        else if ( ::strncmp(p,"pathes",3)     == 0  ) m_printPathes         = true;
        else if ( ::strncmp(p,"positions",3)  == 0  ) m_printPositions      = true;
        else if ( ::strncmp(p,"pointers",3)   == 0  ) m_printPointers       = true;
        else if ( ::strncmp(p,"shapes",3)     == 0  ) m_printShapes         = true;
        else if ( ::strncmp(p,"sensitive",3)  == 0  ) m_printSensitivesOnly = true;
        else if ( ::strncmp(p,"topstats",3)   == 0  ) m_topStat             = true;
        else if ( ::strncmp(p,"detector",3)   == 0  ) m_detector            = av[++i];
        else if ( ::strncmp(p,"help",3)       == 0  )   {
          cout <<
            "Usage: -plugin <name> -arg [-arg]                                                   \n"
            "     -detector <string> Top level DetElement path. Default: '/world'                \n"
            "     -pathes            Print DetElement pathes                                     \n"
            "     -level    <number> Maximal depth to be explored by the scan                    \n"
            "     -positions         Print placement positions                                   \n"
            "     -volume_ids        Print placement volume IDs                                  \n"
            "     -materials         Print volume materials                                      \n"       
            "     -pointers          Debug: Print pointer values                                 \n"       
            "     -shapes            Print shape information                                     \n"
            "     -sensitive         Only print information for sensitive volumes                \n"
            "     -topstats          Print statistics about top level node                       \n"       
            "\tArguments given: " << arguments(ac,av) << endl << flush;
        }
      }
      if ( m_printMaxLevel < 999999 )
        printout(ALWAYS,"VolumeDump","+++ Maximal print level:   %ld",m_printMaxLevel);
      if ( !m_detector.empty() )
        printout(ALWAYS,"VolumeDump","+++ Subdetector path:      %s",m_detector.c_str());
      printout(ALWAYS,"VolumeDump","+++ Printing positions:    %s",yes_no(m_printPositions));
      printout(ALWAYS,"VolumeDump","+++ Printing shapes:       %s",yes_no(m_printShapes));
      printout(ALWAYS,"VolumeDump","+++ Printing materials:    %s",yes_no(m_printMaterials));
      printout(ALWAYS,"VolumeDump","+++ Printing volume ids:   %s",yes_no(m_printVolIDs));
      printout(ALWAYS,"VolumeDump","+++ Print only sensitives: %s",yes_no(m_printSensitivesOnly));
    }
    ~Actor()  {
      printout(ALWAYS,"VolumeDump",
               "+++ Checked %ld physical volume placements.     %3ld are sensitive.",
               m_numNodes, m_numSensitive);
      if ( m_printMaterials )  {
        printout(ALWAYS,"VolumeDump",
                 "+++ Checked %ld materials in volume placements. %3ld are BAD.",
                 m_numMaterial, m_numMaterialERR);
      }
      if ( m_printShapes )   {
        printout(ALWAYS,"VolumeDump","+++ Checked %ld shapes.", m_numShapes);
      }
      if ( m_topStat )  {
        for(const auto& t : top_counts)  {
          if ( t.second > 1 )
            printout(ALWAYS,"VolumeDump",
                     "+++     Top node: %-32s     %8ld placements.",t.first.c_str(),t.second);
        }
      }
    }

    long dump(string prefix, TGeoNode* ideal, TGeoNode* aligned, int level, PlacedVolume::VolIDs volids)  {
      char fmt[128];
      stringstream log;
      PlacedVolume pv(ideal);
      bool sensitive = false;
      string opt_info, pref = prefix;

      if ( level >= m_printMaxLevel )    {
        return 1;
      }
      ++m_numNodes;
      if ( level == 0 )
        currTop = "";
      else if ( level == 1 )   {
        currTop = ideal->GetVolume()->GetName();
        ++top_counts[currTop];
      }
      else if ( level > 1 )   {
        ++top_counts[currTop];
      }

      if ( m_printPathes )   {
        pref += "/";
        pref += aligned->GetName();
      }
      if ( m_printPositions || m_printVolIDs )  {
        if ( m_printPointers )    {
          if ( ideal != aligned )
            ::snprintf(fmt,sizeof(fmt),"Ideal:%p Aligned:%p ",(void*)ideal,(void*)aligned);
          else
            ::snprintf(fmt,sizeof(fmt),"Ideal:%p ",(void*)ideal);
          log << fmt;
        }
        // Top level volume! have no volume ids
        if ( m_printVolIDs && ideal && ideal->GetMotherVolume() )  {
          PlacedVolume::VolIDs vid = pv.volIDs();
          if ( !vid.empty() )  {
            sensitive = true;
            log << " VolID: ";
            volids.insert(volids.end(),vid.begin(),vid.end());
            for( const auto& i : volids )  {
              ::snprintf(fmt, sizeof(fmt), "%s:%2d ",i.first.c_str(),i.second);
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
        opt_info = log.str();
      }
      TGeoVolume* volume = ideal ? ideal->GetVolume() : 0;
      if ( !m_printSensitivesOnly || (m_printSensitivesOnly && sensitive) )  {
        char sens = pv.volume().isSensitive() ? 'S' : ' ';
        if ( m_printPointers )    {
          if ( ideal == aligned )  {
            ::snprintf(fmt,sizeof(fmt),"%03d %%s [Ideal:%p] %%-%ds %%-16s (%%s) \t %c %%s",
                       level+1,(void*)ideal,2*level+1,sens);
          }
          else  {
            ::snprintf(fmt,sizeof(fmt),"%03d %%s Ideal:%p Aligned:%p %%-%ds %%-16s (%%s) %c %%s",
                       level+1,(void*)ideal,(void*)aligned,2*level+1,sens);
          }
        }
        else  {
          if ( ideal == aligned )  {
            ::snprintf(fmt,sizeof(fmt),"%03d %%s %%-%ds %%-16s (%%s) \t %c %%s",
                       level+1,2*level+1,sens);
          }
          else  {
            ::snprintf(fmt,sizeof(fmt),"%03d %%s Ideal:%p Aligned:%p %%-%ds %%-16s (%%s) %c %%s",
                       level+1,(void*)ideal,(void*)aligned,2*level+1,sens);
          }
        }
        printout(INFO,"VolumeDump",fmt,pref.c_str(),"",
                 aligned->GetName(),
                 volume ? volume->GetShape()->IsA()->GetName() : "[Invalid Volume]",
                 opt_info.c_str());
        if ( sens == 'S' ) ++m_numSensitive;
      }
      if ( m_printMaterials )   {
        Volume   vol = pv.volume();
        Material mat = vol.material();
        TGeoMaterial* mptr = mat->GetMaterial();
        bool ok = mat.A() == mptr->GetA() && mat.Z() == mptr->GetZ();
        ::snprintf(fmt,sizeof(fmt),"%03d %%s %%-%ds Material: %%-16s A:%%f %%f   Z:%%f %%f",
                   level+1,2*level+1);
        ++m_numMaterial;
        if ( !ok ) ++m_numMaterialERR;
        printout(ok ? INFO : ERROR, "VolumeDump", fmt,
                 "  ->", "", mat.name(), mat.A(), mptr->GetA(), mat.Z(), mptr->GetZ());
      }
      log.str("");
      if ( m_printShapes )   {
        log << "Shape: " << toStringSolid(pv.volume().solid()) << " \t";
      }
      if ( m_printPositions )  {
        if ( ideal )  {
          const double* trans = ideal->GetMatrix()->GetTranslation();
          ::snprintf(fmt, sizeof(fmt), "Pos: (%f,%f,%f) ",trans[0],trans[1],trans[2]);
        }
        else  {
          ::snprintf(fmt, sizeof(fmt), " <ERROR: INVALID Translation matrix> ");
        }
        log << fmt << " \t";
      }
      if ( !log.str().empty() )  {
        ::snprintf(fmt,sizeof(fmt),"%03d %%s %%-%ds %%s",level+1,2*level+1);
        printout(INFO, "VolumeDump", fmt, "  ->", "", log.str().c_str());
      }
      for (Int_t idau = 0, ndau = aligned->GetNdaughters(); idau < ndau; ++idau)  {
        if ( ideal )   {
          TGeoNode*   ideal_daughter   = ideal->GetDaughter(idau);
          const char* daughter_name    = ideal_daughter->GetName();
          TGeoNode*   aligned_daughter = volume->GetNode(daughter_name);
          dump(pref, ideal_daughter, aligned_daughter, level+1, volids);
        }
        else  {
          printout(ERROR,"VolumeDump"," <ERROR: INVALID IDEAL Translation matrix>: %s",aligned->GetName());
        }
      }
      return 1;
    }
    int operator()()   {
      PlacedVolume pv;
      DetElement   top = description.world();
      detail::tools::PlacementPath path;

      if ( m_detector != "/world" )   {
        top = detail::tools::findElement(description,m_detector);
        if ( !top.isValid() )  {
          except("DD4hep_GeometryDisplay","+++ Invalid DetElement path: %s",m_detector.c_str());
        }
      }
      if ( !top.placement().isValid() )   {
        except("DD4hep_GeometryDisplay","+++ Invalid DetElement placement: %s",m_detector.c_str());
      }
      string place = top.placementPath();
      detail::tools::placementPath(top, path);
      pv = detail::tools::findNode(description.world().placement(),place);
      if ( !pv.isValid() )   {
        except("DD4hep_GeometryDisplay","+++ Invalid placement verification for placement:%s",place.c_str());
      }
      return this->dump("", top.placement().ptr(), pv.ptr(), 0, PlacedVolume::VolIDs());
    }
  };
  Actor actor(description, argc, argv);
  return actor();
}
DECLARE_APPLY(DD4hep_VolumeDump,dump_volume_tree)

// ======================================================================================
/// Plugin function: Apply arbitrary functor callback on the tree of detector elements
/**
 *  Factory: dd4hep_DetElementProcessor
 *
 *  Invokation: -plugin dd4hep_DetElementProcessor 
 *                      -detector /path/to/detElement (default: /world)
 *                      -processor <factory-name> <processor-args>
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    18/11/2016
 */
static int detelement_processor(Detector& description, int argc, char** argv)   {
  bool       recursive = true;
  ProcessorArgs args(argc, argv);
  DetElement det = description.world();
  unique_ptr<DetectorProcessor> proc(dd4hep::createProcessor<DetectorProcessor>(description, args.argc, &args.argv[0]));

  for(int i=0; i<argc; ++i)  {
    if ( i >= args.start && i <= args.end )
      continue;
    else if ( 0 == ::strncmp(argv[i],"-recursive",4) )
      recursive = true;
    else if ( 0 == ::strncmp(argv[i],"-no-recursive",7) )
      recursive = false;
    else if ( 0 == ::strncmp(argv[i],"-detector",4) )  {
      string path = argv[++i];
      det = detail::tools::findElement(description, path);
      if ( det.isValid() )   {
        continue;
      }
      except("DetElementProcessor",
             "++ The detector element path:%s is not part of this description!",
             path.c_str());
    }
    else  {
      except("DetElementProcessor","++ Unknown plugin argument: %s",argv[i]);
    }
  }
  return DetectorScanner().scan(*proc, det, 0, recursive);
}
DECLARE_APPLY(DD4hep_DetElementProcessor,detelement_processor)

// ======================================================================================
/// Plugin function: Apply arbitrary functor callback on the tree of detector elements
/**
 *  Factory: dd4hep_DetElementProcessor
 *
 *  Invokation: -plugin dd4hep_DetElementProcessor 
 *                      -detector /path/to/detElement (default: /world)
 *                      -processor <factory-name> <processor-args>
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    18/11/2016
 */
static int placed_volume_processor(Detector& description, int argc, char** argv)   {
  bool         recursive = true;
  PlacedVolume pv = description.world().placement();
  ProcessorArgs args(argc, argv);
  unique_ptr<PlacedVolumeProcessor> proc(dd4hep::createProcessor<PlacedVolumeProcessor>(description, args.argc, &args.argv[0]));
  
  for(int i=0; i<argc; ++i)  {
    if ( i >= args.start && i <= args.end )
      continue;
    else if ( 0 == ::strncmp(argv[i],"-recursive",4) )
      recursive = true;
    else if ( 0 == ::strncmp(argv[i],"-no-recursive",7) )
      recursive = false;
    else if ( 0 == ::strncmp(argv[i],"-detector",4) )  {
      string     path = argv[++i];
      DetElement det  = detail::tools::findElement(description, path);
      if ( det.isValid() )  {
        pv = det.placement();
        if ( pv.isValid() )  {
          continue;
        }
        except("PlacedVolumeProcessor",
               "++ The detector element with path:%s has no valid placement!",
               path.c_str());
      }
      except("PlacedVolumeProcessor",
             "++ The detector element path:%s is not part of this description!",
             path.c_str());
    }
    else  {
      except("PlacedVolumeProcessor","++ Unknown plugin argument: %s",argv[i]);
    }
  }
  return PlacedVolumeScanner().scanPlacements(*proc, pv, 0, recursive);
}
DECLARE_APPLY(DD4hep_PlacedVolumeProcessor,placed_volume_processor)

/// Basic entry point to print out the detector element hierarchy
/**
 *  Factory: DD4hep_DetectorDump, DD4hep_DetectorVolumeDump
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
template <int flag> long dump_detelement_tree(Detector& description, int argc, char** argv) {
  struct Actor {
    string path;
    long count = 0;
    int have_match = -1, analysis_level = 999999;
    bool dump_materials = false, dump_shapes = false, dump_positions = false;
    bool sensitive_only = false;

    Actor(const string& p, int level, bool mat, bool shap, bool pos, bool sens)
      : path(p), analysis_level(level), dump_materials(mat), dump_shapes(shap), dump_positions(pos), sensitive_only(sens) {}
    ~Actor() {
      printout(ALWAYS,"DetectorDump", "+++ Scanned a total of %ld elements.",count);
    }
    long dump(DetElement de,int level)   {
      const DetElement::Children& c = de.children();
      bool  use_elt = path.empty() || de.path().find(path) != string::npos;
      if ( have_match<0 && use_elt ) have_match = level;
      use_elt = use_elt && (level-have_match)<=analysis_level;
      if ( use_elt )   {
        if ( !sensitive_only || 0 != de.volumeID() )  {
          PlacedVolume place = de.placement();
          char sens = place.isValid() ? place.volume().isSensitive() ? 'S' : ' ' : ' ';
          char fmt[128];
          switch(flag)  {
          case 0:
            ++count;
            if ( de.placement() == de.idealPlacement() )  {
              ::snprintf(fmt,sizeof(fmt),"%03d %%-%ds %%s NumDau:%%d VolID:%%08X Place:%%p  %%c",level+1,2*level+1);
              printout(INFO,"DetectorDump",fmt,"",de.path().c_str(),int(c.size()),
                       (unsigned long)de.volumeID(), (void*)place.ptr(), sens);
              break;
            }
            ::snprintf(fmt,sizeof(fmt),"%03d %%-%ds %%s NumDau:%%d VolID:%%08X Place:%%p [ideal:%%p aligned:%%p]  %%c",
                       level+1,2*level+1);
            printout(INFO,"DetectorDump",fmt,"",de.path().c_str(),int(c.size()),
                     (unsigned long)de.volumeID(), (void*)de.idealPlacement().ptr(),
                     (void*)place.ptr(), sens);
            break;
          case 1:
            ++count;
            ::snprintf(fmt,sizeof(fmt),
                       "%03d %%-%ds Detector: %%s NumDau:%%d VolID:%%p",
                       level+1,2*level+1);
            printout(INFO,"DetectorDump", fmt, "", de.path().c_str(), int(c.size()), (void*)de.volumeID());
            if ( de.placement() == de.idealPlacement() )  {
              ::snprintf(fmt,sizeof(fmt),
                         "%03d %%-%ds Placement: %%s  %%c",
                         level+1,2*level+3);
              printout(INFO,"DetectorDump",fmt,"", de.placementPath().c_str(), sens);
              break;
            }
            ::snprintf(fmt,sizeof(fmt),
                       "%03d %%-%ds Placement: %%s  [ideal:%%p aligned:%%p] %%c",
                       level+1,2*level+3);
            printout(INFO,"DetectorDump",fmt,"", de.placementPath().c_str(),
                     (void*)de.idealPlacement().ptr(), (void*)place.ptr(), sens);          
            break;
          default:
            break;
          }
          if ( (dump_materials || dump_shapes) && place.isValid() )  {
            Volume vol = place.volume();
            Material mat = vol.material();
            ::snprintf(fmt,sizeof(fmt), "%03d %%-%ds Material: %%-12s Shape: %%s", level+1,2*level+3);
            printout(INFO,"DetectorDump",fmt,"", mat.name(), toStringSolid(vol->GetShape()).c_str());
          }
          if ( dump_positions && place.isValid() )  {
            Position pos = place.position();
            Box      box = place.volume().solid();
            double   loc[3] = {0,0,0}, world[3] = {0,0,0};
            TGeoHMatrix   tr = de.nominal().worldTransformation();
            tr.LocalToMaster(loc, world);
            ::snprintf(fmt,sizeof(fmt), "%03d %%-%ds BBox:     (%%9.4f,%%9.4f,%%9.4f) [cm]", level+1,2*level+3);
            printout(INFO,"DetectorDump",fmt,"", box.x(), box.y(), box.z());
            ::snprintf(fmt,sizeof(fmt), "%03d %%-%ds Position: (%%9.4f,%%9.4f,%%9.4f) [cm] w/r to mother", level+1,2*level+3);
            printout(INFO,"DetectorDump",fmt,"", pos.X(), pos.Y(), pos.Z());
            ::snprintf(fmt,sizeof(fmt), "%03d %%-%ds Position: (%%9.4f,%%9.4f,%%9.4f) [cm] w/r to world",  level+1,2*level+3);
            printout(INFO,"DetectorDump",fmt,"", world[0], world[1], world[2]);
          }
        }
      }
      for (DetElement::Children::const_iterator i = c.begin(); i != c.end(); ++i)
        dump((*i).second,level+1);
      return 1;
    }
  };
  int  level = 999999;
  bool sensitive_only = false, materials = false, shapes = false, positions = false;
  string path;
  for(int i=0; i<argc; ++i)  {
    if      ( ::strncmp(argv[i],"--sensitive",     5)==0 ) { sensitive_only = true;    }
    else if ( ::strncmp(argv[i], "-sensitive",     5)==0 ) { sensitive_only = true;    }
    else if ( ::strncmp(argv[i], "--no-sensitive", 8)==0 ) { sensitive_only = false;   }
    else if ( ::strncmp(argv[i], "-materials",     4)==0 ) { materials = true;         }
    else if ( ::strncmp(argv[i], "--materials",    5)==0 ) { materials = true;         }
    else if ( ::strncmp(argv[i], "-shapes",        4)==0 ) { shapes = true;            }
    else if ( ::strncmp(argv[i], "--shapes",       5)==0 ) { shapes = true;            }
    else if ( ::strncmp(argv[i], "-positions",     4)==0 ) { positions = true;         }
    else if ( ::strncmp(argv[i], "--positions",    5)==0 ) { positions = true;         }
    else if ( ::strncmp(argv[i], "-no-sensitive",  7)==0 ) { sensitive_only = false;   }
    else if ( ::strncmp(argv[i], "--detector",     5)==0 ) { path = argv[++i];         }
    else if ( ::strncmp(argv[i], "-detector",      5)==0 ) { path = argv[++i];         }
    else if ( ::strncmp(argv[i], "--level",        5)==0 ) { level= ::atol(argv[++i]); }
    else if ( ::strncmp(argv[i], "-level",         5)==0 ) { level= ::atol(argv[++i]); }
    else   {
      cout << "  "
           << (flag==0 ? "DD4hep_DetectorDump" : "DD4hep_DetectorVolumeDump") << " -arg [-arg]     \n"
        "    --sensitive            Process only sensitive volumes.                                \n"
        "    -sensitive             dto.                                                           \n"
        "    --shapes               Print shape information.                                       \n"
        "    -shapes                dto.                                                           \n"
        "    --positions            Print position information.                                    \n"
        "    -positions             dto.                                                           \n"
        "    --materials            Print material information.                                    \n"
        "    -materials             dto.                                                           \n"
        "    --detector   <path>    Process elements only if <path> is part of the DetElement path.\n"
        "    -detector    <path>    dto.                                                           \n"
        "\tArguments given: " << arguments(argc,argv) << endl << flush;        
      ::exit(EINVAL);
    }
  }
  Actor a(path, level, materials, shapes, positions, sensitive_only);
  return a.dump(description.world(),0);
}
DECLARE_APPLY(DD4hep_DetectorDump,dump_detelement_tree<0>)
DECLARE_APPLY(DD4hep_DetectorVolumeDump,dump_detelement_tree<1>)

/// Basic entry point to print out the volume hierarchy
/**
 *  Factory: dd4hepDetElementCache
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long detelement_cache(Detector& description, int , char** ) {
  struct Actor {
    static long cache(DetElement de) {
      const DetElement::Children& c = de.children();
      de.nominal().worldTransformation();
      de.nominal().detectorTransformation();
      de.placementPath();
      de.path();
      for( const auto& i : c ) cache(i.second);
      return 1;
    }
  };
  return Actor::cache(description.world());
}
DECLARE_APPLY(DD4hep_DetElementCache,detelement_cache)

/// Basic entry point to dump the geometry tree of the description instance
/**
 *  Factory: DD4hep_GeometryTreeDump
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
#include "../GeometryTreeDump.h"
static long exec_GeometryTreeDump(Detector& description, int, char** ) {
  GeometryTreeDump dmp;
  dmp.create(description.world());
  return 1;
}
DECLARE_APPLY(DD4hep_GeometryTreeDump,exec_GeometryTreeDump)

/// Basic entry point to dump the geometry in GDML format
/**
 *  Factory: dd4hepSimpleGDMLWriter
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
#include "../SimpleGDMLWriter.h"
static long exec_SimpleGDMLWriter(Detector& description, int argc, char** argv) {
  if ( argc > 1 )   {
    string output = argv[1];
    ofstream out(output.c_str()+1,ios_base::out);
    SimpleGDMLWriter dmp(out);
    dmp.create(description.world());
  }
  else   {
    SimpleGDMLWriter dmp(cout);
    dmp.create(description.world());
  }
  return 1;
}
DECLARE_APPLY(DD4hep_SimpleGDMLWriter,exec_SimpleGDMLWriter)

/// Basic entry point to print out detector type map
/**
 *  Factory: DD4hep_DetectorTypes
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long detectortype_cache(Detector& description, int , char** ) {
  vector<string> v = description.detectorTypes();
  printout(INFO,"DetectorTypes","Detector type dump:  %ld types:",long(v.size()));
  for(vector<string>::const_iterator i=v.begin(); i!=v.end(); ++i)   {
    const vector<DetElement>& vv=description.detectors(*i);
    printout(INFO,"DetectorTypes","\t --> %ld %s detectors:",long(vv.size()),(*i).c_str());
    for(vector<DetElement>::const_iterator j=vv.begin(); j!=vv.end(); ++j)
      printout(INFO,"DetectorTypes","\t\t %-16s --> %s  [%s]",(*i).c_str(),(*j).name(),(*j).type().c_str());
  }
  return 1;
}
DECLARE_APPLY(DD4hep_DetectorTypes,detectortype_cache)

/// Basic entry point to print out detector type map
/**
 *  Factory: TestSurfaces
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
#include "DD4hep/SurfaceInstaller.h"
typedef SurfaceInstaller TestSurfacesPlugin;
DECLARE_SURFACE_INSTALLER(TestSurfaces,TestSurfacesPlugin)

/// Basic entry point to print out detector type map
/**
 *  Factory: DD4hep_PluginTester
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
#include "DD4hep/PluginTester.h"
static long install_plugin_tester(Detector& description, int , char** ) {
  PluginTester* test = description.extension<PluginTester>(false);
  if ( !test )  {
    description.addExtension<PluginTester>(new PluginTester());
    printout(INFO,"PluginTester",
             "+++ Successfully installed PluginTester instance to Detector.");
  }
  return 1;
}
DECLARE_APPLY(DD4hep_PluginTester,install_plugin_tester)

