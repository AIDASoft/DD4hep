// $Id$
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
// Framework include files
#include "XML/Evaluator.h"
#include "XML/XMLElements.h"
#include "XML/XMLTags.h"

// C/C++ include files
#include <iostream>
#include <stdexcept>
#include <cstdio>
#include <map>

using namespace std;
using namespace DD4hep::XML;
static const size_t INVALID_NODE = ~0U;

// Forward declarations
namespace DD4hep {
  XmlTools::Evaluator& evaluator();
}
// Static storage
namespace {
  XmlTools::Evaluator& eval(DD4hep::evaluator());
}

// Shortcuts
#define _D(x)   Xml(x).d
#define _E(x)   Xml(x).e
#define _N(x)   Xml(x).n
#define _L(x)   Xml(x).l
#define _XE(x)  Xml(x).xe

#ifdef DD4HEP_USE_TINYXML
#include "XML/tinyxml.h"
#define ELEMENT_NODE_TYPE TiXmlNode::ELEMENT
#define getTagName          Value
#define getTextContent      GetText
#define getName             Name
#define getValue            Value
#define getNodeValue        Value
#define getNodeType         Type
#define setNodeValue        SetValue
#define getParentNode       Parent()->ToElement
#define getAttributeNode(x) AttributeNode(x)
#define appendChild         LinkEndChild
#define getOwnerDocument    GetDocument
#define getDocumentElement  RootElement
union Xml {
  Xml(const void* ptr) : p(ptr) {}
  const void* p;
  TiXmlNode* n;
  TiXmlElement* e;
  TiXmlAttribute* a;
  TiXmlDocument* d;
  XmlElement* xe;
};

namespace {
  XmlElement* node_first(XmlElement* e, const XmlChar* t) {
    return e ? (XmlElement*)_E(e)->FirstChildElement(t) : 0;
  }
  size_t node_count(XmlElement* elt, const XmlChar* t) {
    size_t cnt = 0;
    TiXmlElement* e = Xml(elt).e;
    for(e=e->FirstChildElement(t);e; e=e->NextSiblingElement(t)) ++cnt;
    return cnt;
  }
}
XmlChar* DD4hep::XML::XmlString::replicate(const XmlChar* c) {return c ? ::strdup(c) : 0;}
XmlChar* DD4hep::XML::XmlString::transcode(const char* c) {return c ? ::strdup(c) : 0;}
void DD4hep::XML::XmlString::release(char** p) {if(p && *p) {::free(*p); *p=0;}}

#else
#include "xercesc/util/XMLString.hpp"
#include "xercesc/dom/DOMElement.hpp"
#include "xercesc/dom/DOMDocument.hpp"
#include "xercesc/dom/DOMNodeList.hpp"
#include "xercesc/dom/DOM.hpp"
#define ELEMENT_NODE_TYPE xercesc::DOMNode::ELEMENT_NODE
union Xml {
  Xml(const void* ptr)
      : p(ptr) {
  }
  const void* p;
  xercesc::DOMNode* n;
  xercesc::DOMAttr* a;
  xercesc::DOMElement* e;
  xercesc::DOMDocument* d;
  xercesc::DOMNodeList* l;
  XmlElement* xe;
};

XmlChar* DD4hep::XML::XmlString::replicate(const XmlChar* c) {
  return xercesc::XMLString::replicate(c);
}
char* DD4hep::XML::XmlString::transcode(const XmlChar* c) {
  return xercesc::XMLString::transcode(c);
}
XmlChar* DD4hep::XML::XmlString::transcode(const char* c) {
  return xercesc::XMLString::transcode(c);
}
void DD4hep::XML::XmlString::release(XmlChar** p) {
  return xercesc::XMLString::release(p);
}
void DD4hep::XML::XmlString::release(char** p) {
  return xercesc::XMLString::release(p);
}

namespace {
  size_t node_count(XmlElement* e, const XmlChar* t) {
    size_t cnt = 0;
    string tag =  _toString(t);
    xercesc::DOMElement* elt, *ee = e ? Xml(e).e : 0;
    for(elt=ee->getFirstElementChild(); elt; elt=elt->getNextElementSibling()) {
      if ( elt->getParentNode() == ee )   {
	string child_tag = _toString(elt->getTagName());
	if ( child_tag == tag ) ++cnt;
      }
    }
    return cnt;
  }
  XmlElement* node_first(XmlElement* e, const XmlChar* t) {
    if ( e )  {
      size_t cnt = 0;
      string tag = _toString(t);
      xercesc::DOMElement* ee = e ? Xml(e).e : 0;
      if ( ee )  {
	for(xercesc::DOMElement* elt=ee->getFirstElementChild(); elt; elt=elt->getNextElementSibling()) {
	  if ( elt->getParentNode() == ee )   {
	    string child_tag = _toString(elt->getTagName());
	    if ( child_tag == tag ) return _XE(elt);
	  }
	}
      }
    }
    return 0;
  }
}
string DD4hep::XML::_toString(const XmlChar *toTranscode) {
  char *buff = XmlString::transcode(toTranscode);
  string tmp(buff == 0 ? "" : buff);
  XmlString::release(&buff);
  return tmp;
}
#endif

namespace {
  Attribute attribute_node(XmlElement* n, const XmlChar* t) {
    return Attribute(_E(n)->getAttributeNode(t));
  }
  const XmlChar* attribute_value(Attribute a) {
    return Xml(a).a->getValue();
  }
#if 0
int node_type(XmlNode* n) {return Xml(n).n->getNodeType();}
int node_type(Handle_t n) {return Xml(n.ptr()).n->getNodeType();}
#endif
}

string DD4hep::XML::_toString(Attribute attr) {
  if (attr)
    return _toString(attribute_value(attr));
  return "";
}

template <typename T> static inline string __to_string(T value, const char* fmt) {
  char text[128];
  ::snprintf(text, sizeof(text), fmt, value);
  return text;
}

/// Do-nothing version. Present for completeness and argument interchangeability
std::string DD4hep::XML::_toString(const char* s) {
  return string(s ? s : "");
}

/// Do-nothing version. Present for completeness and argument interchangeability
std::string DD4hep::XML::_toString(const std::string& s) {
  return s;
}

/// Format unsigned long integer to string with atrbitrary format
string DD4hep::XML::_toString(unsigned long v, const char* fmt) {
  return __to_string(v, fmt);
}

/// Format unsigned integer (32 bits) to string with atrbitrary format
string DD4hep::XML::_toString(unsigned int v, const char* fmt) {
  return __to_string(v, fmt);
}

/// Format signed integer (32 bits) to string with atrbitrary format
string DD4hep::XML::_toString(int v, const char* fmt) {
  return __to_string(v, fmt);
}

/// Format single procision float number (32 bits) to string with atrbitrary format
string DD4hep::XML::_toString(float v, const char* fmt) {
  return __to_string(v, fmt);
}

/// Format double procision float number (64 bits) to string with atrbitrary format
string DD4hep::XML::_toString(double v, const char* fmt) {
  return __to_string(v, fmt);
}

int DD4hep::XML::_toInt(const XmlChar* value) {
  if (value) {
    string s = _toString(value);
    size_t idx = s.find("(int)");
    if (idx != string::npos)
      s.erase(idx, 5);
    while (s[0] == ' ')
      s.erase(0, 1);
    double result = eval.evaluate(s.c_str());
    if (eval.status() != XmlTools::Evaluator::OK) {
      cerr << s << ": ";
      eval.print_error();
      throw runtime_error("DD4hep: Severe error during expression evaluation of " + s);
    }
    return (int) result;
  }
  return -1;
}

bool DD4hep::XML::_toBool(const XmlChar* value) {
  if (value) {
    string s = _toString(value);
    return s == "true";
  }
  return false;
}

float DD4hep::XML::_toFloat(const XmlChar* value) {
  if (value) {
    string s = _toString(value);
    double result = eval.evaluate(s.c_str());

    if (eval.status() != XmlTools::Evaluator::OK) {
      cerr << s << ": ";
      eval.print_error();
      throw runtime_error("DD4hep: Severe error during expression evaluation of " + s);
    }
    return (float) result;
  }
  return 0.0;
}

double DD4hep::XML::_toDouble(const XmlChar* value) {
  if (value) {
    string s = _toString(value);
    double result = eval.evaluate(s.c_str());
    if (eval.status() != XmlTools::Evaluator::OK) {
      cerr << s << ": ";
      eval.print_error();
      throw runtime_error("DD4hep: Severe error during expression evaluation of " + s);
    }
    return result;
  }
  return 0.0;
}

void DD4hep::XML::_toDictionary(const XmlChar* name, const XmlChar* value) {
  string n = _toString(name).c_str(), v = _toString(value);
  size_t idx = v.find("(int)");
  if (idx != string::npos)
    v.erase(idx, 5);
  while (v[0] == ' ')
    v.erase(0, 1);
  double result = eval.evaluate(v.c_str());
  if (eval.status() != XmlTools::Evaluator::OK) {
    cerr << v << ": ";
    eval.print_error();
    throw runtime_error("DD4hep: Severe error during expression evaluation of " + v);
  }
  eval.setVariable(n.c_str(), result);
}

template <typename B>
static inline string i_add(const string& a, B b) {
  string r = a;
  r += b;
  return r;
}

Strng_t DD4hep::XML::operator+(const Strng_t& a, const string& b) {
  return _toString(a.ptr()) + b;
}

Strng_t DD4hep::XML::operator+(const string& a, const Strng_t& b) {
  return a + _toString(b.ptr());
}

Strng_t DD4hep::XML::operator+(const Strng_t& a, const char* b) {
  return _toString(a.ptr()) + b;
}

Strng_t DD4hep::XML::operator+(const char* a, const Strng_t& b) {
  return string(a) + _toString(b.ptr());
}

Strng_t DD4hep::XML::operator+(const Strng_t& a, const Strng_t& b) {
  return _toString(a.ptr()) + _toString(b.ptr());
}

Tag_t DD4hep::XML::operator+(const Tag_t& a, const char* b) {
  return a.str() + b;
}

Tag_t DD4hep::XML::operator+(const char* a, const Tag_t& b) {
  return a + b.str();
}

Tag_t DD4hep::XML::operator+(const Tag_t& a, const Strng_t& b) {
  return a.str() + _toString(b);
}

Tag_t DD4hep::XML::operator+(const Tag_t& a, const string& b) {
  return a.str() + b;
}

#ifndef DD4HEP_USE_TINYXML
Strng_t DD4hep::XML::operator+(const Strng_t& a, const XmlChar* b) {
  return _toString(a.ptr()) + _toString(b);
}

Strng_t DD4hep::XML::operator+(const XmlChar* a, const Strng_t& b) {
  return _toString(a) + _toString(b.ptr());
}

Strng_t DD4hep::XML::operator+(const XmlChar* a, const string& b) {
  return _toString(a) + b;
}

Strng_t DD4hep::XML::operator+(const string& a, const XmlChar* b) {
  return a + _toString(b);
}

Tag_t DD4hep::XML::operator+(const Tag_t& a, const XmlChar* b) {
  return a.str() + _toString(b);
}

Strng_t& Strng_t::operator=(const XmlChar* s) {
  if (m_xml)
    XmlString::release (&m_xml);
  m_xml = s ? XmlString::replicate(s) : 0;
  return *this;
}
#endif

Strng_t& Strng_t::operator=(const char* s) {
  if (m_xml)
    XmlString::release (&m_xml);
  m_xml = s ? XmlString::transcode(s) : 0;
  return *this;
}

Strng_t& Strng_t::operator=(const Strng_t& s) {
  if (this != &s) {
    if (m_xml)
      XmlString::release (&m_xml);
    m_xml = XmlString::replicate(s.m_xml);
  }
  return *this;
}

Strng_t& Strng_t::operator=(const string& s) {
  if (m_xml)
    XmlString::release (&m_xml);
  m_xml = XmlString::transcode(s.c_str());
  return *this;
}

Tag_t& Tag_t::operator=(const Tag_t& s) {
  if (this != &s) {
    m_str = s.m_str;
    if (m_xml)
      XmlString::release (&m_xml);
    m_xml = XmlString::transcode(m_str.c_str());
  }
  return *this;
}

Tag_t& Tag_t::operator=(const char* s) {
  if (m_xml)
    XmlString::release (&m_xml);
  if (s) {
    m_xml = XmlString::transcode(s);
    m_str = s;
  }
  else {
    m_xml = 0;
    m_str = "";
  }
  return *this;
}

Tag_t& Tag_t::operator=(const Strng_t& s) {
  if (m_xml)  {
    XmlString::release (&m_xml);
  }
  char* ns = s.m_xml ? XmlString::transcode(s.m_xml) : 0;
  m_str = ns ? ns : "";
  m_xml = XmlString::transcode(m_str.c_str());
  if (ns)  {
    XmlString::release(&ns);
  }
  return *this;
}

Tag_t& Tag_t::operator=(const string& s) {
  if (m_xml)
    XmlString::release (&m_xml);
  m_xml = XmlString::transcode(s.c_str());
  m_str = s;
  return *this;
}

/// Copy constructor
NodeList::NodeList(const NodeList& copy)
    : m_node(copy.m_node), m_ptr(0)
#ifndef DD4HEP_USE_TINYXML
      //        , m_index(0)
#endif
{
  m_tag = XmlString::replicate(copy.m_tag);
  reset();
}

/// Initializing constructor
NodeList::NodeList(XmlElement* node, const XmlChar* tag)
    : m_node(node), m_ptr(0)
#ifndef DD4HEP_USE_TINYXML
      //        , m_index(0)
#endif
{
  m_tag = XmlString::replicate(tag);
  reset();
}

/// Default destructor
NodeList::~NodeList() {
  if (m_tag)
    XmlString::release (&m_tag);
}

/// Reset the nodelist
XmlElement* NodeList::reset() {
#ifdef DD4HEP_USE_TINYXML
  return m_ptr=node_first(m_node,m_tag);
#else
  return m_ptr=node_first(m_node,m_tag);
#endif
}

/// Advance to next element
XmlElement* NodeList::next() const {
#ifdef DD4HEP_USE_TINYXML
  return m_ptr = _XE(m_ptr ? _E(m_ptr)->NextSiblingElement(m_tag) : 0);
#else
  xercesc::DOMElement *elt = Xml(m_ptr).e;
  for(elt=elt->getNextElementSibling(); elt; elt=elt->getNextElementSibling()) {
    string child_tag = _toString(elt->getTagName());
    if ( child_tag == m_tag ) return m_ptr=Xml(elt).xe;
  }
  return m_ptr=0;
#endif
}

/// Go back to previous element
XmlElement* NodeList::previous() const {
#ifdef DD4HEP_USE_TINYXML
  return m_ptr = _XE(m_ptr ? _E(m_ptr)->PreviousSiblingElement(m_tag) : 0);
#else
  xercesc::DOMElement *elt = Xml(m_ptr).e;
  for(elt=elt->getPreviousElementSibling(); elt; elt=elt->getPreviousElementSibling()) {
    string child_tag = _toString(elt->getTagName());
    if ( child_tag == m_tag ) return m_ptr=Xml(elt).xe;
  }
  return m_ptr=0;
#endif
}

/// Assignment operator
NodeList& NodeList::operator=(const NodeList& l) {
  if (this != &l) {
    if (m_tag)
      XmlString::release (&m_tag);
    m_tag = XmlString::replicate(l.m_tag);
    m_node = l.m_node;
    reset();
  }
  return *this;
}

/// Unicode text access to the element's tag. Tis must be wrong ....
const XmlChar* Handle_t::rawTag() const {
  return _E(m_node)->getTagName();
}

/// Unicode text access to the element's text
const XmlChar* Handle_t::rawText() const {
  return _E(m_node)->getTextContent();
}

/// Unicode text access to the element's value
const XmlChar* Handle_t::rawValue() const {
  return _N(m_node)->getNodeValue();
}

/// Clone the DOMelement - with the option of a deep copy
Handle_t Handle_t::clone(XmlDocument* new_doc) const {
  if (m_node) {
#ifdef DD4HEP_USE_TINYXML
    if ( new_doc ) {}
    if ( _N(m_node)->Type() == ELEMENT_NODE_TYPE ) {
      XmlElement* e = _XE(_N(m_node)->Clone()->ToElement());
      if ( e ) return e;
    }
    throw runtime_error("TiXml: Handle_t::clone: Invalid source handle type [No element type].");
#else
    return Elt_t(_D(new_doc)->importNode(_E(m_node), true));
#endif
  }
  throw runtime_error("Xml: Handle_t::clone: Invalid source handle.");
}

/// Access the element's parent element
Handle_t Handle_t::parent() const {
  return Elt_t(m_node ? _N(m_node)->getParentNode() : 0);
}

/// Access attribute pointer by the attribute's unicode name (no exception thrown if not present)
Attribute Handle_t::attr_nothrow(const XmlChar* tag) const {
  return attribute_node(m_node, tag);
}

/// Check for the existence of a named attribute
bool Handle_t::hasAttr(const XmlChar* tag) const {
  return m_node && 0 != _E(m_node)->getAttributeNode(tag);
}

/// Retrieve a collection of all attributes of this DOM element
vector<Attribute> Handle_t::attributes() const {
  vector < Attribute > attrs;
  if (m_node) {
#ifdef DD4HEP_USE_TINYXML
    for(TiXmlAttribute* a=_E(m_node)->FirstAttribute(); a; a=a->Next())
    attrs.push_back(Attribute(a));
#else
    xercesc::DOMNamedNodeMap* l = _E(m_node)->getAttributes();
    for (XmlSize_t i = 0, n = l->getLength(); i < n; ++i) {
      xercesc::DOMNode* attr_node = l->item(i);
      attrs.push_back(Attribute(attr_node));
    }
#endif
  }
  return attrs;
}

size_t Handle_t::numChildren(const XmlChar* t, bool throw_exception) const {
  size_t n = node_count(m_node, t);
  if (n == INVALID_NODE && !throw_exception)
    return 0;
  else if (n != INVALID_NODE)
    return n;
  string msg = "Handle_t::numChildren: ";
  if (m_node)
    msg += "Element [" + tag() + "] has no children of type '" + _toString(t) + "'";
  else
    msg += "Element [INVALID] has no children of type '" + _toString(t) + "'";
  throw runtime_error(msg);
}

/// Remove a single child node identified by it's handle from the tree of the element
Handle_t Handle_t::child(const XmlChar* t, bool throw_exception) const {
  Elt_t e = node_first(m_node, t);
  if (e || !throw_exception)
    return e;
  string msg = "Handle_t::child: ";
  if (m_node)
    msg += "Element [" + tag() + "] has no child of type '" + _toString(t) + "'";
  else
    msg += "Element [INVALID]. Cannot remove child of type: '" + _toString(t) + "'";
  throw runtime_error(msg);
}

NodeList Handle_t::children(const XmlChar* tag) const {
  return NodeList(m_node, tag);
}

/// Append a DOM element to the current node
void Handle_t::append(Handle_t e) const {
  _N(m_node)->appendChild(_N(e.ptr()));
}

/// Remove a single child node identified by it's handle from the tree of the element
Handle_t Handle_t::remove(Handle_t node) const {
#ifdef DD4HEP_USE_TINYXML
  bool e = (m_node && node.ptr() ? _N(m_node)->RemoveChild(_N(node.ptr())) : false);
#else
  Elt_t e = Elt_t(m_node && node.ptr() ? _N(m_node)->removeChild(_N(node.ptr())) : 0);
#endif
  if (e)
    return node.ptr();
  string msg = "Handle_t::remove: ";
  if (m_node && node.ptr())
    msg += "Element [" + tag() + "] has no child of type '" + node.tag() + "'";
  else if (node)
    msg += "Element [INVALID]. Cannot remove child of type: '" + node.tag() + "'";
  else if (node)
    msg += "Element [INVALID]. Cannot remove [INVALID] child. Big Shit!!!!";

  throw runtime_error(msg);
}

/// Remove children with a given tag name from the DOM node
void Handle_t::removeChildren(const XmlChar* tag) const {
#ifdef DD4HEP_USE_TINYXML
  for(TiXmlNode* n=_E(m_node)->FirstChildElement(tag);n;n=_E(m_node)->FirstChildElement(tag))
  n->RemoveChild(n);
#else
  xercesc::DOMElement* e = _E(m_node);
  xercesc::DOMNodeList* l = e->getElementsByTagName(tag);
  for (XmlSize_t i = 0, n = l->getLength(); i < n; ++i)
    e->removeChild(l->item(i));
#endif
}

bool Handle_t::hasChild(const XmlChar* tag) const {
  return node_first(m_node, tag) != 0;
}

/// Set the element's value
void Handle_t::setValue(const XmlChar* text) const {
  _N(m_node)->setNodeValue(text);
}

/// Set the element's value
void Handle_t::setValue(const string& text) const {
#ifdef DD4HEP_USE_TINYXML
  _N(m_node)->setNodeValue(text.c_str());
#else
  _N(m_node)->setNodeValue(Strng_t(text));
#endif
}

/// Set the element's text
void Handle_t::setText(const XmlChar* text) const {
#ifdef DD4HEP_USE_TINYXML
  _N(m_node)->LinkEndChild(new TiXmlText(text));
#else
  _N(m_node)->setTextContent(text);
#endif
}

/// Set the element's text
void Handle_t::setText(const string& text) const {
#ifdef DD4HEP_USE_TINYXML
  _N(m_node)->LinkEndChild(new TiXmlText(text.c_str()));
#else
  _N(m_node)->setTextContent(Strng_t(text));
#endif
}

/// Remove all attributes of this element
void Handle_t::removeAttrs() const {
#ifdef DD4HEP_USE_TINYXML
  _E(m_node)->ClearAttributes();
#else
  xercesc::DOMElement* e = _E(m_node);
  xercesc::DOMNamedNodeMap* l = e->getAttributes();
  for (XmlSize_t i = 0, n = l->getLength(); i < n; ++i) {
    xercesc::DOMAttr* a = (xercesc::DOMAttr*) l->item(i);
    e->removeAttributeNode(a);
  }
#endif
}

/// Set attributes as in argument handle
void Handle_t::setAttrs(Handle_t elt) const {
  removeAttrs();
#ifdef DD4HEP_USE_TINYXML
  TiXmlElement* e = Xml(elt).e;
  for(TiXmlAttribute* a=e->FirstAttribute(); a; a=a->Next())
  e->SetAttribute(a->Name(),a->Value());
#else
  xercesc::DOMElement* e = _E(m_node);
  xercesc::DOMNamedNodeMap* l = e->getAttributes();
  for (XmlSize_t i = 0, len = l->getLength(); i < len; ++i) {
    xercesc::DOMNode* n = l->item(i);
    if (n->getNodeType() == xercesc::DOMNode::ATTRIBUTE_NODE) {
      xercesc::DOMAttr* a = (xercesc::DOMAttr*) n;
      e->setAttribute(a->getName(), a->getValue());
    }
  }
#endif
}

/// Access attribute pointer by the attribute's unicode name (throws exception if not present)
Attribute Handle_t::attr_ptr(const XmlChar* t) const {
  Attribute a = attribute_node(m_node, t);
  if (0 != a)
    return a;
  string msg = "Handle_t::attr_ptr: ";
  if (m_node)
    msg += "Element [" + tag() + "] has no attribute of type '" + _toString(t) + "'";
  else
    msg += "Element [INVALID] has no attribute of type '" + _toString(t) + "'";
  throw runtime_error(msg);
}

/// Access attribute name (throws exception if not present)
const XmlChar* Handle_t::attr_name(const Attribute a) const {
  if (a) {
    return Xml(a).a->getName();
  }
  throw runtime_error("Attempt to access invalid XML attribute object!");
}

/// Access attribute value by the attribute's unicode name (throws exception if not present)
const XmlChar* Handle_t::attr_value(const XmlChar* attr) const {
  return attribute_value(attr_ptr(attr));
}

/// Access attribute value by the attribute  (throws exception if not present)
const XmlChar* Handle_t::attr_value(const Attribute attr) const {
  return attribute_value(attr);
}

/// Access attribute value by the attribute's unicode name (no exception thrown if not present)
const XmlChar* Handle_t::attr_value_nothrow(const XmlChar* attr) const {
  Attribute a = attr_nothrow(attr);
  return a ? attribute_value(a) : 0;
}

/// Generic attribute setter with integer value
Attribute Handle_t::setAttr(const XmlChar* name, int val) const {
  char txt[32];
  ::snprintf(txt, sizeof(txt), "%d", val);
  return setAttr(name, Strng_t(txt));
}

/// Generic attribute setter with boolen value
Attribute Handle_t::setAttr(const XmlChar* name, bool val) const {
  char txt[32];
  ::snprintf(txt, sizeof(txt), "%s", val ? "true" : "false");
  return setAttr(name, Strng_t(txt));
}

/// Generic attribute setter with floating point value
Attribute Handle_t::setAttr(const XmlChar* name, float val) const {
  char txt[32];
  ::snprintf(txt, sizeof(txt), "%f", val);
  return setAttr(name, Strng_t(txt));
}

/// Generic attribute setter with double precision floating point value
Attribute Handle_t::setAttr(const XmlChar* name, double val) const {
  char txt[32];
  ::snprintf(txt, sizeof(txt), "%f", val);
  return setAttr(name, Strng_t(txt));
}

/// Generic attribute setter with string value
Attribute Handle_t::setAttr(const XmlChar* name, const string& val) const {
  return setAttr(name, Strng_t(val.c_str()));
}

#ifndef DD4HEP_USE_TINYXML
Attribute Handle_t::setAttr(const XmlChar* name, const char* v) const {
  return setAttr(name, Strng_t(v));
}
#endif

/// Generic attribute setter with XmlAttr value
Attribute Handle_t::setAttr(const XmlChar* name, const Attribute v) const {
  return v ? setAttr(name, attribute_value(v)) : 0;
}

/// Generic attribute setter with unicode value
Attribute Handle_t::setAttr(const XmlChar* name, const XmlChar* value) const {
#ifdef DD4HEP_USE_TINYXML
  TiXmlElement* e = Xml(m_node).e;
  e->SetAttribute(name,value);
  return Attribute(e->AttributeNode(name));
#else
  xercesc::DOMElement* e = _E(m_node);
  xercesc::DOMAttr* a = e->getAttributeNode(name);
  if (!a) {
    a = e->getOwnerDocument()->createAttribute(name);
    e->setAttributeNode(a);
  }
  a->setValue(value);
  return Attribute(a);
#endif
}

/// Add reference child as a new child node. The obj must have the "name" attribute!
Handle_t Handle_t::setRef(const XmlChar* tag, const XmlChar* ref_name) {
  Element me(*this);
  Element ref(me.document(), tag);
  ref.setAttr(Unicode_ref, ref_name);
  me.append(ref);
  return ref;
}

/// Add reference child as a new child node. The obj must have the "name" attribute!
Handle_t Handle_t::setRef(const XmlChar* tag, const string& ref_name) {
  return setRef(tag, Strng_t(ref_name).ptr());
}

/// Checksum (sub-)tree of a xml document/tree
static unsigned int adler32(unsigned int adler, const char* buf, size_t len) {
#define DO1(buf,i)  {s1 +=(unsigned char)buf[i]; s2 += s1;}
#define DO2(buf,i)  DO1(buf,i); DO1(buf,i+1);
#define DO4(buf,i)  DO2(buf,i); DO2(buf,i+2);
#define DO8(buf,i)  DO4(buf,i); DO4(buf,i+4);
#define DO16(buf)   DO8(buf,0); DO8(buf,8);

  static const unsigned int BASE = 65521; /* largest prime smaller than 65536 */
  /* NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1 */
  static const unsigned int NMAX = 5550;
  unsigned int s1 = adler & 0xffff;
  unsigned int s2 = (adler >> 16) & 0xffff;
  int k;

  if (buf == NULL)
    return 1;

  while (len > 0) {
    k = len < NMAX ? (int) len : NMAX;
    len -= k;
    while (k >= 16) {
      DO16(buf);
      buf += 16;
      k -= 16;
    }
    if (k != 0)
      do {
        s1 += (unsigned char) *buf++;
        s2 += s1;
      } while (--k);
    s1 %= BASE;
    s2 %= BASE;
  }
  unsigned int result = (s2 << 16) | s1;
  return result;
}

/// Checksum (sub-)tree of a xml document/tree
unsigned int Handle_t::checksum(unsigned int param, unsigned int (fcn)(unsigned int, const XmlChar*, size_t)) const {
  typedef std::map<std::string, std::string> StringMap;
#ifdef DD4HEP_USE_TINYXML
  TiXmlNode* n = Xml(m_node).n;
  if ( n ) {
    if ( 0 == fcn ) fcn = adler32;
    switch (n->Type()) {
      case TiXmlNode::ELEMENT: {
        map<string,string> m;
        TiXmlElement* e = n->ToElement();
        TiXmlAttribute* p=e->FirstAttribute();
        for(; p; p=p->Next()) m.insert(make_pair(p->Name(),p->Value()));
        param = (*fcn)(param,e->Value(),::strlen(e->Value()));
        for(StringMap::const_iterator i=m.begin();i!=m.end();++i) {
          param = (*fcn)(param,(*i).first.c_str(),(*i).first.length());
          param = (*fcn)(param,(*i).second.c_str(),(*i).second.length());
        }
        break;
      }
      case TiXmlNode::TEXT:
      param = (*fcn)(param,n->ToText()->Value(),::strlen(n->ToText()->Value()));
      break;
      case TiXmlNode::UNKNOWN:
      case TiXmlNode::COMMENT:
      case TiXmlNode::DOCUMENT:
      case TiXmlNode::DECLARATION:
      default:
      break;
    }
    for(TiXmlNode* c=n->FirstChild(); c; c=c->NextSibling())
    param = Handle_t((XmlElement*)c->ToElement()).checksum(param,fcn);
  }
#else

#endif
  return param;
}

/// Create DOM element
Handle_t Document::createElt(const XmlChar* tag) const {
#ifdef DD4HEP_USE_TINYXML
  return _XE(new TiXmlElement(tag));
#else
  return _XE(_D(m_doc)->createElement(tag));
#endif
}

/// Access the ROOT eleemnt of the DOM document
Handle_t Document::root() const {
  if (m_doc)
    return _XE(_D(m_doc)->getDocumentElement());
  throw runtime_error("Document::root: Invalid handle!");
}

/// Standard destructor - releases the document
DocumentHolder::~DocumentHolder() {
#ifdef DD4HEP_USE_TINYXML
  if (m_doc) delete _D(m_doc);
#else
  if (m_doc)
    _D(m_doc)->release();
#endif
  m_doc = 0;
}

/// Constructor from DOM document entity
Element::Element(const Document& document, const XmlChar* type)
    : m_element(Xml(document.createElt(type)).xe) {
}

/// Access the hosting document handle of this DOM element
Document Element::document() const {
  return Document((XmlDocument*) (m_element ? _N(m_element)->getOwnerDocument() : 0));
}

/// Clone the DOM element tree
Handle_t Element::clone(Handle_t h) const {
  if (m_element && h) {
    return h.clone(Document::DOC(document()));
  }
  throw runtime_error("Element::clone: Invalid element pointer -- unable to clone node!");
}

Attribute Element::getAttr(const XmlChar* name) const {
  return m_element ? attribute_node(m_element, name) : 0;
}

/// Set the reference attribute to the node (adds attribute ref="ref-name")
Attribute Element::setRef(const XmlChar* tag, const XmlChar* ref_name) const {
  return setChild(tag).setAttr(Unicode_ref, ref_name);
}

/// Set the reference attribute to the node (adds attribute ref="ref-name")
Attribute Element::setRef(const XmlChar* tag, const string& ref_name) const {
  return setRef(tag, Strng_t(ref_name).ptr());
}

/// Access the value of the reference attribute of the node (attribute ref="ref-name")
const XmlChar* Element::getRef(const XmlChar* tag) const {
  return child(tag).attr < cpXmlChar > (Unicode_ref);
}

/// Add a new child to the DOM node
Handle_t Element::addChild(const XmlChar* tag) const {
  Handle_t e = document().createElt(tag);
  m_element.append(e);
  return e;
}

/// Check if a child with the required tag exists - if not create it and add it to the current node
Handle_t Element::setChild(const XmlChar* t) const {
  Elt_t e = m_element.child(t, false);
  return e ? Handle_t(e) : addChild(t);
}

/// Add comment node to the element
void Element::addComment(const char* text) const {
#ifdef DD4HEP_USE_TINYXML
  _N(m_element)->appendChild(new TiXmlComment(text));
#else
  _N(m_element)->appendChild(_D(document().m_doc)->createComment(Strng_t(text)));
#endif
}

/// Initializing constructor to create a new XMLElement and add it to the document.
RefElement::RefElement(const Document& document, const XmlChar* type, const XmlChar* name)
    : Element(document, type) {
  m_name = name ? setAttr(_U(name), name) : 0;
}

/// Construction from existing object handle
RefElement::RefElement(const Handle_t& e)
    : Element(e) {
  m_name = m_element ? getAttr(_U(name)) : 0;
}

/// Copy constructor
RefElement::RefElement(const RefElement& e)
    : Element(e), m_name(e.m_name) {
}

/// Assignment operator
RefElement& RefElement::operator=(const RefElement& e) {
  m_element = e.m_element;
  return *this;
}

const XmlChar* RefElement::name() const {
  if (0 == m_name)
    cout << "Error:tag=" << m_element.tag() << endl;
  return attribute_value(m_name);
}

const XmlChar* RefElement::refName() const {
  if (0 == m_name)
    cout << "Error:tag=" << m_element.tag() << endl;
  return attribute_value(m_name);
}

void RefElement::setName(const XmlChar* new_name) {
  setAttr(_U(name), new_name);
}

#ifndef DD4HEP_USE_TINYXML
Collection_t::Collection_t(Handle_t element, const XmlChar* tag)
    : m_children(element, tag) {
  m_node = m_children.reset();
}
#endif

Collection_t::Collection_t(Handle_t element, const char* tag)
    : m_children(element, Strng_t(tag)) {
  m_node = m_children.reset();
}

/// Constructor over XmlElements in a node list
Collection_t::Collection_t(NodeList node_list)
    : m_children(node_list) {
  m_node = m_children.reset();
}

/// Reset the collection object to restart the iteration
Collection_t& Collection_t::reset() {
  m_node = m_children.reset();
  return *this;
}

/// Access the collection size. Avoid this call -- sloooow!
size_t Collection_t::size() const {
  return Handle_t(m_children.m_node).numChildren(m_children.m_tag, false);
}

/// Helper function to throw an exception
void Collection_t::throw_loop_exception(const std::exception& e) const {
  if (m_node) {
    throw runtime_error(std::string(e.what()) + "\n" + "DD4hep: Error interpreting XML nodes of type <" + tag() + "/>");
  }
  throw runtime_error(std::string(e.what()) + "\n" + "DD4hep: Error interpreting collections XML nodes.");
}

void Collection_t::operator++() const {
  Elt_t e = this->parent();
  while (m_node) {
    m_node = m_children.next();
    if (m_node && _N(m_node)->getNodeType() == ELEMENT_NODE_TYPE) {
      if (this->parent() == e)
        return;
    }
  }
}

void Collection_t::operator--() const {
  Elt_t e = this->parent();
  while (m_node) {
    m_node = m_children.previous();
    if (m_node && _N(m_node)->getNodeType() == ELEMENT_NODE_TYPE) {
      if (this->parent() == e)
        return;
    }
  }
}

void Collection_t::operator++(int) const {
  ++(*this);
}

void Collection_t::operator--(int) const {
  --(*this);
}

Handle_t Document::clone(Handle_t source) const {
#ifdef DD4HEP_USE_TINYXML
  return _XE(source.clone(0));
#else
  return _XE(_D(m_doc)->importNode(_E(source.ptr()),true));
#endif
}

#ifdef DD4HEP_USE_TINYXML
#include "tinyxml_inl.h"
#include "tinyxmlerror_inl.h"
#include "tinyxmlparser_inl.h"
#endif
