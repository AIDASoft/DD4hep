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
#include <iostream>

using namespace std;
using namespace DD4hep::Geometry;

static bool find_child(TGeoNode* parent, TGeoNode* child, vector<TGeoNode*>& path) {
  if ( parent && child ) {
    if ( parent == child ) {
      path.push_back(child);
      return true;
    }
    TIter next(parent->GetVolume()->GetNodes());
    for (TGeoNode *daughter=(TGeoNode*)next(); daughter; daughter=(TGeoNode*)next() ) {
      if ( daughter == child )   {
	path.push_back(daughter);
	return true;
      }
    }
    next.Reset();
    for (TGeoNode *daughter=(TGeoNode*)next(); daughter; daughter=(TGeoNode*)next() ) {
      bool res = find_child(daughter, child, path);
      if ( res ) {
	path.push_back(daughter);
	return res;
      }
    }
  }
  return false;
}

static bool collect_detector_nodes(const vector<TGeoNode*>& det_nodes, vector<TGeoNode*>& nodes) {
  if ( det_nodes.size() < 1 ) {
    return false;
  }
  if ( det_nodes.size() < 2 ) {
    return true;
  }
  for(size_t i=0, n=det_nodes.size(); i<n-1; ++i) {
    if ( !find_child(det_nodes[i+1],det_nodes[i],nodes) )  {
      return false;
    }
  }
  return true;
}

/// Create cached matrix to transform to positions to an upper level DetElement
static TGeoHMatrix* create_trafo(const vector<TGeoNode*>& det_nodes)   {
  if ( det_nodes.size() < 2 ) {
    return new TGeoHMatrix(*gGeoIdentity);
  }
  vector<TGeoNode*> nodes;
  if ( !collect_detector_nodes(det_nodes,nodes) ) {
    throw runtime_error("DetElement cannot connect "+string(det_nodes[0]->GetName())+
			" to child "+string(det_nodes[1]->GetName()));
  }
  TGeoHMatrix* mat = new TGeoHMatrix(*gGeoIdentity);
  for(vector<TGeoNode*>::const_iterator i=nodes.begin(); i!=nodes.end(); ++i) {
    TGeoMatrix* m = (*i)->GetMatrix();
    mat->MultiplyLeft(m);
  }
  return mat;
}

/// Top detector element
static DetElement _top(DetElement o, vector<TGeoNode*>& det_nodes)   {
  DetElement par = o, pp = o;
  while( par.isValid() ) {
    if ( par.placement().isValid() )   {
      det_nodes.push_back(par.placement().ptr());
      pp = par;
    }
    par = par.parent();
  }
  return pp;
}

/// Top detector element
static DetElement _par(DetElement o, DetElement top, vector<TGeoNode*>& det_nodes)   {
  DetElement par = o, pp = o;
  while( par.isValid() ) {
    if ( par.placement().isValid() )   {
      det_nodes.push_back(par.placement().ptr());
      pp = par;
    }
    if ( par.ptr() == top.ptr() ) break;
    par = par.parent();
  }
  if ( pp.ptr() == top.ptr() ) return pp;
  return DetElement();
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
    vector<TGeoNode*> det_nodes;
    _top(DetElement(asRef()),det_nodes);
    TGeoHMatrix* mat = create_trafo(det_nodes);
    worldTrafo = mat;
  }    
  return worldTrafo;
}

/// Create cached matrix to transform to parent coordinates
TGeoMatrix* DetElement::Object::parentTransformation() {
  if ( !parentTrafo )   {
    vector<TGeoNode*> det_nodes;
    det_nodes.push_back(placement.ptr());
    det_nodes.push_back(DetElement(parent).placement().ptr());
    parentTrafo = create_trafo(det_nodes);
  }
  return parentTrafo;
}

/// Create cached matrix to transform to reference coordinates
TGeoMatrix* DetElement::Object::referenceTransformation() {
  if ( !referenceTrafo )   {
    vector<TGeoNode*> nodes;
    DetElement ref(reference);
    DetElement self(asRef());
    DetElement elt = _par(self,ref,nodes);
    if ( elt.isValid() )   {
      referenceTrafo = create_trafo(nodes);
    }
    else   {
      nodes.clear();
      DetElement elt = _par(ref,self,nodes);
      if ( !elt.isValid() )   {
	throw runtime_error("referenceTransformation: No path from "+string(self.name())+
			    " to reference element "+string(ref.name())+" present!");
      }
      TGeoMatrix* m = create_trafo(nodes);
      referenceTrafo = new TGeoHMatrix(m->Inverse());
      delete m;
    }
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
      string res = "";
      vector<TGeoNode*> nodes, path;
      _top(*this,nodes);
      if ( !collect_detector_nodes(nodes,path) ) {
	throw runtime_error("DetElement cannot determine placement path of "+string(name()));
      }
      path.push_back(gGeoManager->GetTopNode());
      for(vector<TGeoNode*>::const_reverse_iterator i=path.rbegin(); i!=path.rend(); ++i)
	res += (string("/")+(*i)->GetName());
      o.placementPath = res;
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
