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
#include "DD4hep/DetectorTools.h"

// ROOT includes
#include "TInterpreter.h"
#include "TGeoElement.h"
#include "TGeoManager.h"
#include "TGDMLParse.h"
#include "TGDMLWrite.h"

using namespace std;
using namespace dd4hep;

#ifdef ROOT_VERSION_CODE >= ROOT_VERSION(6,13,1)

/// ROOT GDML reader plugin
/**
 *  Factory: DD4hep_ROOTGDMLParse
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long gdml_parse(Detector& description, int argc, char** argv) {
  if ( argc > 0 )   {
    string input, path;
    for(int i = 0; i < argc && argv[i]; ++i)  {
      if ( 0 == ::strncmp("-input",argv[i],2) )
        input = argv[++i];
      if ( 0 == ::strncmp("-path", argv[i],2) )
        path  = argv[++i];
    }
    if ( input.empty() || path.empty() )   {
      cout <<
        "Usage: -plugin <name> -arg [-arg]                                            \n"
        "     name:   factory name     DD4hep_ROOTGDMLParse                           \n"
        "     -input  <string>         Input file name.                               \n"
        "     -path   <string>         Path to parent detector element to attach      \n"
        "                              top volume from GDML file.                     \n"
        "\tArguments given: " << arguments(argc,argv) << endl << flush;
      ::exit(EINVAL);
    }
    printout(INFO,"ROOTGDMLParse","+++ Read geometry from GDML file file:%s",input.c_str());
    vector<string> elements = detail::tools::pathElements(path);
    if ( !elements.empty() )   {
      DetElement world = description.world();
      DetElement parent = world, child;
      for(size_t i=0; i < elements.size(); ++i)  {
        const auto& e = elements[i];
        if ( e == world.name() )
          continue;
        child = parent.child(e);
        if ( child.isValid() )  {
          parent = child;
          continue;
        }
        child = DetElement(e, 0); // Inactive child, no sensitive detector, no ID
        PlacedVolume pv;
        Volume mother = (parent==world) ? description.pickMotherVolume(child) : parent.volume();
        if ( i == elements.size()-1 )  {
          TGDMLParse parser;
          Volume vol = parser.GDMLReadFile(input.c_str());
          if ( vol.isValid() )   {
            vol.import(); // We require the extensions in dd4hep.
            pv = mother.placeVolume(vol);
            child.setPlacement(pv);
            if (parent == world) description.add(child);
            else                 parent.add(child);
            printout(INFO,"ROOTGDMLParse","+++ Attach GDML volume %s to Element path:%s",
                     vol.name(), path.c_str());
            return 1;
          }
          except("ROOTGDMLParse","+++ Failed to parse GDML file:%s for path:%s",
                 input.c_str(),path.c_str());
        }
        pv = mother.placeVolume(Assembly(e));
        child.setPlacement(pv);
        if (parent == world) description.add(child);
        else                 parent.add(child);
      }
    }
    except("ROOTGDMLParse","+++ Invalid DetElement path given: %s", path.c_str());
  }
  except("ROOTGDMLParse","+++ No input file name given.");
  return 0;
}
DECLARE_APPLY(DD4hep_ROOTGDMLParse,gdml_parse)


/// ROOT GDML writer plugin
/**
 *  Factory: DD4hep_ROOTGDMLExtract
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long gdml_extract(Detector& description, int argc, char** argv) {
  if ( argc > 0 )   {
    string output, path;
    for(int i = 0; i < argc && argv[i]; ++i)  {
      if ( 0 == ::strncmp("-output",argv[i],2) )
        output = argv[++i];
      if ( 0 == ::strncmp("-path", argv[i],2) )
        path  = argv[++i];
    }
    if ( output.empty() || path.empty() )   {
      cout <<
        "Usage: -plugin <name> -arg [-arg]                                            \n"
        "     name:   factory name     DD4hep_ROOTGDMLExtract                         \n"
        "     -output <string>         Output file name.                              \n"
        "     -path   <string>         Path to parent detector element to attach      \n"
        "                              top volume from GDML file.                     \n"
        "\tArguments given: " << arguments(argc,argv) << endl << flush;
      ::exit(EINVAL);
    }
    printout(INFO,"ROOTGDMLExtract","+++ Write geometry %s to GDML file file:%s",
             path.c_str(), output.c_str());
    DetElement detector = detail::tools::findElement(description,path);
    if ( detector.isValid() )   {
      TGDMLWrite extract;
      Volume volume = detector.volume();
      extract.WriteGDMLfile(&description.manager(), volume.ptr(), output.c_str());
      return 1;
    }
    except("ROOTGDMLExtract","+++ Invalid DetElement path given: %s", path.c_str());
  }
  except("ROOTGDMLExtract","+++ No output file name given.");
  return 0;
}
DECLARE_APPLY(DD4hep_ROOTGDMLExtract,gdml_extract)

#endif
