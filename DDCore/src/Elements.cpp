#include "Elements.h"
#include "Internals.h"
#include "TGeoManager.h"
#include "TNamed.h"
#include <iostream>
#include <stdexcept>

#include "TMap.h"
#include "TColor.h"
#include "TGeoBBox.h"
#include "TGeoPcon.h"
#include "TGeoPgon.h"
#include "TGeoTube.h"
#include "TGeoCone.h"
#include "TGeoArb8.h"
#include "TGeoTrd2.h"
#include "TGeoBoolNode.h"
#include "TGeoElement.h"
#include "TGeoMatrix.h"
#include "TGeoCompositeShape.h"
#include "Evaluator.h"

#include "LCDD.h"

namespace DetDesc  {
  XmlTools::Evaluator& evaluator();
}

namespace {
  XmlTools::Evaluator& eval(DetDesc::evaluator());
}

using namespace std;
using namespace DetDesc;
using namespace DetDesc::Geometry;

int DetDesc::Geometry::_toInt(const string& value)  {
  string s(value);
  size_t idx = s.find("(int)");
  if ( idx != string::npos ) 
    s.erase(idx,5);
  while(s[0]==' ')s.erase(0,1);
  double result = eval.evaluate(s.c_str());
  if (eval.status() != XmlTools::Evaluator::OK) {
    cerr << value << ": ";
    eval.print_error();
  }
  return (int)result;
}

bool   DetDesc::Geometry::_toBool(const string& value)   {
  return value == "true";
}

float DetDesc::Geometry::_toFloat(const string& value)   {
  double result = eval.evaluate(value.c_str());
  if (eval.status() != XmlTools::Evaluator::OK) {
    cerr << value << ": ";
    eval.print_error();
  }
  return (float)result;
}

double DetDesc::Geometry::_toDouble(const string& value)   {
  double result = eval.evaluate(value.c_str());
  if (eval.status() != XmlTools::Evaluator::OK) {
    cerr << value << ": ";
    eval.print_error();
  }
  return result;
}

void DetDesc::Geometry::_toDictionary(const string& name, const string& value)  {
  string n=name, v=value;
  size_t idx = v.find("(int)");
  if ( idx != string::npos ) 
    v.erase(idx,5);
  idx = v.find("(float)");
  if ( idx != string::npos ) 
    v.erase(idx,7);
  while(v[0]==' ')v.erase(0,1);
  double result = eval.evaluate(v.c_str());
  if (eval.status() != XmlTools::Evaluator::OK) {
    cerr << value << ": ";
    eval.print_error();
  }
  eval.setVariable(n.c_str(),result);
}

string DetDesc::Geometry::_toString(bool value)    {
  char text[32];
  ::sprintf(text,"%s",value ? "true" : "false");
  return text;
}

string DetDesc::Geometry::_toString(int value)   {
  char text[32];
  ::sprintf(text,"%d",value);
  return text;
}

string DetDesc::Geometry::_toString(float value)   {
  char text[32];
  ::sprintf(text,"%f",value);
  return text;
}

string DetDesc::Geometry::_toString(double value)   {
  char text[32];
  ::sprintf(text,"%f",value);
  return text;
}


#include "Volumes.h"
#include "Objects.h"
#include "Readout.h"
#include "SubDetector.h"
#include "Segmentations.h"
Element_t* Document::createElt(const string& tag)  const {
  TNamed* object = 0;
  if ( tag == "box" )
    object = new TGeoBBox();
  else if ( tag == "polycone" )
    object = new TGeoPcon();
#if 0
  else if ( tag == "tube" )
    object = new TGeoTubeSeg();
#endif
  else if ( tag == "cone" )
    object = new TGeoCone();
  else if ( tag == "trap" )
    object = new TGeoTrap();
  else if ( tag == "trd2" )
    object = new TGeoTrd2();
  else if ( tag == "polyhedra" )
    object = new TGeoPgon();
  else if ( tag == "subtraction" )
    object = 0;
    //object = new TGeoCompositeShape();
  else if ( tag == "element" )
    object = new TGeoElement();
  else if ( tag == "material" )
    object = new TGeoMedium();
  else if ( tag == "rotation" )
    object = new TGeoRotation();
  else if ( tag == "position" )
    object = new TGeoTranslation();
  else if ( tag == "identity" )
    object = new TGeoIdentity();
  else if ( tag == "constant" )
    object = new TNamed();
#if 0
  else if ( tag == "volume" )
    object = new Value<TGeoVolume,Volume::Object>();
  else if ( tag == "physvol" )
    object = new Value<TGeoVolume,PhysVol::Object>();
#endif
  else if ( tag == "limitset" )
    object = new Value<TNamed,TMap>();
  else if ( tag == "limit" )
    object = new Value<TNamed,Limit::Object>();
  else if ( tag == "vis" )
    object = new Value<TNamed,VisAttr::Object>();
  else if ( tag == "readout" )
    object = new Value<TNamed,Readout::Object>();
  else if ( tag == "region" )
    object = new Value<TNamed,Region::Object>();
  else if ( tag == "grid_xyz" )
    object = new Value<TNamed,Segmentation::Object>();
  else if ( tag == "global_grid_xy" )
    object = new Value<TNamed,Segmentation::Object>();
  else if ( tag == "cartesian_grid_xy" )
    object = new Value<TNamed,Segmentation::Object>();
  else if ( tag == "projective_cylinder" )
    object = new Value<TNamed,Segmentation::Object>();
  else if ( tag == "nonprojective_cylinder" )
    object = new Value<TNamed,Segmentation::Object>();
  else if ( tag == "projective_zplane" )
    object = new Value<TNamed,Segmentation::Object>();
  else if ( tag == "subdetector" )
    object = new Value<TNamed,Subdetector::Object>();
  else if ( tag == "sensitive_detector" )
    object = new Value<TNamed,SensitiveDetector::Object>();

  if ( object )  {
    object->SetTitle(tag.c_str());
    return object;
  }
  return 0;//m_doc->createElement(tag);
}

Element::Element(const Document& document, const std::string& type) 
: m_element(document.createElt(type))
{ }

RefElement::RefElement(const Document& document, const std::string& type, const std::string& name)  
: Element(document, type)
{
  setName(name);
}

RefElement::RefElement(const Handle_t& e) : Element(e)
{
  //TNamed *p = m_element._ptr<TNamed>();
}

const char* RefElement::name() const  {
  TNamed *p = m_element._ptr<TNamed>();
  return p ? p->GetName() : "";
}

const char* RefElement::refName() const  {
  TNamed *p = m_element._ptr<TNamed>();
  return p ? p->GetName() : "";
}

void RefElement::setName(const std::string& new_name)  {
  TNamed *p = m_element._ptr<TNamed>();
  if ( p ) p->SetName(new_name.c_str());
}
