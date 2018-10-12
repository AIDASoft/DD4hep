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
#include "TUri.h"

using namespace std;
using namespace dd4hep;

#if ROOT_VERSION_CODE >= ROOT_VERSION(6,13,0)

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
    bool wrld = false;
    for(int i = 0; i < argc && argv[i]; ++i)  {
      if ( 0 == ::strncmp("-input",argv[i],2) )
        input = argv[++i];
      else if ( 0 == ::strncmp("-path", argv[i],2) )
        path  = argv[++i];
      else if ( 0 == ::strncmp("-world", argv[i],2) )
        wrld  = true;
    }
    if ( input.empty() || (path.empty() && !wrld) )   {
      cout <<
        "Usage: -plugin <name> -arg [-arg]                                                  \n"
        "     name:   factory name     DD4hep_ROOTGDMLParse                                 \n"
        "     -input  <string>         Input file name.                                     \n"
        "     -path   <string>         Path to parent detector element to attach            \n"
        "                              top volume from GDML file.                           \n"
        "     -world  <string>         Name of the world object if to be imported from gdml.\n"
        "     Note: -path and -world options are exclusive.                                 \n"
        "\tArguments given: " << arguments(argc,argv) << endl << flush;
      ::exit(EINVAL);
    }
    printout(INFO,"ROOTGDMLParse","+++ Read geometry from GDML file file:%s",input.c_str());
    vector<string> elements = detail::tools::pathElements(path);
    if ( !wrld && !elements.empty() )   {
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
          TUri uri(input.c_str());
          input = uri.GetRelativePart();
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
    else if ( wrld )   {
      TGDMLParse parser;
      TUri uri(input.c_str());
      input = uri.GetRelativePart();
      Volume vol = parser.GDMLReadFile(input.c_str());
      if ( vol.isValid() )   {
        vol.import(); // We require the extensions in dd4hep.
        description.manager().SetTopVolume(vol.ptr());
        description.init();
        description.endDocument();
        printout(INFO,"ROOTGDMLParse","+++ Attach GDML volume %s", vol.name());
        return 1;
      }
      except("ROOTGDMLParse","+++ Failed to parse GDML file:%s for path:%s",
             input.c_str(),path.c_str());
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
    bool detector = true, volpath = false, volname = false;
    string output, path;
    for(int i = 0; i < argc && argv[i]; ++i)  {
      if ( 0 == ::strncmp("-output",argv[i],2) )
        output = argv[++i];
      else if ( 0 == ::strncmp("-path", argv[i],2) )
        path  = argv[++i];
      else if ( 0 == ::strncmp("-volpath", argv[i],7) )
        volpath  = true, volname = false, detector = false;
      else if ( 0 == ::strncmp("-volname", argv[i],7) )
        volpath  = false, volname = true, detector = false;
      else if ( 0 == ::strncmp("-detector", argv[i],8) )
        volpath  = false, volname = false, detector = true;
    }
    if ( output.empty() || path.empty() )   {
      cout <<
        "Usage: -plugin <name> -arg [-arg]                                            \n"
        "     name:   factory name     DD4hep_ROOTGDMLExtract                         \n"
        "     -output <string>         Output file name.                              \n"
        "     -path   <string>         Path to parent detector element to extract     \n"
        "                              top volume to GDML file.                       \n"
        "     -detector                Indicate that the path is a DetElement path    \n"
        "     -volpath                  Indicate that the path is a volume path       \n"
        "     -volname                 Indicate that the path is a volume name prefix \n"
        "\tArguments given: " << arguments(argc,argv) << endl << flush;
      ::exit(EINVAL);
    }
    if ( detector )   {
      printout(INFO,"ROOTGDMLExtract","+++ Write geometry %s to GDML file file:%s",
               path.c_str(), output.c_str());
      DetElement de = detail::tools::findElement(description,path);
      if ( de.isValid() )   {
        TGDMLWrite extract;
        TUri uri(output.c_str());
        Volume vol = de.volume();
        extract.WriteGDMLfile(&description.manager(), vol.ptr(), uri.GetRelativePart());
        return 1;
      }
      except("ROOTGDMLExtract","+++ Invalid DetElement path given: %s", path.c_str());
    }
    else  {
      struct Actor {
        bool _volpath;
        const string& _path;
        TGeoVolume*   _volume = 0;
        Actor(const string& p, bool vp) : _volpath(vp), _path(p)  {}
        void scan(TGeoNode* n, const std::string& p="")  {
          string nam;
          if ( _volpath )  {
            nam = p + '/';
            nam += n->GetName();
            if ( _volpath && nam.find(_path) == 0 )
              _volume = n->GetVolume();
            for (Int_t idau = 0, ndau = n->GetNdaughters(); _volume == 0 && idau < ndau; ++idau)
              scan(n->GetDaughter(idau), p);
            return;
          }
          nam = n->GetName();
          if ( nam.find(_path) == 0 )   {
            _volume = n->GetVolume();
            printout(ALWAYS,"Check","+++ Found required volume: %s",_path.c_str());
            printout(ALWAYS,"Check","+++                     -> %s",nam.c_str());
            return;
          }
          for (Int_t idau = 0, ndau = n->GetNdaughters(); _volume == 0 && idau < ndau; ++idau)
            scan(n->GetDaughter(idau),nam);
        }
      };
      Volume top = description.worldVolume();
      TObjArray* ents = top->GetNodes();
      Actor a(path, volpath ? true : volname ? false : true);
      for (Int_t i = 0, n = ents->GetEntries(); i < n && a._volume == 0; ++i)  {
        TGeoNode* node = (TGeoNode*)ents->At(i);
        a.scan(node, node->GetName());
      }
      if ( a._volume )    {
        TGDMLWrite extract;
        TUri uri(output.c_str());
        extract.WriteGDMLfile(&description.manager(), a._volume, uri.GetRelativePart());
        return 1;
      }
      except("ROOTGDMLExtract","+++ Invalid volume path/name given: %s", path.c_str());
    }
  }
  except("ROOTGDMLExtract","+++ No output file name given.");
  return 0;
}
DECLARE_APPLY(DD4hep_ROOTGDMLExtract,gdml_extract)

#endif
