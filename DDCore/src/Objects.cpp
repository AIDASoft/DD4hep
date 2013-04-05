// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DD4hep/LCDD.h"
#include "DD4hep/IDDescriptor.h"

#include "TMap.h"
#include "TROOT.h"
#include "TColor.h"
#include "TGeoMatrix.h"
#include "TGeoManager.h"
#include "TGeoElement.h"
#include "TGeoMaterial.h"

#include <cmath>
#include <sstream>
#include <iomanip>

using namespace std;
using namespace DD4hep::Geometry;

/// Constructor to be used when creating a new DOM tree
Author::Author(LCDD& /* lcdd */)  {
  m_element = new TNamed("","author");
}

/// Constructor to be used when creating a new DOM tree
Header::Header(const string& author, const string& url)   {
  Value<TNamed,Object>* ptr = new Value<TNamed,Object>();
  assign(ptr,author, url);
}

/// Accessor to object name
const std::string Header::name()  const {
  return m_element->GetName();
}

/// Accessor: set object name
void Header::setName(const std::string& new_name) {
  m_element->SetName(new_name.c_str());
}

/// Accessor to object title
const std::string Header::title()  const {
  return m_element->GetTitle();
}

/// Accessor: set object title
void Header::setTitle(const std::string& new_title) {
  m_element->SetTitle(new_title.c_str());
}

/// Accessor to object url
const std::string& Header::url()  const {
  return data<Object>()->url;
}

/// Accessor: set object url
void Header::setUrl(const std::string& new_url) {
  data<Object>()->url = new_url;
}

/// Accessor to object author
const std::string& Header::author()  const {
  return data<Object>()->author;
}

/// Accessor: set object author
void Header::setAuthor(const std::string& new_author) {
  data<Object>()->author = new_author;
}

/// Accessor to object status
const std::string& Header::status()  const {
  return data<Object>()->status;
}

/// Accessor: set object status
void Header::setStatus(const std::string& new_status) {
  data<Object>()->status = new_status;
}

/// Accessor to object version
const std::string& Header::version()  const {
  return data<Object>()->version;
}

/// Accessor: set object version
void Header::setVersion(const std::string& new_version) {
  data<Object>()->version = new_version;
}

/// Accessor to object comment
const std::string& Header::comment()  const {
  return data<Object>()->comment;
}

/// Accessor: set object comment
void Header::setComment(const std::string& new_comment) {
  data<Object>()->comment = new_comment;
}

/// Constructor to be used when creating a new DOM tree
Constant::Constant(const string& nam, const string& val)
{
  m_element = new TNamed(nam.c_str(),val.c_str());
}

/// Constructor to be used when creating a new DOM tree
Constant::Constant(const string& name)   {
  m_element = new TNamed(name.c_str(),"");
}

/// String representation of this object
string Constant::toString()  const {
  stringstream os;
  os << m_element->GetName() << "  \"" << m_element->GetTitle() 
     << "\"  Value:" << _toDouble(m_element->GetTitle());
  return os.str();
}

/// Constructor to be used when creating a new DOM tree
Atom::Atom(const string& name, const string& formula, int Z, int N, double density) {
  TGeoElementTable* t = TGeoElement::GetElementTable();
  TGeoElement* e = t->FindElement(name.c_str());
  if ( !e ) {
    t->AddElement(name.c_str(), formula.c_str(), Z, N, density);
    e = t->FindElement(name.c_str());
  }
  m_element = e;
}

/// Constructor to be used when creating a new DOM tree
Material::Material(const string& name)   {
  //TGeoMaterial* mat = gGeoManager->GetMaterial(name.c_str());
  TGeoMedium* mat = gGeoManager->GetMedium(name.c_str());
  m_element = mat;
}

/// Access the radiation length of the undrelying material
double Material::radLength() const {
  Handle<TGeoMedium>  val(*this);
  if ( val.isValid() ) {
    TGeoMaterial* m = val->GetMaterial();
    if ( m ) return m->GetRadLen();
    throw runtime_error("The medium "+string(val->GetName())+" has an invalid material reference!");
  }
  throw runtime_error("Attempt to access radiation length from invalid material handle!");
}

/// String representation of this object
string Material::toString()  const {
  Handle<TGeoMedium>  val(*this);
  stringstream os;
  os << val->GetName() << " " << val->GetTitle() << " id:" << hex << val->GetId() 
     << " Pointer:" << val->GetPointerName();
  return os.str();
}

/// Constructor to be used when creating a new DOM tree
VisAttr::VisAttr(const string& name)    {
  Value<TNamed,Object>* obj = new Value<TNamed,Object>();
  assign(obj, name, "vis");
  obj->color  = 2;
  setLineStyle(SOLID);
  setDrawingStyle(SOLID);
  setShowDaughters(true);
  setAlpha(0.1f);
}

/// Get Flag to show/hide daughter elements
bool VisAttr::showDaughters() const  {
  return _data().showDaughters;
}

/// Set Flag to show/hide daughter elements
void VisAttr::setShowDaughters(bool value)   {
  _data().showDaughters = value;
}

/// Get visibility flag
bool VisAttr::visible() const   {
  return _data().visible;
}

/// Set visibility flag
void VisAttr::setVisible(bool value)   {
  _data().visible = value;
}

/// Get line style
int VisAttr::lineStyle()  const {
  return _data().lineStyle;
}

/// Set line style
void VisAttr::setLineStyle(int value)  {
  _data().lineStyle = value;
}

/// Get drawing style
int VisAttr::drawingStyle()  const {
  return _data().drawingStyle;
}

/// Set drawing style
void VisAttr::setDrawingStyle(int value)   {
  _data().drawingStyle = value;
}

/// Get alpha value
float VisAttr::alpha() const  {
  //TNamed* obj = first_value<TNamed>(*this);
  //obj->SetAlpha(value);
  return _data().alpha;
}

/// Set alpha value
void VisAttr::setAlpha(float value)   {
  _data().alpha = value;
  //TNamed* obj = first_value<TNamed>(*this);
  //obj->SetAlpha(value);
}

/// Get object color
int VisAttr::color()   const  {
  return _data().color;
}

/// Set object color
void VisAttr::setColor(float red, float green, float blue)   {
  Object& o = _data();
  o.color = TColor::GetColor(red,green,blue);
  o.col   = gROOT->GetColor(o.color);
}

/// Get RGB values of the color (if valid)
bool VisAttr::rgb(float& red, float& green, float& blue) const {
  Object& o = _data();
  if ( o.col ) {
    TColor* c = (TColor*)o.col;
    c->GetRGB(red,green,blue);
    return true;
  }
  return false;
}

/// String representation of this object
string VisAttr::toString()  const {
  const VisAttr::Object* obj = &_data();
  TColor* col = gROOT->GetColor(obj->color);
  char text[256];
  ::sprintf(text,"%-20s RGB:%-8s [%d] %7.2f  Style:%d %d ShowDaughters:%3s Visible:%3s",
	    ptr()->GetName(),col->AsHexString(), obj->color, col->GetAlpha(), 
	    int(obj->drawingStyle), int(obj->lineStyle),
	    obj->showDaughters ? "YES" : "NO", obj->visible ? "YES" : "NO");
  return text;
}

/// Constructor to be used when creating a new aligment entry
AlignmentEntry::AlignmentEntry(const string& path)   {
  TGeoPhysicalNode* obj = new TGeoPhysicalNode(path.c_str());
  assign(obj,path,"*");
}

/// Align the PhysicalNode (translation only)
int AlignmentEntry::align(const Position& pos, bool check, double overlap) {
  return align(pos,Rotation(),check,overlap);
}

/// Align the PhysicalNode (rotation only)
int AlignmentEntry::align(const Rotation& rot, bool check, double overlap) {
  return align(Position(),rot,check,overlap);
}

/// Align the PhysicalNode (translation + rotation)
int AlignmentEntry::align(const Position& pos, const Rotation& rot, bool check, double overlap) {

  if ( isValid() ) {
    TGeoHMatrix* new_matrix = dynamic_cast<TGeoHMatrix*>(m_element->GetOriginalMatrix()->MakeClone());
    TGeoRotation rotation("",rot.Phi()*RAD_2_DEGREE,rot.Theta()*RAD_2_DEGREE,rot.Psi()*RAD_2_DEGREE);
    TGeoCombiTrans m(pos.X(),pos.Y(),pos.Z(),0);
    m.SetRotation(rotation);
    new_matrix->Multiply(&m);
    m_element->Align(new_matrix,0,check,overlap);
    return 1;
  }
  throw runtime_error("Callot align non existing physical node.");
}


/// Assignment operator
Limit& Limit::operator=(const Limit& c) 	{ 
  particles = c.particles;
  name      = c.name;
  unit      = c.unit;
  value     = c.value; 
  content   = c.content;
  return *this;
}

/// Equality operator
bool Limit::operator==(const Limit& c) const {
  return value==c.value && name==c.name && particles == c.particles;
}

/// operator less
bool Limit::operator< (const Limit& c) const {
  if ( value     < c.value      ) return true;
  if ( name      < c.name       ) return true;
  if ( particles < c.particles ) return true;
  return false;
}

/// Conversion to a string representation
string Limit::toString()  const {
  string res = name + " = " + content;
  if ( !unit.empty() ) res += unit + " ";
  res + " (" + particles + ")";
  return res;
}

/// Constructor to be used when creating a new DOM tree
LimitSet::LimitSet(const string& name)   {
  assign(new Value<TNamed,Object>(),name,"limitset");
}

/// Add new limit. Returns true if the new limit was added, false if it already existed.
bool LimitSet::addLimit(const Limit& limit)   {
  pair<Object::iterator,bool> ret = data<Object>()->insert(limit);
  return ret.second;
}

/// Accessor to limits container
const LimitSet::Object& LimitSet::limits() const {
  return *(data<Object>());
}

/// Constructor to be used when creating a new DOM tree
Region::Region(const string& name)   {
  Value<TNamed,Object>* p = new Value<TNamed,Object>();
  assign(p, name, "region");
  p->magic = magic_word();
  p->store_secondaries = false;
  p->threshold = 10.0;
  p->lunit = "mm";
  p->eunit = "MeV";
  p->cut = 10.0;
}

Region& Region::setStoreSecondaries(bool value)  {
  _data().store_secondaries = value;
  return *this;
}

Region& Region::setThreshold(double value)  {
  _data().threshold = value;
  return *this;
}

Region& Region::setCut(double value)  {
  _data().cut = value;
  return *this;
}

Region& Region::setLengthUnit(const string& unit)  {
  _data().lunit = unit;
  return *this;
}

Region& Region::setEnergyUnit(const string& unit)  {
  _data().eunit = unit;
  return *this;
}

/// Access references to user limits
vector<string>& Region::limits() const {
  return _data().user_limits;
}

/// Access cut value
double Region::cut() const {
  return _data().cut;
}

/// Access production threshold
double Region::threshold() const {
  return _data().threshold;
}

/// Access secondaries flag
bool Region::storeSecondaries() const {
  return _data().store_secondaries;
}

/// Access the length unit
const std::string& Region::lengthUnit() const   {
  return _data().lunit;
}

/// Access the energy unit
const std::string& Region::energyUnit() const   {
  return _data().eunit;
}

#undef setAttr

#if 0
IDSpec::IDSpec(LCDD& lcdd, const string& name, const IDDescriptor& dsc) 
: RefElement(doc,Tag_idspec,name)
{
  const IDDescriptor::FieldIDs& f = dsc.ids();
  const IDDescriptor::FieldMap& m = dsc.fields();
  _data().Attr_length = dsc.maxBit();
  for(IDDescriptor::FieldIDs::const_iterator i=f.begin(); i!=f.end();++i)  {
    int ident = (*i).first;
    const string& nam = (*i).second;
    const pair<int,int>& fld = m.find(nam)->second;
    addField(nam,fld);
  }
}

void IDSpec::addField(const string& name, const pair<int,int>& field)  {
  addField(Strng_t(name),field);
}

void IDSpec::addField(const string& name, const pair<int,int>& field)  {
  Element e(document(),Tag_idfield);
  e._data().Attr_signed = field.second<0;
  e._data().Attr_label = name;
  e._data().Attr_start = field.first;
  e._data().Attr_length = abs(field.second);
  m_element.append(e);
}
#endif
