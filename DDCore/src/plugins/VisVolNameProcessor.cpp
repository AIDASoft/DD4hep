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
#include <DD4hep/VolumeProcessor.h>

// C/C++ include files
#include <map>
#include <regex>

/// Namespace for the AIDA detector description toolkit
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
    typedef std::map<std::string,std::regex> Matches;
    Detector&                description;
    Matches                  matches;
    std::string              name;
    size_t                   numApplied = 0;
    bool                     show = false;
    VisAttr                  visattr;
    /// Print properties
    void _show();
  public:
    /// Initializing constructor
    VisVolNameProcessor(Detector& desc);
    /// Default destructor
    virtual ~VisVolNameProcessor();
    /// Set volume matches
    void set_match(const std::vector<std::string>& matches);
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

//#include <DD4hep/VisVolNameProcessor.h>
#include <DD4hep/Printout.h>
#include <DD4hep/DetectorTools.h>
#include <DD4hep/DetectorHelper.h>
#include <DD4hep/DetFactoryHelper.h>
#include <sstream>

using namespace std;
using namespace dd4hep;

/// Initializing constructor
VisVolNameProcessor::VisVolNameProcessor(Detector& desc)
  : description(desc), name()
{
}

/// Default destructor
VisVolNameProcessor::~VisVolNameProcessor()   {
  if ( show )  {
    printout(ALWAYS,name,"++       %8ld vis-attrs '%s' applied.",
	     numApplied, visattr.isValid() ? visattr.name() : "");
  }
}

/// Set volume matches
void VisVolNameProcessor::set_match(const std::vector<std::string>& vals)  {
  for( const auto& v : vals )
    matches[v] = regex(v);
}

/// Callback to output PlacedVolume information of an single Placement
int VisVolNameProcessor::operator()(PlacedVolume pv, int /* level */)   {
  Volume vol = pv.volume();
  if ( vol.visAttributes().ptr() != visattr.ptr() )   {
    string vol_nam = vol.name();
    for ( const auto& m : matches )   {
      if ( std::regex_match(vol_nam, m.second) )  {
	printout(ALWAYS,m.first,"++       Set visattr %s to %s",
		 visattr.isValid() ? visattr.name() : "", vol_nam.c_str());
	vol.setVisAttributes(visattr);
	++numApplied;
	return 1;
      }
      //printout(ALWAYS,m.first,"++       FAILED %s",vol_nam.c_str());
    }
  }
  return 1;
}

static void* create_object(Detector& description, int argc, char** argv)   {
  string         vis_name;
  vector<string> vol_names;
  DetectorHelper helper(description);
  VisVolNameProcessor*  proc = new VisVolNameProcessor(description);
  for ( int i=0; i<argc; ++i )   {
    if ( argv[i] )    {
      if ( ::strncmp(argv[i],"-name",4) == 0 )   {
	proc->name = argv[++i];
        vol_names.push_back(proc->name);
	if ( vis_name.empty() ) vis_name = proc->name;
        continue;
      }
      else if ( ::strncmp(argv[i],"-match",4) == 0 )   {
        vol_names.push_back(argv[++i]);
	if ( vis_name.empty()   ) vis_name = vol_names.back();
	if ( proc->name.empty() ) proc->name = vol_names.back();
        continue;
      }
      else if ( ::strncmp(argv[i],"-vis",4) == 0 )   {
        vis_name = argv[++i];
        continue;
      }
      else if ( ::strncmp(argv[i],"-show",4) == 0 )   {
        proc->show = true;
        continue;
      }
      cout <<
        "Usage: DD4hep_VisVolNameProcessor -arg [-arg]                                       \n"
        "     -match <regex>           Regular expression matching volume name               \n"
        "     -show                    Print setup to output device (stdout)                 \n"
        "\tArguments given: " << arguments(argc,argv) << endl << flush;
      ::exit(EINVAL);
    }
  }
  proc->set_match(vol_names);
  proc->visattr = description.visAttributes(vis_name);
  if ( !proc->visattr.ptr() )   {
    except(proc->name,"+++ Unknown visual attribute: %s",vis_name.c_str());
  }
  PlacedVolumeProcessor* placement_proc = proc;
  return (void*)placement_proc;
}

// first argument is the type from the xml file
DECLARE_DD4HEP_CONSTRUCTOR(DD4hep_VisVolNameProcessor,create_object)

