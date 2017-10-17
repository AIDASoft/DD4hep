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
#ifndef DD4HEP_DDCORE_VISPROCESSOR_H
#define DD4HEP_DDCORE_VISPROCESSOR_H

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
  class VisProcessor : public PlacedVolumeProcessor  {
  public:
    Detector&    description;
    std::vector<Atom>      activeElements;
    std::vector<Material>  activeMaterials;
    std::vector<Material>  inactiveMaterials;
    VisAttr                activeVis;
    VisAttr                inactiveVis;
    double                 fraction = 0e0;
    bool                   setAllInactive = false;

  public:
    /// Initializing constructor
    VisProcessor(Detector& desc);
    /// Default destructor
    virtual ~VisProcessor();
    /// Callback to output PlacedVolume information of an single Placement
    virtual int operator()(PlacedVolume pv, int level);
  };
}
#endif //  DD4HEP_DDCORE_VISPROCESSOR_H

#include "DD4hep/Printout.h"
#include "DD4hep/DetectorHelper.h"
#include "DD4hep/DetFactoryHelper.h"
#include <sstream>

using namespace std;
using namespace dd4hep;

/// Initializing constructor
VisProcessor::VisProcessor(Detector& desc) : description(desc)
{
}

/// Default destructor
VisProcessor::~VisProcessor()   {
}

namespace {
  void set_attr(Volume vol, VisAttr attr)   {
    if ( vol.visAttributes().ptr() != attr.ptr() )  {
      vol.setVisAttributes(attr);
    }
  }
}

/// Callback to output PlacedVolume information of an single Placement
int VisProcessor::operator()(PlacedVolume pv, int /* level */)   {
  Volume vol = pv.volume();
  double frac_active = 0.0;
  VisAttr attr;

  for ( Atom a : activeElements )   {
    frac_active += vol.material().fraction(a);
  }
  //if ( frac_active >= fraction )
    printout(INFO,"VisProcessor",
             "++ Volume:%s [%s] active:%s fraction:%.3f active-vis:%s inactive-vis:%s",
             pv.name(), vol.name(), yes_no(frac_active >= fraction), frac_active,
             yes_no(activeVis.isValid()), yes_no(inactiveVis.isValid()));

  if ( frac_active >= fraction )
    attr = activeVis;
  if ( !attr.isValid() )  {
    for ( Material m : activeMaterials )  {
      if ( m.ptr() == vol.material().ptr() )   {
        attr = activeVis;
        break;
      }
    }
  }
  // If we get here, the material is definitely inactive
  if ( !attr.isValid() && setAllInactive )
    attr = inactiveVis;
  if ( !attr.isValid() )  {
    for ( Material m : inactiveMaterials )   {
      if ( m.ptr() == vol.material().ptr() )   {
        attr = inactiveVis;
        break;
      }
    }
    if ( !activeElements.empty() && frac_active<fraction )   {
      attr = inactiveVis;
    }
  }
  if ( attr.isValid() )  {
    set_attr(vol,activeVis);
  }
  return 1;
}

static void* create_object(Detector& description, int argc, char** argv)   {
  DetectorHelper helper(description);
  VisProcessor* proc = new VisProcessor(description);
  for ( int i=0; i<argc; ++i )   {
    if ( argv[i] )    {
      if ( ::strncmp(argv[i],"-vis-active",6) == 0 )   {
        VisAttr vis = description.visAttributes(argv[++i]);
        if ( vis.isValid() ) proc->activeVis = vis;
        continue;
      }
      else if ( ::strncmp(argv[i],"-vis-inactive",6) == 0 )   {
        VisAttr vis = description.visAttributes(argv[++i]);
        if ( vis.isValid() ) proc->inactiveVis = vis;
        continue;
      }
      else if ( ::strncmp(argv[i],"-elt-active",6) == 0 )   {
        Atom a = helper.element(argv[++i]);
        if ( a.isValid() ) proc->activeElements.push_back(a);
        continue;
      }
      else if ( ::strncmp(argv[i],"-mat-active",6) == 0 )   {
        Material m = helper.material(argv[++i]);
        if ( m.isValid() ) proc->activeMaterials.push_back(m);
        continue;
      }
      else if ( ::strncmp(argv[i],"-mat-inactive",6) == 0 )   {
        Material m = helper.material(argv[++i]);
        if ( m.isValid() ) proc->inactiveMaterials.push_back(m);
        continue;
      }
      else if ( ::strncmp(argv[i],"-all-inactive",6) == 0 )   {
        proc->setAllInactive = true;
        continue;
      }
      else if ( ::strncmp(argv[i],"-fraction",3) == 0 )   {
        stringstream str(argv[++i]);
        if ( str.good() )  {
          str >> proc->fraction;
          if ( !str.fail() ) continue;
        }
      }
      cout <<
        "Usage: DD4hep_VisProcessor -arg [-arg]                                              \n"
        "     -vis-active   <name>     Set the visualization attribute for   active materials\n"
        "     -vis-inactive <name>     Set the visualization attribute for inactive materials\n"
        "     -element      <name>     Add active element by name. If the fractional sum of  \n"
        "                              all active elements in a volume exceeds <fraction>    \n"
        "                              the volume is considered active                       \n"
        "     -mat-active   <name>     Add material by name to the list of   active materials\n"
        "     -mat-inactive <name>     Add material by name to the list of inactive materials\n"
        "     -all-inactive            Auto set all volumes inactive, which are NOT active   \n"
        "     -fraction     <double>   Set the fraction above which the active elment content\n"
        "                              defines an active volume.                             \n"
        "\tArguments given: " << arguments(argc,argv) << endl << flush;
      ::exit(EINVAL);
    }
  }
  return (void*)((PlacedVolumeProcessor*)proc);
}

// first argument is the type from the xml file
DECLARE_DD4HEP_CONSTRUCTOR(DD4hep_VisProcessor,create_object)

