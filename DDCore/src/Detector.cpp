// $Id$
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
#include "DD4hep/objects/DetectorInterna.h"
#include "DD4hep/objects/ConditionsInterna.h"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/World.h"
#include "DD4hep/LCDD.h"

using namespace std;
using namespace DD4hep::Geometry;

/// Clone constructor
DetElement::DetElement(Object* det_data, const string& det_name, const string& det_type)
  : RefObject(det_data) {
  this->assign(det_data, det_name, det_type);
}

/// Constructor for a new subdetector element
DetElement::DetElement(const string& det_name, const string& det_type, int det_id) {
  assign(new Object(det_name,det_id), det_name, det_type);
  ptr()->id = det_id;
}

/// Constructor for a new subdetector element
DetElement::DetElement(const string& det_name, int det_id) {
  assign(new Object(det_name,det_id), det_name, "");
  ptr()->id = det_id;
}

/// Constructor for a new subdetector element
DetElement::DetElement(DetElement det_parent, const string& det_name, int det_id) {
  assign(new Object(det_name,det_id), det_name, det_parent.type());
  ptr()->id = det_id;
  det_parent.add(*this);
}

/// Add an extension object to the detector element
void* DetElement::i_addExtension(void* ext_ptr, const type_info& info, copy_t ctor, destruct_t dtor) const {
  return access()->addExtension(ext_ptr, info, ObjectExtensions::copy_t(ctor), dtor);
}

/// Access an existing extension object from the detector element
void* DetElement::i_extension(const type_info& info) const {
  return access()->extension(info);
}

/// Internal call to extend the detector element with an arbitrary structure accessible by the type
void DetElement::i_addUpdateCall(unsigned int callback_type, const Callback& callback)  const  {
  access()->updateCalls.push_back(make_pair(callback,callback_type));
}

/// Remove callback from object
void DetElement::removeAtUpdate(unsigned int typ, void* pointer)  const {
  access()->removeAtUpdate(typ,pointer);
}

/// Access to the full path to the placed object
string DetElement::placementPath() const {
  Object* o = ptr();
  if ( o ) {
    if (o->placementPath.empty()) {
      o->placementPath = DetectorTools::placementPath(*this);
    }
    return o->placementPath;
  }
  return "";
}

/// Access detector type (structure, tracker, calorimeter, etc.).
string DetElement::type() const {
  return m_element ? m_element->GetTitle() : "";
}

/// Set the type of the sensitive detector
DetElement& DetElement::setType(const string& typ) {
  Object* o = access();
  o->SetTitle(typ.c_str());
  return *this;
}


unsigned int DetElement::typeFlag() const {
  return m_element ? m_element->typeFlag :  0 ;
}

/// Set the type of the sensitive detector
DetElement& DetElement::setTypeFlag(unsigned int types) {
  Object* o = access();
  o->typeFlag = types ;
  return *this;
}

string DetElement::path() const {
  Object* o = ptr();
  if ( o ) {
    if ( o->path.empty() ) {
      DetElement par = o->parent;
      o->path = par.isValid() ? (par.path() + "/") + name() : string("/") + name();
    }
    return o->path;
  }
  return "";
}

int DetElement::id() const {
  return access()->id;
}

bool DetElement::combineHits() const {
  return access()->combineHits != 0;
}

DetElement& DetElement::setCombineHits(bool value, SensitiveDetector& sens) {
  access()->combineHits = value;
  if (sens.isValid())
    sens.setCombineHits(value);
  return *this;
}

/// Access to the alignment information
Alignment DetElement::alignment() const   {
  return access()->alignment;
}

/// Access to the survey alignment information
Alignment DetElement::surveyAlignment() const  {
  return access()->survey;
}

/// Access to the conditions information
DetElement::ConditionsContainer DetElement::conditions() const  {
  Object* o = access();
  if ( o->conditions.isValid() ) return o->conditions;
  o->conditions.assign(new ConditionsContainer::Object(),"conditions","");
  o->conditions->detector = *this;
  return o->conditions;
}

/// Access to the conditions information
DetElement::Condition
DetElement::condition(const std::string& key) const   {
  return access()->conditions[key];
}

/// Access to condition objects. If not present, load condition.
DetElement::Condition
DetElement::condition(const std::string& key, const IOV& iov)   {
  typedef DetElement::ConditionsContainer::Object::Entries _E;
  _E& ents = conditions()->entries;
  _E::iterator i = ents.find(hash32(key));
  if ( i != ents.end() ) return (*i).second;
  World world(Geometry::DetectorTools::topElement(*this));
  return world.getCondition(*this,key,iov);
}

const DetElement::Children& DetElement::children() const {
  return access()->children;
}

/// Access to individual children by name
DetElement DetElement::child(const string& child_name) const {
  if (isValid()) {
    const Children& c = ptr()->children;
    Children::const_iterator i = c.find(child_name);
    return i == c.end() ? DetElement() : (*i).second;
  }
  return DetElement();
}

/// Access to the detector elements's parent
DetElement DetElement::parent() const {
  Object* o = ptr();
  return (o) ? o->parent : 0;
}

void DetElement::check(bool cond, const string& msg) const {
  if (cond) {
    throw runtime_error("DD4hep: " + msg);
  }
}

DetElement& DetElement::add(DetElement sdet) {
  if (isValid()) {
    pair<Children::iterator, bool> r = object<Object>().children.insert(make_pair(sdet.name(), sdet));
    if (r.second) {
      sdet.access()->parent = *this;
      return *this;
    }
    throw runtime_error("DD4hep: DetElement::add: Element " + string(sdet.name()) + " is already present [Double-Insert]");
  }
  throw runtime_error("DD4hep: DetElement::add: Self is not defined [Invalid Handle]");
}

DetElement DetElement::clone(const string& new_name) const {
  Object* o = access();
  return DetElement(o->clone(o->id, COPY_NONE), new_name, o->GetTitle());
}

DetElement DetElement::clone(const string& new_name, int new_id) const {
  Object* o = access();
  return DetElement(o->clone(new_id, COPY_NONE), new_name, o->GetTitle());
}

/// Access to the physical volume of this detector element
PlacedVolume DetElement::placement() const {
  if (isValid()) {
    Object& o = object<Object>();
    if (o.placement.isValid()) {
      return o.placement;
    }
  }
  return PlacedVolume();
}

/// Set the physical volumes of the detector element
DetElement& DetElement::setPlacement(const PlacedVolume& pv) {
  if (pv.isValid()) {
    Object* o = access();
    o->placement = pv;
    if ( !o->idealPlace.isValid() )  {
      o->idealPlace = pv;
    }
    return *this;
  }
  throw runtime_error("DD4hep: DetElement::setPlacement: Placement is not defined [Invalid Handle]");
}

/// The cached VolumeID of this subdetector element
DD4hep::VolumeID DetElement::volumeID() const   {
  if (isValid()) {
    return object<Object>().volumeID;
  }
  return 0;
}

/// Access to the logical volume of the placements (all daughters have the same!)
Volume DetElement::volume() const {
  return access()->placement.volume();
}

DetElement& DetElement::setVisAttributes(const LCDD& lcdd, const string& nam, const Volume& vol) {
  vol.setVisAttributes(lcdd, nam);
  return *this;
}

DetElement& DetElement::setRegion(const LCDD& lcdd, const string& nam, const Volume& vol) {
  if (!nam.empty()) {
    vol.setRegion(lcdd.region(nam));
  }
  return *this;
}

DetElement& DetElement::setLimitSet(const LCDD& lcdd, const string& nam, const Volume& vol) {
  if (!nam.empty()) {
    vol.setLimitSet(lcdd.limitSet(nam));
  }
  return *this;
}

DetElement& DetElement::setAttributes(const LCDD& lcdd, const Volume& vol, const string& region, const string& limits,
                                      const string& vis) {
  return setRegion(lcdd, region, vol).setLimitSet(lcdd, limits, vol).setVisAttributes(lcdd, vis, vol);
}

/// Set detector element for reference transformations. Will delete existing reference trafo.
DetElement& DetElement::setReference(DetElement reference) {
  Object& o = object<Object>();
  if (o.referenceTrafo) {
    delete o.referenceTrafo;
    o.referenceTrafo = 0;
  }
  object<Object>().reference = reference;
  return *this;
}

/// Create cached matrix to transform to world coordinates
const TGeoHMatrix& DetElement::worldTransformation() const   {
  return access()->worldTransformation();
}

/// Create cached matrix to transform to parent coordinates
const TGeoHMatrix& DetElement::parentTransformation() const   {
  return access()->parentTransformation();
}

/// Create cached matrix to transform to reference coordinates
const TGeoHMatrix& DetElement::referenceTransformation() const   {
  return access()->referenceTransformation();
}

/// Transformation from local coordinates of the placed volume to the world system
bool DetElement::localToWorld(const Position& local, Position& global) const {
  Double_t master_point[3] = { 0, 0, 0 }, local_point[3] = { local.X(), local.Y(), local.Z() };
  // If the path is unknown an exception will be thrown inside worldTransformation() !
  worldTransformation().LocalToMaster(local_point, master_point);
  global.SetCoordinates(master_point);
  return true;
}

/// Transformation from local coordinates of the placed volume to the parent system
bool DetElement::localToParent(const Position& local, Position& global) const {
  // If the path is unknown an exception will be thrown inside parentTransformation() !
  Double_t master_point[3] = { 0, 0, 0 }, local_point[3] = { local.X(), local.Y(), local.Z() };
  parentTransformation().LocalToMaster(local_point, master_point);
  global.SetCoordinates(master_point);
  return true;
}

/// Transformation from local coordinates of the placed volume to arbitrary parent system set as reference
bool DetElement::localToReference(const Position& local, Position& global) const {
  // If the path is unknown an exception will be thrown inside referenceTransformation() !
  Double_t master_point[3] = { 0, 0, 0 }, local_point[3] = { local.X(), local.Y(), local.Z() };
  referenceTransformation().LocalToMaster(local_point, master_point);
  global.SetCoordinates(master_point);
  return true;
}

/// Transformation from world coordinates of the local placed volume coordinates
bool DetElement::worldToLocal(const Position& global, Position& local) const {
  // If the path is unknown an exception will be thrown inside worldTransformation() !
  Double_t master_point[3] = { global.X(), global.Y(), global.Z() }, local_point[3] = { 0, 0, 0 };
  worldTransformation().MasterToLocal(master_point, local_point);
  local.SetCoordinates(local_point);
  return true;
}

/// Transformation from parent coordinates of the local placed volume coordinates
bool DetElement::parentToLocal(const Position& global, Position& local) const {
  // If the path is unknown an exception will be thrown inside parentTransformation() !
  Double_t master_point[3] = { global.X(), global.Y(), global.Z() }, local_point[3] = { 0, 0, 0 };
  parentTransformation().MasterToLocal(master_point, local_point);
  local.SetCoordinates(local_point);
  return true;
}

/// Transformation from arbitrary parent system coordinates of the local placed volume coordinates
bool DetElement::referenceToLocal(const Position& global, Position& local) const {
  // If the path is unknown an exception will be thrown inside referenceTransformation() !
  Double_t master_point[3] = { global.X(), global.Y(), global.Z() }, local_point[3] = { 0, 0, 0 };
  referenceTransformation().MasterToLocal(master_point, local_point);
  local.SetCoordinates(local_point);
  return true;
}

/// Constructor
SensitiveDetector::SensitiveDetector(const string& nam, const string& typ) {
  /*
    <calorimeter ecut="0" eunit="MeV" hits_collection="EcalEndcapHits" name="EcalEndcap" verbose="0">
    <global_grid_xy grid_size_x="3.5" grid_size_y="3.5"/>
    <idspecref ref="EcalEndcapHits"/>
    </calorimeter>
  */
  assign(new Object(nam), nam, typ);
  object<Object>().ecut = 0e0;
  object<Object>().verbose = 0;
}

/// Set the type of the sensitive detector
SensitiveDetector& SensitiveDetector::setType(const string& typ) {
  access()->SetTitle(typ.c_str());
  return *this;
}

/// Access the type of the sensitive detector
string SensitiveDetector::type() const {
  return m_element ? m_element->GetTitle() : "";
}

/// Assign the IDDescriptor reference
SensitiveDetector& SensitiveDetector::setReadout(Readout ro) {
  access()->readout = ro;
  return *this;
}

/// Assign the IDDescriptor reference
Readout SensitiveDetector::readout() const {
  return access()->readout;
}

/// Set energy cut off
SensitiveDetector& SensitiveDetector::setEnergyCutoff(double value) {
  access()->ecut = value;
  return *this;
}

/// Access energy cut off
double SensitiveDetector::energyCutoff() const {
  return access()->ecut;
}

/// Assign the name of the hits collection
SensitiveDetector& SensitiveDetector::setHitsCollection(const string& collection) {
  access()->hitsCollection = collection;
  return *this;
}

/// Access the hits collection name
const string& SensitiveDetector::hitsCollection() const {
  return access()->hitsCollection;
}

/// Assign the name of the hits collection
SensitiveDetector& SensitiveDetector::setVerbose(bool value) {
  int v = value ? 1 : 0;
  access()->verbose = v;
  return *this;
}

/// Access flag to combine hist
bool SensitiveDetector::verbose() const {
  return access()->verbose == 1;
}

/// Assign the name of the hits collection
SensitiveDetector& SensitiveDetector::setCombineHits(bool value) {
  int v = value ? 1 : 0;
  access()->combineHits = v;
  return *this;
}

/// Access flag to combine hist
bool SensitiveDetector::combineHits() const {
  return access()->combineHits == 1;
}

/// Set the regional attributes to the sensitive detector
SensitiveDetector& SensitiveDetector::setRegion(Region reg) {
  access()->region = reg;
  return *this;
}

/// Access to the region setting of the sensitive detector (not mandatory)
Region SensitiveDetector::region() const {
  return access()->region;
}

/// Set the limits to the sensitive detector
SensitiveDetector& SensitiveDetector::setLimitSet(LimitSet ls) {
  access()->limits = ls;
  return *this;
}

/// Access to the limit set of the sensitive detector (not mandatory).
LimitSet SensitiveDetector::limits() const {
  return access()->limits;
}

/// Add an extension object to the detector element
void* SensitiveDetector::i_addExtension(void* ext_ptr, const type_info& info, destruct_t dtor)  {
  return access()->addExtension(ext_ptr, info, dtor);
}

/// Access an existing extension object from the detector element
void* SensitiveDetector::i_extension(const type_info& info) const {
  return access()->extension(info);
}
