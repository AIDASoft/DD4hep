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

namespace {
  struct ExtensionEntry {
    void* (*copy)(const void*,DetElement);
    void  (*destruct)(void*);
    int     id;
  };
  typedef map<const type_info*, ExtensionEntry> ExtensionMap;
  static int s_extensionID = 0;
  ExtensionMap& detelement_extensions() {
    static ExtensionMap s_map;
    return s_map;
  }
  ExtensionMap& sensitive_detector_extensions() {
    static ExtensionMap s_map;
    return s_map;
  }
};

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
  : magic(magic_word()), id(0), combineHits(0), readout(), 
    alignment(), placement(), parent(), children(),
    worldTrafo(0), parentTrafo(0), referenceTrafo(0)
{
}

/// Internal object destructor: release extension object(s)
DetElement::Object::~Object() {
  ExtensionMap& m = detelement_extensions();
  for(Extensions::iterator i = extensions.begin(); i!=extensions.end(); ++i) {
    void* ptr = (*i).second;
    if ( ptr ) {
      ExtensionMap::iterator j = m.find((*i).first);
      if ( j != m.end() ) {
	ExtensionEntry& e = (*j).second;
	if ( e.destruct ) (*(e.destruct))(ptr);
      }
    }
  }
  extensions.clear();
}


/// Deep object copy to replicate DetElement trees e.g. for reflection
Value<TNamed,DetElement::Object>* DetElement::Object::clone(int new_id, int flag)  const  {
  Value<TNamed,Object>* obj = new Value<TNamed,Object>();
  const ExtensionMap& m = detelement_extensions();
  Ref_t det(obj);
  obj->id           = new_id;
  obj->combineHits  = combineHits;
  obj->readout      = readout;
  obj->volume       = volume;
  obj->alignment    = Alignment();
  obj->conditions   = Conditions();
  obj->parent       = DetElement();
  obj->placement    = ((flag&COPY_PLACEMENT) == COPY_PLACEMENT) ? placement : PlacedVolume();

  // This implicitly assumes that the children do not access the parent's extensions!
  obj->extensions.clear();
  for(DetElement::Extensions::const_iterator i=extensions.begin(); i != extensions.end(); ++i)  {
    const type_info* info = (*i).first;
    ExtensionMap::const_iterator j = m.find(info);
    const ExtensionEntry& e = (*j).second;
    obj->extensions[info]   = (*(e.copy))((*i).second,det);
  }

  obj->children.clear();
  for(DetElement::Children::const_iterator i=children.begin(); i != children.end(); ++i) {
    const TNamed* pc = (*i).second.ptr();
    const DetElement::Object& d = (*i).second._data();
    DetElement child(d.clone(d.id,COPY_PLACEMENT),pc->GetName(),pc->GetTitle());
    pair<Children::iterator,bool> r = obj->children.insert(make_pair(child.name(),child));
    if ( r.second )   {
      child._data().parent = det;
    }
    else {
      throw runtime_error("DetElement::copy: Element "+string(child.name())+" is already present [Double-Insert]");
    }
  }
  return obj;
}

/// Conversion to reference object
Ref_t DetElement::Object::asRef() {
  TNamed* nam = dynamic_cast<TNamed*>(this);
  if ( nam ) return Ref_t(nam);
  return Ref_t(0);
  //return Ref_t(dynamic_cast<Value<TNamed,DetElement::Object>*>(this));
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
      DetElement me(this->asRef());
      DetElement elt = _par(ref,me,nodes);
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
  assign(new Value<TNamed,Object>(),name,parent.type());
  _data().id = id;
  parent.add(*this);
}

/// Add an extension object to the detector element
void* DetElement::i_addExtension(void* ptr, const type_info& info, void* (*copy)(const void*,DetElement), void (*destruct)(void*)) {
  Object& o = _data();
  Extensions::iterator j = o.extensions.find(&info);
  if ( j == o.extensions.end() )   {
    ExtensionMap& m = detelement_extensions();
    ExtensionMap::iterator i = m.find(&info);
    if ( i == m.end() ) {
      ExtensionEntry entry;
      entry.destruct = destruct;
      entry.copy = copy;
      entry.id = ++s_extensionID;
      m.insert(make_pair(&info,entry));
      i = m.find(&info);
    }
    ExtensionEntry& e = (*i).second;
    //cout << "Extension[" << name() << "]:" << ptr << " " << info.name() << endl;
    return o.extensions[&info] = ptr;
  }
  throw runtime_error("addExtension: The object "+string(name())+
		      " already has an extension of type:"+string(info.name())+".");
}

/// Access an existing extension object from the detector element
void* DetElement::i_extension(const type_info& info)   const {
  Object& o = _data();
  Extensions::const_iterator j = o.extensions.find(&info);
  if ( j != o.extensions.end() )   {
    return (*j).second;
  }
  throw runtime_error("extension: The object "+string(name())+
		      " has no extension of type:"+string(info.name())+".");
}
 
/// Access to the full path to the placed object
string DetElement::placementPath() const {
  if ( isValid() ) {
    Object& o = _data();
    if ( o.placementPath.empty() )   {
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

/// Set the type of the sensitive detector
DetElement& DetElement::setType(const string& typ)   {
  if ( isValid() )  {
    m_element->SetTitle(typ.c_str());
    return *this;
  }
  throw runtime_error("DetElement::setType: Self is not defined [Invalid Handle]");
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
  return _data().combineHits != 0;
}

DetElement& DetElement::setCombineHits(bool value, SensitiveDetector& sens)   {
  _data().combineHits = value;
  if ( sens.isValid() ) sens.setCombineHits(value);
  return *this;
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
DetElement DetElement::child(const string& name) const {
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
    return DetElement(_data().clone(_data().id,COPY_NONE), new_name, ptr()->GetTitle());
  }
  throw runtime_error("DetElement::clone: Self is not defined - clone failed! [Invalid Handle]");
}

DetElement DetElement::clone(const string& new_name, int new_id)  const  {
  if ( isValid() ) {
    return DetElement(_data().clone(new_id, COPY_NONE), new_name, ptr()->GetTitle());
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
                                        const string& region, 
                                        const string& limits, 
                                        const string& vis)
{
  return setRegion(lcdd,region,volume).setLimitSet(lcdd,limits,volume).setVisAttributes(lcdd,vis,volume);
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
  Double_t master_point[3]={0,0,0}, local_point[3] = {local.X(),local.Y(),local.Z()};
  // If the path is unknown an exception will be thrown inside worldTransformation() !
  _data().worldTransformation()->LocalToMaster(local_point,master_point);
  global.SetCoordinates(master_point);
  return true;
}

/// Transformation from local coordinates of the placed volume to the parent system
bool DetElement::localToParent(const Position& local, Position& global)  const {
  // If the path is unknown an exception will be thrown inside parentTransformation() !
  Double_t master_point[3]={0,0,0}, local_point[3] = {local.X(),local.Y(),local.Z()};
  _data().parentTransformation()->LocalToMaster(local_point,master_point);
  global.SetCoordinates(master_point);
  return true;
}

/// Transformation from local coordinates of the placed volume to arbitrary parent system set as reference
bool DetElement::localToReference(const Position& local, Position& global)  const {
  // If the path is unknown an exception will be thrown inside referenceTransformation() !
  Double_t master_point[3]={0,0,0}, local_point[3] = {local.X(),local.Y(),local.Z()};
  _data().referenceTransformation()->LocalToMaster(local_point,master_point);
  global.SetCoordinates(master_point);
  return true;
}

/// Transformation from world coordinates of the local placed volume coordinates
bool DetElement::worldToLocal(const Position& global, Position& local)  const {
  // If the path is unknown an exception will be thrown inside worldTransformation() !
  Double_t master_point[3]={global.X(),global.Y(),global.Z()}, local_point[3] = {0,0,0};
  _data().worldTransformation()->MasterToLocal(master_point,local_point);
  local.SetCoordinates(local_point);
  return true;
}

/// Transformation from parent coordinates of the local placed volume coordinates
bool DetElement::parentToLocal(const Position& global, Position& local)  const {
  // If the path is unknown an exception will be thrown inside parentTransformation() !
  Double_t master_point[3]={global.X(),global.Y(),global.Z()}, local_point[3] = {0,0,0};
  _data().parentTransformation()->MasterToLocal(master_point,local_point);
  local.SetCoordinates(local_point);
  return true;
}

/// Transformation from arbitrary parent system coordinates of the local placed volume coordinates
bool DetElement::referenceToLocal(const Position& global, Position& local)  const {
  // If the path is unknown an exception will be thrown inside referenceTransformation() !
  Double_t master_point[3]={global.X(),global.Y(),global.Z()}, local_point[3] = {0,0,0};
  _data().referenceTransformation()->MasterToLocal(master_point,local_point);
  local.SetCoordinates(local_point);
  return true;
}

/// Internal object destructor: release extension object(s)
SensitiveDetector::Object::~Object() {
  ExtensionMap& m = sensitive_detector_extensions();
  for(Extensions::iterator i = extensions.begin(); i!=extensions.end(); ++i) {
    void* ptr = (*i).second;
    if ( ptr ) {
      ExtensionMap::iterator j = m.find((*i).first);
      if ( j != m.end() ) {
	ExtensionEntry& e = (*j).second;
	if ( e.destruct ) (*(e.destruct))(ptr);
      }
    }
  }
  extensions.clear();
}

/// Constructor
SensitiveDetector::SensitiveDetector(const string& name, const string& type)  {
  /*
    <calorimeter ecut="0" eunit="MeV" hits_collection="EcalEndcapHits" name="EcalEndcap" verbose="0">
      <global_grid_xy grid_size_x="3.5" grid_size_y="3.5"/>
      <idspecref ref="EcalEndcapHits"/>
    </calorimeter>
  */
  assign(new Value<TNamed,Object>(),name,type);
  _data().ecut = 0e0;
  _data().verbose = 0;
}

/// Set the type of the sensitive detector
SensitiveDetector& SensitiveDetector::setType(const string& typ)   {
  if ( isValid() )  {
    m_element->SetTitle(typ.c_str());
    return *this;
  }
  throw runtime_error("SensitiveDetector::setType: Self is not defined [Invalid Handle]");
}

/// Access the type of the sensitive detector
string SensitiveDetector::type() const  {
  return m_element ? m_element->GetTitle() : "";
}

/// Assign the IDDescriptor reference
SensitiveDetector& SensitiveDetector::setReadout(Readout ro)  {
  _data().readout = ro;
  return *this;
}

/// Assign the IDDescriptor reference
Readout SensitiveDetector::readout()  const  {
  return _data().readout;
}

/// Set energy cut off
SensitiveDetector& SensitiveDetector::setEnergyCutoff(double value)   {
  _data().ecut = value;
  return *this;
}

/// Access energy cut off
double SensitiveDetector::energyCutoff()  const {
  return _data().ecut;
}

/// Assign the name of the hits collection
SensitiveDetector& SensitiveDetector::setHitsCollection(const string& collection)  {
  _data().hitsCollection = collection;
  return *this;
}

/// Access the hits collection name
const string& SensitiveDetector::hitsCollection() const {
  return _data().hitsCollection;
}

/// Assign the name of the hits collection
SensitiveDetector& SensitiveDetector::setVerbose(bool value)  {
  int v = value ? 1 : 0;
  _data().verbose = v;
  return *this;
}

/// Access flag to combine hist
bool SensitiveDetector::verbose() const {
  return _data().verbose == 1;
}

/// Assign the name of the hits collection
SensitiveDetector& SensitiveDetector::setCombineHits(bool value)  {
  int v = value ? 1 : 0;
  _data().combineHits = v;
  return *this;
}

/// Access flag to combine hist
bool SensitiveDetector::combineHits() const {
  return _data().combineHits == 1;
}

/// Set the regional attributes to the sensitive detector 
SensitiveDetector& SensitiveDetector::setRegion(Region reg)  {
  _data().region = reg;
  return *this;
}

/// Access to the region setting of the sensitive detector (not mandatory)
Region SensitiveDetector::region() const {
  return _data().region;
}

/// Set the limits to the sensitive detector 
SensitiveDetector& SensitiveDetector::setLimitSet(LimitSet limits)  {
  _data().limits = limits;
  return *this;
}

/// Access to the limit set of the sensitive detector (not mandatory). 
LimitSet SensitiveDetector::limits() const {
  return _data().limits;
}

/// Add an extension object to the detector element
void* SensitiveDetector::i_addExtension(void* ptr, const type_info& info, void (*destruct)(void*)) {
  Object& o = _data();
  Extensions::iterator j = o.extensions.find(&info);
  if ( j == o.extensions.end() )   {
    ExtensionMap& m = sensitive_detector_extensions();
    ExtensionMap::iterator i = m.find(&info);
    if ( i == m.end() ) {
      ExtensionEntry entry;
      entry.destruct = destruct;
      entry.copy = 0;
      entry.id = ++s_extensionID;
      m.insert(make_pair(&info,entry));
      i = m.find(&info);
    }
    ExtensionEntry& e = (*i).second;
    cout << "Extension["<<name()<<"]:" << ptr << " " << typeid(*(TNamed*)ptr).name() << endl;
    return o.extensions[&info] = ptr;
  }
  throw runtime_error("addExtension: The object "+string(name())+
		      " already has an extension of type:"+string(info.name())+".");
}

/// Access an existing extension object from the detector element
void* SensitiveDetector::i_extension(const type_info& info)   const {
  Object& o = _data();
  Extensions::const_iterator j = o.extensions.find(&info);
  if ( j != o.extensions.end() )   {
    return (*j).second;
  }
  throw runtime_error("extension: The object "+string(name())+
		      " has no extension of type:"+string(info.name())+".");
}
 
