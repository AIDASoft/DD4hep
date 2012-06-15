// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DD4hep/IDDescriptor.h"
#include "DD4hep/LCDD.h"
#include "TGeoVolume.h"
#include "TGeoMatrix.h"
#include "TGeoManager.h"

using namespace std;
using namespace DD4hep::Geometry;

static bool traverse_find(TGeoNode* parent, TGeoNode* child, vector<TGeoMatrix*>& trafos) {
  TIter next(parent->GetVolume()->GetNodes());
  for (TGeoNode *daughter=(TGeoNode*)next(); daughter; daughter=(TGeoNode*)next() ) {
    if ( daughter == child )   {
      trafos.push_back(daughter->GetMatrix());
      return true;
    }
  }
  next.Reset();
  for (TGeoNode *daughter=(TGeoNode*)next(); daughter; daughter=(TGeoNode*)next() ) {
    if ( traverse_find(daughter, child, trafos) ) {
      trafos.push_back(daughter->GetMatrix());
      return true;
    }
  }
  return false;
}

static bool traverse_up(const DetElement& parent, const DetElement& child, vector<TGeoMatrix*>& trafos) {
  if ( child.ptr() != parent.ptr() ) {
    for( DetElement par=parent, cld=child; par.isValid(); cld=par, par=par._data().parent ) {
      PlacedVolume cld_place = cld._data().placement;
      PlacedVolume par_place = par._data().placement;
      if ( !traverse_find(par_place.ptr(),cld_place.ptr(),trafos) ) {
	return false;
      }
    }
  }
  return true;
}
#include <iostream>
static string find_path(TGeoNode* top, TGeoNode* child) {
  if ( top == child ) {
    return child->GetName();
  }
  TIter next(top->GetVolume()->GetNodes());
  for (TGeoNode *daughter=(TGeoNode*)next(); daughter; daughter=(TGeoNode*)next() ) {
    if ( daughter == child )   {
      return child->GetName();
    }
  }
  next.Reset();
  for (TGeoNode *daughter=(TGeoNode*)next(); daughter; daughter=(TGeoNode*)next() ) {
    string res = find_path(daughter, child);
    if ( !res.empty() ) {
      return top->GetName() + ("/"+res);
    }
  }
  return "";
}

static string find_child(TGeoNode* top, TGeoNode* child, vector<TGeoNode*>& path) {
  if ( top == child ) {
    path.push_back(child);
    return child->GetName();
  }
  TIter next(top->GetVolume()->GetNodes());
  for (TGeoNode *daughter=(TGeoNode*)next(); daughter; daughter=(TGeoNode*)next() ) {
    if ( daughter == child )   {
      path.push_back(daughter);
      return child->GetName();
    }
  }
  next.Reset();
  for (TGeoNode *daughter=(TGeoNode*)next(); daughter; daughter=(TGeoNode*)next() ) {
    string res = find_child(daughter, child, path);
    if ( !res.empty() ) {
      path.push_back(daughter);
      return top->GetName() + ("/"+res);
    }
  }
  //cout << "FAILED child:" << (unsigned long)child << endl;
  return "";
}

/// Create cached matrix to transform to positions to an upper level DetElement
static TGeoHMatrix* create_trafo(const Ref_t& parent, const Ref_t& child)   {
  if ( parent.isValid() && child.isValid() )   {
    TGeoHMatrix* mat = 0;    // Now collect all transformations
    vector<TGeoMatrix*> trafos;
    if ( !traverse_up(parent,child,trafos) )   {
      trafos.clear();
      if ( !traverse_up(child,parent,trafos) )   {  // Some error, non-existing path!
	throw runtime_error("DetElement "+string(parent.name())+" is not connected to child "+string(child.name())+" [Geo-Error]");
      }
      else {
	/// The two detector elements were only found by reversing the hierarchy.
	/// Hence the transformations must be applied in reverse order.
	for(vector<TGeoMatrix*>::const_reverse_iterator i=trafos.rbegin(); i!=trafos.rend(); ++i) {
	  if ( !mat ) mat = new TGeoHMatrix(*(*i));
	  else mat->MultiplyLeft(*i);
	}
      }
    }
    else {
      /// Combine the transformations to a single transformation.
      for(vector<TGeoMatrix*>::const_iterator i=trafos.begin(); i!=trafos.end(); ++i) {
	if ( !mat ) mat = new TGeoHMatrix(*(*i));
	else mat->MultiplyLeft(*i);
      }
    }
#if 0
    // Test: find arbitrary child by traversing TGeoNode tree
    vector<TGeoNode*> path;
    TGeoNode* top  = gGeoManager->GetTopNode();
    TGeoNode* node = DetElement(child).placement().ptr();
    string res = find_child(top,node,path);
    cout << "Path:" << res << endl;
#endif
    return mat;
  }
  if ( parent.isValid() )
    throw runtime_error("DetElement cannot connect "+string(parent.name())+" to not-existing child!");
  else if ( child.isValid() )
    throw runtime_error("DetElement cannot connect "+string(child.name())+" to not-existing parent!");
  else
    throw runtime_error("DetElement cannot connect nonexisting parent to not-existing child!");
}

/// Top detector element
static DetElement _top(DetElement o)   {
  DetElement par = o, pp = o;
  while(par.isValid()) {
    if ( par.placement().isValid() ) pp = par;
    par = par.parent();
  }
  return pp;
}

/// Default constructor
DetElement::Object::Object()  
  : magic(magic_word()), id(0), combine_hits(0), readout(), 
    alignment(), placement(), placements(), parent(), children(),
    worldTrafo(0), parentTrafo(0), referenceTrafo(0)
{
}

/// Construct new empty object
Value<TNamed,DetElement::Object>* DetElement::Object::construct(int new_id, int flag) const {
  Value<TNamed,Object>* obj = new Value<TNamed,Object>();
  obj->deepCopy(*this,new_id,flag);
  return obj;
}

/// Deep object copy to replicate DetElement trees e.g. for reflection
void DetElement::Object::deepCopy(const Object& source, int new_id, int flag)  {
  DetElement self(Ref_t(dynamic_cast<Value<TNamed,Object>*>(this)));
  id           = new_id;
  combine_hits = source.combine_hits;
  readout      = source.readout;
  volume       = source.volume;
  alignment    = Alignment();
  conditions   = Conditions();
  parent       = DetElement();
  placement    = ((flag&COPY_PLACEMENT) == COPY_PLACEMENT) ? source.placement : PlacedVolume();
  placements   = ((flag&COPY_PLACEMENT) == COPY_PLACEMENT) ? source.placements : Placements();
  children.clear();
  for(DetElement::Children::const_iterator i=source.children.begin(); i != source.children.end(); ++i) {
    const DetElement::Object& d = (*i).second._data();
    const TNamed* pc = (*i).second.ptr();
    DetElement child(d.construct(d.id,COPY_PLACEMENT),pc->GetName(),pc->GetTitle());
    pair<Children::iterator,bool> r = children.insert(make_pair(child.name(),child));
    if ( r.second )   {
      child._data().parent = self;
    }
    else {
      throw runtime_error("DetElement::copy: Element "+string(child.name())+" is already present [Double-Insert]");
    }
  }
}

/// Conversion to reference object
Ref_t DetElement::Object::asRef() {
  return Ref_t(dynamic_cast<Value<TNamed,DetElement::Object>*>(this));
}

/// Conversion to reference object
DetElement::Object::operator Ref_t() {
  return this->asRef();
}

/// Create cached matrix to transform to world coordinates
TGeoMatrix* DetElement::Object::worldTransformation() {
  if ( !worldTrafo ) {
    DetElement top_det = _top(DetElement(asRef()));
    vector<TGeoMatrix*> trafos;
    TGeoHMatrix* mat = create_trafo(top_det,asRef());
    // Now we got the point in the top-most detector element. We now have
    // to translate this to the "world volume", which has no transformation matrix anymore
    TGeoNode* top_node = gGeoManager->GetTopNode();
    PlacedVolume place = top_det.placement();
    if ( !traverse_find(top_node, place.ptr(), trafos) ) {
      // Some error, non-existing path!
      throw runtime_error("DetElement "+string(parent.name())+" is not connected to top geo node [Geo-Error]");
    }    
    for(vector<TGeoMatrix*>::const_iterator i=trafos.begin(); i!=trafos.end(); ++i)
      mat->MultiplyLeft(*i);
    worldTrafo = mat;
  }    
  return worldTrafo;
}

/// Create cached matrix to transform to parent coordinates
TGeoMatrix* DetElement::Object::parentTransformation() {
  if ( !parentTrafo )   {
    parentTrafo = create_trafo(this->parent,asRef());
  }
  return parentTrafo;
}

/// Create cached matrix to transform to reference coordinates
TGeoMatrix* DetElement::Object::referenceTransformation() {
  if ( !referenceTrafo )   {
    referenceTrafo = create_trafo(this->reference,asRef());
  }
  return referenceTrafo;
}

/// Constructor for a new subdetector element
DetElement::DetElement(const string& name, const string& type, int id)   {
  assign(new Value<TNamed,Object>(),name,type);
  _data().id = id;
}

/// Constructor for a new subdetector element
DetElement::DetElement(const string& name, int id)   {
  assign(new Value<TNamed,Object>(),name,"");
  _data().id = id;
}

/// Constructor for a new subdetector element
DetElement::DetElement(DetElement parent, const string& name, int id)   {
  assign(new Value<TNamed,Object>(),name,"");
  _data().id = id;
  parent.add(*this);
}

/// Access to the full path to the placed object
std::string DetElement::placementPath() const {
  if ( isValid() ) {
    Object& o = _data();
    if ( o.placementPath.empty() ) {
      DetElement top =_top(*this);
      o.placementPath = find_path(top.placement().ptr(),placement().ptr());
    }
    return o.placementPath;
  }
  return "";
}

string DetElement::type() const   {
  return m_element ? m_element->GetTitle() : "";
}

string DetElement::path() const   {
  if ( m_element )  {
    Object& o = _data();
    if ( o.path.empty() )   {
      DetElement par = o.parent;
      o.path = par.isValid() ? (par.path()+"/")+name() : string("/") + name();
    }
    return o.path;
  }
  return "";
}

int DetElement::id() const   {
  return _data().id;
}

bool DetElement::combineHits() const   {
  return _data().combine_hits != 0;
}

Readout DetElement::readout() const   {
  return _data().readout;
}

DetElement& DetElement::setReadout(const Readout& readout)   {
  _data().readout = readout;
  return *this;
}

const DetElement::Children& DetElement::children() const   {
  return _data().children;
}

/// Access to individual children by name
DetElement DetElement::child(const std::string& name) const {
  if ( isValid() )  {
    const Children& c = _data().children;
    Children::const_iterator i = c.find(name);
    return i == c.end() ? DetElement() : (*i).second;
  }
  return DetElement();
}

/// Access to the detector elements's parent
DetElement DetElement::parent() const {
  if ( isValid() )  {
    return _data().parent;
  }
  return DetElement();
}

void DetElement::check(bool condition, const string& msg) const  {
  if ( condition )  {
    throw runtime_error(msg);
  }
}

DetElement& DetElement::add(DetElement sdet)  {
  if ( isValid() )  {
    pair<Children::iterator,bool> r = _data().children.insert(make_pair(sdet.name(),sdet));
    if ( r.second )   {
      sdet._data().parent = *this;
      return *this;
    }
    throw runtime_error("DetElement::add: Element "+string(sdet.name())+" is already present [Double-Insert]");
  }
  throw runtime_error("DetElement::add: Self is not defined [Invalid Handle]");
}

DetElement DetElement::clone(const string& new_name)  const  {
  if ( isValid() ) {
    return DetElement(_data().construct(_data().id,COPY_NONE), new_name, ptr()->GetTitle());
  }
  throw runtime_error("DetElement::clone: Self is not defined - clone failed! [Invalid Handle]");
}

DetElement DetElement::clone(const string& new_name, int new_id)  const  {
  if ( isValid() ) {
    return DetElement(_data().construct(new_id, COPY_NONE), new_name, ptr()->GetTitle());
  }
  throw runtime_error("DetElement::clone: Self is not defined - clone failed! [Invalid Handle]");
}

/// Access to the physical volume of this detector element
PlacedVolume DetElement::placement() const {
  if ( isValid() ) {
    Object& o = _data();
    if ( o.placement.isValid() )  {
      return o.placement;
    }
  }
  return PlacedVolume();
}

/// Set the physical volumes of the detector element
DetElement& DetElement::setPlacement(const PlacedVolume& placement) {
  if ( isValid() ) {
    if ( placement.isValid() )  {
      Object& o   = _data();
      o.placement = placement;
      o.volume    = placement.volume();
      return *this;
    }
    throw runtime_error("DetElement::setPlacement: Placement is not defined [Invalid Handle]");
  }
  throw runtime_error("DetElement::setPlacement: Self is not defined [Invalid Handle]");
}

/// Access to the logical volume of the placements (all daughters have the same!)
Volume DetElement::volume() const   {
  if ( isValid() )  {
    return _data().volume;
  }
  throw runtime_error("DetElement::volume: Self is not defined [Invalid Handle]");
}
#if 0
/// Set the logical volume of the placements (all daughters have the same!)
void DetElement::setVolume(Volume vol) {
  if ( isValid() )  {
    _data().volume = vol;
  }
  throw runtime_error("DetElement::setVolume: Self is not defined [Invalid Handle]");
}
#endif
DetElement& DetElement::setVisAttributes(const LCDD& lcdd, const string& name, const Volume& volume)  {
  if ( isValid() )  {
    volume.setVisAttributes(lcdd,name);
    return *this;
  }
  throw runtime_error("DetElement::setVisAttributes: Self is not defined [Invalid Handle]");
}

DetElement& DetElement::setRegion(const LCDD& lcdd, const string& name, const Volume& volume)  {
  if ( isValid() )  {
    if ( !name.empty() )  {
      volume.setRegion(lcdd.region(name));
    }
    return *this;
  }
  throw runtime_error("DetElement::setRegion: Self is not defined [Invalid Handle]");
}

DetElement& DetElement::setLimitSet(const LCDD& lcdd, const string& name, const Volume& volume)  {
  if ( isValid() )  {
    if ( !name.empty() )  {
      volume.setLimitSet(lcdd.limitSet(name));
    }
    return *this;
  }
  throw runtime_error("DetElement::setLimitSet: Self is not defined [Invalid Handle]");
}

DetElement& DetElement::setAttributes(const LCDD& lcdd, const Volume& volume,
                                        const std::string& region, 
                                        const std::string& limits, 
                                        const std::string& vis)
{
  return setRegion(lcdd,region,volume).setLimitSet(lcdd,limits,volume).setVisAttributes(lcdd,vis,volume);
}

DetElement& DetElement::setCombineHits(bool value, SensitiveDetector& sens)   {
  if ( isTracker() )  {
    _data().combine_hits = value;
    sens.setCombineHits(value);
  }
  return *this;
}

bool DetElement::isTracker() const   {
  if ( isValid() )  {
    string typ = type();
    if ( typ.find("Tracker") != string::npos && _data().readout.isValid() )   {
      return true;
    }
  }
  return false;
}

bool DetElement::isCalorimeter() const   {
  if ( isValid() )  {
    string typ = type();
    if ( typ.find("Calorimeter") != string::npos && _data().readout.isValid() ) {
      return true;
    }
  }
  return false;
}

/// Set detector element for reference transformations. Will delete existing reference trafo.
DetElement& DetElement::setReference(DetElement reference) {
  Object& o = _data();
  if ( o.referenceTrafo )  {
    delete o.referenceTrafo;
    o.referenceTrafo = 0;
  }
  _data().reference = reference;
  return *this;
}

/// Transformation from local coordinates of the placed volume to the world system
bool DetElement::localToWorld(const Position& local, Position& global)  const {
  Double_t master_point[3]={0,0,0}, local_point[3] = {local.x,local.y,local.z};
  // If the path is unknown an exception will be thrown inside worldTransformation() !
  _data().worldTransformation()->LocalToMaster(local_point,master_point);
  global.set(master_point[0],master_point[1],master_point[2]);
  return true;
}

/// Transformation from local coordinates of the placed volume to the parent system
bool DetElement::localToParent(const Position& local, Position& global)  const {
  // If the path is unknown an exception will be thrown inside parentTransformation() !
  _data().parentTransformation()->LocalToMaster(&local.x,&global.x);
  return true;
}

/// Transformation from local coordinates of the placed volume to arbitrary parent system set as reference
bool DetElement::localToReference(const Position& local, Position& global)  const {
  // If the path is unknown an exception will be thrown inside referenceTransformation() !
  _data().referenceTransformation()->LocalToMaster(&local.x,&global.x);
  return true;
}

/// Transformation from world coordinates of the local placed volume coordinates
bool DetElement::worldToLocal(const Position& global, Position& local)  const {
  // If the path is unknown an exception will be thrown inside worldTransformation() !
  _data().worldTransformation()->MasterToLocal(&global.x,&local.x);
  return true;
}

/// Transformation from parent coordinates of the local placed volume coordinates
bool DetElement::parentToLocal(const Position& global, Position& local)  const {
  // If the path is unknown an exception will be thrown inside parentTransformation() !
  _data().parentTransformation()->MasterToLocal(&global.x,&local.x);
  return true;
}

/// Transformation from arbitrary parent system coordinates of the local placed volume coordinates
bool DetElement::referenceToLocal(const Position& global, Position& local)  const {
  // If the path is unknown an exception will be thrown inside referenceTransformation() !
  _data().referenceTransformation()->MasterToLocal(&global.x,&local.x);
  return true;
}

/// Constructor
SensitiveDetector::SensitiveDetector(const LCDD& /* lcdd */, const std::string& type, const std::string& name) 
{
  /*
    <calorimeter ecut="0" eunit="MeV" hits_collection="EcalEndcapHits" name="EcalEndcap" verbose="0">
      <global_grid_xy grid_size_x="3.5" grid_size_y="3.5"/>
      <idspecref ref="EcalEndcapHits"/>
    </calorimeter>
  */
  assign(new Value<TNamed,Object>(),name,type);
  _data().ecut = 0e0;
  _data().eunit = "MeV";
  _data().verbose = 0;
}

/// Access the type of the sensitive detector
string SensitiveDetector::type() const  {
  return m_element ? m_element->GetTitle() : "";
}

/// Assign the IDDescriptor reference
SensitiveDetector& SensitiveDetector::setIDSpec(const Ref_t& spec)  {
  _data().id = spec;
  return *this;
}

/// Assign the name of the hits collection
SensitiveDetector& SensitiveDetector::setHitsCollection(const string& collection)  {
  _data().hits_collection = collection;
  return *this;
}

/// Assign the name of the hits collection
SensitiveDetector& SensitiveDetector::setCombineHits(bool value)  {
  int v = value ? 1 : 0;
  _data().combine_hits = v;
  return *this;
}

/// Assign the readout segmentation reference
SensitiveDetector& SensitiveDetector::setSegmentation(const Segmentation& segmentation)   {
  if ( segmentation.isValid() )  {
    _data().segmentation = segmentation;
    return *this;
  }
  throw runtime_error("Readout::setSegmentation: Cannot assign segmentation [Invalid Handle]");
}
