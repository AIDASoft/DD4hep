#include "Objects.h"
#include "IDDescriptor.h"
#include "Internals.h"
#include "TMap.h"
#include "TColor.h"
#include "TGeoMatrix.h"
#include <cmath>

using namespace std;
using namespace DetDesc::Geometry;
using DetDesc::IDDescriptor;

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
Constant::Constant(const Document& doc, const string& name, const string& val)
: RefElement(doc, "constant", name)
{
  TNamed* obj = value<TNamed>(*this);
  obj->SetTitle(val.c_str());
}

/// Constructor to be used when creating a new DOM tree
Constant::Constant(const Document& doc, const string& name)
: RefElement(doc, "constant", name)
{
}

/// Constructor to be used when creating a new DOM tree. Automatically sets attributes
Position::Position(const Document& doc, const std::string& name, double x, double y, double z)
: Matrix(doc,"position",name) 
{
  TGeoTranslation* obj = value<TGeoTranslation>(*this);
  obj->SetTranslation(x,y,z);
}

/// Constructor to be used when creating a new DOM tree. Automatically sets attributes
Rotation::Rotation(const Document& doc, const std::string& name, double x, double y, double z)
: Matrix(doc,"rotation",name) 
{
  TGeoRotation* obj = value<TGeoRotation>(*this);
  obj->SetAngles(x*RAD_2_DEGREE,y*RAD_2_DEGREE,z*RAD_2_DEGREE);
}

/// Constructor to be used when creating a new DOM tree
Atom::Atom(const Document& doc, const string& name)  
: RefElement(doc, "element", name)
{
}

/// Constructor to be used when creating a new DOM tree
Material::Material(const Document& doc, const string& name)  
: RefElement(doc, "material", name)
{
}

/// Constructor to be used when creating a new DOM tree
VisAttr::VisAttr(const Document& doc, const string& name) 
: RefElement(doc,"vis",name) 
{
  Object* obj = second_value<TNamed>(*this);
  obj->color  = 2;
  setLineStyle(SOLID);
  setDrawingStyle(WIREFRAME);
  setShowDaughters(true);
  setAlpha(0.1f);
}

/// Set Flag to show/hide daughter elements
void VisAttr::setShowDaughters(bool value)   {
  setAttr(showDaughters,value);
}

/// Set visibility flag
void VisAttr::setVisible(bool value)   {
  setAttr(visible,value);
}

/// Set line style
void VisAttr::setLineStyle(LineStyle value)  {
  setAttr(lineStyle,value);
}

/// Set drawing style
void VisAttr::setDrawingStyle(DrawingStyle value)   {
  setAttr(drawingStyle,value);
}

/// Set alpha value
void VisAttr::setAlpha(float /* value */)   {
  //TNamed* obj = first_value<TNamed>(*this);
  //obj->SetAlpha(value);
}

/// Set object color
void VisAttr::setColor(float red, float green, float blue)   {
  Object* obj = second_value<TNamed>(*this);
  obj->color = TColor::GetColor(red,green,blue);
}

/// Constructor to be used when creating a new DOM tree
Limit::Limit(const Document& doc, const string& name)
: RefElement(doc,"limit",name)
{
  Value<TNamed,Object>* obj = value<Value<TNamed,Object> >(*this);
  obj->SetName(name.c_str());
  obj->SetTitle("*");
  obj->first  = "mm";
  obj->second = 1.0;
}

void Limit::setParticles(const string& particleNames)   {
  first_value<TNamed>(*this)->SetTitle(particleNames.c_str());
}

void Limit::setValue(double value)   {
  setAttr(second,value);
}

void Limit::setUnit(const string& value)   {
  setAttr(first,value);
}

/// Constructor to be used when creating a new DOM tree
LimitSet::LimitSet(const Document& doc, const string& name) 
: RefElement(doc,"limitset",name)
{
}

void LimitSet::addLimit(const RefElement& limit)   {
  TObject* obj = limit.ptr();
  TMap* m = dynamic_cast<TMap*>(m_element.ptr());
  m->Add(obj,obj);
}

/// Constructor to be used when creating a new DOM tree
Region::Region(const Document& doc, const string& name)  
: RefElement(doc,"region",name)
{
  setAttr(Attr_store_secondaries,false);
  setAttr(Attr_threshold,10.0);
  setAttr(Attr_lunit,"mm");
  setAttr(Attr_eunit,"MeV");
  setAttr(Attr_cut,10.0);
}

Region& Region::setStoreSecondaries(bool value)  {
  setAttr(Attr_store_secondaries,value);
  return *this;
}

Region& Region::setThreshold(double value)  {
  setAttr(Attr_threshold,value);
  return *this;
}

Region& Region::setCut(double value)  {
  setAttr(Attr_cut,value);
  return *this;
}

Region& Region::setLengthUnit(const string& unit)  {
  setAttr(Attr_lunit,unit);
  return *this;
}

Region& Region::setEnergyUnit(const string& unit)  {
  setAttr(Attr_eunit,unit);
  return *this;
}
#undef setAttr

#if 0
IDSpec::IDSpec(const Document& doc, const string& name, const IDDescriptor& dsc) 
: RefElement(doc,Tag_idspec,name)
{
  const IDDescriptor::FieldIDs& f = dsc.ids();
  const IDDescriptor::FieldMap& m = dsc.fields();
  setAttr(Attr_length, dsc.maxBit());
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
  e.setAttr(Attr_signed,field.second<0);
  e.setAttr(Attr_label,name);
  e.setAttr(Attr_start,field.first);
  e.setAttr(Attr_length,abs(field.second));
  m_element.append(e);
}
#endif
