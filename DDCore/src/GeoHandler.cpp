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
#include <DD4hep/Detector.h>
#include <DD4hep/GeoHandler.h>
#include <DD4hep/detail/ObjectsInterna.h>

// ROOT includes
#include <TGeoManager.h>
#include <TGeoCompositeShape.h>
#include <TGeoBoolNode.h>
#include <TClass.h>

// C/C++ include files
#include <iostream>

using namespace dd4hep;

namespace {

  void collectSolid(detail::GeoHandler::GeometryInfo& geo,
                    const std::string& name,
                    const std::string& node,
                    TGeoShape* shape,
                    TGeoMatrix* matrix)
  {
    if ( 0 == ::strncmp(shape->GetName(), "TGeo", 4) )  {
      shape->SetName(name.c_str());
    }
    if ( shape->IsA() == TGeoCompositeShape::Class() )  {
      const TGeoCompositeShape* s = (const TGeoCompositeShape*) shape;
      const TGeoBoolNode* boolean = s->GetBoolNode();
      collectSolid(geo, name + "_left", name + "_left", boolean->GetLeftShape(), boolean->GetLeftMatrix());
      collectSolid(geo, name + "_right", name + "_right", boolean->GetRightShape(), boolean->GetRightMatrix());
    }
    if(geo.solid_set.emplace(shape).second) {
      geo.solids.push_back(shape);
    }
    geo.trafos.emplace_back(node, matrix);
  }
}

/// Default constructor
detail::GeoHandler::GeoHandler()  {
  m_data = new std::map<int, std::set<const TGeoNode*> >();
}

/// Initializing constructor
detail::GeoHandler::GeoHandler(std::map<int, std::set<const TGeoNode*> >* ptr,
                               std::map<const TGeoNode*, std::vector<TGeoNode*> >* daus)
  : m_data(ptr), m_daughters(daus)
{
}

/// Default destructor
detail::GeoHandler::~GeoHandler() {
  if (m_data)
    delete m_data;
  m_data = nullptr;
}

std::map<int, std::set<const TGeoNode*> >* detail::GeoHandler::release() {
  std::map<int, std::set<const TGeoNode*> >* d = m_data;
  m_data = nullptr;
  return d;
}

/// Propagate regions. Returns the previous value
bool detail::GeoHandler::setPropagateRegions(bool value)   {
  bool old = m_propagateRegions;
  m_propagateRegions = value;
  return old;
}

detail::GeoHandler& detail::GeoHandler::collect(DetElement element) {
  DetElement par = element.parent();
  TGeoNode*  par_node = par.isValid() ? par.placement().ptr() : nullptr;
  m_data->clear();
  return i_collect(par_node, element.placement().ptr(), 0, Region(), LimitSet());
}

detail::GeoHandler& detail::GeoHandler::collect(DetElement element, GeometryInfo& info) {
  DetElement par = element.parent();
  TGeoNode* par_node = par.isValid() ? par.placement().ptr() : nullptr;
  m_data->clear();
  i_collect(par_node, element.placement().ptr(), 0, Region(), LimitSet());
  for ( auto i = m_data->rbegin(); i != m_data->rend(); ++i ) {
    const auto& mapped = (*i).second;
    for ( const TGeoNode* n : mapped )  {
      TGeoVolume* v = n->GetVolume();
      if ( v ) {
        Material mat(v->GetMedium());
        Volume   vol(v);
        // Note : assemblies and the world do not have a real volume nor a material
        if ( info.volumeSet.find(vol) == info.volumeSet.end() ) {
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

detail::GeoHandler& detail::GeoHandler::i_collect(const TGeoNode* /* parent */,
                                                  const TGeoNode*    current,
                                                  int level, Region rg, LimitSet ls)
{
  TGeoVolume* vol    = current->GetVolume();
  TObjArray*  nodes  = vol->GetNodes();
  Volume      volume = vol;
  Region      region = volume.region();
  LimitSet    limits = volume.limitSet();

  if ( m_propagateRegions )  {
    if ( !region.isValid() && rg.isValid() )   {
      region = rg;
      volume.setRegion(region);
    }
    if ( !limits.isValid() && ls.isValid() )  {
      limits = ls;
      volume.setLimitSet(limits);
    }
  }
  /// Collect the hierarchy of placements
  (*m_data)[level].emplace(current);
  int num = nodes ? nodes->GetEntriesFast() : 0;
  for (int i = 0; i < num; ++i)
    i_collect(current, (TGeoNode*)nodes->At(i), level + 1, region, limits);
  /// Now collect all the daughters of this volume, so that we can reconnect them in the correct order
  if ( m_daughters && m_daughters->find(current) == m_daughters->end() )  {
    auto [idau,success] = m_daughters->emplace(current, std::vector<TGeoNode*>());
    for (int i = 0; i < num; ++i)
      idau->second.push_back((TGeoNode*)nodes->At(i));
  }
  return *this;
}

/// Initializing constructor
detail::GeoScan::GeoScan(DetElement e)  {
  m_data = GeoHandler().collect(e).release();
}

/// Initializing constructor
detail::GeoScan::GeoScan(DetElement e, bool propagate) {
  GeoHandler h;
  h.setPropagateRegions(propagate);
  m_data = h.collect(e).release();
}

/// Default destructor
detail::GeoScan::~GeoScan() {
  if (m_data)
    delete m_data;
  m_data = 0;
}

/// Work callback
detail::GeoScan& detail::GeoScan::operator()() {
  return *this;
}

