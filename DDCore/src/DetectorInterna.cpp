// $Id: Detector.cpp 1087 2014-04-09 12:25:51Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/objects/ConditionsInterna.h"
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

DD4HEP_INSTANTIATE_HANDLE_NAMED(DetElementObject);
DD4HEP_INSTANTIATE_HANDLE_NAMED(SensitiveDetectorObject);

/// Default constructor
SensitiveDetectorObject::SensitiveDetectorObject()
  : NamedObject(), ObjectExtensions(typeid(SensitiveDetectorObject)), magic(magic_word()), verbose(0), combineHits(0), ecut(0.0), readout(), region(), limits(), hitsCollection() {
  printout(DEBUG,"SensitiveDetectorObject","+++ Created new anonymous SensitiveDetectorObject()");
  InstanceCount::increment(this);
}

/// Initializing constructor
SensitiveDetectorObject::SensitiveDetectorObject(const std::string& nam)
  : NamedObject(), ObjectExtensions(typeid(SensitiveDetectorObject)), magic(magic_word()), verbose(0), combineHits(0), ecut(0.0), readout(), region(), limits(), hitsCollection() {
  SetName(nam.c_str());
  printout(DEBUG,"SensitiveDetectorObject","+++ Created new SensitiveDetectorObject('%s')",nam.c_str());
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
  : NamedObject(), ObjectExtensions(typeid(DetElementObject)), magic(magic_word()), flag(0), id(0), combineHits(0), path(), placementPath(),
    idealPlace(), placement(), volumeID(0), parent(), reference(), children(),
    alignment(), volume_alignments(), conditions(), 
    worldTrafo(), parentTrafo(), referenceTrafo(0) {
  printout(DEBUG,"DetElementObject","+++ Created new anonymous DetElementObject()");
  InstanceCount::increment(this);
}

/// Initializing constructor
DetElementObject::DetElementObject(const std::string& nam, int ident)
  : NamedObject(), ObjectExtensions(typeid(DetElementObject)), magic(magic_word()), flag(0), id(ident), combineHits(0), path(), placementPath(),
    idealPlace(), placement(), volumeID(0), parent(), reference(), children(),
    alignment(), volume_alignments(), conditions(), 
    worldTrafo(), parentTrafo(), referenceTrafo(0) {
  SetName(nam.c_str());
  printout(DEBUG,"DetElementObject","+++ Created new DetElementObject('%s', %d)",nam.c_str(),id);
  InstanceCount::increment(this);
}

/// Internal object destructor: release extension object(s)
DetElementObject::~DetElementObject() {
  for_each(children.begin(), children.end(), destroyHandles(children));
  deletePtr (referenceTrafo);
  destroyHandle(conditions);
  alignment.clear();
  placement.clear();
  idealPlace.clear();
  parent.clear();
  ObjectExtensions::clear();
  InstanceCount::decrement(this);
}

/// Deep object copy to replicate DetElement trees e.g. for reflection
DetElementObject* DetElementObject::clone(int new_id, int flag) const {
  DetElementObject* obj = new DetElementObject();
  obj->id = new_id;
  obj->flag = 0;
  obj->combineHits = combineHits;
  obj->alignment = Alignment();
  obj->conditions = Conditions();
  obj->parent = DetElement();
  if ( (flag & DetElement::COPY_PLACEMENT) == DetElement::COPY_PLACEMENT )  {
    obj->placement  = placement;
    obj->idealPlace = idealPlace;
    obj->placementPath = placementPath;
    obj->path = path;
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
      throw runtime_error("DD4hep: DetElement::copy: Element " + string(child.name()) + " is already present [Double-Insert]");
    }
  }
  return obj;
}

/// Create cached matrix to transform to world coordinates
const TGeoHMatrix& DetElementObject::worldTransformation() {
  if ( (flag&HAVE_WORLD_TRAFO) == 0 ) {
    PlacementPath nodes;
    flag |= HAVE_WORLD_TRAFO;
    DetectorTools::placementPath(this, nodes);
    DetectorTools::placementTrafo(nodes,true,worldTrafo);
  }
  return worldTrafo;
}

/// Create cached matrix to transform to parent coordinates
const TGeoHMatrix& DetElementObject::parentTransformation() {
  if ( (flag&HAVE_PARENT_TRAFO) == 0 ) {
    PlacementPath nodes;
    flag |= HAVE_PARENT_TRAFO;
    DetectorTools::placementPath(DetElement(parent), this, nodes);
    DetectorTools::placementTrafo(nodes,true,parentTrafo);
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
      DetectorTools::placementTrafo(nodes,true,referenceTrafo);
    }
    else if ( DetectorTools::isParentElement(self,ref) ) {
      PlacementPath nodes;
      DetectorTools::placementPath(self,ref,nodes);
      DetectorTools::placementTrafo(nodes,true,referenceTrafo);
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
  DetElement    world = DetectorTools::topElement(det);
  bool   print = (DEBUG > printLevel());
  string place = det.placementPath();
  bool have_world_tr = (flag&HAVE_WORLD_TRAFO);

  DetectorTools::placementPath(par, this, par_path);
  PlacedVolume node = DetectorTools::findNode(world.placement(),place);
  if ( !node.isValid() )  {
    throw runtime_error("DD4hep: DetElement: The placement " + place + " is not part of the hierarchy.");
  }
  //print = (idealPlace.ptr() != node.ptr());
  printout((idealPlace.ptr() != node.ptr()) ? INFO : DEBUG,
	   "DetElement","+++ Invalidate chache of %s -> %s Placement:%p --> %p %s", 
	   det.path().c_str(), DetectorTools::placementPath(par_path).c_str(),
	   placement.ptr(), node.ptr(), (placement.ptr() == node.ptr()) ? "" : "[UPDATE]");

  placement = node;

  if ( have_world_tr && print )  worldTrafo.Print();

  if ( (flag&HAVE_PARENT_TRAFO) )  {
    DetectorTools::placementTrafo(par_path,true,parentTrafo);
  }

  /// Compute world transformations
  if ( parent_world_trafo )  {
    // If possible use the pre-computed values from the parent
    worldTrafo = *parent_world_trafo;
    worldTrafo.Multiply(&parentTransformation());
    flag |= HAVE_WORLD_TRAFO;
  }
  else if ( have_world_tr )  {
    // Else re-compute the transformation to the world.
    PlacementPath world_nodes;
    DetectorTools::placementPath(this, world_nodes);
    DetectorTools::placementTrafo(world_nodes,true,worldTrafo);
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
void DetElementObject::removeAtUpdate(unsigned int type, void* pointer)   {
  for(UpdateCallbacks::iterator i=updateCalls.begin(); i != updateCalls.end(); ++i)  {
    if ( (type&((*i).second)) == type && (*i).first.par == pointer )  {
      updateCalls.erase(i);
      return;
    }
  }
}

/// Trigger update callbacks
void DetElementObject::update(unsigned int tags, void* param)   {
  DetElement det(this);
  const void* args[3] = { (void*)tags, this, param };
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
    if ( (tags&((*i).second)) == tags )  {
      (*i).first.execute(args);
    }
  }
}
