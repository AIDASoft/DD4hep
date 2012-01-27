#include "Internals.h"
#include "Volumes.h"
#include "Objects.h"
#include "SubDetector.h"

// ROOT include files
#include "TColor.h"
#include "TGeoShape.h"
#include "TGeoVolume.h"
#include "TGeoNode.h"
#include "TGeoMatrix.h"
#include "TGeoManager.h"

// C/C++ include files
#include <climits>
#include <stdexcept>
#include <iostream>

using namespace std;
using namespace DetDesc::Geometry;

static UInt_t unique_physvol_id = INT_MAX-1;

namespace DetDesc  { namespace Geometry  {
  template <> struct Value<TGeoVolume,Volume::Object> 
    : public TGeoVolume, public Volume::Object  
  {
    Value(const char* name, TGeoShape* s=0, TGeoMedium* m=0) 
      : TGeoVolume(name,s,m) {}
    virtual ~Value() {}
  };
}}

Volume::Volume(const Document& document, const string& name)
  : RefElement(0)
{
  m_element.m_node = new Value<TGeoVolume,Volume::Object>(name.c_str());
}

Volume::Volume(const Document& document, const string& name, const Solid& s, const Material& m)   
  : RefElement(0)
{
  m_element.m_node = new Value<TGeoVolume,Volume::Object>(name.c_str());
  setSolid(s);
  setMaterial(m);
  //s.shape().ComputeBBox();

  // hack !!!
  TGeoVolume* vol = first_value<TGeoVolume>(*this);
  // debug only
  vol->SetVisibility(kTRUE);
  vol->SetVisDaughters(kTRUE);
  vol->SetVisLeaves(kTRUE);
  vol->SetVisContainers(kTRUE);
  vol->SetTransparency(70);
}

Volume::Volume(Handle_t e) : RefElement(e)  {
}

void Volume::setMaterial(const Material& m)  const  {
  if ( m.isValid() )   {
    TGeoMedium* medium = value<TGeoMedium>(m);
    if ( medium )  {
      TGeoVolume* vol = first_value<TGeoVolume>(*this);
      vol->SetMedium(medium);
      return;
    }
    throw runtime_error("Volume: Medium "+string(m.name())+" is not registered with geometry manager.");
  }
  throw runtime_error("Volume: Attempt to assign invalid material.");
}

void Volume::setSolid(const Solid& solid)  const  {
  TGeoVolume* val = first_value<TGeoVolume>(*this);
  val->SetShape(&solid.shape());
}

void Volume::addPhysVol(const PhysVol& volume, const Position& pos, const Rotation& rot)  const  {
  Volume vol(volume);
  TGeoVolume* phys_vol = first_value<TGeoVolume>(vol);
  TGeoVolume* log_vol  = first_value<TGeoVolume>(*this);
  if ( phys_vol )   {
    TGeoTranslation* t = value<TGeoTranslation>(pos);
    TGeoRotation*    r = value<TGeoRotation>(rot);
    TGeoCombiTrans*  c = new TGeoCombiTrans(*t,*r);
    log_vol->AddNode(phys_vol, --unique_physvol_id, c);
    return;
  }
  throw runtime_error("Volume: Attempt to assign an invalid physical volume.");
}

void Volume::addPhysVol(const PhysVol& volume, const Matrix& matrix)  const  {
  Volume vol(volume);
  TGeoVolume* phys_vol = first_value<TGeoVolume>(vol);
  TGeoVolume* log_vol  = first_value<TGeoVolume>(*this);
  if ( phys_vol )   {
    TGeoMatrix* m = value<TGeoMatrix>(matrix);
    log_vol->AddNode(phys_vol, --unique_physvol_id, m);
    return;
  }
  throw runtime_error("Volume: Attempt to assign an invalid physical volume.");
}

#if 0
void Volume::addPhysVol(const Volume& volume, const Position& pos, const Rotation& rot)  const  {
  TGeoVolume* phys_vol = first_value<TGeoVolume>(volume);
  TGeoVolume* log_vol  = first_value<TGeoVolume>(*this);
  if ( phys_vol )   {
    TGeoTranslation* t = value<TGeoTranslation>(pos);
    TGeoRotation*    r = value<TGeoRotation>(rot);
    TGeoCombiTrans*  c = new TGeoCombiTrans(*t,*r);
    log_vol->AddNode(phys_vol, phys_vol->GetUniqueID(), c);
    return;
  }
  throw runtime_error("Volume: Attempt to assign an invalid physical volume.");
}
void Volume::addPhysVol(const Volume& volume, const Matrix& matrix)  const  {
  TGeoVolume* phys_vol = first_value<TGeoVolume>(volume);
  TGeoVolume* log_vol  = first_value<TGeoVolume>(*this);
  if ( phys_vol )   {
    TGeoMatrix* m = value<TGeoMatrix>(matrix);
    log_vol->AddNode(phys_vol, phys_vol->GetUniqueID(), m);
    return;
  }
  throw runtime_error("Volume: Attempt to assign an invalid physical volume.");
}
#endif

void Volume::setRegion(const Region& obj)  const   {
  Object* val = second_value<TGeoVolume>(*this);
  val->Attr_region = obj;
}

void Volume::setLimitSet(const LimitSet& obj)  const   {
  Object* val = second_value<TGeoVolume>(*this);
  val->Attr_limits = obj;
}

void Volume::setSensitiveDetector(const SensitiveDetector& obj) const  {
  Object* val = second_value<TGeoVolume>(*this);
  val->Attr_sens_det = obj;
}

void Volume::setVisAttributes(const VisAttr& attr) const   {
  TGeoVolume* vol = first_value<TGeoVolume>(*this);
  Object*     val = second_value<TGeoVolume>(*this);
  if ( attr.isValid() )  {
    VisAttr::Object* vis = second_value<TNamed>(attr);
    Color_t bright = TColor::GetColorBright(vis->color);
    Color_t dark   = TColor::GetColorDark(vis->color);
    vol->SetFillColor(bright);
    vol->SetLineColor(dark);
    vol->SetLineStyle(vis->lineStyle);
    vol->SetLineWidth(10);
    vol->SetVisibility(vis->visible ? kTRUE : kFALSE);
    vol->SetVisDaughters(vis->showDaughters ? kTRUE : kFALSE);
  }
  // debug only
  vol->SetVisibility(kTRUE);
  vol->SetVisDaughters(kTRUE);
  vol->SetVisLeaves(kTRUE);
  vol->SetVisContainers(kTRUE);
  vol->SetTransparency(70);

  val->Attr_vis = attr;
}

Solid Volume::solid() const   {
  return Solid(first_value<TGeoVolume>(*this)->GetShape());
}

Material Volume::material() const   {
  return Material(first_value<TGeoVolume>(*this)->GetMaterial());
}

VisAttr Volume::visAttributes() const   {
  Object* val = second_value<TGeoVolume>(*this);
  return val->Attr_vis;
}

RefElement Volume::sensitiveDetector() const    {
  Object* val = second_value<TGeoVolume>(*this);
  return val->Attr_sens_det;
}

Region Volume::region() const   {
  Object* val = second_value<TGeoVolume>(*this);
  return val->Attr_region;
}

const TGeoVolume* Volume::volume() const  {
  return first_value<TGeoVolume>(*this);
}

PhysVol& PhysVol::addPhysVolID(const std::string& /* name */, int /* value */) {
  return *this;
}

#if 0

/// Constructor to be used when creating a new DOM tree
PhysVol::PhysVol(const Document& document, const Volume& volume, const string& name) 
: RefElement(document,"physvol",name)
{
  TGeoVolume* log_vol  = first_value<TGeoVolume>(volume);
  TGeoVolume* phys_vol = first_value<TGeoVolume>(*this);
  Object* obj = second_value<TGeoVolume>(*this);
  phys_vol->SetUniqueID(--unique_physvol_id);
  phys_vol->SetMedium(log_vol->GetMedium());
  phys_vol->SetShape(log_vol->GetShape());
  phys_vol->SetName(name.c_str());
  obj->Attr_volume = volume;
  phys_vol->SetTitle(phys_vol->GetName());

  // Copy vis attrs from volume prototype
  VisAttr attr = second_value<TGeoVolume>(volume)->Attr_vis;
  if ( attr.isValid() )  {
    VisAttr::Object* vis = second_value<TNamed>(attr);
    Color_t bright = TColor::GetColorBright(vis->color);
    Color_t dark   = TColor::GetColorDark(vis->color);
    phys_vol->SetFillColor(bright);
    phys_vol->SetLineColor(dark);
    //cout << "Phys.Volume:" << this->name() << " Color:" << hex << bright << dec 
    //     << " Visibility:" << (vis->visible ? "true" : "false") << endl;
    phys_vol->SetLineStyle(vis->lineStyle);
    phys_vol->SetLineWidth(10);
    phys_vol->SetVisibility(vis->visible ? kTRUE : kFALSE);
    phys_vol->SetVisDaughters(vis->showDaughters ? kTRUE : kFALSE);
  }
// hack!!!
    phys_vol->SetLineWidth(10);
    phys_vol->SetVisibility(kTRUE);
    phys_vol->SetVisDaughters(kTRUE);
    phys_vol->SetTransparency(70);
}

/// Constructor to be used when creating a new DOM tree
PhysVol::PhysVol(const Document& document, const Volume& volume) 
: RefElement(document,"physvol",volume.name()) 
{
  string name = volume.name();
  TGeoVolume* log_vol  = first_value<TGeoVolume>(volume);
  TGeoVolume* phys_vol = first_value<TGeoVolume>(*this);
  Object* obj = second_value<TGeoVolume>(*this);
  phys_vol->SetUniqueID(--unique_physvol_id);
  phys_vol->SetMedium(log_vol->GetMedium());
  phys_vol->SetShape(log_vol->GetShape());
  phys_vol->SetName(name.c_str());
  obj->Attr_volume = volume;
  phys_vol->SetTitle(phys_vol->GetName());
  // Copy vis attrs from volume prototype
  VisAttr attr = second_value<TGeoVolume>(volume)->Attr_vis;
  if ( attr.isValid() )  {
    VisAttr::Object* vis = second_value<TNamed>(attr);
    Color_t bright = TColor::GetColorBright(vis->color);
    Color_t dark   = TColor::GetColorDark(vis->color);
    phys_vol->SetFillColor(bright);
    phys_vol->SetLineColor(dark);
    //cout << "Phys.Volume:" << this->name() << " Color:" << hex << bright << dec 
    //     << " Visibility:" << (vis->visible ? "true" : "false") << endl;
    phys_vol->SetLineStyle(vis->lineStyle);
    phys_vol->SetLineWidth(10);
    phys_vol->SetVisibility(vis->visible ? kTRUE : kFALSE);
    phys_vol->SetVisDaughters(vis->showDaughters ? kTRUE : kFALSE);
  }
// hack!!!
    phys_vol->SetLineWidth(10);
    phys_vol->SetVisibility(kTRUE);
    phys_vol->SetVisDaughters(kTRUE);
    phys_vol->SetTransparency(70);
}

PhysVol& PhysVol::addPhysVolID(const string& name, int value)  {
  Object* p = second_value<TGeoVolume>(*this);
  p->Attr_ids.insert(make_pair(name,value));
  return *this;
}

Volume PhysVol::logVolume() const  {
  return second_value<TGeoVolume>(*this)->Attr_volume;
}

TGeoVolume* PhysVol::volume() const  {
  return first_value<TGeoVolume>(*this);
}

#endif
