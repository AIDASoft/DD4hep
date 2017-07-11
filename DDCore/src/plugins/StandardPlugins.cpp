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
#include "TClass.h"
#include "TRint.h"

// C/C++ include files
#include <cerrno>
#include <cstdlib>
#include <fstream>
#include <sstream>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

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
 *  Factory: DD4hepGeometryDisplay
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long display(Detector& description, int argc, char** argv) {
  TGeoManager& mgr = description.manager();
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
 *  Factory: dd4hepRint
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long run_interpreter(Detector& /* description */, int argc, char** argv) {
  pair<int, char**> a(argc,argv);
  TRint app("dd4hep", &a.first, a.second);
  for(int i=0; i<argc; ++i)   {
    printout(INFO,"DD4hepRint","Excecute[%d]: %s",i,argv[i]);
    gInterpreter->ProcessLine(argv[i]);
  }
  app.Run();
  return 1;
}
DECLARE_APPLY(DD4hepRint,run_interpreter)

/// Basic entry point to start the ROOT interpreter.
/**
 *  The UI will show up in the ROOT prompt and is accessible
 *  in the interpreter with the global variable 
 *  dd4hep::DD4hepUI* gdd4hepUI;
 *
 *  Factory: DD4hepInteractiveUI
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long root_ui(Detector& description, int /* argc */, char** /* argv */) {
  char cmd[256];
  DD4hepUI* ui = new DD4hepUI(description);
  ::snprintf(cmd,sizeof(cmd),"dd4hep::detail::DD4hepUI* gDD4hepUI = (dd4hep::detail::DD4hepUI*)%p;",(void*)ui);
  gInterpreter->ProcessLine(cmd);
  printout(ALWAYS,"DD4hepUI",
           "Use the ROOT interpreter variable gDD4hepUI "
           "to interact with the detector description.");
  return 1;
}
DECLARE_APPLY(DD4hepInteractiveUI,root_ui)

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
        ::printf("DD4hepElementTable -opt [-opt]                         \n"
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
    "      ++++   Linear collider detector description in C++       ++++\n"
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
DECLARE_APPLY(DD4hepElementTable,root_elements)

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
        ::printf("DD4hepElementTable -opt [-opt]                         \n"
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
    "      ++++   Linear collider detector description in C++       ++++\n"
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
DECLARE_APPLY(DD4hepMaterialTable,root_materials)

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
DECLARE_APPLY(DD4hepXMLProcessor,process_xml_doc)

/// Basic entry point to load the volume manager object
/**
 *  Factory: DD4hepVolumeManager
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
DECLARE_APPLY(DD4hepVolumeManager,load_volmgr)

/// Basic entry point to dump a dd4hep geometry to a ROOT file
/**
 *  Factory: DD4hepGeometry2ROOT
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
DECLARE_APPLY(DD4hepGeometry2ROOT,dump_geometry2root)

/// Basic entry point to load a dd4hep geometry directly from the ROOT file
/**
 *  Factory: DD4hepRootLoader
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long load_geometryFromroot(Detector& description, int argc, char** argv) {
  if ( argc > 0 )   {
    string input = argv[0];
    printout(INFO,"DD4hepRootLoader","+++ Read geometry from root file:%s",input.c_str());
    if ( 1 == DD4hepRootPersistency::load(description,input.c_str(),"Geometry") )  {
      return 1;
    }
  }
  printout(ERROR,"DD4hepRootLoader","+++ No input file name given.");
  return 0;
}
DECLARE_APPLY(DD4hepRootLoader,load_geometryFromroot)

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
DECLARE_APPLY(DD4hepCheckSensitives,check_sensitives)

/// Basic entry point to check sensitive detector strictures
/**
 *  Factory: DD4hepCheckDetectors
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long check_detectors(Detector& description, int /* argc */, char** /* argv */) {
  DD4hepRootCheck check(&description);
  return check.checkDetectors();
}
DECLARE_APPLY(DD4hepCheckDetectors,check_detectors)

/// Basic entry point to check sensitive detector strictures
/**
 *  Factory: DD4hepCheckReadouts
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long check_readouts(Detector& description, int /* argc */, char** /* argv */) {
  DD4hepRootCheck check(&description);
  return check.checkReadouts();
}
DECLARE_APPLY(DD4hepCheckReadouts,check_readouts)

/// Basic entry point to check IDDescriptors of the detector object
/**
 *  Factory: DD4hepCheckIdspecs
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long check_idspecs(Detector& description, int /* argc */, char** /* argv */) {
  DD4hepRootCheck check(&description);
  return check.checkIdSpecs();
}
DECLARE_APPLY(DD4hepCheckIdspecs,check_idspecs)

/// Basic entry point to print out the volume hierarchy
/**
 *  Factory: DD4hepVolumeDump
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long dump_volume_tree(Detector& description, int argc, char** argv) {
  struct Actor {
    bool m_printVolIDs = false;
    bool m_printPointers = false;
    bool m_printPositions = false;
    bool m_printSensitivesOnly = false;
    Actor(int ac, char** av)  {
      for(int i=0; i<ac; ++i)  {
        char c = ::tolower(av[i][0]);
        char* p = av[i];
        if ( c == '-' ) { ++p; c = ::tolower(av[i][1]); }
        if ( ::strncmp(p,"volume_ids",3)==0 ) m_printVolIDs = true;
        else if ( ::strncmp(p,"positions",3)==0 ) m_printPositions = true;
        else if ( ::strncmp(p,"pointers",3)==0  ) m_printPointers  = true;
        else if ( ::strncmp(p,"sensitive",3)==0 ) m_printSensitivesOnly = true;
      }
    }

    long dump(TGeoNode* ideal, TGeoNode* aligned,int level, PlacedVolume::VolIDs volids) const {
      char fmt[128];
      string opt_info;
      PlacedVolume pv(ideal);
      bool sensitive = false;
      if ( m_printPositions || m_printVolIDs )  {
        stringstream log;
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
        if ( m_printPositions )  {
          if ( ideal )  {
            const double* trans = ideal->GetMatrix()->GetTranslation();
            ::snprintf(fmt, sizeof(fmt), "Pos: (%f,%f,%f) ",trans[0],trans[1],trans[2]);
          }
          else  {
            ::snprintf(fmt, sizeof(fmt), " <ERROR: INVALID Translation matrix> ");
          }
          log << fmt;
        }
        opt_info = log.str();
      }
      TGeoVolume* volume = ideal ? ideal->GetVolume() : 0;
      if ( !m_printSensitivesOnly || (m_printSensitivesOnly && sensitive) )  {
        char sens = pv.volume().isSensitive() ? 'S' : ' ';
        if ( m_printPointers )    {
          if ( ideal == aligned )  {
            ::snprintf(fmt,sizeof(fmt),"%03d [Ideal:%p] %%-%ds %%-16s (%%s) \t %c %%s",
                       level+1,(void*)ideal,2*level+1,sens);
          }
          else  {
            ::snprintf(fmt,sizeof(fmt),"%03d Ideal:%p Aligned:%p %%-%ds %%-16s (%%s) %c %%s",
                       level+1,(void*)ideal,(void*)aligned,2*level+1,sens);
          }
        }
        else  {
          if ( ideal == aligned )  {
            ::snprintf(fmt,sizeof(fmt),"%03d %%-%ds %%-16s (%%s) \t %c %%s",
                       level+1,2*level+1,sens);
          }
          else  {
            ::snprintf(fmt,sizeof(fmt),"%03d Ideal:%p Aligned:%p %%-%ds %%-16s (%%s) %c %%s",
                       level+1,(void*)ideal,(void*)aligned,2*level+1,sens);
          }
        }
        printout(INFO,"VolumeDump",fmt,"",
                 aligned->GetName(),
                 volume ? volume->GetShape()->IsA()->GetName() : "[Invalid Volume]",
                 opt_info.c_str());
      }
      for (Int_t idau = 0, ndau = aligned->GetNdaughters(); idau < ndau; ++idau)  {
        if ( ideal )   {
          TGeoNode*   ideal_daughter   = ideal->GetDaughter(idau);
          const char* daughter_name    = ideal_daughter->GetName();
          TGeoNode*   aligned_daughter = volume->GetNode(daughter_name);
          dump(ideal_daughter, aligned_daughter, level+1, volids);
        }
        else  {
          printout(ERROR,"VolumeDump"," <ERROR: INVALID IDEAL Translation matrix>: %s",aligned->GetName());
        }
      }
      return 1;
    }
  };
  string place = description.world().placementPath();
  detail::tools::PlacementPath path;
  detail::tools::placementPath(description.world(), path);
  PlacedVolume  pv = detail::tools::findNode(description.world().placement(),place);
  Actor actor(argc,argv);
  return actor.dump(description.world().placement().ptr(),pv.ptr(),0,PlacedVolume::VolIDs());
}
DECLARE_APPLY(DD4hepVolumeDump,dump_volume_tree)

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
  DetElement det = description.world();
  unique_ptr<DetectorProcessor> proc(dd4hep::createProcessor<DetectorProcessor>(description, argc, argv));
  for(int i=0, num=std::min(argc,3); i<num; ++i)  {
    if ( 0 == ::strncmp(argv[i],"-detector",4) )  {
      det = detail::tools::findElement(description, argv[++i]);
      break;
    }
  }
  return DetectorScanner().scan(*proc,det);
}
DECLARE_APPLY(DD4hep_DetElementProcessor,detelement_processor)

/// Basic entry point to print out the detector element hierarchy
/**
 *  Factory: DD4hepDetectorDump, DD4hepDetectorVolumeDump
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
template <int flag> long dump_detelement_tree(Detector& description, int argc, char** argv) {
  struct Actor {
    static long dump(DetElement de,int level, bool sensitive_only) {
      const DetElement::Children& c = de.children();
      if ( !sensitive_only || 0 != de.volumeID() )  {
        PlacedVolume place = de.placement();
        char sens = place.isValid() ? place.volume().isSensitive() ? 'S' : ' ' : ' ';
        char fmt[128];
        switch(flag)  {
        case 0:
          if ( de.placement() == de.idealPlacement() )  {
            ::snprintf(fmt,sizeof(fmt),"%03d %%-%ds %%s NumDau:%%d VolID:%%08X Place:%%p  %%c",level+1,2*level+1);
            printout(INFO,"DetectorDump",fmt,"",de.path().c_str(),int(c.size()),
                     (unsigned long)de.volumeID(), (void*)place.ptr(), sens);
            break;
          }
          ::snprintf(fmt,sizeof(fmt),"%03d %%-%ds %%s NumDau:%%d VolID:%%08X Place:%%p [ideal:%%p aligned:%%p]  %%c",
                     level+1,2*level+1);
          printout(INFO,"DetectorDump",fmt,"",de.path().c_str(),int(c.size()),
                   (unsigned long)de.volumeID(), (void*)de.idealPlacement().ptr(), (void*)place.ptr(), sens);
          break;
        case 1:
          ::snprintf(fmt,sizeof(fmt),"%03d %%-%ds Detector: %%s NumDau:%%d VolID:%%p",level+1,2*level+1);
          printout(INFO,"DetectorDump", fmt, "", de.path().c_str(), int(c.size()), (void*)de.volumeID());
          if ( de.placement() == de.idealPlacement() )  {
            ::snprintf(fmt,sizeof(fmt),"%03d %%-%ds Placement: %%s  %%c",level+1,2*level+3);
            printout(INFO,"DetectorDump",fmt,"", de.placementPath().c_str(), sens);
            break;
          }
          ::snprintf(fmt,sizeof(fmt),"%03d %%-%ds Placement: %%s  [ideal:%%p aligned:%%p] %%c",level+1,2*level+3);
          printout(INFO,"DetectorDump",fmt,"", de.placementPath().c_str(),
                   (void*)de.idealPlacement().ptr(), (void*)place.ptr(), sens);          
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
  return Actor::dump(description.world(),0,sensitive_only);
}
DECLARE_APPLY(DD4hepDetectorDump,dump_detelement_tree<0>)
DECLARE_APPLY(DD4hepDetectorVolumeDump,dump_detelement_tree<1>)

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
DECLARE_APPLY(DD4hepDetElementCache,detelement_cache)

/// Basic entry point to dump the geometry tree of the description instance
/**
 *  Factory: DD4hepGeometryTreeDump
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
DECLARE_APPLY(DD4hepGeometryTreeDump,exec_GeometryTreeDump)

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
DECLARE_APPLY(DD4hepSimpleGDMLWriter,exec_SimpleGDMLWriter)

/// Basic entry point to print out detector type map
/**
 *  Factory: DD4hepDetectorTypes
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
DECLARE_APPLY(DD4hepDetectorTypes,detectortype_cache)

#include "DD4hep/SurfaceInstaller.h"
typedef SurfaceInstaller TestSurfacesPlugin;
DECLARE_SURFACE_INSTALLER(TestSurfaces,TestSurfacesPlugin)

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
DECLARE_APPLY(DD4hepPluginTester,install_plugin_tester)

