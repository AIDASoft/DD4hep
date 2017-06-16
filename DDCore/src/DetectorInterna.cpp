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

DD4HEP_INSTANTIATE_HANDLE_NAMED(WorldObject);
DD4HEP_INSTANTIATE_HANDLE_NAMED(DetElementObject);
DD4HEP_INSTANTIATE_HANDLE_NAMED(SensitiveDetectorObject);

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

/// detaill object destructor: release extension object(s)
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

/// detaill object destructor: release extension object(s)
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
    obj->placement  = placement;
    obj->idealPlace = idealPlace;
    obj->placementPath = placementPath;
  }
  // This implicitly assumes that the children do not access the parent's extensions!
  obj->ObjectExtensions::clear();
  obj->ObjectExtensions::copyFrom(extensions, obj);

  obj->children.clear();
  for (const auto& i : children )  {
    const NamedObject* pc = i.second.ptr();
    const DetElementObject& d = i.second._data();
    DetElement child(d.clone(d.id, DetElement::COPY_PLACEMENT), pc->GetName(), pc->GetTitle());
    pair<DetElement::Children::iterator, bool> r = obj->children.insert(make_pair(child.name(), child));
    if (r.second) {
      child._data().parent = obj;
    }
    else {
      throw runtime_error("dd4hep: DetElement::copy: Element " + string(child.name()) +
                          " is already present [Double-Insert]");
    }
  }
  return obj;
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

/// detaill object destructor: release extension object(s)
WorldObject::~WorldObject()  {
}
