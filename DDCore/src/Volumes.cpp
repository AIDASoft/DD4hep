// $Id$
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
  template <> _VolWrap<TGeoVolumeAssembly>::_VolWrap(const char* name, TGeoShape* /* s */, TGeoMedium* /* m */)
  : TGeoVolumeAssembly(name) {}
  
  template <> struct Value<TGeoVolume,Volume::Object>
  : public _VolWrap<TGeoVolume>, public Volume::Object  {
    Value(const char* name, TGeoShape* s=0, TGeoMedium* m=0) 
      : _VolWrap<TGeoVolume>(name,s,m) {magic = magic_word();}
    virtual ~Value() {}
    TGeoVolume *_copyVol(TGeoShape *newshape) const {
      typedef Value<TGeoVolume,Volume::Object> _Vol;
      _Vol *vol = new _Vol(GetName(), newshape, fMedium);
      vol->copy(*this);
      return vol;
    }
    virtual TGeoVolume* MakeCopyVolume(TGeoShape *newshape) {
      // make a copy of this volume. build a volume with same name, shape and medium
      TGeoVolume *vol = _copyVol(newshape);
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
      TGeoVolume *vol = _copyVol(fShape);
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
PlacedVolume& PlacedVolume::addPhysVolID(const string& name, int value)   {
  Object* obj = data<Object>();
  obj->volIDs[name] = value;
  return *this;
}

/// Volume material
Material PlacedVolume::material() const 
{  return Material::handle_t(m_element ? m_element->GetMedium() : 0);  }

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

Volume::Object* _data(const Volume& v) {
  if ( v.ptr() && v.ptr()->IsA() == TGeoVolume::Class() ) return v.data<Volume::Object>();
  return dynamic_cast<Volume::Object*>(v.ptr());
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
  if ( transform && transform != identityTransform() ) {
    string nam = string(daughter->GetName())+"_placement";
    transform->SetName(nam.c_str());
  }
  parent->AddNode(daughter,id,transform);
  TGeoNodeMatrix* n = dynamic_cast<TGeoNodeMatrix*>(parent->GetNode(id));
  return PlacedVolume(n);
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
  _data(*this)->vis = attr;
}

/// Set Visualization attributes to the volume
void Volume::setVisAttributes(const LCDD& lcdd, const string& name)  const {
  if ( !name.empty() )   {
    VisAttr attr = lcdd.visAttributes(name);
    _data(*this)->vis = attr;
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
{  _data(*this)->region = obj;                            }

/// Set the limits to the volume
void Volume::setLimitSet(const LimitSet& obj)  const   
{  _data(*this)->limits = obj;                            }

/// Assign the sensitive detector structure
void Volume::setSensitiveDetector(const SensitiveDetector& obj) const   {
  //cout << "Setting sensitive detector '" << obj.name() << "' to volume:" << ptr() << " " << name() << endl;
  _data(*this)->sens_det = obj;                          
}

/// Access to the handle to the sensitive detector
Ref_t Volume::sensitiveDetector() const
{  return _data(*this)->sens_det;                         }

/// Accessor if volume is sensitive (ie. is attached to a sensitive detector)
bool Volume::isSensitive() const
{  return _data(*this)->sens_det.isValid();               }

/// Access to Solid (Shape)
Solid Volume::solid() const   
{  return Solid((*this)->GetShape());                       }

/// Access to the Volume material
Material Volume::material() const   
{  return Ref_t(m_element->GetMedium());   }

/// Access the visualisation attributes
VisAttr Volume::visAttributes() const
{  return _data(*this)->vis;                              }

/// Access to the handle to the region structure
Region Volume::region() const   
{  return _data(*this)->region;                           }

/// Access to the limit set
LimitSet Volume::limitSet() const   
{  return _data(*this)->limits;                           }

/// Constructor to be used when creating a new geometry tree.
Assembly::Assembly(const string& name) {
  m_element = new Value<TGeoVolumeAssembly,Volume::Object>(name.c_str());
}

