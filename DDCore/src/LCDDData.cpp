// $Id: LCDDImp.cpp 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

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
using namespace std;

/// Default constructor
LCDDData::LCDDData()
  : m_manager(0), m_world(), m_trackers(), m_worldVol(), 
    m_trackingVol(), m_field("global"), 
    m_extensions(typeid(LCDDData))
{
  InstanceCount::increment(this);
}

/// Standard destructor
LCDDData::~LCDDData() {
  clearData();
  InstanceCount::decrement(this);
}

/// Clear data content: releases all allocated resources
void LCDDData::destroyData()   {
  m_extensions.clear();
  destroyHandle(m_world);
  destroyHandle(m_field);
  destroyHandle(m_header);
  for_each(m_readouts.begin(), m_readouts.end(), destroyHandles(m_readouts));
  for_each(m_idDict.begin(), m_idDict.end(), destroyHandles(m_idDict));
  for_each(m_limits.begin(), m_limits.end(), destroyHandles(m_limits));
  for_each(m_regions.begin(), m_regions.end(), destroyHandles(m_regions));
  for_each(m_alignments.begin(), m_alignments.end(), destroyHandles(m_alignments));
  for_each(m_sensitive.begin(), m_sensitive.end(), destroyHandles(m_sensitive));
  for_each(m_display.begin(), m_display.end(), destroyHandles(m_display));
  for_each(m_fields.begin(), m_fields.end(), destroyHandles(m_fields));
  for_each(m_define.begin(), m_define.end(), destroyHandles(m_define));

  m_properties.clear();
  m_trackers.clear();
  m_worldVol.clear();
  m_trackingVol.clear();
  m_invisibleVis.clear();
  m_materialVacuum.clear();
  m_materialAir.clear();
  deletePtr(m_manager);
}


/// Clear data content: releases all allocated resources
void LCDDData::clearData()   {
  m_extensions.clear(false);
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
  m_manager = 0;
}

/// Adopt all data from source structure
void LCDDData::adoptData(LCDDData& source)   {
  m_extensions = source.m_extensions;
  m_world = source.m_world;
  m_field = source.m_field;
  m_header = source.m_header;
  m_properties = source.m_properties;
  m_readouts = source.m_readouts;
  m_idDict = source.m_idDict;
  m_limits = source.m_limits;
  m_regions = source.m_regions;
  m_alignments = source.m_alignments;
  m_sensitive = source.m_sensitive;
  m_display = source.m_display;
  m_fields = source.m_fields;
  m_define = source.m_define;
  m_trackers = source.m_trackers;
  m_worldVol = source.m_worldVol;
  m_trackingVol = source.m_trackingVol;
  m_invisibleVis = source.m_invisibleVis;
  m_materialVacuum = source.m_materialVacuum;
  m_materialAir = source.m_materialAir;
  m_manager = source.m_manager;
  source.clearData();
}


