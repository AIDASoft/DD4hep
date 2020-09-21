//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//
//==========================================================================

#include <DD4hep/Detector.h>
#include <DD4hep/Factories.h>
#include <DD4hep/Printout.h>
#include <DD4hep/DetectorTools.h>

// ROOT includes
#include "ROOT/RDirectory.hxx"
#include "ROOT/REveGeomViewer.hxx"


// C/C++ include files
#include <cerrno>
#include <cstdlib>
#include <fstream>
#include <sstream>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;


/// Basic entry point to display the currently loaded geometry using the ROOT7 jsroot viewer
/**
 *  Factory: DD4hep_GeometryWebDisplay
 *
 */
static long webdisplay(Detector& description, int argc, char** argv) {
  TGeoManager& mgr = description.manager();
  int vislevel = 6, visopt = 1;
  string detector = "/world";
  const char* opt = "";
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
      except("DD4hep_GeometryWebDisplay","+++ Invalid DetElement path: %s",detector.c_str());
    }
    if ( !elt.placement().isValid() )   {
      except("DD4hep_GeometryWebDisplay","+++ Invalid DetElement placement: %s",detector.c_str());
    }
    vol = elt.placement().volume();
  }

  if (vol) {
    auto viewer = std::make_shared<ROOT::Experimental::REveGeomViewer>(&mgr);
    viewer->SelectVolume(vol->GetName());
    viewer->SetLimits();
    viewer->SetDrawOptions(opt);
    viewer->Show();
    // add to global heap to avoid immediate destroy of RGeomViewer
    ROOT::Experimental::RDirectory::Heap().Add( "geom_viewer", viewer );
    return 1;
  }
  return 0;
}
DECLARE_APPLY(DD4hep_GeometryWebDisplay,webdisplay)

