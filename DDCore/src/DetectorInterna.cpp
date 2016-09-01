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
#include "DD4hep/objects/ConditionsInterna.h"
#include "DD4hep/objects/AlignmentsInterna.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/Handle.inl"
#include "DD4hep/Printout.h"
#include "TGeoVolume.h"
#include "TGeoMatrix.h"
#include "TGeoManager.h"
#include "DD4hep/objects/DetectorInterna.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;
typedef DetectorTools::PlacementPath PlacementPath;
typedef DetectorTools::ElementPath   ElementPath;

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
    idealPlace(), placement(), volumeID(0), parent(), reference(), children(),
    nominal(), survey(), alignments(), conditions(),
    worldTrafo(), parentTrafo(), referenceTrafo(0) {
  printout(VERBOSE,"DetElementObject","+++ Created new anonymous DetElementObject()");
  InstanceCount::increment(this);
}

/// Initializing constructor
DetElementObject::DetElementObject(const std::string& nam, int ident)
  : NamedObject(), ObjectExtensions(typeid(DetElementObject)), magic(magic_word()),
    flag(0), id(ident), combineHits(0), typeFlag(0), level(-1), key(0), path(), placementPath(),
    idealPlace(), placement(), volumeID(0), parent(), reference(), children(),
    nominal(), survey(), alignments(), conditions(),
    worldTrafo(), parentTrafo(), referenceTrafo(0) {
  SetName(nam.c_str());
  printout(VERBOSE,"DetElementObject","+++ Created new DetElementObject('%s', %d)",nam.c_str(),id);
  InstanceCount::increment(this);
}

/// Internal object destructor: release extension object(s)
DetElementObject::~DetElementObject() {
  destroyHandles(children);
  deletePtr (referenceTrafo);
  destroyHandle (conditions);
  conditions = ConditionsContainer();
  destroyHandle (alignments);
  alignments = AlignmentsContainer();
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
  obj->nominal     = Alignment();
  obj->survey      = Alignment();
  obj->conditions  = ConditionsContainer();
  obj->alignments  = AlignmentsContainer();
  obj->parent = DetElement();
  if ( (flg & DetElement::COPY_PLACEMENT) == DetElement::COPY_PLACEMENT )  {
    obj->placement  = placement;
    obj->idealPlace = idealPlace;
    obj->placementPath = placementPath;
  }
  // This implicitly assumes that the children do not access the parent's extensions!
  obj->ObjectExtensions::clear();
  obj->ObjectExtensions::copyFrom(extensions, obj);

  obj->children.clear();
  for (DetElement::Children::const_iterator i = children.begin(); i != children.end(); ++i) {
    const NamedObject* pc = (*i).second.ptr();
    const DetElementObject& d = (*i).second._data();
    DetElement child(d.clone(d.id, DetElement::COPY_PLACEMENT), pc->GetName(), pc->GetTitle());
    pair<Children::iterator, bool> r = obj->children.insert(make_pair(child.name(), child));
    if (r.second) {
      child._data().parent = obj;
    }
    else {
      throw runtime_error("DD4hep: DetElement::copy: Element " + string(child.name()) +
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

/// Create cached matrix to transform to world coordinates
const TGeoHMatrix& DetElementObject::worldTransformation() {
  if ( (flag&HAVE_WORLD_TRAFO) == 0 ) {
    PlacementPath nodes;
    flag |= HAVE_WORLD_TRAFO;
    DetectorTools::placementPath(this, nodes);
    DetectorTools::placementTrafo(nodes,false,worldTrafo);
  }
  return worldTrafo;
}

/// Create cached matrix to transform to parent coordinates
const TGeoHMatrix& DetElementObject::parentTransformation() {
  if ( (flag&HAVE_PARENT_TRAFO) == 0 ) {
    PlacementPath nodes;
    flag |= HAVE_PARENT_TRAFO;
    DetectorTools::placementPath(DetElement(parent), this, nodes);
    DetectorTools::placementTrafo(nodes,false,parentTrafo);
  }
  return parentTrafo;
}

/// Create cached matrix to transform to reference coordinates
const TGeoHMatrix& DetElementObject::referenceTransformation() {
  if (!referenceTrafo) {
    DetElement  ref(reference);
    DetElement  self(this);
    if ( ref.ptr() == self.ptr() )  {
      referenceTrafo = new TGeoHMatrix(gGeoIdentity->Inverse());
    }
    else if ( DetectorTools::isParentElement(ref,self) ) {
      PlacementPath nodes;
      DetectorTools::placementPath(ref,self,nodes);
      DetectorTools::placementTrafo(nodes,false,referenceTrafo);
    }
    else if ( DetectorTools::isParentElement(self,ref) ) {
      PlacementPath nodes;
      DetectorTools::placementPath(self,ref,nodes);
      DetectorTools::placementTrafo(nodes,false,referenceTrafo);
    }
    else  {
      throw runtime_error("DD4hep: referenceTransformation: No path from " + string(self.name()) +
                          " to reference element " + string(ref.name()) + " present!");
    }
  }
  return *referenceTrafo;
}

/// Revalidate the caches
void DetElementObject::revalidate(TGeoHMatrix* parent_world_trafo)  {
  PlacementPath par_path;
  DetElement    det(this);
  DetElement    par(det.parent());
  DetElement    wrld  = world();
  bool          print = (DEBUG > printLevel());
  string        place = det.placementPath();
  bool     have_trafo = (flag&HAVE_WORLD_TRAFO);

  DetectorTools::placementPath(par, this, par_path);
  PlacedVolume node = DetectorTools::findNode(wrld.placement(),place);
  if ( !node.isValid() )  {
    throw runtime_error("DD4hep: DetElement: The placement " + place + " is not part of the hierarchy.");
  }
  //print = (idealPlace.ptr() != node.ptr());
  printout((idealPlace.ptr() != node.ptr()) ? INFO : DEBUG,
           "DetElement","+++ Invalidate chache of %s -> %s Placement:%p --> %p %s",
           det.path().c_str(), DetectorTools::placementPath(par_path).c_str(),
           placement.ptr(), node.ptr(), (placement.ptr() == node.ptr()) ? "" : "[UPDATE]");

  placement = node;

  if ( have_trafo && print )  worldTrafo.Print();

  if ( (flag&HAVE_PARENT_TRAFO) )  {
    DetectorTools::placementTrafo(par_path,false,parentTrafo);
  }

  /// Compute world transformations
  if ( parent_world_trafo )  {
    // If possible use the pre-computed values from the parent
    worldTrafo = *parent_world_trafo;
    worldTrafo.Multiply(&parentTransformation());
    flag |= HAVE_WORLD_TRAFO;
  }
  else if ( have_trafo )  {
    // Else re-compute the transformation to the world.
    PlacementPath world_nodes;
    DetectorTools::placementPath(this, world_nodes);
    DetectorTools::placementTrafo(world_nodes,false,worldTrafo);
    flag |= HAVE_WORLD_TRAFO;
  }

  if ( (flag&HAVE_PARENT_TRAFO) && print )  worldTrafo.Print();
  deletePtr (referenceTrafo);

  /// Now iterate down the children....
  for(Children::const_iterator i = children.begin(); i!=children.end(); ++i)  {
    DetElement d((*i).second);
    d->revalidate(&worldTrafo);
  }
}

/// Remove callback from object
void DetElementObject::removeAtUpdate(unsigned int typ, void* pointer)   {
  for(UpdateCallbacks::iterator i=updateCalls.begin(); i != updateCalls.end(); ++i)  {
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
    printout(INFO,"DetElement","+++ Need to update chaches and child caches of %s",
             det.path().c_str());
    DetElement par = det.parent();
    TGeoHMatrix* parent_world_trafo = 0;
    if ( par.isValid() && (par->flag&HAVE_PARENT_TRAFO) )  {
      parent_world_trafo = &par->worldTrafo;
    }
    revalidate(parent_world_trafo);
  }
  for(UpdateCallbacks::const_iterator i=updateCalls.begin(); i != updateCalls.end(); ++i)  {
    if ( (tags&((*i).second)) )  {
      (*i).first.execute(args);
    }
  }
}

Conditions::Container DetElementObject::assign_conditions()  {
  if ( !conditions.isValid() )  {
    conditions.assign(new Conditions::Container::Object(this),"conditions","");
  }
  return conditions;
}

/// Initializing constructor
WorldObject::WorldObject(LCDD& _lcdd, const string& nam) 
  : DetElementObject(nam,0), lcdd(&_lcdd),
    conditionsLoader(0), conditionsManager(0), alignmentsLoader(0), alignmentsManager(0)
{
}

/// Internal object destructor: release extension object(s)
WorldObject::~WorldObject()  {
}
