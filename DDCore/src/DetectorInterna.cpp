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
#include "DD4hep/detail/Handle.inl"
#include "DD4hep/detail/DetectorInterna.h"
#include "DD4hep/detail/ConditionsInterna.h"
#include "DD4hep/detail/AlignmentsInterna.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/Printout.h"
#include "TGeoVolume.h"
#include "TGeoMatrix.h"
#include "TGeoManager.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;
typedef detail::tools::PlacementPath PlacementPath;
typedef detail::tools::ElementPath   ElementPath;

DD4HEP_INSTANTIATE_HANDLE_NAMED(DetElementObject);
DD4HEP_INSTANTIATE_HANDLE_NAMED(SensitiveDetectorObject);
DD4HEP_INSTANTIATE_HANDLE_NAMED(WorldObject,DetElementObject);

/// Default constructor
SensitiveDetectorObject::SensitiveDetectorObject()
  : NamedObject(), ObjectExtensions(typeid(SensitiveDetectorObject)), magic(magic_word()),
    verbose(0), combineHits(0), ecut(0.0), readout(), region(), limits(), hitsCollection() {
  printout(VERBOSE,"SensitiveDetectorObject","+++ Created new anonymous SensitiveDetectorObject()");
  InstanceCount::increment(this);
}

/// Initializing constructor
SensitiveDetectorObject::SensitiveDetectorObject(const std::string& nam)
  : NamedObject(), ObjectExtensions(typeid(SensitiveDetectorObject)), magic(magic_word()),
    verbose(0), combineHits(0), ecut(0.0), readout(), region(), limits(), hitsCollection() {
  SetName(nam.c_str());
  printout(VERBOSE,"SensitiveDetectorObject","+++ Created new SensitiveDetectorObject('%s')",nam.c_str());
  InstanceCount::increment(this);
}

/// Internal object destructor: release extension object(s)
SensitiveDetectorObject::~SensitiveDetectorObject() {
  readout.clear();
  region.clear();
  limits.clear();
  ObjectExtensions::clear();
  InstanceCount::decrement(this);
}

/// Default constructor
DetElementObject::DetElementObject()
  : NamedObject(), ObjectExtensions(typeid(DetElementObject)), magic(magic_word()),
    flag(0), id(0), combineHits(0), typeFlag(0), level(-1), key(0), path(), placementPath(),
    idealPlace(), placement(), volumeID(0), parent(), children(),
    nominal(), survey()
{
  printout(VERBOSE,"DetElementObject","+++ Created new anonymous DetElementObject()");
  InstanceCount::increment(this);
}

/// Initializing constructor
DetElementObject::DetElementObject(const std::string& nam, int ident)
  : NamedObject(), ObjectExtensions(typeid(DetElementObject)), magic(magic_word()),
    flag(0), id(ident), combineHits(0), typeFlag(0), level(-1), key(0), path(), placementPath(),
    idealPlace(), placement(), volumeID(0), parent(), children(),
    nominal(), survey()
{
  SetName(nam.c_str());
  printout(VERBOSE,"DetElementObject","+++ Created new DetElementObject('%s', %d)",nam.c_str(),id);
  InstanceCount::increment(this);
}

/// Internal object destructor: release extension object(s)
DetElementObject::~DetElementObject() {
  destroyHandles(children);
  destroyHandle (nominal);
  destroyHandle (survey);
  placement.clear();
  idealPlace.clear();
  parent.clear();
  ObjectExtensions::clear();
  InstanceCount::decrement(this);
}

/// Deep object copy to replicate DetElement trees e.g. for reflection
DetElementObject* DetElementObject::clone(int new_id, int flg) const {
  DetElementObject* obj = new DetElementObject();
  obj->id          = new_id;
  obj->typeFlag    = typeFlag;
  obj->flag        = 0;
  obj->key         = 0;
  obj->level       = -1;
  obj->combineHits = combineHits;
  obj->nominal     = AlignmentCondition();
  obj->survey      = AlignmentCondition();
  obj->parent      = DetElement();
  if ( (flg & DetElement::COPY_PLACEMENT) == DetElement::COPY_PLACEMENT )  {
    obj->placement     = placement;
    obj->idealPlace    = idealPlace;
    obj->placementPath = "";
  }
  // This implicitly assumes that the children do not access the parent's extensions!
  obj->ObjectExtensions::clear();
  obj->ObjectExtensions::copyFrom(extensions, obj);

  obj->children.clear();
  for (const auto& i : children )  {
    const DetElementObject& d = i.second._data();
    DetElement c = d.clone(d.id, DetElement::COPY_PLACEMENT);
    c->SetName(d.GetName());
    c->SetTitle(d.GetTitle());
    bool r = obj->children.emplace(c.name(), c).second;
    if ( r ) {
      c._data().parent = obj;
    }
    else {
      except("DetElement","+++ DetElement::copy: Element %s is already "
             "present [Double-Insert]", c.name());
    }
  }
  return obj;
}

/// Reflect all volumes in a DetElement sub-tree and re-attach the placements
pair<DetElement,Volume> DetElementObject::reflect(const std::string& new_name, int new_id, SensitiveDetector sd)   {
  struct ChildMapper  {
    std::map<TGeoNode*,TGeoNode*> nodes;
    void match(DetElement de)   {
      auto i = nodes.find(de.placement().ptr());
      if ( i == nodes.end() )  {
        except("DetElement","reflect: Something went wrong when reflecting the source volume!");
      }
      de.setPlacement((*i).second);
      const auto& childrens = de.children();
      for( const auto& c : childrens )
        match(c.second);
    }
    void map(TGeoNode* n1, TGeoNode* n2)   {
      if ( nodes.find(n1) == nodes.end() )   {
        TGeoVolume* v1 = n1->GetVolume();
        TGeoVolume* v2 = n2->GetVolume();
        nodes.insert(make_pair(n1,n2));
        for(Int_t i=0; i<v1->GetNdaughters(); ++i)
          map(v1->GetNode(i), v2->GetNode(i));
      }
    }
  } mapper;
  DetElement  det(this);
  DetElement  det_ref   = det.clone(new_name, new_id);
  Volume      vol       = det.volume();
  TGeoVolume* vol_det   = vol.ptr();
  TGeoVolume* vol_ref   = vol.reflect(sd);
  const auto& childrens = det.children();

  for(Int_t i=0; i<vol_det->GetNdaughters(); ++i)
    mapper.map(vol_det->GetNode(i), vol_ref->GetNode(i));
  for(const auto& c : childrens)
    mapper.match(c.second);
  return make_pair(det_ref,vol_ref);
}

/// Access to the world object. Only possible once the geometry is closed.
World DetElementObject::i_access_world()   {
  if ( !privateWorld.isValid() )  {
    DetElementObject* p = parent.ptr();
    if ( 0 == p )  {
      privateWorld = (WorldObject*)this;
      return privateWorld;
    }
    return p->world();
  }
  return privateWorld;
}

/// Revalidate the caches
void DetElementObject::revalidate()  {
  PlacementPath par_path;
  DetElement    det(this);
  DetElement    par(det.parent());
  DetElement    wrld  = world();
  string        place = det.placementPath();

  detail::tools::placementPath(par, this, par_path);
  PlacedVolume node = detail::tools::findNode(wrld.placement(),place);
  if ( !node.isValid() )  {
    except("DetElement","The placement %s is not part of the hierarchy.",place.c_str());
  }
  printout((idealPlace.ptr() != node.ptr()) ? INFO : DEBUG,
           "DetElement","+++ Invalidate chache of %s -> %s Placement:%p --> %p %s",
           det.path().c_str(), detail::tools::placementPath(par_path).c_str(),
           placement.ptr(), node.ptr(), (placement.ptr() == node.ptr()) ? "" : "[UPDATE]");
  if ( idealPlace.ptr() != node.ptr() && 0 == node->GetUserExtension() )  {
    auto ext = idealPlace->GetUserExtension();
    node->SetUserExtension(ext);
  }
  Volume node_vol = node.volume();
  Volume plac_vol = idealPlace.volume();
  if ( node_vol.ptr() != plac_vol.ptr() && 0 == node_vol->GetUserExtension() )  {
    auto ext = plac_vol->GetUserExtension();
    node_vol->SetUserExtension(ext);    
  }
  // Now we can assign the new placement to the object
  placement = node;
  /// Now iterate down the children....
  for(const auto& i : children )
    i.second->revalidate();
}

/// Remove callback from object
void DetElementObject::removeAtUpdate(unsigned int typ, void* pointer)   {
  for (auto i=updateCalls.begin(); i != updateCalls.end(); ++i)  {
    if ( (typ&((*i).second)) == typ && (*i).first.par == pointer )  {
      updateCalls.erase(i);
      return;
    }
  }
}

/// Trigger update callbacks
void DetElementObject::update(unsigned int tags, void* param)   {
  DetElement det(this);
  const void* args[3] = { (void*)((unsigned long)tags), this, param };
  if ( (tags&DetElement::PLACEMENT_CHANGED)==DetElement::PLACEMENT_CHANGED &&
       (tags&DetElement::PLACEMENT_HIGHEST)==DetElement::PLACEMENT_HIGHEST )  {
    printout(INFO,"DetElement",
             "++ Need to update local and child caches of %s",
             det.path().c_str());
    revalidate();
  }
  for ( const auto& i : updateCalls )  {
    if ( (tags&i.second) )
      i.first.execute(args);
  }
}

/// Initializing constructor
WorldObject::WorldObject(Detector& _description, const string& nam) 
  : DetElementObject(nam,0), description(&_description)
{
}

/// Internal object destructor: release extension object(s)
WorldObject::~WorldObject()  {
}
