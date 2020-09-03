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
#include "DD4hep/Detector.h"
#include "DD4hep/GeoHandler.h"
#include "DD4hep/detail/ObjectsInterna.h"

// ROOT includes
#include "TGeoManager.h"
#include "TGeoCompositeShape.h"
#include "TGeoBoolNode.h"
#include "TClass.h"

// C/C++ include files
#include <iostream>

using namespace dd4hep::detail;
using namespace dd4hep;
using namespace std;

namespace {
  void collectSolid(GeoHandler::GeometryInfo& geo, const string& name, const string& node, TGeoShape* shape,
                    TGeoMatrix* matrix) {
    if (0 == ::strncmp(shape->GetName(), "TGeo", 4)) {
      shape->SetName(name.c_str());
    }
    if (shape->IsA() == TGeoCompositeShape::Class()) {
      const TGeoCompositeShape* s = (const TGeoCompositeShape*) shape;
      const TGeoBoolNode* boolean = s->GetBoolNode();
      collectSolid(geo, name + "_left", name + "_left", boolean->GetLeftShape(), boolean->GetLeftMatrix());
      collectSolid(geo, name + "_right", name + "_right", boolean->GetRightShape(), boolean->GetRightMatrix());
    }
    geo.solids.emplace(shape);
    geo.trafos.emplace_back(node, matrix);
  }
}

/// Default constructor
GeoHandler::GeoHandler() : m_propagateRegions(false)  {
  m_data = new map<int,set<const TGeoNode*> >();
}

/// Initializing constructor
GeoHandler::GeoHandler(map<int,set<const TGeoNode*> >* ptr)
  : m_propagateRegions(false), m_data(ptr) {
}

/// Default destructor
GeoHandler::~GeoHandler() {
  if (m_data)
    delete m_data;
  m_data = 0;
}

map<int,set<const TGeoNode*> >* GeoHandler::release() {
  map<int,set<const TGeoNode*> >* d = m_data;
  m_data = 0;
  return d;
}

/// Propagate regions. Returns the previous value
bool GeoHandler::setPropagateRegions(bool value)   {
  bool old = m_propagateRegions;
  m_propagateRegions = value;
  return old;
}

GeoHandler& GeoHandler::collect(DetElement element) {
  m_data->clear();
  return i_collect(element.placement().ptr(), 0, Region(), LimitSet());
}

GeoHandler& GeoHandler::collect(DetElement element, GeometryInfo& info) {
  m_data->clear();
  i_collect(element.placement().ptr(), 0, Region(), LimitSet());
  for (auto i = m_data->rbegin(); i != m_data->rend(); ++i) {
    const auto& mapped = (*i).second;
    for (const TGeoNode* n : mapped )  {
      TGeoVolume* v = n->GetVolume();
      if (v) {
        Material mat(v->GetMedium());
        Volume   vol(v);
        // Note : assemblies and the world do not have a real volume nor a material
        if (info.volumeSet.find(vol) == info.volumeSet.end()) {
          info.volumeSet.emplace(vol);
          info.volumes.emplace_back(vol);
        }
        if ( mat.isValid() )
          info.materials.emplace(mat);
        if (dynamic_cast<Volume::Object*>(v)) {
          VisAttr vis = vol.visAttributes();
          //Region      reg = vol.region();
          //LimitSet    lim = vol.limitSet();
          //SensitiveDetector det = vol.sensitiveDetector();

          if (vis.isValid())
            info.vis.emplace(vis);
          //if ( lim.isValid() ) info.limits[lim.ptr()].emplace(v);
          //if ( reg.isValid() ) info.regions[reg.ptr()].emplace(v);
          //if ( det.isValid() ) info.sensitives[det.ptr()].emplace(v);
        }
        collectSolid(info, v->GetName(), n->GetName(), v->GetShape(), n->GetMatrix());
      }
    }
  }
  return *this;
}

GeoHandler& GeoHandler::i_collect(const TGeoNode* current, int level, Region rg, LimitSet ls) {
  TGeoVolume* volume = current->GetVolume();
  TObjArray* nodes = volume->GetNodes();
  int num_children = nodes ? nodes->GetEntriesFast() : 0;
  Volume vol(volume);
  Region   region = vol.region();
  LimitSet limits = vol.limitSet();

  if ( m_propagateRegions )  {
    if ( !region.isValid() && rg.isValid() )   {
      region = rg;
      vol.setRegion(region);
    }
    if ( !limits.isValid() && ls.isValid() )  {
      limits = ls;
      vol.setLimitSet(limits);
    }
  }
  (*m_data)[level].emplace(current);
  //printf("GeoHandler: collect level:%d %s\n",level,current->GetName());
  if (num_children > 0) {
    for (int i = 0; i < num_children; ++i) {
      TGeoNode* node = (TGeoNode*) nodes->At(i);
      i_collect(node, level + 1, region, limits);
    }
  }
  return *this;
}

/// Initializing constructor
GeoScan::GeoScan(DetElement e) {
  m_data = GeoHandler().collect(e).release();
}

/// Initializing constructor
GeoScan::GeoScan(DetElement e, bool propagate) {
  GeoHandler h;
  h.setPropagateRegions(propagate);
  m_data = h.collect(e).release();
}

/// Default destructor
GeoScan::~GeoScan() {
  if (m_data)
    delete m_data;
  m_data = 0;
}

/// Work callback
GeoScan& GeoScan::operator()() {
  return *this;
}

