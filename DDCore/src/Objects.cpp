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
using DD4hep::IDDescriptor;

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
Constant::Constant(LCDD& lcdd, const string& nam, const string& val)
{
  m_element = new TNamed(nam.c_str(),val.c_str());
  lcdd.add(*this);
}

/// Constructor to be used when creating a new DOM tree
Constant::Constant(LCDD& lcdd, const string& name)   {
  m_element = new TNamed(name.c_str(),"");
  lcdd.add(*this);
}

/// String representation of this object
string Constant::toString()  const {
  stringstream os;
  os << m_element->GetName() << "  \"" << m_element->GetTitle() 
     << "\"  Value:" << _toDouble(m_element->GetTitle());
  return os.str();
}

/// Constructor to be used when creating a new DOM tree
Atom::Atom(LCDD& /* lcdd */, const string& name, const string& formula, int Z, int N, double density) {
  TGeoElementTable* t = TGeoElement::GetElementTable();
  TGeoElement* e = t->FindElement(name.c_str());
  if ( !e ) {
    t->AddElement(name.c_str(), formula.c_str(), Z, N, density);
    e = t->FindElement(name.c_str());
  }
  m_element = e;
}

/// Constructor to be used when creating a new DOM tree
Material::Material(LCDD& /* lcdd */, const string& name)   {
  TGeoMaterial* mat = gGeoManager->GetMaterial(name.c_str());
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
VisAttr::VisAttr(LCDD& /* lcdd */, const string& name)    {
  Value<TNamed,Object>* obj = new Value<TNamed,Object>();
  assign(obj, name, "vis");
  obj->color  = 2;
  setLineStyle(SOLID);
  setDrawingStyle(WIREFRAME);
  setShowDaughters(true);
  setAlpha(0.1f);
}

/// Set Flag to show/hide daughter elements
void VisAttr::setShowDaughters(bool value)   {
  _data().showDaughters = value;
}

/// Set visibility flag
void VisAttr::setVisible(bool value)   {
  _data().visible = value;
}

/// Set line style
void VisAttr::setLineStyle(LineStyle value)  {
  _data().lineStyle = value;
}

/// Set drawing style
void VisAttr::setDrawingStyle(DrawingStyle value)   {
  _data().drawingStyle = value;
}

/// Set alpha value
void VisAttr::setAlpha(float /* value */)   {
  //TNamed* obj = first_value<TNamed>(*this);
  //obj->SetAlpha(value);
}

/// Set object color
void VisAttr::setColor(float red, float green, float blue)   {
  _data().color = TColor::GetColor(red,green,blue);
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

/// Constructor to be used when creating a new DOM tree
Limit::Limit(LCDD& /* lcdd */, const string& name)   {
  Value<TNamed,Object>* obj = new Value<TNamed,Object>();
  assign(obj,name,"*");
  obj->first  = "mm";
  obj->second = 1.0;
}

void Limit::setParticles(const string& particleNames)   {
  m_element->SetTitle(particleNames.c_str());
}

void Limit::setValue(double value)   {
  _data().second = value;
}

void Limit::setUnit(const string& value)   {
  _data().first = value;
}

/// Constructor to be used when creating a new DOM tree
LimitSet::LimitSet(LCDD& /* lcdd */, const string& name)   {
  assign(new Value<TNamed,TMap>(),name,"limitset");
}

void LimitSet::addLimit(const Ref_t& limit)   {
  data<TMap>()->Add(limit.ptr(),limit.ptr());
}

/// Constructor to be used when creating a new DOM tree
Region::Region(LCDD& /* lcdd */, const string& name)   {
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
