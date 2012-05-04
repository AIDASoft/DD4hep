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

using namespace std;
using namespace DD4hep::Geometry;

/// Default constructor
DetElement::Object::Object()  
  : magic(magic_word()), id(0), combine_hits(0), readout(), 
    alignment(), placements(), parent(), children()
{
}

/// Construct new empty object
Value<TNamed,DetElement::Object>* DetElement::Object::construct(int new_id) const {
  Value<TNamed,Object>* obj = new Value<TNamed,Object>();
  obj->deepCopy(*this,new_id);
  return obj;
}

/// Deep object copy to replicate DetElement trees e.g. for reflection
void DetElement::Object::deepCopy(const Object& source, int new_id)  {
  id           = new_id;
  combine_hits = source.combine_hits;
  readout      = source.readout;
  volume       = source.volume;
  alignment    = Alignment();
  conditions   = Conditions();
  placements   = Placements();
  parent       = DetElement();
  for(DetElement::Children::const_iterator i=source.children.begin(); i != source.children.end(); ++i) {
    DetElement child = (*i).second.clone((*i).second->GetName());
    children.insert(make_pair((*i).first,child));
  }
}


/// Constructor for a new subdetector element
DetElement::DetElement(const LCDD& /* lcdd */, const string& name, const string& type, int id)
{
  assign(new Value<TNamed,Object>(),name,type);
  _data().id = id;
}

/// Constructor for a new subdetector element
DetElement::DetElement(const string& name, const string& type, int id)
{
  assign(new Value<TNamed,Object>(),name,type);
  _data().id = id;
}

string DetElement::type() const   {
  return m_element ? m_element->GetTitle() : "";
}

string DetElement::path() const   {
  if ( m_element )  {
    Object& o = _data();
    if ( o.path.empty() )   {
      DetElement par = o.parent;
      if ( par.isValid() )
	return (par.path()+"/")+name();
      return string("/") + name();
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
  const Children& c = _data().children;
  Children::const_iterator i = c.find(name);
  return i == c.end() ? DetElement() : (*i).second;
}

void DetElement::check(bool condition, const string& msg) const  {
  if ( condition )  {
    throw runtime_error(msg);
  }
}

DetElement& DetElement::add(const DetElement& sdet)  {
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

DetElement::Placements DetElement::placements() const    {
  return _data().placements;
}

DetElement DetElement::clone(const string& new_name)  const  {
  if ( isValid() ) {
    return DetElement(_data().construct(_data().id), new_name, ptr()->GetTitle());
  }
  throw runtime_error("DetElement::clone: Self is not defined - clone failed! [Invalid Handle]");
}

DetElement DetElement::clone(const string& new_name, int new_id)  const  {
  if ( isValid() ) {
    return DetElement(_data().construct(new_id), new_name, ptr()->GetTitle());
  }
  throw runtime_error("DetElement::clone: Self is not defined - clone failed! [Invalid Handle]");
}

DetElement& DetElement::addPlacement(const PlacedVolume& placement)  {
  if ( isValid() ) {
    if ( placement.isValid() )  {
      _data().placements.push_back(placement);
      _data().volume = placement.volume();
      placement.setDetElement(*this);
      return *this;
    }
    throw runtime_error("DetElement::addPlacement: Placement is not defined [Invalid Handle]");
  }
  throw runtime_error("DetElement::addPlacement: Self is not defined [Invalid Handle]");
}

/// Access to the logical volume of the placements (all daughters have the same!)
Volume DetElement::volume() const {
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
  throw runtime_error("DetElement::setRegion: Self is not defined [Invalid Handle]");
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
