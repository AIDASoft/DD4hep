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
/* 
   Plugin invocation:
   ==================
   This plugin behaves like a main program.
   Invoke the plugin with something like this:

   geoPluginRun -destroy -plugin DD4hep_VolumeScannerTest -opt [-opt]

*/
// Framework include files
#include "DD4hep/Volumes.h"
#include "DD4hep/Detector.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Factories.h"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/VolumeProcessor.h"
#include "TClass.h"
#include <iomanip>

namespace   {
  class MyVolumeProcessor : public dd4hep::PlacedVolumeProcessor   {
  public:
    int count = 0;
    std::vector<std::string> volume_stack;
  public:
    /// Default constructor
    MyVolumeProcessor() = default;
    /// Default destructir
    virtual ~MyVolumeProcessor() = default;
    /// Callback to output PlacedVolume information of an entire Placement
    virtual int process(dd4hep::PlacedVolume pv, int level, bool recursive)    {
      volume_stack.push_back(pv.name());
      int ret = this->dd4hep::PlacedVolumeProcessor::process(pv, level, recursive);
      volume_stack.pop_back();
      return ret;
    }
    /// Volume callback
    virtual int operator()(dd4hep::PlacedVolume pv, int level)   {
      dd4hep::Volume vol = pv.volume();
      std::cout << "Hierarchical level:" << level << "   Placement:";
      for(const auto& i : volume_stack ) std::cout << "/" << i;
      std::cout << std::endl
                << "\tMaterial:" << vol.material().name() 
                << "\tSolid:   " << vol.solid().name()
                << " [" << vol.solid()->IsA()->GetName() << "]" << std::endl;
      ++count;
      return 1;
    }
  };
}

using namespace std;
using namespace dd4hep;

/// Plugin function: Test example of the volume scanner using a customized callback functor
/**
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    20/01/2018
 */
static int scan_volumes (Detector& detector, int argc, char** argv)  {
  bool help = false;
  string det_element_path, placed_vol_path;
  for(int i=0; i<argc && argv[i]; ++i)  {
    if ( 0 == ::strncmp("-help",argv[i],4) )
      help = true;
    else if ( 0 == ::strncmp("-path",argv[i],4) )
      placed_vol_path = argv[++i];
    else if ( 0 == ::strncmp("-detector",argv[i],4) )
      det_element_path = argv[++i];
    else
      help = true;
  }
  if ( help )   {
    /// Help printout describing the basic command line interface
    cout <<
      "Usage: -plugin <name> -arg [-arg]                                                  \n"
      "     name:   factory name     DD4hep_PlacedVolumeScannerTest                       \n"
      "     -detector <name>         Path to the detector element where to start the scan.\n"
      "     -path     <name>         Alternatively specify the physical volume path.      \n"
      "     -help                    Ahow this help.                                      \n"
      "\tArguments given: " << arguments(argc,argv) << endl << flush;
    ::exit(EINVAL);
  }

  // Detectine the proper placed volume for the start (default=/world_volume)
  PlacedVolume start_pv;
  DetElement   start_de, de = detector.world();
  if ( !det_element_path.empty() )
    start_de = detail::tools::findElement(detector, det_element_path);
  else if ( !placed_vol_path.empty() )
    start_pv = detail::tools::findNode(de.placement(),placed_vol_path);

  if ( !start_pv.isValid() )   {
    if ( !start_de.isValid() )   {      
      except("VolumeScanner","Failed to find start conditions for the volume scan");
    }
    start_pv = start_de.placement();
  }

  // Let's scan
  MyVolumeProcessor proc;
  PlacedVolumeScanner().scanPlacements(proc, start_pv, 0, true);
  
  printout(ALWAYS,"VolumeScanner","+++ Visited a total of %d placed volumes.",proc.count);
  return 1;
}

DECLARE_APPLY(DD4hep_PlacedVolumeScannerTest,scan_volumes)
