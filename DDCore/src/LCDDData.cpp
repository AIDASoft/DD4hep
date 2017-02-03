// $Id: $
//==========================================================================
//  AIDA Detector description implementation for LCD
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
#include "DD4hep/LCDDData.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/objects/ObjectsInterna.h"
#include "DD4hep/objects/DetectorInterna.h"

// ROOT include files
#include "TGeoManager.h"

namespace DD4hep {  namespace Geometry {    class LCDDImp;  }}

using namespace DD4hep::Geometry;
using namespace DD4hep;

/// Default constructor
LCDDData::LCDDData()
  : m_manager(0), m_world(), m_trackers(), m_worldVol(),
    m_trackingVol(), m_field("global"),
    m_buildType(BUILD_DEFAULT), m_extensions(typeid(LCDDData)), m_volManager(),
    m_inhibitConstants(false)
{
  InstanceCount::increment(this);
}

/// Standard destructor
LCDDData::~LCDDData() {
  clearData();
  InstanceCount::decrement(this);
}

/// Clear data content: releases all allocated resources
void LCDDData::destroyData(bool destroy_mgr)   {
  m_extensions.clear();
  m_motherVolumes.clear();

  destroyHandle(m_world);
  destroyHandle(m_field);
  destroyHandle(m_header);
  destroyHandles(m_readouts);
  destroyHandles(m_idDict);
  destroyHandles(m_limits);
  destroyHandles(m_regions);
  destroyHandles(m_alignments);
  destroyHandles(m_sensitive);
  destroyHandles(m_display);
  destroyHandles(m_fields);
  destroyHandles(m_define);

  destroyHandle(m_volManager);
  m_properties.clear();
  m_trackers.clear();
  m_trackingVol.clear();
  m_worldVol.clear();
  m_invisibleVis.clear();
  m_materialVacuum.clear();
  m_materialAir.clear();
  m_inhibitConstants = false;
  if ( destroy_mgr )
    deletePtr(m_manager);
  else  {
    gGeoManager = m_manager;
    m_manager = 0;
  }
}


/// Clear data content: releases all allocated resources
void LCDDData::clearData()   {
  m_extensions.clear(false);
  m_motherVolumes.clear();
  m_world.clear();
  m_field.clear();
  m_header.clear();
  m_properties.clear();
  m_readouts.clear();
  m_idDict.clear();
  m_limits.clear();
  m_regions.clear();
  m_alignments.clear();
  m_sensitive.clear();
  m_display.clear();
  m_fields.clear();
  m_define.clear();
  m_trackers.clear();
  m_worldVol.clear();
  m_trackingVol.clear();
  m_invisibleVis.clear();
  m_materialVacuum.clear();
  m_materialAir.clear();
  m_volManager.clear();
  m_manager = 0;
  m_inhibitConstants = false;
}

/// Adopt all data from source structure
void LCDDData::adoptData(LCDDData& source)   {
  m_inhibitConstants = source.m_inhibitConstants;
  m_extensions.move(source.m_extensions);
  m_motherVolumes  = source.m_motherVolumes;
  m_world          = source.m_world;
  m_field          = source.m_field;
  m_header         = source.m_header;
  m_properties     = source.m_properties;
  m_readouts       = source.m_readouts;
  m_idDict         = source.m_idDict;
  m_limits         = source.m_limits;
  m_regions        = source.m_regions;
  m_alignments     = source.m_alignments;
  m_sensitive      = source.m_sensitive;
  m_display        = source.m_display;
  m_fields         = source.m_fields;
  m_define         = source.m_define;
  m_trackers       = source.m_trackers;
  m_worldVol       = source.m_worldVol;
  m_trackingVol    = source.m_trackingVol;
  m_invisibleVis   = source.m_invisibleVis;
  m_materialVacuum = source.m_materialVacuum;
  m_materialAir    = source.m_materialAir;
  m_manager        = source.m_manager;
  m_volManager     = source.m_volManager;
  source.clearData();
}
