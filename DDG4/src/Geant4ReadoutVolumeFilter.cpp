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
#include "DD4hep/Readout.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/detail/ObjectsInterna.h"
#include "DDG4/Geant4Mapping.h"
#include "DDG4/Geant4StepHandler.h"
#include "DDG4/Geant4VolumeManager.h"
#include "DDG4/Geant4FastSimHandler.h"
#include "DDG4/Geant4ReadoutVolumeFilter.h"

using namespace dd4hep::sim;

/// Standard constructor
Geant4ReadoutVolumeFilter::Geant4ReadoutVolumeFilter(Geant4Context* ctxt, 
                                                     const std::string& nam, 
                                                     Readout ro, 
                                                     const std::string& coll)
  : Geant4Filter(ctxt, nam), m_readout(ro), m_collection(0), m_key(0)
{
  InstanceCount::increment(this);
  for(size_t i=0; i<ro->hits.size(); ++i)  {
    const HitCollection& c = ro->hits[i];
    if ( c.name == coll )   {
      m_collection = &c;
      m_key = ro.idSpec().field(c.key);
      return;
    }
  }
  except("+++ Custom collection name '%s' not defined in the Readout object: %s.",
         coll.c_str(), ro.name());      
}

/// Default destructor
Geant4ReadoutVolumeFilter::~Geant4ReadoutVolumeFilter() {
  InstanceCount::decrement(this);
}

/// Filter action. Return true if hits should be processed
bool Geant4ReadoutVolumeFilter::operator()(const G4Step* step) const    {
  Geant4StepHandler stepH(step);
  Geant4VolumeManager volMgr = Geant4Mapping::instance().volumeManager();
  VolumeID id  = volMgr.volumeID(stepH.preTouchable());
  FieldID  key = m_key->value(id);
  if ( m_collection->key_min <= key && m_collection->key_max >= key )
    return true;
  return false;
}

/// GFLASH/FastSim interface: Filter action. Return true if hits should be processed
bool Geant4ReadoutVolumeFilter::operator()(const Geant4FastSimSpot* spot) const    {
  Geant4FastSimHandler spotH(spot);
  Geant4VolumeManager volMgr = Geant4Mapping::instance().volumeManager();
  VolumeID id  = volMgr.volumeID(spotH.touchable());
  FieldID  key = m_key->value(id);
  if ( m_collection->key_min <= key && m_collection->key_max >= key )
    return true;
  return false;
}
