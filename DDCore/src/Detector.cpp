// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/objects/DetectorInterna.h"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/LCDD.h"

using namespace std;
using namespace DD4hep::Geometry;

/// Clone constructor
DetElement::DetElement(Object* data, const string& name, const string& type)
  : RefObject(data) {
  this->assign(data, name, type);
}

/// Constructor for a new subdetector element
DetElement::DetElement(const string& name, const string& type, int id) {
  assign(new Object(), name, type);
  ptr()->id = id;
}

/// Constructor for a new subdetector element
DetElement::DetElement(const string& name, int id) {
  assign(new Object(), name, "");
  ptr()->id = id;
}

/// Constructor for a new subdetector element
DetElement::DetElement(DetElement parent, const string& name, int id) {
  assign(new Object(), name, parent.type());
  ptr()->id = id;
  parent.add(*this);
}

/// Add an extension object to the detector element
void* DetElement::i_addExtension(void* ptr, const type_info& info, copy_t ctor, destruct_t dtor) const {
  return access()->addExtension(ptr, info, ObjectExtensions::copy_t(ctor), dtor);
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
void DetElement::removeAtUpdate(unsigned int type, void* pointer)  const {
  access()->removeAtUpdate(type,pointer);
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

/// Access to the conditions information 
Conditions DetElement::conditions() const  {
  return access()->conditions;
}

const DetElement::Children& DetElement::children() const {
  return access()->children;
}

/// Access to individual children by name
DetElement DetElement::child(const string& name) const {
  if (isValid()) {
    const Children& c = ptr()->children;
    Children::const_iterator i = c.find(name);
    return i == c.end() ? DetElement() : (*i).second;
  }
  return DetElement();
}

/// Access to the detector elements's parent
DetElement DetElement::parent() const {
  Object* o = ptr();
  return (o) ? o->parent : 0;
}

void DetElement::check(bool condition, const string& msg) const {
  if (condition) {
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
DetElement& DetElement::setPlacement(const PlacedVolume& placement) {
  if (placement.isValid()) {
    Object* o = access();
    o->placement = placement;
    if ( !o->idealPlace.isValid() )  {
      o->idealPlace = placement;
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

DetElement& DetElement::setVisAttributes(const LCDD& lcdd, const string& name, const Volume& volume) {
  volume.setVisAttributes(lcdd, name);
  return *this;
}

DetElement& DetElement::setRegion(const LCDD& lcdd, const string& name, const Volume& volume) {
  if (!name.empty()) {
    volume.setRegion(lcdd.region(name));
  }
  return *this;
}

DetElement& DetElement::setLimitSet(const LCDD& lcdd, const string& name, const Volume& volume) {
  if (!name.empty()) {
    volume.setLimitSet(lcdd.limitSet(name));
  }
  return *this;
}

DetElement& DetElement::setAttributes(const LCDD& lcdd, const Volume& volume, const string& region, const string& limits,
    const string& vis) {
  return setRegion(lcdd, region, volume).setLimitSet(lcdd, limits, volume).setVisAttributes(lcdd, vis, volume);
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
SensitiveDetector::SensitiveDetector(const string& name, const string& type) {
  /*
   <calorimeter ecut="0" eunit="MeV" hits_collection="EcalEndcapHits" name="EcalEndcap" verbose="0">
   <global_grid_xy grid_size_x="3.5" grid_size_y="3.5"/>
   <idspecref ref="EcalEndcapHits"/>
   </calorimeter>
   */
  assign(new Object(), name, type);
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
SensitiveDetector& SensitiveDetector::setLimitSet(LimitSet limits) {
  access()->limits = limits;
  return *this;
}

/// Access to the limit set of the sensitive detector (not mandatory).
LimitSet SensitiveDetector::limits() const {
  return access()->limits;
}

/// Add an extension object to the detector element
void* SensitiveDetector::i_addExtension(void* ptr, const type_info& info, destruct_t dtor)  {
  return access()->addExtension(ptr, info, dtor);
}

/// Access an existing extension object from the detector element
void* SensitiveDetector::i_extension(const type_info& info) const {
  return access()->extension(info);
}
