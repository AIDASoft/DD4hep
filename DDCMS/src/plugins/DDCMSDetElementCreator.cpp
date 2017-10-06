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
//
// Specialized generic detector constructor
// 
//==========================================================================

// Framework include files
#include "DD4hep/VolumeProcessor.h"
#include "DD4hep/DetFactoryHelper.h"

using namespace std;
using namespace dd4hep;


namespace {

  
  class DDCMSDetElementCreator : public PlacedVolumeProcessor  {
    struct Data {
      bool sensitive = false;
      Data() = default;
      Data(const Data& d) = default;
      Data& operator=(const Data& d) = default;
    };
    typedef std::vector<std::pair<PlacedVolume,Data> > VolumeStack;
    VolumeStack* stack;

  public:
    /// Initializing constructor
    DDCMSDetElementCreator();
    /// Default destructor
    virtual ~DDCMSDetElementCreator();
    /// Callback to output PlacedVolume information of an single Placement
    virtual int operator()(PlacedVolume pv, int level)  const;    
    /// Callback to output PlacedVolume information of an entire Placement
    virtual int process(PlacedVolume pv, int level, bool recursive)  const;
  };
}


#include "DD4hep/Printout.h"

/// Initializing constructor
DDCMSDetElementCreator::DDCMSDetElementCreator()  {
  stack = new VolumeStack();
}

/// Default destructor
DDCMSDetElementCreator::~DDCMSDetElementCreator()   {
  detail::deletePtr(stack);
}

/// Callback to output PlacedVolume information of an single Placement
int DDCMSDetElementCreator::operator()(PlacedVolume pv, int level)  const  {
  Volume vol = pv.volume();
  char sens  = vol.isSensitive() ? 'S' : ' ';
  printout(INFO,"DDCMSDetElementCreator",
           "++ %3d  %s [%s] %c", level, pv.name(), vol.name(), sens);
  return 1;
}

/// Callback to output PlacedVolume information of an entire Placement
int DDCMSDetElementCreator::process(PlacedVolume pv, int level, bool recursive)  const   {
  stack->push_back(make_pair(pv,Data()));
  int ret = PlacedVolumeProcessor::process(pv,level,recursive);
  stack->pop_back();
  return ret;
}

static void* create_object(Detector& /* description */, int /* argc */, char** /* argv */)   {
  PlacedVolumeProcessor* proc = new DDCMSDetElementCreator();
  return proc;
}

// first argument is the type from the xml file
DECLARE_DD4HEP_CONSTRUCTOR(DDCMS_DetElementCreator,create_object)

