// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DD4hep/LCDD.h"

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

namespace DD4hep  { namespace Geometry  {
  
  template <> struct Value<TGeoNodeMatrix,PlacedVolume::Object> 
  : public TGeoNodeMatrix, public PlacedVolume::Object  
  {
    Value(const TGeoVolume* v, const TGeoMatrix* m) : TGeoNodeMatrix(v,m), PlacedVolume::Object() {
      magic = magic_word();
    }
    Value(const Value& c) : TGeoNodeMatrix(c.GetVolume(),c.GetMatrix()), PlacedVolume::Object(c) {
    }
    virtual TGeoNode *MakeCopyNode() const {
      TGeoNodeMatrix *node = new Value<TGeoNodeMatrix,PlacedVolume::Object>(*this);
      node->SetName(GetName());
      // set the mother
      node->SetMotherVolume(fMother);
      // set the copy number
      node->SetNumber(fNumber);
      // copy overlaps
      if (fNovlp>0) {
	if (fOverlaps) {
	  Int_t *ovlps = new Int_t[fNovlp];
	  memcpy(ovlps, fOverlaps, fNovlp*sizeof(Int_t));
	  node->SetOverlaps(ovlps, fNovlp);
	} else {
	  node->SetOverlaps(fOverlaps, fNovlp);
	}
      }
      // copy VC
      if (IsVirtual()) node->SetVirtual();
      return node;
    }
  };
  
  template <class T> struct _VolWrap : public T  {
    _VolWrap(const char* name, TGeoShape* s=0, TGeoMedium* m=0);
    virtual ~_VolWrap() {}
    virtual void AddNode(const TGeoVolume *vol, Int_t copy_no, TGeoMatrix *mat, Option_t* = "") {
      TGeoMatrix *matrix = mat;
      if ( matrix == 0 ) matrix = gGeoIdentity;
      else               matrix->RegisterYourself();
      if (!vol)   {
        this->T::Error("AddNode", "Volume is NULL");
        return;
      }
      if (!vol->IsValid())   {
        this->T::Error("AddNode", "Won't add node with invalid shape");
        printf("### invalid volume was : %s\n", vol->GetName());
        return;
      }
      if ( !this->T::fNodes ) this->T::fNodes = new TObjArray();   
      
      if ( this->T::fFinder )    {
        // volume already divided.
        this->T::Error("AddNode", "Cannot add node %s_%i into divided volume %s", vol->GetName(), copy_no, this->T::GetName());
        return;
      }
      
      TGeoNodeMatrix *node = new Value<TGeoNodeMatrix,PlacedVolume::Object>(vol, matrix);
      //node = new TGeoNodeMatrix(vol, matrix);
      node->SetMotherVolume(this);
      this->T::fNodes->Add(node);
      TString name = TString::Format("%s_%d", vol->GetName(), copy_no);
      if (this->T::fNodes->FindObject(name))
        this->T::Warning("AddNode", "Volume %s : added node %s with same name", this->T::GetName(), name.Data());
      node->SetName(name);
      node->SetNumber(copy_no);
    }
  };
  
  template <> _VolWrap<TGeoVolume>::_VolWrap(const char* name, TGeoShape* s, TGeoMedium* m)
  : TGeoVolume(name,s,m) {}
  template <> _VolWrap<TGeoVolumeAssembly>::_VolWrap(const char* name, TGeoShape* s, TGeoMedium* m)
  : TGeoVolumeAssembly(name) {}
  
  template <> struct Value<TGeoVolume,Volume::Object>
  : public _VolWrap<TGeoVolume>, public Volume::Object  {
    Value(const char* name, TGeoShape* s=0, TGeoMedium* m=0) : _VolWrap<TGeoVolume>(name,s,m) {magic = magic_word();}
    virtual ~Value() {}
    virtual TGeoVolume* MakeCopyVolume(TGeoShape *newshape) {
      // make a copy of this volume. build a volume with same name, shape and medium
      TGeoVolume *vol = new Value<TGeoVolume,Volume::Object>(GetName(), newshape, fMedium);
      vol->SetVisibility(IsVisible());
      vol->SetLineColor(GetLineColor());
      vol->SetLineStyle(GetLineStyle());
      vol->SetLineWidth(GetLineWidth());
      vol->SetFillColor(GetFillColor());
      vol->SetFillStyle(GetFillStyle());
      vol->SetField(fField);
      if (fFinder) vol->SetFinder(fFinder);
      CloneNodesAndConnect(vol);
      ((TObject*)vol)->SetBit(kVolumeClone);
      return vol;       
    }
    virtual TGeoVolume* CloneVolume() const    {
      TGeoVolume *vol = new Value<TGeoVolume,Volume::Object>(GetName(), fShape, fMedium);
      Int_t i;
      // copy volume attributes
      vol->SetLineColor(GetLineColor());
      vol->SetLineStyle(GetLineStyle());
      vol->SetLineWidth(GetLineWidth());
      vol->SetFillColor(GetFillColor());
      vol->SetFillStyle(GetFillStyle());
      // copy other attributes
      Int_t nbits = 8*sizeof(UInt_t);
      for (i=0; i<nbits; i++) 
        vol->SetAttBit(1<<i, TGeoAtt::TestAttBit(1<<i));
      for (i=14; i<24; i++)
        vol->SetBit(1<<i, TestBit(1<<i));   
      
      // copy field
      vol->SetField(fField);
      // Set bits
      for (i=0; i<nbits; i++) 
        vol->SetBit(1<<i, TObject::TestBit(1<<i));
      vol->SetBit(kVolumeClone);   
      // copy nodes
      //   CloneNodesAndConnect(vol);
      vol->MakeCopyNodes(this);   
      // if volume is divided, copy finder
      vol->SetFinder(fFinder);
      // copy voxels
      TGeoVoxelFinder *voxels = 0;
      if (fVoxels) {
        voxels = new TGeoVoxelFinder(vol);
        vol->SetVoxelFinder(voxels);
      }   
      // copy option, uid
      vol->SetOption(fOption);
      vol->SetNumber(fNumber);
      vol->SetNtotal(fNtotal);
      return vol;
    }
  };
  
  template <> struct Value<TGeoVolumeAssembly,Assembly::Object> 
  : public _VolWrap<TGeoVolumeAssembly>, public Assembly::Object  {
    Value(const char* name) : _VolWrap<TGeoVolumeAssembly>(name,0,0) {  magic = magic_word(); }
    virtual ~Value() {}
    
    TGeoVolume *CloneVolume() const    {
      TGeoVolume *vol = new Value<TGeoVolumeAssembly,Assembly::Object>(GetName());
      Int_t i;
      // copy other attributes
      Int_t nbits = 8*sizeof(UInt_t);
      for (i=0; i<nbits; i++) 
        vol->SetAttBit(1<<i, TGeoAtt::TestAttBit(1<<i));
      for (i=14; i<24; i++)
        vol->SetBit(1<<i, TestBit(1<<i));   
      
      // copy field
      vol->SetField(fField);
      // Set bits
      for (i=0; i<nbits; i++) 
        vol->SetBit(1<<i, TObject::TestBit(1<<i));
      vol->SetBit(kVolumeClone);   
      // make copy nodes
      vol->MakeCopyNodes(this);
      ((TGeoShapeAssembly*)vol->GetShape())->NeedsBBoxRecompute();
      // copy voxels
      TGeoVoxelFinder *voxels = 0;
      if (fVoxels) {
        voxels = new TGeoVoxelFinder(vol);
        vol->SetVoxelFinder(voxels);
      }   
      // copy option, uid
      vol->SetOption(fOption);
      vol->SetNumber(fNumber);
      vol->SetNtotal(fNtotal);
      return vol;
    }
  };

}}

/// Add identifier
PlacedVolume& PlacedVolume::addPhysVolID(const std::string& name, int value)   {
  Object* obj = data<Object>();
  obj->volIDs[name] = value;
  return *this;
}

/// Volume material
Material PlacedVolume::material() const 
{  return Material::handle_t(m_element ? m_element->GetMedium()->GetMaterial() : 0);  }

/// Logical volume of this placement
Volume   PlacedVolume::volume() const 
{  return Volume::handle_t(m_element ? m_element->GetVolume() : 0);                   }

/// Parent volume (envelope)
Volume PlacedVolume::motherVol() const 
{  return Volume::handle_t(m_element ? m_element->GetMotherVolume() : 0);             }

/// Access to the volume IDs
const PlacedVolume::VolIDs& PlacedVolume::volIDs() const 
{  return data<Object>()->volIDs;                                                     }

/// String dump
string PlacedVolume::toString() const {
  stringstream s;
  Object* obj = data<Object>();
  s << m_element->GetName() << ":  vol='" << m_element->GetVolume()->GetName()
    << "' mat:'" << m_element->GetMatrix()->GetName() << "' volID[" << obj->volIDs.size() << "] ";
  for(VolIDs::const_iterator i=obj->volIDs.begin(); i!=obj->volIDs.end();++i)
    s << (*i).first << "=" << (*i).second << "  ";
  s << ends;
  return s.str();
}

/// Constructor to be used when creating a new geometry tree.
Volume::Volume(const string& name)    {
  m_element = new Value<TGeoVolume,Volume::Object>(name.c_str());
}

/// Constructor to be used when creating a new geometry tree. Also sets materuial and solid attributes
Volume::Volume(const string& name, const Solid& s, const Material& m) {
  m_element = new Value<TGeoVolume,Volume::Object>(name.c_str(),s);
  setMaterial(m);
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
  parent->AddNode(daughter,id,transform);
  TGeoNodeMatrix* n = dynamic_cast<TGeoNodeMatrix*>(parent->GetNode(id));
  return PlacedVolume(n);
}

/// Place translated and rotated daughter volume
PlacedVolume Volume::placeVolume(const Volume& volume, const Position& pos, const Rotation& rot)  const  {
  if ( volume.isValid() )   {
    string nam = string(volume.name())+"_placement";
    TGeoRotation rotation("",rot.phi*RAD_2_DEGREE,rot.theta*RAD_2_DEGREE,rot.psi*RAD_2_DEGREE);
    TGeoCombiTrans* transform = new TGeoCombiTrans(nam.c_str(),pos.x,pos.y,pos.z,0);
    transform->SetRotation(rotation);
    return _addNode(m_element,volume,transform);
  }
  throw runtime_error("Volume: Attempt to assign an invalid physical volume.");
}

/// Place un-rotated daughter volume at the given position.
PlacedVolume Volume::placeVolume(const Volume& volume, const Position& pos)  const  {
  if ( volume.isValid() )   {
    string nam = string(volume.name())+"_placement";
    TGeoTranslation* transform = new TGeoTranslation(nam.c_str(),pos.x,pos.y,pos.z);
    return _addNode(m_element,volume,transform);
  }
  throw runtime_error("Volume: Attempt to assign an invalid physical volume.");
}

/// Place rotated daughter volume. The position is automatically the identity position
PlacedVolume Volume::placeVolume(const Volume& volume, const Rotation& rot)  const  {
  if ( volume.isValid() )   {
    string nam = string(volume.name())+"_placement";
    TGeoRotation* transform = new TGeoRotation(nam.c_str(),rot.phi*RAD_2_DEGREE,rot.theta*RAD_2_DEGREE,rot.psi*RAD_2_DEGREE);
    return _addNode(m_element,volume,transform);
  }
  throw runtime_error("Volume: Attempt to assign an invalid physical volume.");
}

/// Place daughter volume. The position and rotation are the identity
PlacedVolume Volume::placeVolume(const Volume& volume, const IdentityPos& /* pos */)  const  {
  if ( volume.isValid() )   {
    string nam = string(volume.name())+"_placement";
    return _addNode(m_element,volume,new TGeoIdentity(nam.c_str()));
  }
  throw runtime_error("Volume: Attempt to assign an invalid physical volume.");
}

/// Place daughter volume. The position and rotation are the identity
PlacedVolume Volume::placeVolume(const Volume& volume, const IdentityRot& /* rot */)  const  {
  if ( volume.isValid() )   {
    string nam = string(volume.name())+"_placement";
    return _addNode(m_element,volume,new TGeoIdentity(nam.c_str()));
  }
  throw runtime_error("Volume: Attempt to assign an invalid physical volume.");
}

/// Set Visualization attributes to the volume
void Volume::setVisAttributes(const VisAttr& attr) const   {
  if ( attr.isValid() )  {
    VisAttr::Object* vis = attr.data<VisAttr::Object>();
    Color_t bright = TColor::GetColorBright(vis->color);
    Color_t dark   = TColor::GetColorDark(vis->color);
    m_element->SetFillColor(bright);
    m_element->SetLineColor(dark);
    m_element->SetLineStyle(vis->lineStyle);
    m_element->SetLineWidth(10);
    m_element->SetVisibility(vis->visible ? kTRUE : kFALSE);
    m_element->SetVisDaughters(vis->showDaughters ? kTRUE : kFALSE);
  }
  data<Object>()->vis = attr;
}

/// Set Visualization attributes to the volume
void Volume::setVisAttributes(const LCDD& lcdd, const string& name)  const {
  if ( !name.empty() )   {
    VisAttr attr = lcdd.visAttributes(name);
    data<Object>()->vis = attr;
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
{  m_element->SetShape(solid);                              }

/// Set the regional attributes to the volume
void Volume::setRegion(const Region& obj)  const   
{  data<Object>()->region = obj;                            }

/// Set the limits to the volume
void Volume::setLimitSet(const LimitSet& obj)  const   
{  data<Object>()->limits = obj;                            }

/// Assign the sensitive detector structure
void Volume::setSensitiveDetector(const SensitiveDetector& obj) const  
{  data<Object>()->sens_det = obj;                          }

/// Access to the handle to the sensitive detector
Ref_t Volume::sensitiveDetector() const
{  return data<Object>()->sens_det;                         }

/// Accessor if volume is sensitive (ie. is attached to a sensitive detector)
bool Volume::isSensitive() const
{  return data<Object>()->sens_det.isValid();               }

/// Access to Solid (Shape)
Solid Volume::solid() const   
{  return Solid((*this)->GetShape());                       }

/// Access to the Volume material
Material Volume::material() const   
{  return Handle<TGeoMaterial>(m_element->GetMaterial());   }

/// Access the visualisation attributes
VisAttr Volume::visAttributes() const
{  return data<Object>()->vis;                              }

/// Access to the handle to the region structure
Region Volume::region() const   
{  return data<Object>()->region;                           }

/// Access to the limit set
LimitSet Volume::limitSet() const   
{  return data<Object>()->limits;                           }

/// Constructor to be used when creating a new geometry tree.
Assembly::Assembly(const std::string& name) {
  m_element = new Value<TGeoVolumeAssembly,Volume::Object>(name.c_str());
}

