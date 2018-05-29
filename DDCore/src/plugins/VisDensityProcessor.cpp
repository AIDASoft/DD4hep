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
#ifndef DD4HEP_DDCORE_VISDENSITYPROCESSOR_H
#define DD4HEP_DDCORE_VISDENSITYPROCESSOR_H

// Framework include files
#include "DD4hep/VolumeProcessor.h"

namespace dd4hep  {
  
  /// DD4hep DetElement creator for the CMS geometry.
  /*  Set visualization attributes for sensitive volumes
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CORE
   */
  class VisDensityProcessor : public PlacedVolumeProcessor  {
  public:
    Detector&              description;
    std::string            name;
    VisAttr                minVis;
    double                 minDensity  = 5e0;
    size_t                 numInactive = 0;
    bool                   show = false;
    /// Print properties
    void _show();
  public:
    /// Initializing constructor
    VisDensityProcessor(Detector& desc);
    /// Default destructor
    virtual ~VisDensityProcessor();
    /// Callback to output PlacedVolume information of an single Placement
    virtual int operator()(PlacedVolume pv, int level);
  };
}
#endif //  DD4HEP_DDCORE_VISDENSITYPROCESSOR_H

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

//#include "DD4hep/VisDensityProcessor.h"
#include "DD4hep/Printout.h"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/DetectorHelper.h"
#include "DD4hep/DetFactoryHelper.h"
#include <sstream>

using namespace std;
using namespace dd4hep;

/// Initializing constructor
VisDensityProcessor::VisDensityProcessor(Detector& desc) : description(desc), name("VisDensityProcessor")
{
}

/// Default destructor
VisDensityProcessor::~VisDensityProcessor()   {
  if ( show && minVis.isValid() )  {
    printout(ALWAYS,name,"++       %8ld vis-attrs applied: %s", numInactive, minVis.name());
  }
}

/// Print properties
void VisDensityProcessor::_show()   {
  if ( show && minVis.isValid() )   {
    printout(ALWAYS,name,
             "++ SETUP Minimal material density: %.4f [g/cm3]  Vis: %s",
             minDensity,minVis.name());
  }
}

/// Callback to output PlacedVolume information of an single Placement
int VisDensityProcessor::operator()(PlacedVolume pv, int /* level */)   {
  Volume   vol = pv.volume();
  Material mat = vol.material();
  if ( vol.visAttributes().ptr() != minVis.ptr() )  {
    if ( mat.density() <= minDensity )  {
      vol.setVisAttributes(minVis);
    }
    ++numInactive;
  }
  return 1;
}

static void* create_object(Detector& description, int argc, char** argv)   {
  DetectorHelper helper(description);
  VisDensityProcessor*  proc = new VisDensityProcessor(description);
  for ( int i=0; i<argc; ++i )   {
    if ( argv[i] )    {
      if ( ::strncmp(argv[i],"-vis",6) == 0 )   {
        VisAttr vis = description.visAttributes(argv[++i]);
        if ( vis.isValid() ) proc->minVis = vis;
        continue;
      }
      else if ( ::strncmp(argv[i],"-min-vis",6) == 0 )   {
        VisAttr vis = description.visAttributes(argv[++i]);
        if ( vis.isValid() ) proc->minVis = vis;
        continue;
      }
      else if ( ::strncmp(argv[i],"-min-density",6) == 0 )   {
        double density = _toDouble(argv[++i]);
        proc->minDensity = density;
        continue;
      }
      else if ( ::strncmp(argv[i],"-name",4) == 0 )   {
        string     name = argv[++i];
        proc->name = name;
        continue;
      }
      else if ( ::strncmp(argv[i],"-show",4) == 0 )   {
        proc->show = true;
        continue;
      }
      cout <<
        "Usage: DD4hep_VisDensityProcessor -arg [-arg]                                       \n"
        "     -vis          <name>     Set the visualization attribute for inactive materials\n"
        "     -min-vis      <name>     Set the visualization attribute for inactive materials\n"
        "     -min-density  <number>   Minimal density to show the volume.                   \n"
        "     -show                    Print setup to output device (stdout)                 \n"
        "\tArguments given: " << arguments(argc,argv) << endl << flush;
      ::exit(EINVAL);
    }
  }
  proc->_show();
  PlacedVolumeProcessor* placement_proc = proc;
  return (void*)placement_proc;
}

// first argument is the type from the xml file
DECLARE_DD4HEP_CONSTRUCTOR(DD4hep_VisDensityProcessor,create_object)
