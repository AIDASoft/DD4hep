#include "Internals.h"
#include "IDDescriptor.h"
#include "SubDetector.h"
#include "LCDD.h"

using namespace std;
using namespace DetDesc::Geometry;

Subdetector::Object::Object()  
: Attr_id(0), Attr_combine_hits(0), Attr_envelope(0),
  Attr_volume(0), Attr_material(0), Attr_visualization(0), Attr_readout(0)
{
}

/// Constructor for a new subdetector element
Subdetector::Subdetector(const Document& document, const string& name, const string& type, int id)
: RefElement(document, "subdetector", name)
{
  first_value<TNamed>(*this)->SetTitle(type.c_str());
  setAttr(Attr_id,id);
}

string Subdetector::type() const   {
  return first_value<TNamed>(*this)->GetTitle();
}

int Subdetector::id() const   {
  return getAttr(Attr_id);
}

Material Subdetector::material() const  {
  return getAttr(Attr_material);
}

bool Subdetector::combineHits() const   {
  return getAttr(Attr_combine_hits) != 0;
}

VisAttr Subdetector::visAttr() const  {
  return getAttr(Attr_visualization);
}

Readout Subdetector::readout() const   {
  return getAttr(Attr_readout);
}

Subdetector& Subdetector::setReadout(const Readout& readout)   {
  setAttr(Attr_readout,readout);
  return *this;
}

const Subdetector::Children& Subdetector::children() const   {
  return getAttr(Attr_children);
}

void Subdetector::check(bool condition, const string& msg) const  {
  if ( condition )  {
    throw runtime_error(msg);
  }
}

Subdetector& Subdetector::add(const Subdetector& sdet)  {
  Object* o = second_value<TNamed>(*this);
  if ( o )  {
    pair<Children::iterator,bool> r = o->Attr_children.insert(make_pair(sdet.name(),sdet));
    if ( r.second ) return *this;
    throw runtime_error("Subdetector::add: Element "+string(sdet.name())+" is already present [Double-Insert]");
  }
  throw runtime_error("Subdetector::add: Self is not defined [Invalid Handle]");
}

Volume  Subdetector::volume() const    {
  return getAttr(Attr_volume);
}

Subdetector& Subdetector::setVolume(const Volume& volume)  {
  setAttr(Attr_volume,volume);
  setAttr(Attr_material,volume.material());
  return *this;
}

Solid  Subdetector::envelope() const    {
  return getAttr(Attr_envelope);
}

Subdetector& Subdetector::setEnvelope(const Solid& solid)   {
  setAttr(Attr_envelope,solid);
  return *this;
}
#include "TGeoVolume.h"
Subdetector& Subdetector::setVisAttributes(const LCDD& lcdd, const string& name, const Volume& volume)  {
  if ( !name.empty() )   {
    VisAttr attr = lcdd.visAttributes(name);
    setAttr(Attr_visualization,attr);
    volume.setVisAttributes(attr);
  }
  else  {
    string nam = this->name();
    //string vol = volume.name();
    //string what = nam+":"+vol;
    TGeoVolume* vol = (TGeoVolume*)volume.volume();
    vol->SetVisibility(kTRUE);
    vol->SetVisDaughters(kTRUE);
    vol->SetVisLeaves(kTRUE);
    vol->SetVisContainers(kTRUE);

    /*
    string tag = node.tag();
    if ( tag == Tag_slice.str() )  // Slices turned off by default
      volume.setVisAttributes(lcdd.visAttributes(Attr_InvisibleNoDaughters));
    else if ( tag == Tag_layer.str() )  // Layers turned off, but daughters possibly visible
      volume.setVisAttributes(lcdd.visAttributes(Attr_InvisibleWithDaughters));
    else if ( tag == Tag_module.str() )  // Tracker modules similar to layers
      volume.setVisAttributes(lcdd.visAttributes(Attr_InvisibleWithDaughters));
    else if ( tag == Tag_module_component.str() )  // Tracker modules components turned off by default
      volume.setVisAttributes(lcdd.visAttributes(Attr_InvisibleNoDaughters));
  */
  }
  return *this;
}

Subdetector& Subdetector::setRegion(const LCDD& lcdd, const string& name, const Volume& volume)  {
  Object* o = second_value<TNamed>(*this);
  if ( o )  {
    if ( !name.empty() )  {
      volume.setRegion(lcdd.region(name));
    }
    return *this;
  }
  throw runtime_error("Subdetector::setRegion: Self is not defined [Invalid Handle]");
}

Subdetector& Subdetector::setLimitSet(const LCDD& lcdd, const string& name, const Volume& volume)  {
  Object* o = second_value<TNamed>(*this);
  if ( o )  {
    if ( !name.empty() )  {
      volume.setLimitSet(lcdd.limitSet(name));
    }
    return *this;
  }
  throw runtime_error("Subdetector::setLimitSet: Self is not defined [Invalid Handle]");
}

Subdetector& Subdetector::setAttributes(const LCDD& lcdd, const Volume& volume,
                                        const std::string& region, 
                                        const std::string& limits, 
                                        const std::string& vis)
{
  setRegion(lcdd,region,volume);
  setLimitSet(lcdd,limits,volume);
  return setVisAttributes(lcdd,vis,volume);
}

Subdetector& Subdetector::setCombineHits(bool value, SensitiveDetector& sens)   {
  if ( isTracker() )  {
    setAttr(Attr_combine_hits,value);
    sens.setCombineHits(value);
  }
  return *this;
}

bool Subdetector::isTracker() const   {
  if ( isValid() )  {
    string typ = type();
    if ( typ.find("Tracker") != string::npos && getAttr(Attr_readout).isValid() )   {
      return true;
    }
  }
  return false;
}

bool Subdetector::isCalorimeter() const   {
  if ( isValid() )  {
    string typ = type();
    if ( typ.find("Calorimeter") != string::npos && getAttr(Attr_readout.isValid()) ) {
      return true;
    }
  }
  return false;
}

//#if 0
bool Subdetector::isInsideTrackingVolume() const  {
  //if ( isValid() && hasAttr(Attr_insideTrackingVolume) )
  //  return attr<bool>(Attr_insideTrackingVolume);
  //else if ( isTracker() )
  //  return true;
  return false;
}
//#endif

SensitiveDetector::SensitiveDetector(const Document& doc, const std::string& type, const std::string& name) 
: RefElement(doc, "sensitive_detector", name)
{
  /*
    <calorimeter ecut="0" eunit="MeV" hits_collection="EcalEndcapHits" name="EcalEndcap" verbose="0">
      <global_grid_xy grid_size_x="3.5" grid_size_y="3.5"/>
      <idspecref ref="EcalEndcapHits"/>
    </calorimeter>
  */
  first_value<TNamed>(*this)->SetTitle(type.c_str());
  setAttr(Attr_ecut,0e0);
  setAttr(Attr_eunit,"MeV");
  setAttr(Attr_verbose,0);
}

/// Access the type of the sensitive detector
string SensitiveDetector::type() const  {
  TNamed *p = first_value<TNamed>(*this);
  return p ? p->GetTitle() : "";
}

/// Assign the IDDescriptor reference
SensitiveDetector& SensitiveDetector::setIDSpec(const RefElement& spec)  {
  setAttr(Attr_id,spec);
  return *this;
}

/// Assign the name of the hits collection
SensitiveDetector& SensitiveDetector::setHitsCollection(const string& collection)  {
  setAttr(Attr_hits_collection,collection);
  return *this;
}

/// Assign the name of the hits collection
SensitiveDetector& SensitiveDetector::setCombineHits(bool value)  {
  int v = value ? 1 : 0;
  setAttr(Attr_combine_hits,v);
  return *this;
}

/// Assign the readout segmentation reference
SensitiveDetector& SensitiveDetector::setSegmentation(Element segmentation)   {
  if ( segmentation.isValid() )  {
    setAttr(Attr_segmentation,segmentation);
    return *this;
  }
  throw runtime_error("Readout::setSegmentation: Cannot assign segmentation [Invalid Handle]");
}
