// $Id:$
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

#if 0
Header& Header::fromCompact(Document doc, Handle_t element, const string& fname)   {
  Element    info = element;
  Author     aut(doc);
  RefElement det(doc,Tag_detector,info.attr<cpXMLCh>(Attr_name));
  RefElement gen(doc,Tag_generator, Strng_t("GeomCnv++"));

  gen.setAttr(Attr_file,fname);  
  gen.setAttr(Attr_version,"1.0");
  gen.setAttr(Attr_checksum,"None");

  aut.setAttr(Attr_name,info.attr<cpXMLCh>(Tag_author));

  append(gen);
  append(aut);
  append(doc.clone(info.child(Tag_comment),true));
  return *this;
}
#endif

/// Constructor to be used when creating a new DOM tree
Author::Author(LCDD& /* lcdd */)  {
  m_element = new TNamed("","author");
}

/// Constructor to be used when creating a new DOM tree
Header::Header(LCDD& /* lcdd */)   {
  m_element = new TNamed("","header");
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

/// Rotates the position vector around the x-axis.
Position& Position::rotateX(double angle_in_rad) {
  double s = std::sin(angle_in_rad);
  double c = std::cos(angle_in_rad);
  double t = y * c - z * s;
  z = z * c + y * s;
  y = t;
  return *this;
}

/// Rotates the position vector around the y-axis.
Position& Position::rotateY(double angle_in_rad) {
  double s = std::sin(angle_in_rad);
  double c = std::cos(angle_in_rad);
  double t = z * c - x * s;
  x = x * c + z * s;
  z = t;
  return *this;
}

/// Rotates the position vector around the z-axis.
Position& Position::rotateZ(double angle_in_rad) {
  double s = std::sin(angle_in_rad);
  double c = std::cos(angle_in_rad);
  double t = x * c - y * s;
  y = y * c + x * s;
  x = t;
  return *this;
}

/// Rotates the rotation vector around the x-axis.
Rotation& Rotation::rotateX(double angle_in_rad) {
  theta += angle_in_rad;
  return *this;
}

/// Rotates the rotation vector around the y-axis.
Rotation& Rotation::rotateY(double angle_in_rad) {
  psi += angle_in_rad;
  return *this;
}

/// Rotates the rotation vector around the z-axis.
Rotation& Rotation::rotateZ(double angle_in_rad) {
  phi += angle_in_rad;
  return *this;
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
  setDrawingStyle(WIREFRAME);
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
    if ( pos.isNull() && rot.isNull() )  
      return 0;
    TGeoHMatrix* new_matrix = dynamic_cast<TGeoHMatrix*>(m_element->GetOriginalMatrix()->MakeClone());
    if ( rot.isNull() ) {
      TGeoTranslation m(pos.x,pos.y,pos.z);
      new_matrix->Multiply(&m);
    }
    else if ( pos.isNull() ) {
      TGeoRotation m("",rot.phi*RAD_2_DEGREE,rot.theta*RAD_2_DEGREE,rot.psi*RAD_2_DEGREE);
      new_matrix->Multiply(&m);
    }
    else {
      TGeoRotation rotation("",rot.phi*RAD_2_DEGREE,rot.theta*RAD_2_DEGREE,rot.psi*RAD_2_DEGREE);
      TGeoCombiTrans m(pos.x,pos.y,pos.z,0);
      m.SetRotation(rotation);
      new_matrix->Multiply(&m);
    }
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
  if ( value    < c.value      ) return true;
  if ( name     < c.name       ) return true;
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
