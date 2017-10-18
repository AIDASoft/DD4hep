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
#ifndef DD4HEP_DDCORE_VISVOLNAMEPROCESSOR_H
#define DD4HEP_DDCORE_VISVOLNAMEPROCESSOR_H

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
  class VisVolNameProcessor : public PlacedVolumeProcessor  {
  public:
    Detector&              description;
    std::string            name;
    size_t                 numApplied = 0;
    bool                   show = false;
    /// Print properties
    void _show();
  public:
    /// Initializing constructor
    VisVolNameProcessor(Detector& desc);
    /// Default destructor
    virtual ~VisVolNameProcessor();
    /// Callback to output PlacedVolume information of an single Placement
    virtual int operator()(PlacedVolume pv, int level);
  };
}
#endif //  DD4HEP_DDCORE_VISVOLNAMEPROCESSOR_H

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

//#include "DD4hep/VisVolNameProcessor.h"
#include "DD4hep/Printout.h"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/DetectorHelper.h"
#include "DD4hep/DetFactoryHelper.h"
#include <sstream>

using namespace std;
using namespace dd4hep;

/// Initializing constructor
VisVolNameProcessor::VisVolNameProcessor(Detector& desc) : description(desc), name("VisVolNameProcessor")
{
}

/// Default destructor
VisVolNameProcessor::~VisVolNameProcessor()   {
  if ( show )  {
    printout(ALWAYS,name,"++       %8ld vis-attrs applied.", numApplied);
  }
}

/// Callback to output PlacedVolume information of an single Placement
int VisVolNameProcessor::operator()(PlacedVolume pv, int /* level */)   {
  Volume   vol = pv.volume();
  VisAttr  vis = description.visAttributes(vol.name());
  if ( vol.visAttributes().ptr() != vis.ptr() )  {
    vol.setVisAttributes(vis);
    ++numApplied;
  }
  return 1;
}

static void* create_object(Detector& description, int argc, char** argv)   {
  DetectorHelper helper(description);
  VisVolNameProcessor*  proc = new VisVolNameProcessor(description);
  for ( int i=0; i<argc; ++i )   {
    if ( argv[i] )    {
      if ( ::strncmp(argv[i],"-name",4) == 0 )   {
        string     name = argv[++i];
        proc->name = name;
        continue;
      }
      else if ( ::strncmp(argv[i],"-show",4) == 0 )   {
        proc->show = true;
        continue;
      }
      cout <<
        "Usage: DD4hep_VisVolNameProcessor -arg [-arg]                                       \n"
        "     -min-density  <number>   Minimal density to show the volume.                   \n"
        "     -show                    Print setup to output device (stdout)                 \n"
        "\tArguments given: " << arguments(argc,argv) << endl << flush;
      ::exit(EINVAL);
    }
  }
  PlacedVolumeProcessor* placement_proc = proc;
  return (void*)placement_proc;
}

// first argument is the type from the xml file
DECLARE_DD4HEP_CONSTRUCTOR(DD4hep_VisVolNameProcessor,create_object)

