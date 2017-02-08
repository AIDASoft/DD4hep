// $Id$
//==========================================================================
//  AIDA Detector description implementation for LCD
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
#include "DD4hep/LCDD.h"
#include "DD4hep/Printout.h"
#include "DD4hep/IDDescriptor.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/objects/ObjectsInterna.h"

#include "TMap.h"
#include "TROOT.h"
#include "TColor.h"
#include "TGeoMatrix.h"
#include "TGeoManager.h"
#include "TGeoElement.h"
#include "TGeoMaterial.h"

// C/C++ include files
#include <cmath>
#include <sstream>
#include <iomanip>

using namespace std;
using namespace DD4hep::Geometry;

/// Constructor to be used when creating a new DOM tree
Author::Author(LCDD& /* lcdd */) {
  m_element = new NamedObject("", "author");
}

/// Access the auhor's name
std::string Author::authorName() const {
  return m_element->GetName();
}

/// Set the author's name
void Author::setAuthorName(const std::string& nam) {
  m_element->SetName(nam.c_str());
}

/// Access the auhor's email address
std::string Author::authorEmail() const {
  return m_element->GetTitle();
}

/// Set the author's email address
void Author::setAuthorEmail(const std::string& addr) {
  m_element->SetTitle(addr.c_str());
}

/// Constructor to be used when creating a new DOM tree
Header::Header(const string& author_name, const string& descr_url) {
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
Constant::Constant(const string& nam, const string& val, const string& typ) {
  m_element = new Object(nam, val, typ);
}

/// Constructor to be used when creating a new DOM tree
Constant::Constant(const string& nam) {
  m_element = new Object(nam.c_str(), "", "number");
}

/// Access the constant
string Constant::dataType() const   {
  if ( isValid() )  {
    return m_element->dataType;
  }
  throw runtime_error("DD4hep: Attempt to access internals from invalid Constant handle!");
}

/// String representation of this object
string Constant::toString() const {
  stringstream os;
  os << m_element->GetName() << "  \"" << m_element->GetTitle() << "\"  ";
  if ( m_element->dataType == "string" ) os << "Value:" << m_element->GetTitle();
  else os << "Value:" << _toDouble(m_element->GetTitle());
  return os.str();
}

/// Constructor to be used when creating a new DOM tree
Atom::Atom(const string& nam, const string& formula, int Z, int N, double density) {
  TGeoElementTable* t = TGeoElement::GetElementTable();
  TGeoElement* e = t->FindElement(nam.c_str());
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
    TGeoMaterial* m = val->GetMaterial();
    if (m)
      return m->GetZ();
    throw runtime_error("DD4hep: The medium " + string(val->GetName()) + " has an invalid material reference!");
  }
  throw runtime_error("DD4hep: Attempt to access proton number from invalid material handle!");
}
/// atomic number of the underlying material
double  Material::A() const {
  Handle < TGeoMedium > val(*this);
  if (val.isValid()) {
    TGeoMaterial* m = val->GetMaterial();
    if (m)
      return m->GetA();
    throw runtime_error("DD4hep: The medium " + string(val->GetName()) + " has an invalid material reference!");
  }
  throw runtime_error("DD4hep: Attempt to access atomic number from invalid material handle!");
}

/// density of the underlying material
double  Material::density() const {
  Handle < TGeoMedium > val(*this);
  if (val.isValid()) {
    TGeoMaterial* m = val->GetMaterial();
    if (m)
      return m->GetDensity();
    throw runtime_error("DD4hep: The medium " + string(val->GetName()) + " has an invalid material reference!");
  }
  throw runtime_error("DD4hep: Attempt to access density from invalid material handle!");
}

/// Access the radiation length of the underlying material
double Material::radLength() const {
  Handle < TGeoMedium > val(*this);
  if (val.isValid()) {
    TGeoMaterial* m = val->GetMaterial();
    if (m)
      return m->GetRadLen();
    throw runtime_error("DD4hep: The medium " + string(val->GetName()) + " has an invalid material reference!");
  }
  throw runtime_error("DD4hep: Attempt to access radiation length from invalid material handle!");
}

/// Access the radiation length of the underlying material
double Material::intLength() const {
  Handle < TGeoMedium > val(*this);
  if (val.isValid()) {
    TGeoMaterial* m = val->GetMaterial();
    if (m)
      return m->GetIntLen();
    throw runtime_error("The medium " + string(val->GetName()) + " has an invalid material reference!");
  }
  throw runtime_error("Attempt to access interaction length from invalid material handle!");
}

/// String representation of this object
string Material::toString() const {
  Handle < TGeoMedium > val(*this);
  stringstream os;
  os << val->GetName() << " " << val->GetTitle() << " id:" << hex << val->GetId() << " Pointer:" << val->GetPointerName();
  return os.str();
}

/// Constructor to be used when creating a new entity
VisAttr::VisAttr(const string& nam) {
  Object* obj = new Object();
  assign(obj, nam, "vis");
  obj->color = 2;
  setLineStyle (SOLID);
  setDrawingStyle(SOLID);
  setShowDaughters(true);
  setAlpha(0.9f);
}

/// Constructor to be used when creating a new entity
VisAttr::VisAttr(const char* nam) {
  Object* obj = new Object();
  assign(obj, nam, "vis");
  obj->color = 2;
  setLineStyle (SOLID);
  setDrawingStyle(SOLID);
  setShowDaughters(true);
  setAlpha(0.9f);
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
  //NamedObject* obj = first_value<NamedObject>(*this);
  //obj->SetAlpha(value);
  return object<Object>().alpha;
}

/// Set alpha value
void VisAttr::setAlpha(float value) {
  object<Object>().alpha = value;
  //NamedObject* obj = first_value<NamedObject>(*this);
  //obj->SetAlpha(value);
}

/// Get object color
int VisAttr::color() const {
  return object<Object>().color;
}

/// Set object color
void VisAttr::setColor(float red, float green, float blue) {
  Object& o = object<Object>();
  o.color = TColor::GetColor(red, green, blue);
  o.col = gROOT->GetColor(o.color);
}

/// Get RGB values of the color (if valid)
bool VisAttr::rgb(float& red, float& green, float& blue) const {
  Object& o = object<Object>();
  if (o.col) {
    TColor* c = (TColor*) o.col;
    c->GetRGB(red, green, blue);
    return true;
  }
  return false;
}

/// String representation of this object
string VisAttr::toString() const {
  const VisAttr::Object* obj = &object<Object>();
  TColor* col = gROOT->GetColor(obj->color);
  char text[256];
  ::snprintf(text, sizeof(text), "%-20s RGB:%-8s [%d] %7.2f  Style:%d %d ShowDaughters:%3s Visible:%3s", ptr()->GetName(),
             col->AsHexString(), obj->color, col->GetAlpha(), int(obj->drawingStyle), int(obj->lineStyle),
             yes_no(obj->showDaughters), yes_no(obj->visible));
  return text;
}

/// Constructor to be used when creating a new aligment entry
AlignmentEntry::AlignmentEntry(const string& path) {
  TGeoPhysicalNode* obj = new TGeoPhysicalNode(path.c_str());
  assign(obj, path, "*");
}

/// Align the PhysicalNode (translation only)
int AlignmentEntry::align(const Position& pos, bool check, double overlap) {
  return align(pos, RotationZYX(), check, overlap);
}

/// Align the PhysicalNode (rotation only)
int AlignmentEntry::align(const RotationZYX& rot, bool check, double overlap) {
  return align(Position(), rot, check, overlap);
}

/// Align the PhysicalNode (translation + rotation)
int AlignmentEntry::align(const Position& pos, const RotationZYX& rot, bool check, double overlap) {

  if (isValid()) {
    TGeoHMatrix* new_matrix = dynamic_cast<TGeoHMatrix*>(m_element->GetOriginalMatrix()->MakeClone());
    TGeoRotation rotation("", rot.Phi() * RAD_2_DEGREE, rot.Theta() * RAD_2_DEGREE, rot.Psi() * RAD_2_DEGREE);
    TGeoCombiTrans m(pos.X(), pos.Y(), pos.Z(), 0);
    m.SetRotation(rotation);
    new_matrix->Multiply(&m);
    m_element->Align(new_matrix, 0, check, overlap);
    return 1;
  }
  throw runtime_error("DD4hep: Cannot align non existing physical node.");
}

/// Assignment operator
Limit& Limit::operator=(const Limit& c) {
  if (this != &c) {
    particles = c.particles;
    name = c.name;
    unit = c.unit;
    value = c.value;
    content = c.content;
  }
  return *this;
}

/// Equality operator
bool Limit::operator==(const Limit& c) const {
  return value == c.value && name == c.name && particles == c.particles;
}

/// operator less
bool Limit::operator<(const Limit& c) const {
  if (value < c.value)
    return true;
  if (name < c.name)
    return true;
  if (particles < c.particles)
    return true;
  return false;
}

/// Conversion to a string representation
string Limit::toString() const {
  string res = name + " = " + content;
  if (!unit.empty())
    res += unit + " ";
  res + " (" + particles + ")";
  return res;
}

/// Constructor to be used when creating a new DOM tree
LimitSet::LimitSet(const string& nam) {
  assign(new Object(), nam, "limitset");
}

/// Add new limit. Returns true if the new limit was added, false if it already existed.
bool LimitSet::addLimit(const Limit& limit) {
  pair<Object::iterator, bool> ret = data<Object>()->insert(limit);
  return ret.second;
}

/// Accessor to limits container
const set<Limit>& LimitSet::limits() const {
  const Object* o = data<Object>();
  return *o;
}

/// Constructor to be used when creating a new DOM tree
Region::Region(const string& nam) {
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
vector<string>& Region::limits() const {
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
  IDSpec(LCDD& doc, const std::string& name, const IDDescriptor& dsc);
  void addField(const std::string& name, const std::pair<int,int>& field);
};

IDSpec::IDSpec(LCDD& lcdd, const string& name, const IDDescriptor& dsc)
  : RefElement(doc,Tag_idspec,name)
{
  const IDDescriptor::FieldIDs& f = dsc.ids();
  const IDDescriptor::FieldMap& m = dsc.fields();
  object<Object>().Attr_length = dsc.maxBit();
  for(const auto& i : f )  {
    const string& nam = i.second;
    const pair<int,int>& fld = m.find(nam)->second;
    addField(nam,fld);
  }
}

void IDSpec::addField(const string& name, const pair<int,int>& field) {
  addField(Strng_t(name),field);
}

void IDSpec::addField(const string& name, const pair<int,int>& field) {
  Element e(document(),Tag_idfield);
  e.object<Object>().Attr_signed = field.second<0;
  e.object<Object>().Attr_label = name;
  e.object<Object>().Attr_start = field.first;
  e.object<Object>().Attr_length = abs(field.second);
  m_element.append(e);
}
#endif
