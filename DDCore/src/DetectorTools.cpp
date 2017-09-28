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
#define DETECTORTOOLS_CPP
#include "DD4hep/DetectorTools.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Detector.h"
#include "DD4hep/detail/DetectorInterna.h"

// C/C++ include files
#include <stdexcept>
#include <memory>

// ROOT include files
#include "TGeoMatrix.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Helper namespace used to answer detector element specific questons
  /**
   * @author  M.Frank
   * @version 1.0
   */
  namespace detail { namespace tools  {
      /// Assemble the path of the PlacedVolume selection
      std::string elementPath(const PlacementPath& nodes, bool reverse);
      /// Collect detector elements to any parent detector element
      void elementPath(DetElement parent, DetElement elt, ElementPath& detectors);
      /// Collect detector elements placements to the top detector element (world) [fast, but may have holes!]
      void elementPath(DetElement elt, PlacementPath& nodes);
      /// Collect detector elements placements to the parent detector element [no holes!]
      void elementPath(DetElement parent, DetElement element, PlacementPath& nodes);
      /// Find Child of PlacedVolume and assemble on the fly the path of PlacedVolumes
      bool findChild(PlacedVolume parent, PlacedVolume child, PlacementPath& path);


      // Internal helper
      static void makePlacementPath(PlacementPath det_nodes, PlacementPath& all_nodes);
    }}
}

using namespace std;
using namespace dd4hep;

/// Find path between the child element and the parent element
bool detail::tools::isParentElement(DetElement parent, DetElement child)   {
  if ( parent.isValid() && child.isValid() )  {
    if ( parent.ptr() == child.ptr() ) return true;
    for(DetElement par=child; par.isValid(); par=par.parent())  {
      if ( par.ptr() == parent.ptr() ) return true;
    }
  }
  throw runtime_error("Search for parent detector element with invalid handles not allowed.");
}

/// Find Child of PlacedVolume and assemble on the fly the path of PlacedVolumes
bool detail::tools::findChild(PlacedVolume parent, PlacedVolume child, PlacementPath& path) {
  if ( parent.isValid() && child.isValid() ) {
    // Check self
    if ( parent.ptr() == child.ptr() ) {
      path.push_back(child);
      return true;
    }
    TIter next(parent->GetVolume()->GetNodes());
    // Now check next layer children
    for (TGeoNode *daughter = (TGeoNode*) next(); daughter; daughter = (TGeoNode*) next()) {
      if ( daughter == child.ptr() ) {
        path.push_back(daughter);
        return true;
      }
    }
    next.Reset();
    // Finally crawl down the tree
    for (TGeoNode *daughter = (TGeoNode*) next(); daughter; daughter = (TGeoNode*) next()) {
      PlacementPath sub_path;
      bool res = findChild(daughter, child, sub_path);
      if (res) {
        path.insert(path.end(), sub_path.begin(), sub_path.end());
        path.push_back(daughter);
        return res;
      }
    }
  }
  return false;
}

/// Find Child of PlacedVolume and assemble on the fly the path of PlacedVolumes
static bool findChildByName(PlacedVolume parent, PlacedVolume child, detail::tools::PlacementPath& path) {
  if ( parent.isValid() && child.isValid() ) {
    // Check self
    if ( 0 == ::strcmp(parent.ptr()->GetName(),child.ptr()->GetName()) ) {
      path.push_back(child);
      return true;
    }
    TIter next(parent->GetVolume()->GetNodes());
    // Now check next layer children
    for (TGeoNode *daughter = (TGeoNode*) next(); daughter; daughter = (TGeoNode*) next()) {
      if ( 0 == ::strcmp(daughter->GetName(),child.ptr()->GetName()) ) {
        path.push_back(daughter);
        return true;
      }
    }
    next.Reset();
    // Finally crawl down the tree
    for (TGeoNode *daughter = (TGeoNode*) next(); daughter; daughter = (TGeoNode*) next()) {
      detail::tools::PlacementPath sub_path;
      bool res = findChildByName(daughter, child, sub_path);
      if (res) {
        path.insert(path.end(), sub_path.begin(), sub_path.end());
        path.push_back(daughter);
        return res;
      }
    }
  }
  return false;
}

/// Collect detector elements to the top detector element (world)
void detail::tools::elementPath(DetElement element, ElementPath& detectors) {
  for(DetElement par = element; par.isValid(); par = par.parent())
    detectors.push_back(par);
}

/// Collect detector elements to any parent detector element
void detail::tools::elementPath(DetElement parent, DetElement child, ElementPath& detectors)  {
  detectors.clear();
  if ( parent.isValid() && child.isValid() )  {
    if ( parent.ptr() == child.ptr() )  {
      detectors.push_back(child);
      return;
    }
    ElementPath elements;
    for(DetElement par = child; par.isValid(); par = par.parent())  {
      elements.push_back(par);
      if ( par.ptr() == parent.ptr() )  {
        detectors = elements;
        return;
      }
    }
    throw runtime_error(string("The detector element ")+parent.name()+string(" is no parent of ")+child.name());
  }
  throw runtime_error("Search for parent detector element with invalid handles not allowed.");
}

/// Collect detector elements placements to the top detector element (world) [fast, but may have holes!]
void detail::tools::elementPath(DetElement parent, DetElement element, PlacementPath& det_nodes) {
  for(DetElement par = element; par.isValid(); par = par.parent())  {
    PlacedVolume pv = par.placement();
    if ( pv.isValid() )  {
      det_nodes.push_back(pv);
    }
    if ( par.ptr() == parent.ptr() ) return;
  }
  throw runtime_error(string("The detector element ")+parent.name()+string(" is no parent of ")+element.name());
}

/// Collect detector elements placements to the top detector element (world) [fast, but may have holes!]
void detail::tools::elementPath(DetElement element, PlacementPath& det_nodes) {
  for(DetElement par = element; par.isValid(); par = par.parent())  {
    PlacedVolume pv = par.placement();
    if ( pv.isValid() )  {
      det_nodes.push_back(pv);
    }
  }
}

/// Assemble the path of the PlacedVolume selection
std::string detail::tools::elementPath(const PlacementPath& nodes, bool reverse)   {
  string s = "";
  if ( reverse )  {
    for(auto i=nodes.rbegin(); i != nodes.rend(); ++i)
      s += "/" + string((*i).name());
  }
  else  {
    for(auto i=begin(nodes); i != end(nodes); ++i)
      s += "/" + string((*i)->GetName());
  }
  return s;
}

/// Assemble the path of the PlacedVolume selection
std::string detail::tools::elementPath(const ElementPath& nodes, bool reverse)  {
  string s = "";
  if ( reverse )  {
    for(ElementPath::const_reverse_iterator i=nodes.rbegin();i!=nodes.rend();++i)
      s += "/" + string((*i)->GetName());
  }
  else  {
    for(ElementPath::const_iterator i=nodes.begin();i!=nodes.end();++i)
      s += "/" + string((*i)->GetName());
  }
  return s;
}

/// Assemble the path of a particular detector element
std::string detail::tools::elementPath(DetElement element)  {
  ElementPath nodes;
  elementPath(element,nodes);
  return elementPath(nodes);
}

/// Find DetElement as child of the top level volume by it's absolute path
DetElement detail::tools::findElement(Detector& description, const std::string& path)   {
  return findDaughterElement(description.world(),path);
}

/// Find DetElement as child of a parent by it's relative or absolute path
DetElement detail::tools::findDaughterElement(DetElement parent, const std::string& subpath)  {
  if ( parent.isValid() )   {
    size_t idx = subpath.find('/',1);
    if ( subpath[0] == '/' )   {
      DetElement top = topElement(parent);
      if ( idx == string::npos ) return top;
      return findDaughterElement(top,subpath.substr(idx+1));
    }
    if ( idx == string::npos )
      return parent.child(subpath);
    string name = subpath.substr(0,idx);
    DetElement node = parent.child(name);
    if ( node.isValid() )   {
      return findDaughterElement(node,subpath.substr(idx+1));
    }
    throw runtime_error("dd4hep: DetElement "+parent.path()+" has no child named:"+name+" [No such child]");
  }
  throw runtime_error("dd4hep: Cannot determine child with path "+subpath+" from invalid parent [invalid handle]");
}

/// Determine top level element (=world) for any element walking up the detector element tree
DetElement detail::tools::topElement(DetElement child)   {
  if ( child.isValid() )   {
    if ( child.parent().isValid() )
      return topElement(child.parent());
    return child;
  }
  throw runtime_error("dd4hep: DetElement cannot determine top parent (world) [invalid handle]");
}

static void detail::tools::makePlacementPath(PlacementPath det_nodes, PlacementPath& all_nodes)   {
  for (size_t i = 0, n = det_nodes.size(); n > 0 && i < n-1; ++i)   {
    if (!findChildByName(det_nodes[i + 1], det_nodes[i], all_nodes))   {
      throw runtime_error("dd4hep: DetElement cannot determine placement path of "
                          + string(det_nodes[i].name()) + " [internal error]");
    }
  }
  if ( det_nodes.size() > 0 )   {
    all_nodes.push_back(det_nodes.back());
  }
}

/// Collect detector elements placements to the top detector element (world) [no holes!]
void detail::tools::placementPath(DetElement element, PlacementPath& all_nodes)   {
  PlacementPath det_nodes;
  elementPath(element,det_nodes);
  makePlacementPath(det_nodes, all_nodes);
}

/// Collect detector elements placements to the parent detector element [no holes!]
void detail::tools::placementPath(DetElement parent, DetElement element, PlacementPath& all_nodes)   {
  PlacementPath det_nodes;
  elementPath(parent,element,det_nodes);
  makePlacementPath(det_nodes, all_nodes);
}

/// Assemble the path of the PlacedVolume selection
std::string detail::tools::placementPath(DetElement element)  {
  PlacementPath path;
  placementPath(element,path);
  return placementPath(path);
}

/// Assemble the path of the PlacedVolume selection
std::string detail::tools::placementPath(const PlacementPath& nodes, bool reverse)  {
  string s="";
  if ( reverse )  {
    for(PlacementPath::const_reverse_iterator i=nodes.rbegin();i!=nodes.rend();++i)
      s += "/" + string((*i)->GetName());
  }
  else  {
    for(PlacementPath::const_iterator i=nodes.begin();i!=nodes.end();++i)
      s += "/" + string((*i)->GetName());
  }
  return s;
}

/// Assemble the path of the PlacedVolume selection
std::string detail::tools::placementPath(const std::vector<const TGeoNode*>& nodes, bool reverse)   {
  string s="";
  if ( reverse )  {
    for(std::vector<const TGeoNode*>::const_reverse_iterator i=nodes.rbegin();i!=nodes.rend();++i)
      s += "/" + string((*i)->GetName());
  }
  else  {
    for(std::vector<const TGeoNode*>::const_iterator i=nodes.begin();i!=nodes.end();++i)
      s += "/" + string((*i)->GetName());
  }
  return s;
}

/// Update cached matrix to transform to positions to an upper level Placement
void detail::tools::placementTrafo(const PlacementPath& nodes, bool inverse, TGeoHMatrix*& mat) {
  if ( !mat ) mat = new TGeoHMatrix(*gGeoIdentity);
  placementTrafo(nodes,inverse,*mat);
}

/// Update cached matrix to transform to positions to an upper level Placement
void detail::tools::placementTrafo(const PlacementPath& nodes, bool inverse, TGeoHMatrix& mat) {
  mat = *gGeoIdentity;
  if (nodes.size() > 0) {
    for (size_t i = 0, n=nodes.size(); n>0 && i < n-1; ++i)  {
      const PlacedVolume& p = nodes[i];
      mat.MultiplyLeft(p->GetMatrix());
    }
    if ( inverse ) mat = mat.Inverse();
  }
}

/// Find a given node in the hierarchy starting from the top node (absolute placement!)
PlacedVolume detail::tools::findNode(PlacedVolume top_place, const std::string& place)   {
  TGeoNode* top = top_place.ptr();
  const char* path = place.c_str();
  // Check if a geometry path is valid without changing the state of the navigator.
  Int_t length = strlen(path);
  if (!length) return 0;
  TString spath = path;
  TGeoVolume *vol;
  // Check first occurance of a '/'
  Int_t ind1 = spath.Index("/");
  if (ind1<0) {
    // No '/' so we check directly the path against the name of the top
    if ( strcmp(path,top->GetName()) ) return 0;
    return top;
  }
  Int_t ind2 = ind1;
  Bool_t end = kFALSE;
  if (ind1>0) ind1 = -1;   // no trailing '/'
  else ind2 = spath.Index("/", ind1+1);

  if (ind2<0) ind2 = length;
  TString name(spath(ind1+1, ind2-ind1-1));
  if ( name == top->GetName() ) {
    if (ind2>=length-1) return top;
    ind1 = ind2;
  }
  else  {
    return 0;
  }
  TGeoNode *node = top;
  // Deeper than just top level
  while (!end) {
    ind2 = spath.Index("/", ind1+1);
    if (ind2<0) {
      ind2 = length;
      end  = kTRUE;
    }
    vol = node->GetVolume();
    name = spath(ind1+1, ind2-ind1-1);
    node = vol->GetNode(name.Data());
    if (!node)
      return 0;
    else if (ind2>=length-1)
      return node;
    ind1 = ind2;
  }
  return node;
}

/// Convert VolumeID to string
std::string detail::tools::toString(const PlacedVolume::VolIDs& ids)   {
  stringstream log;
  for( const auto& v : ids )
    log << v.first << "=" << v.second << "; ";
  return log.str();
}

/// Convert VolumeID to string
std::string detail::tools::toString(const IDDescriptor& dsc, const PlacedVolume::VolIDs& ids, VolumeID code)   {
  stringstream log;
  for( const auto& id : ids )  {
    const BitFieldElement* f = dsc.field(id.first);
    VolumeID value = f->value(code);
    log << id.first << "=" << id.second << "," << value << " [" << f->offset() << "," << f->width() << "] ";
  }
  return log.str();
}


