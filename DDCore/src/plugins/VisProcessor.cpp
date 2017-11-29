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
    Detector&              description;
    std::string            name;
    std::vector<Atom>      activeElements;
    std::vector<Material>  activeMaterials;
    std::vector<Material>  inactiveMaterials;
    VisAttr                activeVis;
    VisAttr                inactiveVis;
    double                 fraction = 100e-2;
    size_t                 numActive = 0;
    size_t                 numInactive = 0;
    bool                   setAllInactive = false;
    bool                   show = false;
    /// Print properties
    void _show();
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

//#include "DD4hep/VisProcessor.h"
#include "DD4hep/Printout.h"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/DetectorHelper.h"
#include "DD4hep/DetFactoryHelper.h"
#include <sstream>

using namespace std;
using namespace dd4hep;

namespace {
  void set_attr(Volume vol, VisAttr attr)   {
    if ( vol.visAttributes().ptr() != attr.ptr() )  {
      vol.setVisAttributes(attr);
    }
  }
}


/// Initializing constructor
VisProcessor::VisProcessor(Detector& desc) : description(desc), name("VisProcessor")
{
}

/// Default destructor
VisProcessor::~VisProcessor()   {
  if ( show )  {
    if ( activeVis.isValid() )
      printout(ALWAYS,name,"++       %8ld   active vis-attrs applied: %s", numActive, activeVis.name());
    if ( inactiveVis.isValid() )
      printout(ALWAYS,name,"++       %8ld inactive vis-attrs applied: %s", numInactive, inactiveVis.name());
  }
}

/// Print properties
void VisProcessor::_show()   {
  if ( show )  {
    if ( activeVis.isValid() )   {
      for ( Atom atom : activeElements )   {
        printout(ALWAYS,name,"++ SETUP   Active element:  %-11s  Vis: %s",atom.name(),activeVis.name());
      }
      for ( Material mat : activeMaterials )  {
        printout(ALWAYS,name,"++ SETUP   Active material: %-11s  Vis: %s", mat.name(),activeVis.name());
      }
    }
    if ( inactiveVis.isValid() )   {
      if ( setAllInactive || !activeElements.empty() )  {
        printout(ALWAYS,name,"++ SETUP Inactive material: %-11s  Vis: %s", "All-non-active",inactiveVis.name());
      }
      for ( Material mat : inactiveMaterials )  {
        printout(ALWAYS,name,"++ SETUP Inactive material: %-11s  Vis: %s", mat.name(),activeVis.name());
      }
    }
  }
}

/// Callback to output PlacedVolume information of an single Placement
int VisProcessor::operator()(PlacedVolume pv, int /* level */)   {
  Volume vol = pv.volume();
  double frac_active = 0.0;
  VisAttr attr;

  for ( Atom atom : activeElements )   {
    frac_active += vol.material().fraction(atom);
  }
  //if ( frac_active >= fraction )
  printout(DEBUG,name,
           "++ Volume:%s [%s] active:%s fraction:%.3f active-vis:%s inactive-vis:%s",
           pv.name(), vol.name(), yes_no(frac_active >= fraction), frac_active,
           yes_no(activeVis.isValid()), yes_no(inactiveVis.isValid()));
  if ( activeVis.isValid() )   {
    if ( frac_active >= fraction )  {
      attr = activeVis;
      ++numActive;
    }
    if ( !attr.isValid() )  {
      for ( Material mat : activeMaterials )  {
        if ( mat.ptr() == vol.material().ptr() )   {
          attr = activeVis;
          ++numActive;
          break;
        }
      }
    }
  }
  // If we get here, the material is definitely inactive
  if ( inactiveVis.isValid() )  {
    if ( !attr.isValid() && setAllInactive )   {
      attr = inactiveVis;
      ++numInactive;
    }
    else if ( frac_active<fraction )   {
      attr = inactiveVis;
      ++numInactive;
    }
    if ( !attr.isValid() && inactiveVis.isValid() )  {
      for ( Material imat : inactiveMaterials )   {
        if ( imat.ptr() == vol.material().ptr() )   {
          attr = inactiveVis;
          ++numInactive;
          break;
        }
      }
    }
  }
  if ( attr.isValid() )  {
    set_attr(vol,attr);
  }
  return 1;
}

static void* create_object(Detector& description, int argc, char** argv)   {
  DetectorHelper helper(description);
  VisProcessor*  proc = new VisProcessor(description);
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
        Atom atom = helper.element(argv[++i]);
        if ( atom.isValid() ) proc->activeElements.push_back(atom);
        continue;
      }
      else if ( ::strncmp(argv[i],"-mat-active",6) == 0 )   {
        Material mat = helper.material(argv[++i]);
        if ( mat.isValid() ) proc->activeMaterials.push_back(mat);
        continue;
      }
      else if ( ::strncmp(argv[i],"-mat-inactive",6) == 0 )   {
        Material mat = helper.material(argv[++i]);
        if ( mat.isValid() ) proc->inactiveMaterials.push_back(mat);
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
      else if ( ::strncmp(argv[i],"-path",4) == 0 )   {
        string     path = argv[++i];
        DetElement de = detail::tools::findElement(description,path);
        if ( de.isValid() ) continue;
        printout(ERROR,"VisProcessor","++ Invalid DetElement path: %s",path.c_str());
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
        "Usage: DD4hep_VisProcessor -arg [-arg]                                              \n"
        "     -vis-active   <name>     Set the visualization attribute for   active materials\n"
        "     -vis-inactive <name>     Set the visualization attribute for inactive materials\n"
        "     -elt-active   <name>     Add active element by name. If the fractional sum of  \n"
        "                              all active elements in a volume exceeds <fraction>    \n"
        "                              the volume is considered active                       \n"
        "     -mat-active   <name>     Add material by name to the list of   active materials\n"
        "     -mat-inactive <name>     Add material by name to the list of inactive materials\n"
        "     -all-inactive            Auto set all volumes inactive, which are NOT active   \n"
        "     -fraction     <double>   Set the fraction above which the active elment content\n"
        "                              defines an active volume.                             \n"
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
DECLARE_DD4HEP_CONSTRUCTOR(DD4hep_VisProcessor,create_object)

