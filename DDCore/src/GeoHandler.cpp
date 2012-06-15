// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DD4hep/LCDD.h"
#include "DD4hep/GeoHandler.h"

// ROOT includes
#include "TGeoManager.h"
#include "TGeoCompositeShape.h"
#include "TGeoBoolNode.h"

// C/C++ include files
#include <iostream>

using namespace DD4hep::Geometry;
using namespace DD4hep;
using namespace std;

namespace {
  void collectSolid(GeoHandler::GeometryInfo& geo, const string& name, const string& node, TGeoShape* shape, TGeoMatrix* matrix)     {
    if ( shape->IsA() == TGeoCompositeShape::Class() ) {
      const TGeoCompositeShape* s = (const TGeoCompositeShape*)shape;
      const TGeoBoolNode* boolean = s->GetBoolNode();
      collectSolid(geo, name+"_left",  name+"_left",  boolean->GetLeftShape(),  boolean->GetLeftMatrix());
      collectSolid(geo, name+"_right", name+"_right", boolean->GetRightShape(), boolean->GetRightMatrix());
    }
    geo.solids.insert(make_pair(name,shape));
    geo.trafos.push_back(make_pair(node,matrix));
  }
}

/// Default constructor
GeoHandler::GeoHandler() {
  m_data = new Data();
}

/// Initializing constructor
GeoHandler::GeoHandler(Data* ptr) : m_data(ptr) {
}

/// Default destructor
GeoHandler::~GeoHandler() {
  if ( m_data ) delete m_data;
  m_data = 0;
}

GeoHandler::Data* GeoHandler::release() {
  Data* d = m_data;
  m_data = 0;
  return d;
}

GeoHandler& GeoHandler::collect(DetElement element) {
  m_data->clear();
  return i_collect(element.placement().ptr(),0);
}

GeoHandler& GeoHandler::collect(DetElement element, GeometryInfo& info) {
  m_data->clear();
  i_collect(element.placement().ptr(),0);
  for(Data::const_reverse_iterator i=m_data->rbegin(); i != m_data->rend(); ++i)   {
    int level = (*i).first;
    const Data::mapped_type& v = (*i).second;
    for(Data::mapped_type::const_iterator j=v.begin(); j != v.end(); ++j) {
      const TGeoNode* n = *j;
      TGeoVolume* v = n->GetVolume();
      TGeoMedium* m = v->GetMedium();
      Volume      vol = Handle<>(v);
      VisAttr     vis = vol.visAttributes();

      info.volumes.insert(make_pair(v->GetName(),v));
      info.materials[m->GetName()] = m;
      if ( vis.isValid() ) info.vis[v->GetName()] = vis;
      collectSolid(info,v->GetName(),n->GetName(),v->GetShape(),n->GetMatrix());
    }
  }
  return *this;
}

GeoHandler& GeoHandler::i_collect(const TGeoNode* current, int level) {
  TGeoVolume* volume = current->GetVolume();
  TObjArray*  nodes  = volume->GetNodes();
  int   num_children = nodes ? nodes->GetEntriesFast() : 0;

  (*m_data)[level].insert(current);
  if ( num_children > 0 )   {
    for(int i=0; i<num_children; ++i)   {
      TGeoNode* node = (TGeoNode*)nodes->At(i);
      i_collect(node,level+1);
    }
  }
  return *this;
}

/// Initializing constructor
GeoScan::GeoScan(DetElement e) {
  m_data = GeoHandler().collect(e).release();
}

/// Default destructor
GeoScan::~GeoScan() {
  if ( m_data ) delete m_data;
  m_data = 0;
}

/// Work callback
GeoScan& GeoScan::operator()() {
  return *this;
}

