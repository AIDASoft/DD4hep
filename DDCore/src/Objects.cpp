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
#include <DD4hep/Detector.h>
#include <DD4hep/Printout.h>
#include <DD4hep/IDDescriptor.h>
#include <DD4hep/InstanceCount.h>
#include <DD4hep/detail/ObjectsInterna.h>

#include <TMap.h>
#include <TROOT.h>
#include <TColor.h>
#include <TGeoMatrix.h>
#include <TGeoManager.h>
#include <TGeoElement.h>
#include <TGeoMaterial.h>

// C/C++ include files
#include <cmath>
#include <sstream>

using namespace dd4hep;

/// Constructor to be used when creating a new DOM tree
Author::Author(Detector& /* description */) {
  m_element = new NamedObject("", "author");
}

/// Access the author's name
std::string Author::authorName() const {
  return m_element->GetName();
}

/// Set the author's name
void Author::setAuthorName(const std::string& nam) {
  m_element->SetName(nam.c_str());
}

/// Access the author's email address
std::string Author::authorEmail() const {
  return m_element->GetTitle();
}

/// Set the author's email address
void Author::setAuthorEmail(const std::string& addr) {
  m_element->SetTitle(addr.c_str());
}

/// Constructor to be used when creating a new DOM tree
Header::Header(const std::string& author_name, const std::string& descr_url) {
  Object* obj_ptr = new Object();
  assign(obj_ptr, author_name, descr_url);
}

/// Accessor to object name
const std::string Header::name() const {
  return m_element->GetName();
}

/// Accessor: set object name
void Header::setName(const std::string& new_name) {
  m_element->SetName(new_name.c_str());
}

/// Accessor to object title
const std::string Header::title() const {
  return m_element->GetTitle();
}

/// Accessor: set object title
void Header::setTitle(const std::string& new_title) {
  m_element->SetTitle(new_title.c_str());
}

/// Accessor to object url
const std::string& Header::url() const {
  return data<Object>()->url;
}

/// Accessor: set object url
void Header::setUrl(const std::string& new_url) {
  data<Object>()->url = new_url;
}

/// Accessor to object author
const std::string& Header::author() const {
  return data<Object>()->author;
}

/// Accessor: set object author
void Header::setAuthor(const std::string& new_author) {
  data<Object>()->author = new_author;
}

/// Accessor to object status
const std::string& Header::status() const {
  return data<Object>()->status;
}

/// Accessor: set object status
void Header::setStatus(const std::string& new_status) {
  data<Object>()->status = new_status;
}

/// Accessor to object version
const std::string& Header::version() const {
  return data<Object>()->version;
}

/// Accessor: set object version
void Header::setVersion(const std::string& new_version) {
  data<Object>()->version = new_version;
}

/// Accessor to object comment
const std::string& Header::comment() const {
  return data<Object>()->comment;
}

/// Accessor: set object comment
void Header::setComment(const std::string& new_comment) {
  data<Object>()->comment = new_comment;
}

/// Constructor to be used when creating a new DOM tree
Constant::Constant(const std::string& nam, const std::string& val, const std::string& typ) {
  m_element = new Object(nam, val, typ);
}

/// Constructor to be used when creating a new DOM tree
Constant::Constant(const std::string& nam) {
  m_element = new Object(nam.c_str(), "", "number");
}

/// Access the constant
std::string Constant::dataType() const   {
  if ( isValid() )  {
    return m_element->dataType;
  }
  throw std::runtime_error("dd4hep: Attempt to access internals from invalid Constant handle!");
}

/// String representation of this object
std::string Constant::toString() const {
  std::stringstream os;
  os << m_element->GetName() << "  \"" << m_element->GetTitle() << "\"  ";
  if ( m_element->dataType == "string" ) os << "Value:" << m_element->GetTitle();
  else os << "Value:" << _toDouble(m_element->GetTitle());
  return os.str();
}

/// Constructor to be used when creating a new DOM tree
Atom::Atom(const std::string& nam, const std::string& formula, int Z, int N, double density) {
  TGeoElementTable* t = TGeoElement::GetElementTable();
  TGeoElement*      e = t->FindElement(nam.c_str());
  if (!e) {
    t->AddElement(nam.c_str(), formula.c_str(), Z, N, density);
    e = t->FindElement(nam.c_str());
  }
  m_element = e;
}

/// proton number of the underlying material
double  Material::Z() const {
  Handle < TGeoMedium > val(*this);
  if (val.isValid()) {
    TGeoMaterial* mat = val->GetMaterial();
    if ( mat )
      return mat->GetZ();
    throw std::runtime_error("dd4hep: The medium " + std::string(val->GetName()) + " has an invalid material reference!");
  }
  throw std::runtime_error("dd4hep: Attempt to access proton number from invalid material handle!");
}

/// atomic number of the underlying material
double  Material::A() const {
  if ( isValid() ) {
    TGeoMaterial* mat = ptr()->GetMaterial();
    if ( mat )
      return mat->GetA();
    throw std::runtime_error("dd4hep: The medium " + std::string(ptr()->GetName()) + " has an invalid material reference!");
  }
  throw std::runtime_error("dd4hep: Attempt to access atomic number from invalid material handle!");
}

/// density of the underlying material
double  Material::density() const {
  if ( isValid() )  {
    TGeoMaterial* mat = ptr()->GetMaterial();
    if ( mat )
      return mat->GetDensity();
    throw std::runtime_error("dd4hep: The medium " + std::string(ptr()->GetName()) + " has an invalid material reference!");
  }
  throw std::runtime_error("dd4hep: Attempt to access density from invalid material handle!");
}

/// Access the radiation length of the underlying material
double Material::radLength() const {
  if ( isValid() ) {
    TGeoMaterial* mat = ptr()->GetMaterial();
    if ( mat )
      return mat->GetRadLen();
    throw std::runtime_error("dd4hep: The medium " + std::string(ptr()->GetName()) + " has an invalid material reference!");
  }
  throw std::runtime_error("dd4hep: Attempt to access radiation length from invalid material handle!");
}

/// Access the radiation length of the underlying material
double Material::intLength() const {
  if ( isValid() ) {
    TGeoMaterial* mat = ptr()->GetMaterial();
    if ( mat )
      return mat->GetIntLen();
    throw std::runtime_error("The medium " + std::string(ptr()->GetName()) + " has an invalid material reference!");
  }
  throw std::runtime_error("Attempt to access interaction length from invalid material handle!");
}

/// Access the fraction of an element within the material
double Material::fraction(Atom atom) const    {
  double frac = 0e0, tot = 0e0;
  TGeoElement*  elt = atom.access();
  TGeoMaterial* mat = access()->GetMaterial();
  for ( int i=0, n=mat->GetNelements(); i<n; ++i )  {
    TGeoElement* e = mat->GetElement(i);
    if ( mat->IsMixture() )  {
      TGeoMixture* mix = (TGeoMixture*)mat;
      tot  += mix->GetWmixt()[i];
    }
    else {
      tot = 1e0;
    }
    if ( e == elt )   {
      if ( mat->IsMixture() )  {
        TGeoMixture* mix = (TGeoMixture*)mat;
        frac += mix->GetWmixt()[i];
      }
      else  {
        frac = 1e0;
      }
    }
  }
  return tot>1e-20 ? frac/tot : 0.0;
}

/// Access to tabular properties of the optical surface
Material::Property Material::property(const char* nam)  const    {
  return access()->GetMaterial()->GetProperty(nam);
}

/// Access to tabular properties of the optical surface
Material::Property Material::property(const std::string& nam)  const   {
  return access()->GetMaterial()->GetProperty(nam.c_str());
}

/// Access string property value from the material table
std::string Material::propertyRef(const std::string& name, const std::string& default_value)    {
  auto* o = access()->GetMaterial();
  const char* p = o->GetPropertyRef(name.c_str());
  if ( p ) return p;
  return default_value;
}

/// Access to tabular properties of the optical surface
double Material::constProperty(const std::string& nam)  const   {
  Bool_t err = kFALSE;
  auto* o = access()->GetMaterial();
  double value = o->GetConstProperty(nam.c_str(), &err);
  if ( err != kTRUE ) return value;
  throw std::runtime_error("Attempt to access non existing material const property: "+nam);
}

/// Access string property value from the material table
std::string Material::constPropertyRef(const std::string& name, const std::string& default_value)    {
  auto* o = access()->GetMaterial();
  const char* p = o->GetConstPropertyRef(name.c_str());
  if ( p ) return p;
  return default_value;
}

/// String representation of this object
std::string Material::toString() const {
  if ( isValid() ) {
    TGeoMedium*  val = ptr();
    std::stringstream out;
    out << val->GetName() << " " << val->GetTitle()
        << " id:" << std::hex << val->GetId()
        << " Pointer:" << val->GetPointerName();
    return out.str();
  }
  throw std::runtime_error("Attempt to convert invalid material handle to string!");
}

/// Constructor to be used when creating a new entity
VisAttr::VisAttr(const std::string& nam) {
  Object* obj = new Object();
  assign(obj, nam, "vis");
  obj->color = gROOT->GetColor(kWhite);
  obj->alpha = 0.9f;
  setLineStyle (SOLID);
  setDrawingStyle(SOLID);
  setShowDaughters(true);
  setColor(1e0, 1e0, 1e0, 1e0);
}

/// Constructor to be used when creating a new entity
VisAttr::VisAttr(const char* nam) {
  Object* obj = new Object();
  assign(obj, nam, "vis");
  obj->color = gROOT->GetColor(kWhite);
  obj->alpha = 0.9f;
  setLineStyle (SOLID);
  setDrawingStyle(SOLID);
  setShowDaughters(true);
  setColor(1e0, 1e0, 1e0, 1e0);
}

/// Get Flag to show/hide daughter elements
bool VisAttr::showDaughters() const {
  return object<Object>().showDaughters;
}

/// Set Flag to show/hide daughter elements
void VisAttr::setShowDaughters(bool value) {
  object<Object>().showDaughters = value;
}

/// Get visibility flag
bool VisAttr::visible() const {
  return object<Object>().visible;
}

/// Set visibility flag
void VisAttr::setVisible(bool value) {
  object<Object>().visible = value;
}

/// Get line style
int VisAttr::lineStyle() const {
  return object<Object>().lineStyle;
}

/// Set line style
void VisAttr::setLineStyle(int value) {
  object<Object>().lineStyle = value;
}

/// Get drawing style
int VisAttr::drawingStyle() const {
  return object<Object>().drawingStyle;
}

/// Set drawing style
void VisAttr::setDrawingStyle(int value) {
  object<Object>().drawingStyle = value;
}

/// Get alpha value
float VisAttr::alpha() const {
  return object<Object>().alpha;
}

/// Get object color
int VisAttr::color() const {
  return object<Object>().color->GetNumber();
}

/// Set object color
void VisAttr::setColor(float alpha, float red, float green, float blue) {
  Object& o  = object<Object>();
  const auto num_before = gROOT->GetListOfColors()->GetLast();
  // Set tolerance high enough to always lookup from existing palette. This
  // helps to preserve colors when saving TGeo to .root files.
  TColor::SetColorThreshold(1.0f/31.0f);
  Int_t col  = TColor::GetColor(red, green, blue);
  const auto num_after = gROOT->GetListOfColors()->GetLast();
  if (num_before != num_after) {
    printout(INFO,"VisAttr","+++ %s Allocated a Color: r:%02X g:%02X b:%02X, this will not save to a ROOT file",
	     this->name(), int(red*255.), int(green*255.), int(blue*255));
  }
  o.alpha    = alpha;
  o.color    = gROOT->GetColor(col);
  if ( !o.color )    {
    except("VisAttr","+++ %s Failed to allocate Color: r:%02X g:%02X b:%02X",
	   this->name(), int(red*255.), int(green*255.), int(blue*255));
  }
  o.colortr = new TColor(gROOT->GetListOfColors()->GetLast()+1,
			 o.color->GetRed(), o.color->GetGreen(), o.color->GetBlue());
  o.colortr->SetAlpha(alpha);
}

/// Get RGB values of the color (if valid)
bool VisAttr::rgb(float& red, float& green, float& blue) const {
  Object& o = object<Object>();
  if ( o.color )  {
    o.color->GetRGB(red, green, blue);
    return true;
  }
  return false;
}

/// Get alpha and RGB values of the color (if valid)
bool VisAttr::argb(float& alpha, float& red, float& green, float& blue) const {
  Object& o = object<Object>();
  if ( o.color )  {
    alpha = o.alpha;
    o.color->GetRGB(red, green, blue);
    return true;
  }
  return false;
}

/// String representation of this object
std::string VisAttr::toString() const {
  const VisAttr::Object* obj = &object<Object>();
  TColor* c = obj->color;
  char text[256];
  std::snprintf(text, sizeof(text), "%-20s RGB:%-8s [%d] %7.2f  Style:%d %d ShowDaughters:%3s Visible:%3s", ptr()->GetName(),
                  c->AsHexString(), c->GetNumber(), c->GetAlpha(), int(obj->drawingStyle), int(obj->lineStyle),
                  yes_no(obj->showDaughters), yes_no(obj->visible));
  return text;
}

/// Equality operator
bool Limit::operator==(const Limit& c) const {
  return value == c.value && name == c.name && particles == c.particles;
}

/// operator less
bool Limit::operator<(const Limit& c) const {
  if (name < c.name)
    return true;
  if (value < c.value)
    return true;
  if (particles < c.particles)
    return true;
  return false;
}

/// Conversion to a string representation
std::string Limit::toString() const {
  std::string res = name + " = " + content;
  if (!unit.empty())
    res += unit + " ";
  res += " (" + particles + ")";
  return res;
}

/// Constructor to be used when creating a new DOM tree
LimitSet::LimitSet(const std::string& nam) {
  assign(new Object(), nam, "limitset");
}

/// Add new limit. Returns true if the new limit was added, false if it already existed.
bool LimitSet::addLimit(const Limit& limit) {
  std::pair<Object::iterator, bool> ret = data<Object>()->limits.insert(limit);
  return ret.second;
}

/// Accessor to limits container
const std::set<Limit>& LimitSet::limits() const {
  const Object* o = data<Object>();
  return o->limits;
}

/// Add new limit. Returns true if the new limit was added, false if it already existed.
bool LimitSet::addCut(const Limit& cut_obj)   {
  std::pair<Object::iterator, bool> ret = data<Object>()->cuts.insert(cut_obj);
  return ret.second;
}

/// Accessor to limits container
const std::set<Limit>& LimitSet::cuts() const    {
  return data<Object>()->cuts;
}

/// Constructor to be used when creating a new DOM tree
Region::Region(const std::string& nam) {
  Object* p = new Object();
  assign(p, nam, "region");
  p->magic = magic_word();
  p->store_secondaries = false;
  p->threshold = 10.0;
  p->cut = 10.0;
  p->use_default_cut = true;
  p->was_threshold_set = false;
}

Region& Region::setStoreSecondaries(bool value) {
  object<Object>().store_secondaries = value;
  return *this;
}

Region& Region::setThreshold(double value) {
  object<Object>().threshold = value;
  object<Object>().was_threshold_set = true;
  return *this;
}

Region& Region::setCut(double value) {
  object<Object>().cut = value;
  object<Object>().use_default_cut = false;
  return *this;
}

/// Access references to user limits
std::vector<std::string>& Region::limits() const {
  return object<Object>().user_limits;
}

/// Access cut value
double Region::cut() const {
  return object<Object>().cut;
}

/// Access production threshold
double Region::threshold() const {
  return object<Object>().threshold;
}

/// Access secondaries flag
bool Region::storeSecondaries() const {
  return object<Object>().store_secondaries;
}

bool Region::useDefaultCut() const {
  return object<Object>().use_default_cut;
}

bool Region::wasThresholdSet() const {
  return object<Object>().was_threshold_set;
}

#undef setAttr

#if 0

/** @class IDSpec Objects.h
 *
 *  @author  M.Frank
 *  @version 1.0
 */
struct IDSpec : public Ref_t {
  /// Constructor to be used when reading the already parsed DOM tree
  template <typename Q>
  IDSpec(const Handle<Q>& e) : Ref_t(e) {}
  /// Constructor to be used when creating a new DOM tree
  IDSpec(Detector& doc, const std::string& name, const IDDescriptor& dsc);
  void addField(const std::string& name, const std::pair<int,int>& field);
};

IDSpec::IDSpec(Detector& description, const std::string& name, const IDDescriptor& dsc)
  : RefElement(doc,Tag_idspec,name)
{
  const IDDescriptor::FieldIDs& f = dsc.ids();
  const IDDescriptor::FieldMap& m = dsc.fields();
  object<Object>().Attr_length = dsc.maxBit();
  for(const auto& i : f )  {
    const std::string& nam = i.second;
    const pair<int,int>& fld = m.find(nam)->second;
    addField(nam,fld);
  }
}

void IDSpec::addField(const std::string& name, const pair<int,int>& field) {
  addField(Strng_t(name),field);
}

void IDSpec::addField(const std::string& name, const pair<int,int>& field) {
  Element e(document(),Tag_idfield);
  e.object<Object>().Attr_signed = field.second<0;
  e.object<Object>().Attr_label = name;
  e.object<Object>().Attr_start = field.first;
  e.object<Object>().Attr_length = abs(field.second);
  m_element.append(e);
}
#endif
