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
#define DD4HEP_MUST_USE_DETECTORIMP_H
#include <DD4hep/Detector.h>
#include <DD4hep/DetectorImp.h>
#include <DD4hep/Memory.h>
#include <DD4hep/DD4hepUI.h>
#include <DD4hep/Factories.h>
#include <DD4hep/Printout.h>
#include <DD4hep/DD4hepUnits.h>
#include <DD4hep/DetectorTools.h>
#include <DD4hep/PluginCreators.h>
#include <DD4hep/VolumeProcessor.h>
#include <DD4hep/DetectorProcessor.h>
#include <DD4hep/DD4hepRootPersistency.h>
#include <XML/DocumentHandler.h>
#include <XML/XMLElements.h>
#include <XML/XMLTags.h>

// ROOT includes
#include <TInterpreter.h>
#include <TGeoElement.h>
#include <TGeoManager.h>
#include <TGeoVolume.h>
#include <TSystem.h>
#include <TClass.h>
#include <TRint.h>
#include <TGDMLMatrix.h>

// C/C++ include files
#include <cerrno>
#include <cstdlib>
#include <sstream>

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
            argv.emplace_back(av[i]);
            return;
          }
          else if ( 0 == ::strncmp(av[i],"-end-plugin",4) )  { // End of current plugin
            argv.emplace_back((char*)"-end-processor");
            return;
          }
          else if ( 0 == ::strncmp(av[i],"-plugin",4) )  {     // Start of next plugin
            argv.emplace_back((char*)"-end-processor");
            return;
          }
          argv.emplace_back(av[i]);
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

/// Basic entry point to create/access the Detector instance
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
/** Invoke the ROOT geometry display using the factory mechanism.
 *
 *  Factory: DD4hep_GeometryDisplay
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long display(Detector& description, int argc, char** argv) {
  TGeoManager& mgr = description.manager();
  int vislevel = 6, visopt = 1;
  std::string detector = "/world";
  const char* opt = "ogl";
  for( int i = 0; i < argc && argv[i]; ++i )  {
    if ( 0 == ::strncmp("-option",argv[i],4) )
      opt = argv[++i];
    else if ( 0 == ::strncmp("-level",argv[i],4) )
      vislevel = ::atol(argv[++i]);
    else if ( 0 == ::strncmp("-visopt",argv[i],4) )
      visopt = ::atol(argv[++i]);
    else if ( 0 == ::strncmp("-detector",argv[i],4) )
      detector = argv[++i];
    else  {
      std::cout <<
        "Usage: -plugin DD4hep_GeometryDisplay  -arg [-arg]                                \n\n"
        "     Invoke the ROOT geometry display using the factory mechanism.                \n\n"
        "     -detector <string> Top level DetElement path. Default: '/world'                \n"
        "     -option   <string> ROOT Draw option.    Default: 'ogl'                         \n"
        "     -level    <number> Visualization level  [TGeoManager::SetVisLevel]  Default: 4 \n"
        "     -visopt   <number> Visualization option [TGeoManager::SetVisOption] Default: 1\n"       
        "     -load              Only load the geometry. Do not invoke the display          \n"
        "     -help              Print this help output  \n"       
        "     Arguments given: " << arguments(argc,argv) << std::endl << std::flush;
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
  std::string lib, func;
  std::vector<char*> args;
  for(int i = 0; i < argc && argv[i]; ++i)  {
    if ( 0 == ::strncmp("-library",argv[i],4) )
      lib = argv[++i];
    else if ( 0 == ::strncmp("-function",argv[i],4) )
      func = argv[++i];
    else
      args.emplace_back(argv[i]);
  }
  if ( lib.empty() || func.empty() )  {
    std::cout <<
      "Usage: -plugin DD4hep_Function -arg [-arg]                        \n\n"
      "       Execute a function without arguments inside a library.     \n\n"
      "     -library   <string>    Library to be loaded                    \n"
      "     -function  <string>    name of the entry point to be executed. \n"
      "\tArguments given: " << arguments(argc,argv) << std::endl << std::flush;
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
    std::pair<int, char**> a(argc,argv);
    gApplication = new TRint("DD4hepRint", &a.first, a.second);
    printout(INFO,"DD4hepRint","++ Created ROOT interpreter instance for DD4hepUI.");
  }
  for(int i=0; i<argc; ++i)   {
    printout(INFO,"DD4hepRint","Excecute[%d]: %s", i, argv[i]);
    gInterpreter->ProcessLine(argv[i]);
  }
  if ( !gApplication->IsRunning() )  {
    gApplication->Run();
  }
  return 1;
}
DECLARE_APPLY(DD4hep_Rint,run_interpreter)

/// Basic entry point to start the ROOT UI
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
  std::snprintf(cmd, sizeof(cmd),
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
static long root_dump_gdml_tables(Detector& description, int /* argc */, char** /* argv */) {
  size_t num_tables = 0;
  size_t num_elements = 0;
  const TObjArray* c = description.manager().GetListOfGDMLMatrices();
  TObjArrayIter arr(c);
  printout(INFO,"Dump_GDMLTables","+++ Dumping known GDML tables from TGeoManager.");
  for(TObject* i = arr.Next(); i; i=arr.Next())   {
    TGDMLMatrix* gdmlMat = (TGDMLMatrix*)i;
    num_elements += (gdmlMat->GetRows()*gdmlMat->GetCols());
    ++num_tables;
    gdmlMat->Print();
  }
  printout(INFO,"Dump_GDMLTables",
           "+++ Successfully dumped %ld GDML tables with %ld elements.",
           num_tables, num_elements);
  return 1;
}
DECLARE_APPLY(DD4hep_Dump_GDMLTables,root_dump_gdml_tables)

/// Basic entry point to dump all known optical surfaces
/**
 *
 *  Factory: DD4hep_Dump_OpticalSurfaces
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long root_dump_optical_surfaces(Detector& description, int /* argc */, char** /* argv */) {
  size_t num_surfaces = 0;
  printout(ALWAYS,"","");
  const TObjArray* c = description.manager().GetListOfOpticalSurfaces();
  TObjArrayIter arr(c);
  printout(ALWAYS,"Dump_OpticalSurfaces","+++ Dumping known Optical Surfaces from TGeoManager.");
  for(TObject* i = arr.Next(); i; i=arr.Next())   {
    TGeoOpticalSurface* optSurt = (TGeoOpticalSurface*)i;
    ++num_surfaces;
    optSurt->Print();
  }
  printout(ALWAYS,"Dump_OpticalSurfaces",
           "+++ Successfully dumped %ld Optical surfaces.",num_surfaces);
  return 1;
}
DECLARE_APPLY(DD4hep_Dump_OpticalSurfaces,root_dump_optical_surfaces)

/// Basic entry point to dump all known skin surfaces
/**
 *
 *  Factory: DD4hep_Dump_SkinSurfaces
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long root_dump_skin_surfaces(Detector& description, int /* argc */, char** /* argv */) {
  size_t num_surfaces = 0;
  printout(ALWAYS,"","");
  const TObjArray* c = description.manager().GetListOfSkinSurfaces();
  TObjArrayIter arr(c);
  printout(ALWAYS,"Dump_SkinSurfaces","+++ Dumping known Skin Surfaces from TGeoManager.");
  for(TObject* i = arr.Next(); i; i=arr.Next())   {
    TGeoSkinSurface* skinSurf = (TGeoSkinSurface*)i;
    ++num_surfaces;
    skinSurf->Print();
  }
  printout(ALWAYS,"Dump_SkinSurfaces",
           "+++ Successfully dumped %ld Skin surfaces.",num_surfaces);
  return 1;
}
DECLARE_APPLY(DD4hep_Dump_SkinSurfaces,root_dump_skin_surfaces)

/// Basic entry point to dump all known border surfaces
/**
 *
 *  Factory: DD4hep_Dump_BorderSurfaces
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long root_dump_border_surfaces(Detector& description, int /* argc */, char** /* argv */) {
  size_t num_surfaces = 0;
  printout(ALWAYS,"","");
  const TObjArray* c = description.manager().GetListOfBorderSurfaces();
  TObjArrayIter arr(c);
  printout(ALWAYS,"Dump_BorderSurfaces","+++ Dumping known Border Surfaces from TGeoManager.");
  for(TObject* i = arr.Next(); i; i=arr.Next())   {
    TGeoBorderSurface* bordSurt = (TGeoBorderSurface*)i;
    ++num_surfaces;
    bordSurt->Print();
  }
  printout(ALWAYS,"Dump_BorderSurfaces",
           "+++ Successfully dumped %ld Border surfaces.",num_surfaces);
  return 1;
}
DECLARE_APPLY(DD4hep_Dump_BorderSurfaces,root_dump_border_surfaces)

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
  using elt_h = xml::Element;

  /// Generic printer object. Calls the print method of TObject
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/04/2014
   */
  struct ElementPrint {
    /// Default constructor
    ElementPrint() = default;
    /// Default destructor
    virtual ~ElementPrint() = default;
    /// Element writer
    virtual void operator()(TGeoElement* elt)  {   elt->Print();    }
    /// Isotope writer
    virtual void operator()(TGeoIsotope* iso)  {   iso->Print();    }
  };

  /// XML printer to produce XML output
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/04/2014
   */
  struct ElementPrintXML : public ElementPrint  {
    /// Root XML element
    elt_h root;
    /// Initializing constructor
    ElementPrintXML(elt_h r) : root(r) {}
    /// Default destructor
    virtual ~ElementPrintXML() {}
    /// Element writer
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
    /// Isotope writer
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

  std::string type = "text", output = "";
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

  xml::Document        doc(0);
  xml::DocumentHandler docH;
  xml::Element         element(0);
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

  using elt_h = xml::Element;

  /// Material printer object
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/04/2014
   */
  struct MaterialPrint {
  public:
    /// Reference to the detector description
    Detector& description;

  public:
    /// Initializing constructor
    MaterialPrint(Detector& desc) : description(desc) {}
    /// Default destructor
    virtual ~MaterialPrint() = default;
    /// Print single material
    virtual elt_h print(TGeoMaterial* mat)  {
      const char* st = "Undefined";
      switch( mat->GetState() )   {
      case TGeoMaterial::kMatStateSolid:     st = "solid";     break;
      case TGeoMaterial::kMatStateLiquid:    st = "liquid";    break;
      case TGeoMaterial::kMatStateGas:       st = "gas";       break;
      case TGeoMaterial::kMatStateUndefined:
      default:                               st = "Undefined"; break;
      }
      ::printf("%-32s %-8s\n", mat->GetName(), mat->IsMixture() ? "Mixture" : "Material");
      ::printf("         Aeff=%7.3f Zeff=%7.4f rho=%8.3f [g/mole] radlen=%8.3g [cm] intlen=%8.3g [cm] index=%3d\n",
               mat->GetA(), mat->GetZ(), mat->GetDensity(),
               mat->GetRadLen(), mat->GetIntLen(), mat->GetIndex());
      ::printf("         Temp=%.3g [Kelvin] Pressure=%.5g [hPa] state=%s\n",
               mat->GetTemperature(), mat->GetPressure()/dd4hep::pascal/100.0, st);
      return elt_h(0);
    }
    /// Print element entry
    virtual void print(elt_h, TGeoElement* elt, double frac)   {
      ::printf("  %-6s Fraction: %7.3f Z=%3d A=%6.2f N=%3d Neff=%6.2f\n",
               elt->GetName(), frac, elt->Z(), elt->A(), elt->N(), elt->Neff());
    }
    /// Print material property
    virtual void printProperty(elt_h, TNamed* prop, TGDMLMatrix* matrix)   {
      if ( matrix )
        ::printf("  Property: %-20s [%ld x %ld] --> %s\n",
                 prop->GetName(), long(matrix->GetRows()), long(matrix->GetCols()), prop->GetTitle());
      else
        ::printf("  Property: %-20s [ERROR: NO TABLE!] --> %s\n",
                 prop->GetName(), prop->GetTitle());
    }
    virtual void operator()(TGeoMaterial* mat)  {
      Double_t* mix = mat->IsMixture() ? ((TGeoMixture*)mat)->GetWmixt() : 0;
      elt_h     mh  = print(mat);
      for( int n=mat->GetNelements(), i=0; i<n; ++i )   {
        TGeoElement* elt = mat->GetElement(i);
        print(mh, elt, mix ? mix[i] : 1);
      }
      TListIter mat_iter(&mat->GetProperties());
      for( TObject* i = mat_iter.Next(); i; i=mat_iter.Next() )   {
        printProperty(mh, (TNamed*)i, description.manager().GetGDMLMatrix(i->GetTitle()));
      }
    }
  };
  /// XML printer to produce XML output
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/04/2014
   */
  struct MaterialPrintXML : public MaterialPrint  {
    elt_h root;
    MaterialPrintXML(elt_h elt, Detector& desc) : MaterialPrint(desc), root(elt) {}
    virtual ~MaterialPrintXML() {}
    virtual elt_h print(TGeoMaterial* mat)  {
      elt_h elt = root.addChild(_U(material));
      elt.setAttr(_U(name),mat->GetName());
      if ( ::strlen(mat->GetTitle())>0 )   {
        elt.setAttr(_U(formula),mat->GetTitle());
      }
      else if ( mat->GetNelements() == 1 )   {
        elt.setAttr(_U(formula),mat->GetElement(0)->GetName());
#if 0
        // We do not need this. It shall be computed by TGeo using the Geant4 formula.
        elt_h RL = elt.addChild(_U(RL));
        RL.setAttr(_U(type),  "X0");
        RL.setAttr(_U(value), mat->GetRadLen());
        RL.setAttr(_U(unit),  "cm");
        elt_h NIL = elt.addChild(_U(NIL));
        NIL.setAttr(_U(type),  "lambda");
        NIL.setAttr(_U(value), mat->GetIntLen());
        NIL.setAttr(_U(unit),  "cm");
#endif
      }
      elt_h D = elt.addChild(_U(D));
      D.setAttr(_U(type),  "density");
      D.setAttr(_U(value), mat->GetDensity());
      D.setAttr(_U(unit),  "g/cm3");
      if ( mat->GetTemperature() != description.stdConditions().temperature )  {
        elt_h T = elt.addChild(_U(T));
        T.setAttr(_U(type), "temperature");
        T.setAttr(_U(value), mat->GetTemperature());
        T.setAttr(_U(unit), "kelvin");
      }
      if ( mat->GetPressure() != description.stdConditions().pressure )  {
        elt_h P = elt.addChild(_U(P));
        P.setAttr(_U(type),  "pressure");
        P.setAttr(_U(value), mat->GetPressure());
        P.setAttr(_U(unit),  "pascal");
      }
      return elt;
    }
    virtual void print(elt_h mat, TGeoElement* element, double frac)  {
      elt_h elt = mat.addChild(_U(composite));
      elt.setAttr(_U(n),frac);
      elt.setAttr(_U(ref),element->GetName());
    }
    virtual void printProperty(elt_h mat, TNamed* prop, TGDMLMatrix* /* matrix */)   {
      elt_h elt = mat.addChild(_U(property));
      elt.setAttr(_U(name),prop->GetName());
      elt.setAttr(_U(ref), prop->GetTitle());
    }
  };

  std::string type = "text", output = "", name = "";
  for(int i=0; i<argc; ++i)  {
    if ( argv[i][0] == '-' )  {
      char c = ::tolower(argv[i][1]);
      if ( c == 't' && i+1<argc ) type = argv[++i];
      else if ( c == 'n' && i+1<argc ) name   = argv[++i];
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

  xml::Document        doc(0);
  xml::DocumentHandler docH;
  xml::Element         element(0);
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
                                    ? new MaterialPrintXML(element, description)
                                    : new MaterialPrint(description));
  TObject* obj = 0;
  TList*   mats = description.manager().GetListOfMaterials();
  dd4hep_ptr<TIterator> iter(mats->MakeIterator());
  while( (obj=iter->Next()) != 0 )  {
    TGeoMaterial* mat = (TGeoMaterial*)obj;
    if ( name.empty() || name == mat->GetName() )
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
    std::string input = argv[0];
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
    std::string input = argv[0];
    if ( argc > 1 )  {
      type = buildType(argv[1]);
      printout(INFO,"XMLLoader","+++ Processing XML file: %s with flag %s",
               input.c_str(), argv[1]);
      description.fromXML(input, type);
      return 1;
    }
    printout(INFO,"XMLLoader","+++ Processing XML file: %s",input.c_str());
    description.fromXML(input, description.buildType());
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
      xml::Handle_t    input(h);
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
  catch (const std::exception& e) {
    except("DD4hep_VolumeManager", "Exception: %s\n     %s", e.what(),
           "dd4hep: while programming VolumeManager. Are your volIDs correct?");
  }
  catch (...) {
    except("DD4hep_VolumeManager",
           "UNKNOWN exception while programming VolumeManager. Are your volIDs correct?");
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
    std::string output;
    for(int i = 0; i < argc && argv[i]; ++i)  {
      if ( 0 == ::strncmp("-output",argv[i],4) )
        output = argv[++i];
    }
    if ( output.empty() )   {
      std::cout <<
        "Usage: -plugin DD4hep_Geometry2ROOT -arg [-arg]                             \n\n"
        "     Output DD4hep detector description object to a ROOT file.              \n\n"
        "     -output <string>         Output file name.                               \n"
        "\tArguments given: " << arguments(argc,argv) << std::endl << std::flush;
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
    std::string input = argv[0];  // <-- for backwards compatibility
    for(int i = 0; i < argc && argv[i]; ++i)  {
      if ( 0 == ::strncmp("-input",argv[i],4) )
        input = argv[++i];
    }
    if ( input.empty() )   {
      std::cout <<
        "Usage: DD4hep_RootLoader -arg [-arg]                                        \n\n"
        "     Load DD4hep detector description from ROOT file to memory.             \n\n"
        "     -input  <string>         Input file name.                                \n"
        "\tArguments given: " << arguments(argc,argv) << std::endl << std::flush;
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

/// Basic entry point to dump a dd4hep geometry as TGeo to a ROOT file
/**
 *  Factory: DD4hep_Geometry2TGeo
 *
 *  \author  W.Deconinck
 *  \version 1.0
 *  \date    14/09/2022
 */
static long dump_geometry2tgeo(Detector& description, int argc, char** argv) {
  if ( argc > 0 )   {
    std::string output( argc == 1 ? argv[0] : "" );
    printout(INFO,"Geometry2TGeo","+++ output: %d %s", argc, output.c_str());
    for(int i = 0; i < argc && argv[i]; ++i)  {
      if ( 0 == ::strncmp("-output",argv[i],4) )
        output = argv[++i];
    }
    if ( output.empty() )   {
      std::cout <<
        "Usage: -plugin <name> -arg [-arg]                                             \n"
        "     Output TGeo information to a ROOT file.                                \n\n"
        "     name:   factory name     DD4hepGeometry2TGeo                             \n"
        "     -output <string>         Output file name.                               \n"
        "\tArguments given: " << arguments(argc,argv) << std::endl << std::flush;
      ::exit(EINVAL);
    }
    printout(INFO,"Geometry2TGeo","+++ Dump geometry to root file:%s",output.c_str());
    if ( description.manager().Export(output.c_str()) > 1 ) {
      return 1;
    }
  }
  printout(ERROR,"Geometry2TGeo","+++ No output file name given.");
  return 0;
}
DECLARE_APPLY(DD4hep_Geometry2TGeo,dump_geometry2tgeo)
DECLARE_APPLY(DD4hepGeometry2TGeo,dump_geometry2tgeo)

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
  auto ret = check.checkDetectors();
  return ret.first > 0 && ret.second == 0 ? 1 : 0;
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
  auto ret = check.checkSensitives();
  return ret.first > 0 && ret.second == 0 ? 1 : 0;
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
  auto ret = check.checkSegmentations();
  return ret.first > 0 && ret.second == 0 ? 1 : 0;
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
  auto ret = check.checkReadouts();
  return ret.first > 0 && ret.second == 0 ? 1 : 0;
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
  auto ret = check.checkIdSpecs();
  return ret.first > 0 && ret.second == 0 ? 1 : 0;
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
  auto ret = check.checkVolManager();
  return ret.first > 0 && ret.second == 0 ? 1 : 0;
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
  auto ret = check.checkNominals();
  return ret.first > 0 && ret.second == 0 ? 1 : 0;
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
    bool             m_printVis            = false;
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
    std::string                currTop;
    std::map<std::string,long> top_counts;
    
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
        else if ( ::strncmp(p,"vis",3)        == 0  ) m_printVis            = true;
        else if ( ::strncmp(p,"detector",3)   == 0  ) m_detector            = av[++i];
        else if ( ::strncmp(p,"help",3)       == 0  )   {
          std::cout <<
            "Usage: -plugin <name> -arg [-arg]                                                   \n"
            "     -detector <string> Top level DetElement path. Default: '/world'                \n"
            "     -pathes            Print DetElement pathes                                     \n"
            "     -level    <number> Maximal depth to be explored by the scan                    \n"
            "     -positions         Print placement positions                                   \n"
            "     -volume_ids        Print placement volume IDs                                  \n"
            "     -materials         Print volume materials                                      \n"       
            "     -pointers          Debug: Print pointer values                                 \n"       
            "     -shapes            Print shape information                                     \n"
            "     -vis               Print visualisation attribute name(s) if present            \n"
            "     -sensitive         Only print information for sensitive volumes                \n"
            "     -topstats          Print statistics about top level node                       \n"       
            "\tArguments given: " << arguments(ac,av) << std::endl << std::flush;
          _exit(0);
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
      printout(ALWAYS,"VolumeDump","+++ Printing visattrs:     %s",yes_no(m_printVis));
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

    long dump(std::string prefix, TGeoNode* ideal, TGeoNode* aligned, int level, PlacedVolume::VolIDs volids)  {
      char fmt[128];
      std::stringstream log;
      PlacedVolume pv(ideal);
      bool sensitive = false;
      std::string opt_info, pref = std::move(prefix);

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
      /// 
      TGeoVolume* mother = ideal ? ideal->GetMotherVolume() : nullptr;
      if ( m_printPositions || m_printVolIDs )  {
        if ( m_printPointers )    {
          if ( ideal != aligned )
            std::snprintf(fmt,sizeof(fmt),"Ideal:%p Aligned:%p ",
                          (void*)ideal,(void*)aligned);
          else
            std::snprintf(fmt,sizeof(fmt),"Ideal:%p MotherVol:%p",
                          (void*)ideal, (void*)mother);
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
              std::snprintf(fmt, sizeof(fmt), "%s:%2d ",i.first.c_str(),i.second);
              log << fmt;
            }
            if ( !vid.empty() || pv.volume().isSensitive() )  {
              SensitiveDetector sd = pv.volume().sensitiveDetector();
              if ( sd.isValid() )  {
                IDDescriptor dsc = sd.readout().idSpec();
                if ( dsc.isValid() )  {
                  log << std::hex << " (0x" << std::setfill('0') << std::setw(8)
                      << dsc.encode(volids)
                      << std::setfill(' ') << std::dec << ") ";
                }
              }
            }
          }
        }
        opt_info = log.str();
        log.str("");
      }
      ///  
      if ( m_printVis && pv.volume().visAttributes().isValid() )   {
        log << " Vis:" << pv.volume().visAttributes().name();
        opt_info += log.str();
        log.str("");
      }
      TGeoVolume* volume = ideal ? ideal->GetVolume() : 0;
      if ( !m_printSensitivesOnly || (m_printSensitivesOnly && sensitive) )  {
        Volume vol = pv.volume();
        char  sens = vol.isSensitive() ? 'S' : ' ';
        if ( m_printPointers )    {
          if ( ideal == aligned )  {
            std::snprintf(fmt,sizeof(fmt),"%03d %%s [Ideal:%p Vol:%p MotherVol:%p] %%-%ds %%-16s Vol:%%s shape:%%s \t %c %%s",
                          level+1,(void*)ideal,(void*)vol, (void*)mother, 2*level+1, sens);
          }
          else  {
            std::snprintf(fmt,sizeof(fmt),"%03d %%s Ideal:%p Aligned:%p %%-%ds %%-16s Vol:%%s shape:%%s %c %%s",
                          level+1,(void*)ideal,(void*)aligned,2*level+1,sens);
          }
        }
        else  {
          if ( ideal == aligned )  {
            std::snprintf(fmt,sizeof(fmt),"%03d %%s %%-%ds %%-16s Vol:%%s shape:%%s \t %c %%s",
                          level+1,2*level+1,sens);
          }
          else  {
            std::snprintf(fmt,sizeof(fmt),"%03d %%s Ideal:%p Aligned:%p %%-%ds %%-16s Vol:%%s shape:%%s %c %%s",
                          level+1,(void*)ideal,(void*)aligned,2*level+1,sens);
          }
        }
        const auto* sh = volume ? volume->GetShape() : nullptr;
        printout(INFO,"VolumeDump",fmt,pref.c_str(),"",
                 aligned->GetName(),
                 vol.name(),
                 sh ? sh->IsA()->GetName() : "[Invalid Shape]",
                 opt_info.c_str());
        if ( sens == 'S' ) ++m_numSensitive;
      }
      if ( m_printMaterials )   {
        Volume   vol = pv.volume();
        Material mat = vol.material();
        TGeoMaterial* mptr = mat->GetMaterial();
        bool ok = mat.A() == mptr->GetA() && mat.Z() == mptr->GetZ();
        std::snprintf(fmt,sizeof(fmt),"%03d %%s %%-%ds Material: %%-16s A:%%6.2f %%6.2f  Z:%%6.2f %%6.2f",
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
          std::snprintf(fmt, sizeof(fmt), "Pos: (%f,%f,%f) ",trans[0],trans[1],trans[2]);
        }
        else  {
          std::snprintf(fmt, sizeof(fmt), " <ERROR: INVALID Translation matrix> ");
        }
        log << fmt << " \t";
      }
      if ( !log.str().empty() )  {
        std::snprintf(fmt,sizeof(fmt),"%03d %%s %%-%ds %%s",level+1,2*level+1);
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
    ///
    int operator()()   {
      PlacedVolume pv;
      DetElement   top = description.world();
      detail::tools::PlacementPath path;

      if ( m_detector != "/world" )   {
        top = detail::tools::findElement(description,m_detector);
        if ( !top.isValid() )  {
          except("VolumeDump","+++ Invalid DetElement path: %s",m_detector.c_str());
        }
      }
      if ( !top.placement().isValid() )   {
        except("VolumeDump","+++ Invalid DetElement placement: %s",m_detector.c_str());
      }
      std::string place = top.placementPath();
      detail::tools::placementPath(top, path);
      pv = detail::tools::findNode(description.world().placement(),place);
      if ( !pv.isValid() )   {
        except("VolumeDump","+++ Invalid placement verification for placement:%s",place.c_str());
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
  bool          recursive = true;
  ProcessorArgs args(argc, argv);
  DetElement    det = description.world();
  std::unique_ptr<DetectorProcessor>
    proc(dd4hep::createProcessor<DetectorProcessor>(description, args.argc, &args.argv[0]));

  for(int i=0; i<argc; ++i)  {
    if ( i >= args.start && i <= args.end )
      continue;
    else if ( 0 == ::strncmp(argv[i],"-recursive",4) )
      recursive = true;
    else if ( 0 == ::strncmp(argv[i],"-no-recursive",7) )
      recursive = false;
    else if ( 0 == ::strncmp(argv[i],"-detector",4) )  {
      std::string path = argv[++i];
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
  return DetectorScanner().scan(*proc, det, 0, recursive) > 0 ? 1 : 0;
}
DECLARE_APPLY(DD4hep_DetElementProcessor,detelement_processor)

// ======================================================================================
/// Plugin function: Apply arbitrary functor callback on the tree of detector elements
/**
 *  Factory: DD4hep_DetElementProcessor
 *
 *  Invokation: -plugin DD4hep_DetElementProcessor 
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
  std::unique_ptr<PlacedVolumeProcessor>
    proc(dd4hep::createProcessor<PlacedVolumeProcessor>(description, args.argc, &args.argv[0]));

  std::string path = "/world";
  for(int i=0; i<argc; ++i)  {
    if ( i >= args.start && i <= args.end )
      continue;
    else if ( 0 == ::strncmp(argv[i],"-recursive",4) )
      recursive = true;
    else if ( 0 == ::strncmp(argv[i],"--recursive",5) )
      recursive = true;
    else if ( 0 == ::strncmp(argv[i],"-no-recursive",7) )
      recursive = false;
    else if ( 0 == ::strncmp(argv[i],"--no-recursive",8) )
      recursive = false;
    else if ( 0 == ::strncmp(argv[i],"-detector",4) )
      path = argv[++i];
    else if ( 0 == ::strncmp(argv[i],"--detector",5) )
      path = argv[++i];
    else
      except("PlacedVolumeProcessor","++ Unknown plugin argument: %s",argv[i]);
  }
  DetElement det  = detail::tools::findElement(description, path);
  if ( det.isValid() )  {
    pv = det.placement();
    if ( pv.isValid() )  {
      return PlacedVolumeScanner().scanPlacements(*proc, pv, 0, recursive) > 0 ? 1 : 0;
    }
    except("PlacedVolumeProcessor",
           "++ The detector element with path:%s has no valid placement!",
           path.c_str());
  }
  except("PlacedVolumeProcessor",
         "++ The detector element path:%s is not part of this description!",
         path.c_str());
  return 0;
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
  using VolIDs = PlacedVolume::VolIDs;
  /// Local helper class
  struct Actor {
    Detector& descr;
    std::string path     { };
    DetElement det_world { };
    long count          = 0;
    int  have_match     = -1;
    int  analysis_level = 999999;
    bool dump_materials = false;
    bool dump_shapes    = false;
    bool dump_positions = false;
    bool dump_volids    = false;
    bool sensitive_only = false;

    /// Initializing constructor
    Actor(Detector& det) : descr(det) {
      det_world = descr.world();
    }
    /// Default destructor
    ~Actor() {
      printout(ALWAYS,"DetectorDump", "+++ Scanned a total of %ld elements.",count);
    }
    void parse_args(int ac, char** av)   {
      for(int i=0; i<ac; ++i)  {
        if      ( ::strncmp(av[i],"--sensitive",     5)==0 ) { sensitive_only = true;  }
        else if ( ::strncmp(av[i], "-sensitive",     5)==0 ) { sensitive_only = true;  }
        else if ( ::strncmp(av[i], "--no-sensitive", 8)==0 ) { sensitive_only = false; }
        else if ( ::strncmp(av[i], "-materials",     4)==0 ) { dump_materials = true;  }
        else if ( ::strncmp(av[i], "--materials",    5)==0 ) { dump_materials = true;  }
        else if ( ::strncmp(av[i], "-shapes",        4)==0 ) { dump_shapes    = true;  }
        else if ( ::strncmp(av[i], "--shapes",       5)==0 ) { dump_shapes    = true;  }
        else if ( ::strncmp(av[i], "-positions",     4)==0 ) { dump_positions = true;  }
        else if ( ::strncmp(av[i], "--positions",    5)==0 ) { dump_positions = true;  }
        else if ( ::strncmp(av[i], "-no-sensitive",  7)==0 ) { sensitive_only = false; }
        else if ( ::strncmp(av[i], "--volids",       5)==0 ) { dump_volids    = true;  }
        else if ( ::strncmp(av[i], "-volids",        5)==0 ) { dump_volids    = true;  }
        else if ( ::strncmp(av[i], "--detector",     5)==0 ) { path = av[++i];       }
        else if ( ::strncmp(av[i], "-detector",      5)==0 ) { path = av[++i];       }
        else if ( ::strncmp(av[i], "--level",        5)==0 ) { analysis_level = ::atol(av[++i]); }
        else if ( ::strncmp(av[i], "-level",         5)==0 ) { analysis_level = ::atol(av[++i]); }
        else   {
          std::cout << "  "
                    << (flag==0 ? "DD4hep_DetectorDump" : "DD4hep_DetectorVolumeDump") << " -arg [-arg]   \n\n"
                    << "         Dump " << (flag==0 ? "DetElement" : "Detector volume") << " tree.          \n"
                    << "         Configure produced output information using the following options:       \n\n"
            "   --sensitive             Process only sensitive volumes.                                \n"
            "    -sensitive             dto.                                                           \n"
            "   --no-sensitive          Invert sensitive only flag.                                    \n"
            "    -no-sensitive          dto.                                                           \n"
            "   --shapes                Print shape information.                                       \n"
            "    -shapes                dto.                                                           \n"
            "   --positions             Print position information.                                    \n"
            "    -positions             dto.                                                           \n"
            "   --materials             Print material information.                                    \n"
            "    -materials             dto.                                                           \n"
            "   --detector    <path>    Process elements only if <path> is part of the DetElement path.\n"
            "    -detector    <path>    dto.                                                           \n"
            "    -level       <number>  Maximal depth to be explored by the scan                       \n"
            "   --level       <number>  dto.                                                           \n"
            "    -volids                Print volume identifiers of placements.                        \n"
            "   --volids                dto.                                                           \n"
            "\tArguments given: " << arguments(ac,av) << std::endl << std::flush;        
          ::exit(EINVAL);
        }
      }
    }
    IDDescriptor get_id_descriptor(PlacedVolume pv)   {
      if ( pv.isValid() )  {
        Volume v = pv.volume();
        SensitiveDetector sd = v.sensitiveDetector();
        if ( sd.isValid() )   {
          IDDescriptor dsc = sd.readout().idSpec();
          if ( dsc.isValid() ) return dsc;
        }
        for (Int_t idau = 0, ndau = v->GetNdaughters(); idau < ndau; ++idau)  {
          IDDescriptor dsc = get_id_descriptor(v->GetNode(idau));
          if ( dsc.isValid() ) return dsc;
        }
      }
      return IDDescriptor();
    }
    void validate_id_descriptor(DetElement de, IDDescriptor& desc)   {
      desc = (!de.parent() || de.parent() == det_world)
        ? IDDescriptor() : get_id_descriptor(de.placement());
    }

    long dump(DetElement de, int level, IDDescriptor& id_desc, VolIDs chain)   {
      char fmt[256];
      PlacedVolume place = de.placement();
      const DetElement::Children& children = de.children();
      bool use_elt = path.empty() || de.path().find(path) != std::string::npos;

      if ( have_match < 0 && use_elt )  {
        have_match = level;
      }

      use_elt &= ((level-have_match) <= analysis_level);
      if ( !place.isValid() )    {
        std::snprintf(fmt,sizeof(fmt),"%03d %%-%ds %%s DetElement with INVALID PLACEMENT!", level+1, 2*level+1);
        printout(ERROR,"DetectorDump", fmt, "", de.path().c_str());
        use_elt = false;
      }

      if ( place.isValid() && de != det_world )    {
        chain.insert(chain.end(), place.volIDs().begin(), place.volIDs().end());
      }
      if ( use_elt )   {
        if ( !sensitive_only || 0 != de.volumeID() )  {
          char sens = place.isValid() && place.volume().isSensitive() ? 'S' : ' ';
          switch( flag )  {
          case 0:
            ++count;
            if ( de.placement() == de.idealPlacement() )  {
              std::snprintf(fmt, sizeof(fmt), "%03d %%-%ds %%s NumDau:%%d VolID:%%08X Place:%%p  %%c", level+1, 2*level+1);
              printout(INFO, "DetectorDump", fmt, "", de.path().c_str(), int(children.size()),
                       (unsigned long)de.volumeID(), (void*)place.ptr(), sens);
              break;
            }
            std::snprintf(fmt, sizeof(fmt), "%03d %%-%ds %%s NumDau:%%d VolID:%%08X Place:%%p [ideal:%%p aligned:%%p]  %%c",
                          level+1, 2*level+1);
            printout(INFO, "DetectorDump", fmt, "", de.path().c_str(), int(children.size()),
                     (unsigned long)de.volumeID(), (void*)de.idealPlacement().ptr(),
                     (void*)place.ptr(), sens);
            break;
          case 1:
            ++count;
            std::snprintf(fmt, sizeof(fmt), "%03d %%-%ds Detector: %%s NumDau:%%d VolID:%%p", level+1, 2*level+1);
            printout(INFO, "DetectorDump", fmt, "", de.path().c_str(), int(children.size()), (void*)de.volumeID());
            if ( de.placement() == de.idealPlacement() )  {
              std::snprintf(fmt, sizeof(fmt), "%03d %%-%ds Placement: %%s  %%c", level+1, 2*level+3);
              printout(INFO,"DetectorDump",fmt,"", de.placementPath().c_str(), sens);
              break;
            }
            std::snprintf(fmt,sizeof(fmt), "%03d %%-%ds Placement: %%s  [ideal:%%p aligned:%%p] %%c",
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
            std::snprintf(fmt,sizeof(fmt), "%03d %%-%ds Material: %%-12s Shape: %%s", level+1,2*level+3);
            printout(INFO,"DetectorDump",fmt,"", mat.name(), toStringSolid(vol->GetShape()).c_str());
          }
          if ( dump_positions && place.isValid() )  {
            Position pos = place.position();
            Box      box = place.volume().solid();
            double   loc[3] = {0,0,0}, world[3] = {0,0,0};
            TGeoHMatrix   tr = de.nominal().worldTransformation();
            tr.LocalToMaster(loc, world);
            std::snprintf(fmt,sizeof(fmt), "%03d %%-%ds BBox:     (%%9.4f,%%9.4f,%%9.4f) [cm]", level+1,2*level+3);
            printout(INFO,"DetectorDump",fmt,"", box.x(), box.y(), box.z());
            std::snprintf(fmt,sizeof(fmt), "%03d %%-%ds Position: (%%9.4f,%%9.4f,%%9.4f) [cm] w/r to mother", level+1,2*level+3);
            printout(INFO,"DetectorDump",fmt,"", pos.X(), pos.Y(), pos.Z());
            std::snprintf(fmt,sizeof(fmt), "%03d %%-%ds Position: (%%9.4f,%%9.4f,%%9.4f) [cm] w/r to world",  level+1,2*level+3);
            printout(INFO,"DetectorDump",fmt,"", world[0], world[1], world[2]);
          }
          if ( dump_volids && !place.volIDs().empty() )    {
            std::stringstream log;
            log << " VID:";
            for( const auto& i : chain )
              log << " " << i.first << ':' << std::dec << std::setw(2) << i.second;
            if ( id_desc.isValid() )  {
              log << " (encoded:0x" << std::setfill('0') << std::setw(8) << std::hex
                  << id_desc.encode(chain)
                  << std::setfill(' ') << std::dec << ") ";
            }
            std::snprintf(fmt,sizeof(fmt),"%03d %%-%ds %%s", level+1, 2*level+1);
            printout(INFO,"DetectorDump", fmt, "", log.str().c_str());
          }
        }
      }
      for ( const auto& c : children )   {
        validate_id_descriptor(c.second, id_desc);
        dump(c.second, level+1, id_desc, chain);
      }
      return 1;
    }
  };
  VolIDs       chain;
  IDDescriptor id_descriptor;
  Actor        actor(description);
  actor.parse_args(argc, argv);
  return actor.dump(description.world(), 0, id_descriptor, std::move(chain));
}
DECLARE_APPLY(DD4hep_DetectorDump,dump_detelement_tree<0>)
DECLARE_APPLY(DD4hep_DetectorVolumeDump,dump_detelement_tree<1>)

/// Basic entry point to print out the volume hierarchy
/**
 *  Factory: DD4hep_DetElementCache
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long detelement_cache(Detector& description, int argc, char** argv) {
  struct Actor {
    long cache(DetElement de) {
      const DetElement::Children& c = de.children();
      de.nominal().worldTransformation();
      de.nominal().detectorTransformation();
      de.placementPath();
      de.path();
      for( const auto& i : c ) cache(i.second);
      return 1;
    }
  } actor;
  std::string detector = "/world";
  for(int i = 0; i < argc && argv[i]; ++i)  {
    if ( 0 == ::strncmp("-detector",argv[i],4) )
      detector = argv[++i];
    else if ( 0 == ::strncmp("--detector",argv[i],5) )
      detector = argv[++i];
    else  {
      std::cout <<
        "Usage: -plugin DD4hep_DetElementCache  -arg [-arg]                 \n\n"
        "     Fill cache with transformation information in DetElements.    \n\n"
        "     -detector <string>  Top level DetElement path. Default: '/world'\n"
        "     --detector <string> dto.                                        \n"
        "     -help              Print this help.                             \n"
        "     Arguments given: " << arguments(argc,argv) << std::endl << std::flush;
      ::exit(EINVAL);
    }
  }
  DetElement element = description.world();
  if ( detector != "/world" )   {
    element = detail::tools::findElement(description, detector);
    if ( !element.isValid() )  {
      except("VolumeDump","+++ Invalid DetElement path: %s", detector.c_str());
    }
  }
  return actor.cache(element);
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

/// Basic entry point to print out detector type map
/**
 *  Factory: DD4hep_DetectorTypes
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long detectortype_cache(Detector& description, int argc, char** argv)  {
  std::vector<std::string> types;
  for(int i = 0; i < argc && argv[i]; ++i)  {
    if ( 0 == ::strncmp("-type",argv[i],4) )
      types.push_back(argv[++i]);
    else if ( 0 == ::strncmp("--type",argv[i],4) )
      types.push_back(argv[++i]);
    else   {
      std::cout <<
        "Usage: DD4hep_DetectorTypes -type <type> -arg [-arg]                          \n"
        "     Dump detector types from detector description.                         \n\n"
        "     -type   <string>         Add new type to be listed. Multiple possible.   \n"
        "\tArguments given: " << arguments(argc,argv) << std::endl << std::flush;
      ::exit(EINVAL);
    }
  }
  if ( types.empty() )   {
    types = description.detectorTypes();
  }
  printout(INFO,"DetectorTypes","Detector type dump:  %ld types:", long(types.size()));
  for( const auto& type : types )   {
    const std::vector<DetElement>& detectors = description.detectors(type);
    printout(INFO,"DetectorTypes","\t --> %ld %s detectors:",long(detectors.size()), type.c_str());
    for( const auto& d : detectors )   {
      printout(INFO,"DetectorTypes","\t\t %-16s --> %s  [%s]",type.c_str(),d.name(),d.type().c_str());
    }
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
#include <DD4hep/SurfaceInstaller.h>
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
#include <DD4hep/PluginTester.h>
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

