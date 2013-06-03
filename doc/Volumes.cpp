// $Id: Volumes.cpp 574 2013-05-17 20:38:31Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/LCDD.h"
#include "DD4hep/InstanceCount.h"

// ROOT include files
#include "TColor.h"
#include "TGeoShape.h"
#include "TGeoVolume.h"
#include "TGeoNode.h"
#include "TGeoMatrix.h"
#include "TGeoMedium.h"
#include "TGeoVoxelFinder.h"
#include "TGeoShapeAssembly.h"

// C/C++ include files
#include <climits>
#include <iostream>
#include <stdexcept>
#include <sstream>

using namespace std;
using namespace DD4hep::Geometry;

/// Default constructor
PlacedVolume::Object::Object() : refCount(0), magic(0), volIDs() {
  InstanceCount::increment(this);
}

/// Copy constructor
PlacedVolume::Object::Object(const Object& c) : refCount(0), magic(c.magic), volIDs(c.volIDs) {
  InstanceCount::increment(this);
}

/// Default destructor
PlacedVolume::Object::~Object()   {
  InstanceCount::decrement(this);
}

/// TGeoExtension overload: Method called whenever requiring a pointer to the extension
TGeoExtension* PlacedVolume::Object::Grab() const   {
  Object* ext = const_cast<Object*>(this);
  ++ext->refCount;
  return ext;
}

/// TGeoExtension overload: Method called always when the pointer to the extension is not needed anymore
void PlacedVolume::Object::Release() const  {
  Object* ext = const_cast<Object*>(this);
  --ext->refCount;
  if ( 0 == ext->refCount ) delete ext;
}

/// Lookup volume ID
vector<PlacedVolume::VolID>::const_iterator 
PlacedVolume::VolIDs::find(const string& name)  const   {
  for(Base::const_iterator i=this->Base::begin(); i!=this->Base::end(); ++i)  
    if ( name == (*i).first ) return i;
  return this->end();
}

/// Insert a new value into the volume ID container
std::pair<vector<PlacedVolume::VolID>::iterator,bool> 
PlacedVolume::VolIDs::insert(const string& name, int value)   {
  Base::iterator i = this->Base::begin();
  for(; i!=this->Base::end(); ++i)  
    if ( name == (*i).first ) break;
  //
  if ( i != this->Base::end() ) {
    return make_pair(i,false);
  }
  i = this->Base::insert(this->Base::end(),make_pair(name,value));
  return make_pair(i,true);
}

/// Accessor to user structure
PlacedVolume::Object& PlacedVolume::data() const   {
  Object* o = (Object*)(ptr()->GetUserExtension());
  return *o;
}

/// Add identifier
PlacedVolume& PlacedVolume::addPhysVolID(const string& name, int value)   {
  data().volIDs.push_back(VolID(name,value));
  return *this;
}

/// Volume material
Material PlacedVolume::material() const 
{  return Material::handle_t(m_element ? m_element->GetMedium() : 0);     }

/// Logical volume of this placement
Volume   PlacedVolume::volume() const 
{  return Volume::handle_t(m_element ? m_element->GetVolume() : 0);       }

/// Parent volume (envelope)
Volume PlacedVolume::motherVol() const 
{  return Volume::handle_t(m_element ? m_element->GetMotherVolume() : 0); }

/// Access to the volume IDs
const PlacedVolume::VolIDs& PlacedVolume::volIDs() const 
{  return data().volIDs;                                                  }

/// String dump
string PlacedVolume::toString() const {
  stringstream s;
  Object& obj = data();
  s << m_element->GetName() << ":  vol='" << m_element->GetVolume()->GetName()
    << "' mat:'" << m_element->GetMatrix()->GetName() << "' volID[" << obj.volIDs.size() << "] ";
  for(VolIDs::const_iterator i=obj.volIDs.begin(); i!=obj.volIDs.end();++i)
    s << (*i).first << "=" << (*i).second << "  ";
  s << ends;
  return s.str();
}

/// Default constructor
Volume::Object::Object() : refCount(0), magic(0), region(), limits(), vis(), sens_det()  {
  InstanceCount::increment(this);
}

/// Default destructor
Volume::Object::~Object()  {
  vis.clear();
  region.clear();
  limits.clear();
  sens_det.clear();
  InstanceCount::decrement(this);
}

/// TGeoExtension overload: Method called whenever requiring a pointer to the extension
TGeoExtension* Volume::Object::Grab() const  {
  Object* ext = const_cast<Object*>(this);
  ++ext->refCount;
  return ext;
}

/// TGeoExtension overload: Method called always when the pointer to the extension is not needed anymore
void Volume::Object::Release() const  {
  Object* ext = const_cast<Object*>(this);
  --ext->refCount;
  if ( 0 == ext->refCount )   {
    delete ext;
  }
  else  {
    cout << "Volume::Object::Release::refCount:" << ext->refCount << endl;
  }
}

/// Constructor to be used when creating a new geometry tree.
Volume::Volume(const string& name)    {
  m_element = new TGeoVolume();
  m_element->SetName(name.c_str());
  m_element->SetUserExtension(new Object());
}

/// Constructor to be used when creating a new geometry tree. Also sets materuial and solid attributes
Volume::Volume(const string& name, const Solid& s, const Material& m) {
  m_element = new TGeoVolume(name.c_str(), s.ptr(), m.ptr());
  m_element->SetUserExtension(new Object());
}

/// Accessor to user structure
Volume::Object& Volume::data() const   {
  Object* o = (Object*)(ptr()->GetUserExtension());
  return *o;
}

/// Set the volume's material
void Volume::setMaterial(const Material& m)  const  {
  if ( m.isValid() )   {
    TGeoMedium* medium = m._ptr<TGeoMedium>();
    if ( medium )  {
      m_element->SetMedium(medium);
      return;
    }
    throw runtime_error("Volume: Medium "+string(m.name())+" is not registered with geometry manager.");
  }
  throw runtime_error("Volume: Attempt to assign invalid material.");
}

static PlacedVolume _addNode(TGeoVolume* par, TGeoVolume* daughter, TGeoMatrix* transform) {
  TGeoVolume* parent = par;
  TObjArray* a = parent->GetNodes();
  Int_t id = a ? a->GetEntries() : 0;
  if ( transform && transform != identityTransform() ) {
    string nam = string(daughter->GetName())+"_placement";
    transform->SetName(nam.c_str());
  }
  parent->AddNode(daughter,id,transform);
  TGeoNodeMatrix* n = dynamic_cast<TGeoNodeMatrix*>(parent->GetNode(id));
  PlacedVolume pv(n);
  pv->SetUserExtension(new PlacedVolume::Object());
  return pv;
}

static TGeoTranslation* _translation(const Position& pos) {
  return new TGeoTranslation("",pos.X()*MM_2_CM,pos.Y()*MM_2_CM,pos.Z()*MM_2_CM);
}

static TGeoRotation* _rotation(const Rotation& rot) {
  return new TGeoRotation("",rot.Phi()*RAD_2_DEGREE,rot.Theta()*RAD_2_DEGREE,rot.Psi()*RAD_2_DEGREE);
}

/// Place daughter volume according to generic Transform3D
PlacedVolume Volume::placeVolume(const Volume& volume, const Transform3D& tr)  const  {
  Rotation rot;
  Position pos;
  tr.GetDecomposition(rot,pos);
  return placeVolume(volume,rot,pos);
}

/// Place translated and rotated daughter volume
PlacedVolume Volume::placeVolume(const Volume& volume, const Position& pos, const Rotation& rot)  const  {
  if ( volume.isValid() )   {
    TGeoCombiTrans* transform = new TGeoCombiTrans("",pos.X()*MM_2_CM,pos.Y()*MM_2_CM,pos.Z()*MM_2_CM,_rotation(rot));
    return _addNode(m_element,volume,transform);
  }
  throw runtime_error("Volume: Attempt to assign an invalid physical volume.");
}

/// Place daughter volume in rotated and then translated mother coordinate system
PlacedVolume Volume::placeVolume(const Volume& volume, const Rotation& rot, const Position& pos)  const  {
  if ( volume.isValid() )   {
    TGeoHMatrix *trans = new TGeoHMatrix();
    double t[3];
    trans->RotateZ(rot.Phi()*RAD_2_DEGREE);
    trans->RotateY(rot.Theta()*RAD_2_DEGREE);
    trans->RotateX(rot.Psi()*RAD_2_DEGREE);
    pos.GetCoordinates(t);
    trans->SetDx(t[0]*MM_2_CM);
    trans->SetDy(t[1]*MM_2_CM);
    trans->SetDz(t[2]*MM_2_CM);
    return _addNode(m_element,volume,trans);
  }
  throw runtime_error("Volume: Attempt to assign an invalid physical volume.");
}

/// Place un-rotated daughter volume at the given position.
PlacedVolume Volume::placeVolume(const Volume& volume, const Position& pos)  const  {
  if ( volume.isValid() )   {
    return _addNode(m_element,volume,_translation(pos));
  }
  throw runtime_error("Volume: Attempt to assign an invalid physical volume.");
}

/// Place rotated daughter volume. The position is automatically the identity position
PlacedVolume Volume::placeVolume(const Volume& volume, const Rotation& rot)  const  {
  if ( volume.isValid() )   {
    return _addNode(m_element,volume,_rotation(rot));
  }
  throw runtime_error("Volume: Attempt to assign an invalid physical volume.");
}

/// Place daughter volume. The position and rotation are the identity
PlacedVolume Volume::placeVolume(const Volume& volume, const IdentityPos& /* pos */)  const  {
  if ( volume.isValid() )   {
    return _addNode(m_element,volume,identityTransform());
  }
  throw runtime_error("Volume: Attempt to assign an invalid physical volume.");
}

/// Place daughter volume. The position and rotation are the identity
PlacedVolume Volume::placeVolume(const Volume& volume, const IdentityRot& /* rot */)  const  {
  if ( volume.isValid() )   {
    return _addNode(m_element,volume,identityTransform());
  }
  throw runtime_error("Volume: Attempt to assign an invalid physical volume.");
}

/// Set Visualization attributes to the volume
void Volume::setVisAttributes(const VisAttr& attr) const   {
  if ( attr.isValid() )  {
    VisAttr::Object* vis = attr.data<VisAttr::Object>();
    Color_t bright = TColor::GetColorBright(vis->color);
    Color_t dark   = TColor::GetColorDark(vis->color);
    int draw_style = vis->drawingStyle;
    int line_style = vis->lineStyle;
    m_element->SetLineColor(dark);
    if ( draw_style == VisAttr::SOLID )  {
      m_element->SetFillColor(bright);
      m_element->SetFillStyle(1001); // Root: solid
    }
    else {
      m_element->SetFillColor(0);
      m_element->SetFillStyle(0);    // Root: hollow
    }
    if ( line_style == VisAttr::SOLID )
      m_element->SetFillStyle(1);
    else if ( line_style == VisAttr::DASHED )
      m_element->SetFillStyle(2);
    else
      m_element->SetFillStyle(line_style);

    m_element->SetLineWidth(10);
    m_element->SetVisibility(vis->visible ? kTRUE : kFALSE);
    m_element->SetAttBit(TGeoAtt::kVisContainers,kTRUE);
    m_element->SetVisDaughters(vis->showDaughters ? kTRUE : kFALSE);
  }
  data().vis = attr;
}

/// Set Visualization attributes to the volume
void Volume::setVisAttributes(const LCDD& lcdd, const string& name)  const {
  if ( !name.empty() )   {
    VisAttr attr = lcdd.visAttributes(name);
    data().vis = attr;
    setVisAttributes(attr);
  }
  else  {
    /*
     string tag = this->name();
     if ( ::strstr(tag.c_str(),"_slice") )       // Slices turned off by default
     setVisAttributes(lcdd.visAttributes("InvisibleNoDaughters"));
     else if ( ::strstr(tag.c_str(),"_layer") )  // Layers turned off, but daughters possibly visible
     setVisAttributes(lcdd.visAttributes("InvisibleWithDaughters"));
     else if ( ::strstr(tag.c_str(),"_module") ) // Tracker modules similar to layers
     setVisAttributes(lcdd.visAttributes("InvisibleWithDaughters"));
     else if ( ::strstr(tag.c_str(),"_module_component") ) // Tracker modules similar to layers
     setVisAttributes(lcdd.visAttributes("InvisibleNoDaughters"));
     */
  }
}

/// Attach attributes to the volume
void Volume::setAttributes(const LCDD& lcdd,
                           const string& region, 
                           const string& limits, 
                           const string& vis)   const
{
  if ( !region.empty() ) setRegion(lcdd.region(region));
  if ( !limits.empty() ) setLimitSet(lcdd.limitSet(limits));
  setVisAttributes(lcdd,vis);
}

/// Set the volume's solid shape
void Volume::setSolid(const Solid& solid)  const  
{  m_element->SetShape(solid);                     }

/// Set the regional attributes to the volume
void Volume::setRegion(const Region& obj)  const   
{  data().region = obj;                            }

/// Set the limits to the volume
void Volume::setLimitSet(const LimitSet& obj)  const   
{  data().limits = obj;                            }

/// Assign the sensitive detector structure
void Volume::setSensitiveDetector(const SensitiveDetector& obj) const   {
  //cout << "Setting sensitive detector '" << obj.name() << "' to volume:" << ptr() << " " << name() << endl;
  data().sens_det = obj;                          
}

/// Access to the handle to the sensitive detector
Ref_t Volume::sensitiveDetector() const
{  return data().sens_det;                         }

/// Accessor if volume is sensitive (ie. is attached to a sensitive detector)
bool Volume::isSensitive() const
{  return data().sens_det.isValid();               }

/// Access to Solid (Shape)
Solid Volume::solid() const   
{  return Solid((*this)->GetShape());              }

/// Access to the Volume material
Material Volume::material() const   
{  return Ref_t(m_element->GetMedium());           }

/// Access the visualisation attributes
VisAttr Volume::visAttributes() const
{  return data().vis;                              }

/// Access to the handle to the region structure
Region Volume::region() const   
{  return data().region;                           }

/// Access to the limit set
LimitSet Volume::limitSet() const   
{  return data().limits;                           }

/// Constructor to be used when creating a new geometry tree.
Assembly::Assembly(const string& name)   {
  Object* ext = new Object();
  m_element = new TGeoVolumeAssembly(name.c_str());
  m_element->SetUserExtension(ext);
}

