//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

// Framework include files
#include "DD4hep/Detector.h"
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/MatrixHelpers.h"
#include "DD4hep/detail/ObjectsInterna.h"

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
using namespace dd4hep;
using namespace dd4hep::detail;

#ifdef DD4HEP_EMULATE_TGEOEXTENSIONS
namespace dd4hep {
  namespace detail {

    struct DDExtension  {
      TGeoExtension* m_extension;
      DDExtension() : m_extension(0) {}
      DDExtension(const DDExtension& c) : m_extension(0) {
        if ( c.m_extension ) m_extension = c.m_extension->Grab();
      }
      virtual ~DDExtension() {
        if ( m_extension ) m_extension->Release();
      }
      DDExtension& operator=(const DDExtension& c) {
        if ( this != &c ) SetUserExtension(c.GetUserExtension());
        return *this;
      }
      void SetUserExtension(TGeoExtension *ext)  {
        if (m_extension) m_extension->Release();
        m_extension = 0;
        if (ext) m_extension = ext->Grab();
      }
      TGeoExtension* GetUserExtension() const  {
        return m_extension;
      }
    };
    struct DD_TGeoNodeMatrix : public TGeoNodeMatrix, public DDExtension  {
    private:
      DD_TGeoNodeMatrix& operator=(const DD_TGeoNodeMatrix&) { return *this; }
    public:
      DD_TGeoNodeMatrix(const TGeoVolume* v, const TGeoMatrix* m)
        : TGeoNodeMatrix(v, m), DDExtension() {
        INCREMENT_COUNTER;
      }
      DD_TGeoNodeMatrix(const DD_TGeoNodeMatrix& c)
        : TGeoNodeMatrix(c.GetVolume(), c.GetMatrix()), DDExtension(c) {
        INCREMENT_COUNTER;
      }
      virtual ~DD_TGeoNodeMatrix() {
        DECREMENT_COUNTER;
      }
      virtual TGeoNode *MakeCopyNode() const {
        TGeoNodeMatrix *node = new DD_TGeoNodeMatrix(*this);
        node->SetName(this->TGeoNodeMatrix::GetName());
        // set the mother
        node->SetMotherVolume(fMother);
        // set the copy number
        node->SetNumber(fNumber);
        // copy overlaps
        if (fNovlp > 0) {
          if (fOverlaps) {
            Int_t *ovlps = new Int_t[fNovlp];
            memcpy(ovlps, fOverlaps, fNovlp * sizeof(Int_t));
            node->SetOverlaps(ovlps, fNovlp);
          }
          else {
            node->SetOverlaps(fOverlaps, fNovlp);
          }
        }
        // copy VC
        if (IsVirtual())
          node->SetVirtual();
        return node;
      }
    };

    template <class T> struct _VolWrap: public T, public DDExtension {
    public:
      _VolWrap(const char* name) : T(name), DDExtension() {
        INCREMENT_COUNTER;
      }
      virtual ~_VolWrap() {
        DECREMENT_COUNTER;
      }
      virtual void AddNode(const TGeoVolume *vol, Int_t copy_no, TGeoMatrix *mat, Option_t* = "") {
        TGeoMatrix *matrix = mat;
        if (matrix == 0)
          matrix = gGeoIdentity;
        else
          matrix->RegisterYourself();
        if (!vol) {
          this->T::Error("AddNode", "Volume is NULL");
          return;
        }
        if (!vol->IsValid()) {
          this->T::Error("AddNode", "Won't add node with invalid shape");
          printf("### invalid volume was : %s\n", vol->GetName());
          return;
        }
        if (!this->T::fNodes)
          this->T::fNodes = new TObjArray();

        if (this->T::fFinder) {
          // volume already divided.
          this->T::Error("AddNode", "Cannot add node %s_%i into divided volume %s", vol->GetName(), copy_no,
                         this->T::GetName());
          return;
        }

        TGeoNodeMatrix *node = new DD_TGeoNodeMatrix(vol, matrix);
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

    template <> _VolWrap<TGeoVolume>::_VolWrap(const char* name) : TGeoVolume(name,0,0), DDExtension() {
      INCREMENT_COUNTER;
    }

    struct TGeoVolumeValue : public _VolWrap<TGeoVolume> {
      TGeoVolumeValue(const char* name, TGeoShape* s, TGeoMedium* m) : _VolWrap<TGeoVolume>(name) {
        SetShape(s);
        SetMedium(m);
      }
      virtual ~TGeoVolumeValue() {      }
      TGeoVolume *_copyVol(TGeoShape *newshape) const {
        TGeoVolumeValue *vol = new TGeoVolumeValue(this->TGeoVolume::GetName(),newshape,fMedium);
        if ( m_extension ) vol->m_extension = m_extension->Grab();
        //vol->copy(*this);
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
        if (fFinder)
          vol->SetFinder(fFinder);
        CloneNodesAndConnect(vol);
        ((TObject*) vol)->SetBit(kVolumeClone);
        return vol;
      }
      virtual TGeoVolume* CloneVolume() const {
        TGeoVolume *vol = _copyVol(fShape);
        Int_t i;
        // copy volume attributes
        vol->SetLineColor(GetLineColor());
        vol->SetLineStyle(GetLineStyle());
        vol->SetLineWidth(GetLineWidth());
        vol->SetFillColor(GetFillColor());
        vol->SetFillStyle(GetFillStyle());
        // copy other attributes
        Int_t nbits = 8 * sizeof(UInt_t);
        for (i = 0; i < nbits; i++)
          vol->SetAttBit(1 << i, TGeoAtt::TestAttBit(1 << i));
        for (i = 14; i < 24; i++)
          vol->SetBit(1 << i, this->TGeoVolume::TestBit(1 << i));

        // copy field
        vol->SetField(fField);
        // Set bits
        for (i = 0; i < nbits; i++)
          vol->SetBit(1 << i, this->TGeoVolume::TestBit(1 << i));
        vol->SetBit(kVolumeClone);
        // copy nodes
        //   CloneNodesAndConnect(vol);
        vol->MakeCopyNodes(this);
        // if volume is divided, copy finder
        vol->SetFinder(fFinder);
        // copy voxels
        if (fVoxels) {
          TGeoVoxelFinder *voxels = new TGeoVoxelFinder(vol);
          vol->SetVoxelFinder(voxels);
        }
        // copy option, uid
        vol->SetOption(fOption);
        vol->SetNumber(fNumber);
        vol->SetNtotal(fNtotal);
        return vol;
      }
    };

    struct TGeoVolumeAssemblyValue : public _VolWrap<TGeoVolumeAssembly>  {
      TGeoVolumeAssemblyValue(const char* name) : _VolWrap<TGeoVolumeAssembly>(name) {
      }
      virtual ~TGeoVolumeAssemblyValue() {
      }
      TGeoVolume *CloneVolume() const {
        TGeoVolumeAssemblyValue *vol = new TGeoVolumeAssemblyValue(this->TGeoVolume::GetName());
        if ( m_extension ) vol->m_extension = m_extension->Grab();
        Int_t i;
        // copy other attributes
        Int_t nbits = 8 * sizeof(UInt_t);
        for (i = 0; i < nbits; i++)
          vol->SetAttBit(1 << i, TGeoAtt::TestAttBit(1 << i));
        for (i = 14; i < 24; i++)
          vol->SetBit(1 << i, this->TGeoVolumeAssembly::TestBit(1 << i));

        // copy field
        vol->SetField(fField);
        // Set bits
        for (i = 0; i < nbits; i++)
          vol->SetBit(1 << i, this->TGeoVolumeAssembly::TestBit(1 << i));
        vol->SetBit(kVolumeClone);
        // make copy nodes
        vol->MakeCopyNodes(this);
        ((TGeoShapeAssembly*) vol->GetShape())->NeedsBBoxRecompute();
        // copy voxels
        if (fVoxels) {
          TGeoVoxelFinder *voxels = new TGeoVoxelFinder(vol);
          vol->SetVoxelFinder(voxels);
        }
        // copy option, uid
        vol->SetOption(fOption);
        vol->SetNumber(fNumber);
        vol->SetNtotal(fNtotal);
        return vol;
      }
    };

  }
}
typedef DD_TGeoNodeMatrix       geo_node_t;
typedef TGeoVolumeValue         geo_volume_t;
typedef TGeoVolumeAssemblyValue geo_assembly_t;

template <typename T> static typename T::Object* _userExtension(const T& v)  {
  typedef typename T::Object O;
  const DDExtension* p = dynamic_cast<const DDExtension*>(v.ptr());
  O* o = (O*)(p ? p->GetUserExtension() : 0);
  return o;
}
#else

/*
 *  The section below uses the new ROOT features using user extensions to volumes
 *  and placements. Once this is common, the above mechanism should be thrown away....
 *
 *  M.Frank
 */

typedef TGeoNode                geo_node_t;
typedef TGeoVolume              geo_volume_t;
typedef TGeoVolumeAssembly      geo_assembly_t;
template <typename T> static typename T::Object* _userExtension(const T& v)  {
  typedef typename T::Object O;
  O* o = (O*)(v.ptr()->GetUserExtension());
  return o;
}
#endif
ClassImp(PlacedVolumeExtension)

static TGeoVolume* _createTGeoVolume(const string& name, TGeoShape* s, TGeoMedium* m)  {
  geo_volume_t* e = new geo_volume_t(name.c_str(),s,m);
  e->SetUserExtension(new Volume::Object());
  return e;
}
static TGeoVolume* _createTGeoVolumeAssembly(const string& name)  {
  geo_assembly_t* e = new geo_assembly_t(name.c_str()); // It is important to use the correct constructor!!
  e->SetUserExtension(new Assembly::Object());
  return e;
}

/// Default constructor
PlacedVolumeExtension::PlacedVolumeExtension()
  : TGeoExtension(), magic(0), refCount(0), volIDs() {
  magic = magic_word();
  INCREMENT_COUNTER;
}

/// Copy constructor
PlacedVolumeExtension::PlacedVolumeExtension(const PlacedVolumeExtension& c)
  : TGeoExtension(), magic(c.magic), refCount(0), volIDs(c.volIDs) {
  INCREMENT_COUNTER;
}

/// Default destructor
PlacedVolumeExtension::~PlacedVolumeExtension() {
  DECREMENT_COUNTER;
}

/// TGeoExtension overload: Method called whenever requiring a pointer to the extension
TGeoExtension* PlacedVolumeExtension::Grab()   {
  ++this->refCount;
#ifdef ___print_vols
  else  cout << "Placement grabbed....." << endl;
#endif
  return this;
}

/// TGeoExtension overload: Method called always when the pointer to the extension is not needed anymore
void PlacedVolumeExtension::Release() const  {
  PlacedVolumeExtension* ext = const_cast<PlacedVolumeExtension*>(this);
  --ext->refCount;
  if ( 0 == ext->refCount ) delete ext;
}

/// Lookup volume ID
vector<PlacedVolumeExtension::VolID>::const_iterator
PlacedVolumeExtension::VolIDs::find(const string& name) const {
  for (Base::const_iterator i = this->Base::begin(); i != this->Base::end(); ++i)
    if (name == (*i).first)
      return i;
  return this->end();
}

/// Insert a new value into the volume ID container
std::pair<vector<PlacedVolumeExtension::VolID>::iterator, bool>
PlacedVolumeExtension::VolIDs::insert(const string& name, int value) {
  Base::iterator i = this->Base::begin();
  for (; i != this->Base::end(); ++i)
    if (name == (*i).first)
      break;
  //
  if (i != this->Base::end()) {
    return make_pair(i, false);
  }
  i = this->Base::insert(this->Base::end(), make_pair(name, value));
  return make_pair(i, true);
}

/// String representation for debugging
string PlacedVolumeExtension::VolIDs::str()  const   {
  stringstream str;
  str << hex;
  for(const auto& i : *this )   {
    str << i.first << "=" << setw(4) << right
        << setfill('0') << i.second << setfill(' ') << " ";
  }
  return str.str();
}

static PlacedVolume::Object* _data(const PlacedVolume& v) {
  PlacedVolume::Object* o = _userExtension(v);
  if (o) return o;
  throw runtime_error("dd4hep: Attempt to access invalid handle of type: PlacedVolume");
}

/// Check if placement is properly instrumented
PlacedVolume::Object* PlacedVolume::data() const   {
  PlacedVolume::Object* o = _userExtension(*this);
  return o;
}

/// Add identifier
PlacedVolume& PlacedVolume::addPhysVolID(const string& nam, int value) {
  Object* obj = _data(*this);
  obj->volIDs.push_back(make_pair(nam, value));
  return *this;
}

/// Volume material
Material PlacedVolume::material() const {
  return Material(m_element ? m_element->GetMedium() : 0);
}

/// Logical volume of this placement
Volume PlacedVolume::volume() const {
  return Volume(m_element ? m_element->GetVolume() : 0);
}

/// Parent volume (envelope)
Volume PlacedVolume::motherVol() const {
  return Volume(m_element ? m_element->GetMotherVolume() : 0);
}

/// Access to the volume IDs
const PlacedVolume::VolIDs& PlacedVolume::volIDs() const {
  return _data(*this)->volIDs;
}

/// String dump
string PlacedVolume::toString() const {
  stringstream s;
  Object* obj = _data(*this);
  s << m_element->GetName() << ":  vol='" << m_element->GetVolume()->GetName() << "' mat:'" << m_element->GetMatrix()->GetName()
    << "' volID[" << obj->volIDs.size() << "] ";
  for (VolIDs::const_iterator i = obj->volIDs.begin(); i != obj->volIDs.end(); ++i)
    s << (*i).first << "=" << (*i).second << "  ";
  s << ends;
  return s.str();
}

/// Enable ROOT persistency
ClassImp(VolumeExtension)

/// Default constructor
VolumeExtension::VolumeExtension()
: TGeoExtension(), magic(0), refCount(0), region(), limits(), vis(), sens_det(), referenced(0) {
  INCREMENT_COUNTER;
}

/// Default destructor
VolumeExtension::~VolumeExtension() {
  region.clear();
  limits.clear();
  vis.clear();
  sens_det.clear();
  DECREMENT_COUNTER;
}

/// TGeoExtension overload: Method called whenever requiring a pointer to the extension
TGeoExtension* VolumeExtension::Grab()  {
  VolumeExtension* ext = const_cast<VolumeExtension*>(this);
  ++ext->refCount;
#ifdef ___print_vols
  if ( ext->sens_det.isValid() )
    cout << "Volume grabbed with valid sensitive detector....." << endl;
  else
    cout << "Volume grabbed....." << endl;
#endif
  return ext;
}

/// TGeoExtension overload: Method called always when the pointer to the extension is not needed anymore
void VolumeExtension::Release() const  {
  VolumeExtension* ext = const_cast<VolumeExtension*>(this);
  --ext->refCount;
  if ( 0 == ext->refCount )   {
#ifdef ___print_vols
    cout << "Volume deleted....." << endl;
#endif
    delete ext;
  }
  else  {
#ifdef ___print_vols
    cout << "VolumeExtension::Release::refCount:" << ext->refCount << endl;
#endif
  }
}

/// Accessor to the data part of the Volume
Volume::Object* _data(const Volume& v, bool throw_exception = true) {
  //if ( v.ptr() && v.ptr()->IsA() == TGeoVolume::Class() ) return v.data<Volume::Object>();
  Volume::Object* o = _userExtension(v);
  if (o)
    return o;
  else if (!throw_exception)
    return 0;
  throw runtime_error("dd4hep: Attempt to access invalid handle of type: PlacedVolume");
}

/// Constructor to be used when creating a new geometry tree.
Volume::Volume(const string& nam) {
  m_element = _createTGeoVolume(nam,0,0);
}

/// Constructor to be used when creating a new geometry tree. Also sets materuial and solid attributes
Volume::Volume(const string& nam, const Solid& s, const Material& m) {
  m_element = _createTGeoVolume(nam,s.ptr(),m.ptr());
}

/// Check if placement is properly instrumented
Volume::Object* Volume::data() const   {
  Volume::Object* o = _userExtension(*this);
  return o;
}

static PlacedVolume _addNode(TGeoVolume* par, TGeoVolume* daughter, TGeoMatrix* transform) {
  if ( !daughter )   {
    throw runtime_error("dd4hep: Volume: Attempt to assign an invalid physical daughter volume.");
  }
  TGeoVolume* parent = par;
  TObjArray* a = parent->GetNodes();
  Int_t id = a ? a->GetEntries() : 0;
  if (transform && transform != detail::matrix::_identity()) {
    string nam = string(daughter->GetName()) + "_placement";
    transform->SetName(nam.c_str());
  }
  TGeoShape* shape = daughter->GetShape();
  // Need to fix the daughter's BBox of assemblies, if the BBox was not calculated....
  if ( shape->IsA() == TGeoShapeAssembly::Class() )  {
    TGeoShapeAssembly* as = (TGeoShapeAssembly*)shape;
    if ( std::fabs(as->GetDX()) < numeric_limits<double>::epsilon() &&
         std::fabs(as->GetDY()) < numeric_limits<double>::epsilon() &&
         std::fabs(as->GetDZ()) < numeric_limits<double>::epsilon() )  {
      as->NeedsBBoxRecompute();
      as->ComputeBBox();
    }
  }
  parent->AddNode(daughter, id, transform);
  geo_node_t* n = static_cast<geo_node_t*>(parent->GetNode(id));
  n->geo_node_t::SetUserExtension(new PlacedVolume::Object());
  return PlacedVolume(n);
}

/// Place daughter volume according to generic Transform3D
PlacedVolume Volume::placeVolume(const Volume& volume, const Transform3D& trans) const {
  return _addNode(m_element, volume, detail::matrix::_transform(trans));
}

/// Place daughter volume. The position and rotation are the identity
PlacedVolume Volume::placeVolume(const Volume& volume) const {
  return _addNode(m_element, volume, detail::matrix::_identity());
}

/// Place un-rotated daughter volume at the given position.
PlacedVolume Volume::placeVolume(const Volume& volume, const Position& pos) const {
  return _addNode(m_element, volume, detail::matrix::_translation(pos));
}

/// Place rotated daughter volume. The position is automatically the identity position
PlacedVolume Volume::placeVolume(const Volume& volume, const RotationZYX& rot) const {
  return _addNode(m_element, volume, detail::matrix::_rotationZYX(rot));
}

/// Place rotated daughter volume. The position is automatically the identity position
PlacedVolume Volume::placeVolume(const Volume& volume, const Rotation3D& rot) const {
  return _addNode(m_element, volume, detail::matrix::_rotation3D(rot));
}

/// Set the volume's material
const Volume& Volume::setMaterial(const Material& m) const {
  if (m.isValid()) {
    TGeoMedium* medium = m._ptr<TGeoMedium>();
    if (medium) {
      m_element->SetMedium(medium);
      return *this;
    }
    throw runtime_error("dd4hep: Volume: Medium " + string(m.name()) + " is not registered with geometry manager.");
  }
  throw runtime_error("dd4hep: Volume: Attempt to assign invalid material.");
}

/// Access to the Volume material
Material Volume::material() const {
  return Ref_t(m_element->GetMedium());
}

#include "TROOT.h"

/// Set Visualization attributes to the volume
const Volume& Volume::setVisAttributes(const VisAttr& attr) const {
  if (attr.isValid()) {
    VisAttr::Object* vis = attr.data<VisAttr::Object>();
    Color_t bright = vis->color;//kBlue;//TColor::GetColorBright(vis->color);
    Color_t dark = vis->color;//kRed;//TColor::GetColorDark(vis->color);
    TColor* c = vis->col;//gROOT->GetColor(dark);
    int draw_style = vis->drawingStyle;
    int line_style = vis->lineStyle;

    m_element->SetVisibility(vis->visible ? kTRUE : kFALSE);
    m_element->SetVisContainers(kTRUE);
    //m_element->SetAttBit(TGeoAtt::kVisContainers, kTRUE);
    m_element->SetVisDaughters(vis->showDaughters ? kTRUE : kFALSE);
    printout(DEBUG,"setVisAttributes",
             "Set color %3d [%02X,%02X,%02X] DrawingStyle:%9s LineStyle:%6s for volume %s",
             int(vis->color),
             c ? int(255*c->GetRed()) : 0xFF,
             c ? int(255*c->GetGreen()) : 0xFF,
             c ? int(255*c->GetBlue()) : 0xFF,
             draw_style == VisAttr::SOLID ? "Solid" : "Wireframe",
             line_style == VisAttr::SOLID ? "Solid" : "Dashed",
             name()
             );
    m_element->SetLineWidth(10);
    m_element->SetLineColor(dark);
    if (draw_style == VisAttr::SOLID) {
      m_element->SetLineColor(bright);
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,34,25)
      m_element->SetFillColorAlpha(bright,vis->alpha);
#else
      m_element->SetFillColor(bright);
#endif
      m_element->SetFillStyle(1001);   // Root: solid
      // Suggested by Nikiforos. Not optimal.
      //m_element->GetMedium()->GetMaterial()->SetTransparency((1-vis->alpha)*100);

#if ROOT_VERSION_CODE >= ROOT_VERSION(6,0,0)
      // As suggested by Valentin Volkl https://sft.its.cern.ch/jira/browse/DDFORHEP-20
      //
      // According to https://root.cern.ch/phpBB3/viewtopic.php?t=2309#p66013
      // a transparency>50 will make a volume invisible in the normal pad.
      // Hence: possibly restrict transparency to a maximum of 50.
      //        but let's see first how this behaves.
      m_element->SetTransparency((1-vis->alpha)*100);
#endif
    }
    else {
      printout(DEBUG,"setVisAttributes","Set to wireframe vis:%s",name());
      m_element->SetLineColor(kBlack);
      m_element->SetFillColor(0);
      m_element->SetFillStyle(0);      // Root: hollow
    }
    if (line_style == VisAttr::SOLID)  // Root line style: 1=solid, 2=dash, 3=dot, 4=dash-dot.
      m_element->SetLineStyle(1);
    else if (line_style == VisAttr::DASHED)
      m_element->SetLineStyle(2);
    else
      m_element->SetLineStyle(line_style);


    /*
      m_element->SetVisibility(kTRUE);
      m_element->SetAttBit(TGeoAtt::kVisContainers, kTRUE);
      m_element->SetVisDaughters(kTRUE);
      printout(INFO,"setVisAttributes","Set Line color for volume %s",name());
      m_element->SetLineColor(bright);
      m_element->SetFillColor(bright);
      m_element->SetFillStyle(1001);   // Root: solid
      if (line_style == VisAttr::SOLID)
      m_element->SetFillStyle(1);
      else if (line_style == VisAttr::DASHED)
      m_element->SetFillStyle(2);
      else
      m_element->SetFillStyle(line_style);
      m_element->SetLineWidth(10);
    */
  }
  Volume::Object* o = _userExtension(*this);
  if ( o ) o->vis = attr;
  return *this;
}

/// Set Visualization attributes to the volume
const Volume& Volume::setVisAttributes(const Detector& description, const string& nam) const {
  if (!nam.empty()) {
    VisAttr attr = description.visAttributes(nam);
    setVisAttributes(attr);
  }
  else {
    /*
      string tag = this->name();
      if ( ::strstr(tag.c_str(),"_slice") )       // Slices turned off by default
      setVisAttributes(description.visAttributes("InvisibleNoDaughters"));
      else if ( ::strstr(tag.c_str(),"_layer") )  // Layers turned off, but daughters possibly visible
      setVisAttributes(description.visAttributes("InvisibleWithDaughters"));
      else if ( ::strstr(tag.c_str(),"_module") ) // Tracker modules similar to layers
      setVisAttributes(description.visAttributes("InvisibleWithDaughters"));
      else if ( ::strstr(tag.c_str(),"_module_component") ) // Tracker modules similar to layers
      setVisAttributes(description.visAttributes("InvisibleNoDaughters"));
    */
  }
  return *this;
}

/// Attach attributes to the volume
const Volume& Volume::setAttributes(const Detector& description, const string& rg, const string& ls, const string& vis) const {
  if (!rg.empty())
    setRegion(description.region(rg));
  if (!ls.empty())
    setLimitSet(description.limitSet(ls));
  setVisAttributes(description, vis);
  return *this;
}

/// Access the visualisation attributes
VisAttr Volume::visAttributes() const {
  Object* o = _data(*this, false);
  if (o)
    return o->vis;
  return VisAttr();
}

/// Set the volume's solid shape
const Volume& Volume::setSolid(const Solid& s) const {
  m_element->SetShape(s);
  return *this;
}

/// Access to Solid (Shape)
Solid Volume::solid() const {
  return Solid((*this)->GetShape());
}

/// Set the regional attributes to the volume
const Volume& Volume::setRegion(const Detector& description, const string& nam) const {
  if (!nam.empty()) {
    return setRegion(description.region(nam));
  }
  return *this;
}

/// Set the regional attributes to the volume
const Volume& Volume::setRegion(const Region& obj) const {
  _data(*this)->region = obj;
  return *this;
}

/// Access to the handle to the region structure
Region Volume::region() const {
  return _data(*this)->region;
}

/// Set the limits to the volume
const Volume& Volume::setLimitSet(const Detector& description, const string& nam) const {
  if (!nam.empty()) {
    return setLimitSet(description.limitSet(nam));
  }
  return *this;
}

/// Set the limits to the volume
const Volume& Volume::setLimitSet(const LimitSet& obj) const {
  _data(*this)->limits = obj;
  return *this;
}

/// Access to the limit set
LimitSet Volume::limitSet() const {
  return _data(*this)->limits;
}

/// Assign the sensitive detector structure
const Volume& Volume::setSensitiveDetector(const SensitiveDetector& obj) const {
  //cout << "Setting sensitive detector '" << obj.name() << "' to volume:" << ptr() << " " << name() << endl;
  _data(*this)->sens_det = obj;
  return *this;
}

/// Access to the handle to the sensitive detector
Ref_t Volume::sensitiveDetector() const {
  const Object* o = _data(*this);
  return o->sens_det;
}

/// Accessor if volume is sensitive (ie. is attached to a sensitive detector)
bool Volume::isSensitive() const {
  return _data(*this)->sens_det.isValid();
}

/// Constructor to be used when creating a new geometry tree.
Assembly::Assembly(const string& nam) {
  m_element = _createTGeoVolumeAssembly(nam);
}

