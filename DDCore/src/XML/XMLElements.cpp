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
#ifdef DD4HEP_USE_TINYXML
#include "XML/tinyxml.h"
#else
#include <xercesc/util/Xerces_autoconf_config.hpp>
#include "xercesc/util/XMLString.hpp"
#include "xercesc/dom/DOMElement.hpp"
#include "xercesc/dom/DOMDocument.hpp"
#include "xercesc/dom/DOMNodeList.hpp"
#include "xercesc/dom/DOM.hpp"
#include "XML/config.h"
#endif

#include "XML/Evaluator.h"
#include "XML/XMLElements.h"
#include "XML/Printout.h"
#include "XML/XMLTags.h"

// C/C++ include files
#include <iostream>
#include <stdexcept>
#include <cstdio>
#include <map>

using namespace std;
using namespace dd4hep::xml;
static const size_t INVALID_NODE = ~0U;

// Forward declarations
namespace dd4hep {
  dd4hep::tools::Evaluator& evaluator();
}
// Static storage
namespace {
  bool s_resolve_environment = true;

  dd4hep::tools::Evaluator& eval(dd4hep::evaluator());
  string _checkEnviron(const string& env)  {
    if ( s_resolve_environment )  {
      string r = getEnviron(env);
      return r.empty() ? env : r;
    }
    return env;
  }
}

// Shortcuts
#define _D(x)   Xml(x).d
#define _E(x)   Xml(x).e
#define _N(x)   Xml(x).n
#define _L(x)   Xml(x).l
#define _XE(x)  Xml(x).xe

#ifdef DD4HEP_USE_TINYXML
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
#define getDocumentURI      Value

/// Union to ease castless object access in TinyXML
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
  XmlElement* node_first(XmlElement* e, const Tag_t& t) {
    if ( t.str()=="*" ) return e ? (XmlElement*)_E(e)->FirstChildElement() : 0;
    return e ? (XmlElement*)_E(e)->FirstChildElement(t.str()) : 0;
  }
  size_t node_count(XmlElement* elt, const Tag_t& t) {
    size_t cnt = 0;
    TiXmlElement* e = Xml(elt).e;
    if ( t.str()=="*" )
      for(e=e->FirstChildElement();e; e=e->NextSiblingElement()) ++cnt;
    else
      for(e=e->FirstChildElement(t.str());e; e=e->NextSiblingElement(t.str())) ++cnt;
    return cnt;
  }
}
XmlChar* dd4hep::xml::XmlString::replicate(const XmlChar* c) {
  return c ? ::strdup(c) : 0;
}
XmlChar* dd4hep::xml::XmlString::transcode(const char* c)    {return c ? ::strdup(c) : 0;
}
void dd4hep::xml::XmlString::release(char** p) {
  if(p && *p)  {::free(*p); *p=0;}
}
size_t dd4hep::xml::XmlString::length(const char* p)  {
  return p ? ::strlen(p) : 0;
}

#else
#define ELEMENT_NODE_TYPE xercesc::DOMNode::ELEMENT_NODE

/// Union to ease castless object access when using XercesC
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

XmlChar* dd4hep::xml::XmlString::replicate(const XmlChar* c) {
  return xercesc::XMLString::replicate(c);
}
char* dd4hep::xml::XmlString::transcode(const XmlChar* c) {
  return xercesc::XMLString::transcode(c);
}
XmlChar* dd4hep::xml::XmlString::transcode(const char* c) {
  return xercesc::XMLString::transcode(c);
}
void dd4hep::xml::XmlString::release(XmlChar** p) {
  return xercesc::XMLString::release(p);
}
void dd4hep::xml::XmlString::release(char** p) {
  return xercesc::XMLString::release(p);
}
size_t dd4hep::xml::XmlString::length(const char* p)  {
  return p ? xercesc::XMLString::stringLen(p) : 0;
}
size_t dd4hep::xml::XmlString::length(const XmlChar* p)  {
  return p ? xercesc::XMLString::stringLen(p) : 0;
}

namespace {
  size_t node_count(XmlElement* e, const Tag_t& t) {
    size_t cnt = 0;
    if ( e )  {
      const string& tag = t;
      xercesc::DOMElement *ee = Xml(e).e;
      if ( ee )  {
        for(xercesc::DOMElement* elt=ee->getFirstElementChild(); elt; elt=elt->getNextElementSibling()) {
          if ( elt->getParentNode() == ee )   {
            string child_tag = _toString(elt->getTagName());
            if ( tag == "*" || child_tag == tag ) ++cnt;
          }
        }
      }
    }
    return cnt;
  }
  XmlElement* node_first(XmlElement* e, const Tag_t& t) {
    if ( e )  {
      const string& tag = t;
      xercesc::DOMElement* ee = Xml(e).e;
      if ( ee )  {
        for(xercesc::DOMElement* elt=ee->getFirstElementChild(); elt; elt=elt->getNextElementSibling()) {
          if ( elt->getParentNode() == ee )   {
            if ( tag == "*" ) return _XE(elt);
            string child_tag = _toString(elt->getTagName());
            if ( child_tag == tag ) return _XE(elt);
          }
        }
      }
    }
    return 0;
  }
}

/// Convert XML char to std::string
string dd4hep::xml::_toString(const XmlChar *toTranscode) {
  char *buff = XmlString::transcode(toTranscode);
  string tmp(buff == 0 ? "" : buff);
  XmlString::release(&buff);
  if ( tmp.length()<3 ) return tmp;
  if ( !(tmp[0] == '$' && tmp[1] == '{') ) return tmp;
  tmp = _checkEnviron(tmp);
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

string dd4hep::xml::_toString(Attribute attr) {
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
string dd4hep::xml::_toString(const char* s) {
  if ( !s || *s == 0 ) return "";
  else if ( !(*s == '$' && *(s+1) == '{') ) return s;
  return _checkEnviron(s);
}

/// Do-nothing version. Present for completeness and argument interchangeability
string dd4hep::xml::_toString(const string& s) {
  if ( s.length() < 3 || s[0] != '$' ) return s;
  else if ( !(s[0] == '$' && s[1] == '{') ) return s;
  return _checkEnviron(s);
}

/// Format unsigned long integer to string with arbitrary format
string dd4hep::xml::_toString(unsigned long v, const char* fmt) {
  return __to_string(v, fmt);
}

/// Format unsigned integer (32 bits) to string with arbitrary format
string dd4hep::xml::_toString(unsigned int v, const char* fmt) {
  return __to_string(v, fmt);
}

/// Format signed integer (32 bits) to string with arbitrary format
string dd4hep::xml::_toString(int v, const char* fmt) {
  return __to_string(v, fmt);
}

/// Format signed long integer to string with arbitrary format
string dd4hep::xml::_toString(long v, const char* fmt)   {
  return __to_string(v, fmt);
}

/// Format single procision float number (32 bits) to string with arbitrary format
string dd4hep::xml::_toString(float v, const char* fmt) {
  return __to_string(v, fmt);
}

/// Format double procision float number (64 bits) to string with arbitrary format
string dd4hep::xml::_toString(double v, const char* fmt) {
  return __to_string(v, fmt);
}

/// Convert Strng_t to std::string
string dd4hep::xml::_toString(const Strng_t& s)   {
  return _toString(Tag_t(s));
}
/// Convert Tag_t to std::string
string dd4hep::xml::_toString(const Tag_t& s)     {
  return s.str();
}

/// Format pointer to string with arbitrary format
string dd4hep::xml::_ptrToString(const void* v, const char* fmt) {
  return __to_string(v, fmt);
}

long dd4hep::xml::_toLong(const XmlChar* value) {
  if (value) {
    string s = _toString(value);
    size_t idx = s.find("(int)");
    if (idx != string::npos)
      s.erase(idx, 5);
    idx = s.find("(long)");
    if (idx != string::npos)
      s.erase(idx, 6);
    while (s[0] == ' ')
      s.erase(0, 1);
    double result = eval.evaluate(s.c_str());
    if (eval.status() != tools::Evaluator::OK) {
      cerr << s << ": ";
      eval.print_error();
      throw runtime_error("dd4hep: Severe error during expression evaluation of " + s);
    }
    return (long) result;
  }
  return -1;
}

unsigned long dd4hep::xml::_toULong(const XmlChar* value) {
  long val = _toLong(value);
  if ( val > 0 ) return (unsigned long) val;
  string s = _toString(value);
  throw runtime_error("dd4hep: Severe error during expression evaluation of " + s);
}

int dd4hep::xml::_toInt(const XmlChar* value)   {
  return (int)_toLong(value);
}

unsigned int dd4hep::xml::_toUInt(const XmlChar* value) {
  return (unsigned int)_toULong(value);
}

bool dd4hep::xml::_toBool(const XmlChar* value) {
  if (value) {
    string s = _toString(value);
    char   c = ::toupper(s[0]);
    if ( c == 'T' || c == '1' ) return true;
    if ( c == 'F' || c == '0' ) return false;
    return _toInt(value) != 0;
  }
  return false;
}

float dd4hep::xml::_toFloat(const XmlChar* value) {
  if (value) {
    string s = _toString(value);
    double result = eval.evaluate(s.c_str());

    if (eval.status() != tools::Evaluator::OK) {
      cerr << s << ": ";
      eval.print_error();
      throw runtime_error("dd4hep: Severe error during expression evaluation of " + s);
    }
    return (float) result;
  }
  return 0.0;
}

double dd4hep::xml::_toDouble(const XmlChar* value) {
  if (value) {
    string s = _toString(value);
    double result = eval.evaluate(s.c_str());
    if (eval.status() != tools::Evaluator::OK) {
      cerr << s << ": ";
      eval.print_error();
      throw runtime_error("dd4hep: Severe error during expression evaluation of " + s);
    }
    return result;
  }
  return 0.0;
}

void dd4hep::xml::_toDictionary(const XmlChar* name, const XmlChar* value) {
  string n = _toString(name).c_str(), v = _toString(value);
  size_t idx = v.find("(int)");
  if (idx != string::npos)
    v.erase(idx, 5);
  while (v[0] == ' ')
    v.erase(0, 1);
  double result = eval.evaluate(v.c_str());
  if (eval.status() != tools::Evaluator::OK) {
    cerr << v << ": ";
    eval.print_error();
    throw runtime_error("dd4hep: Severe error during expression evaluation of " + v);
  }
  eval.setVariable(n.c_str(), result);
}

/// Helper function to populate the evaluator dictionary  \ingroup DD4HEP_XML
void dd4hep::xml::_toDictionary(const XmlChar* name, const Strng_t& s)   {
  return _toDictionary(name, s.ptr());
}

/// Helper function to populate the evaluator dictionary  \ingroup DD4HEP_XML
void dd4hep::xml::_toDictionary(const XmlChar* name, const Tag_t& t)   {
  return _toDictionary(name, t.ptr());
}

template <typename T>
void dd4hep::xml::_toDictionary(const XmlChar* name, T value)   {
  Strng_t item = _toString(value);
  const XmlChar* item_value = item;
  _toDictionary(name, item_value);
}

#ifndef DD4HEP_USE_TINYXML
template void dd4hep::xml::_toDictionary(const XmlChar* name, const char* value);
#endif
template void dd4hep::xml::_toDictionary(const XmlChar* name, const Tag_t& value);
template void dd4hep::xml::_toDictionary(const XmlChar* name, const Strng_t& value);
template void dd4hep::xml::_toDictionary(const XmlChar* name, const string& value);
template void dd4hep::xml::_toDictionary(const XmlChar* name, unsigned long value);
template void dd4hep::xml::_toDictionary(const XmlChar* name, unsigned int value);
template void dd4hep::xml::_toDictionary(const XmlChar* name, unsigned short value);
template void dd4hep::xml::_toDictionary(const XmlChar* name, int value);
template void dd4hep::xml::_toDictionary(const XmlChar* name, long value);
template void dd4hep::xml::_toDictionary(const XmlChar* name, short value);
template void dd4hep::xml::_toDictionary(const XmlChar* name, float value);
template void dd4hep::xml::_toDictionary(const XmlChar* name, double value);

/// Evaluate string constant using environment stored in the evaluator
string dd4hep::xml::getEnviron(const string& env)   {
  size_t id1 = env.find("${");
  size_t id2 = env.rfind("}");
  if ( id1 == string::npos || id2 == string::npos )   {
    return "";
  }
  else  {
    string v = env.substr(id1,id2-id1+1);
    const char* ret = eval.getEnviron(v.c_str());
    if (eval.status() != tools::Evaluator::OK) {
      cerr << env << ": ";
      eval.print_error();
      throw runtime_error("dd4hep: Severe error during environment lookup of " + env);
    }
    v = env.substr(0,id1);
    v += ret;
    v += env.substr(id2+1);
    return v;
  }
}

/// Enable/disable environment resolution when parsing strings
bool dd4hep::xml::enableEnvironResolution(bool new_value)   {
  bool tmp = s_resolve_environment;
  s_resolve_environment = new_value;
  return tmp;
}

template <typename B>
static inline string i_add(const string& a, B b) {
  string r = a;
  r += b;
  return r;
}

Strng_t dd4hep::xml::operator+(const Strng_t& a, const string& b) {
  return _toString(a.ptr()) + b;
}

Strng_t dd4hep::xml::operator+(const string& a, const Strng_t& b) {
  return a + _toString(b.ptr());
}

Strng_t dd4hep::xml::operator+(const Strng_t& a, const char* b) {
  return _toString(a.ptr()) + b;
}

Strng_t dd4hep::xml::operator+(const char* a, const Strng_t& b) {
  return string(a) + _toString(b.ptr());
}

Strng_t dd4hep::xml::operator+(const Strng_t& a, const Strng_t& b) {
  return _toString(a.ptr()) + _toString(b.ptr());
}

Tag_t dd4hep::xml::operator+(const Tag_t& a, const char* b) {
  string res = a.str() + b;
  return Tag_t(res);
}

Tag_t dd4hep::xml::operator+(const char* a, const Tag_t& b) {
  string res = a + b.str();
  return Tag_t(res);
}

Tag_t dd4hep::xml::operator+(const Tag_t& a, const Strng_t& b) {
  string res = a.str() + _toString(b);
  return Tag_t(res);
}

Tag_t dd4hep::xml::operator+(const Tag_t& a, const string& b) {
  string res = a.str() + b;
  return Tag_t(res);
}

#ifndef DD4HEP_USE_TINYXML
Strng_t dd4hep::xml::operator+(const Strng_t& a, const XmlChar* b) {
  string res = _toString(a.ptr()) + _toString(b);
  return Tag_t(res);
}

Strng_t dd4hep::xml::operator+(const XmlChar* a, const Strng_t& b) {
  string res = _toString(a) + _toString(b.ptr());
  return Tag_t(res);
}

Strng_t dd4hep::xml::operator+(const XmlChar* a, const string& b) {
  string res = _toString(a) + b;
  return Tag_t(res);
}

Strng_t dd4hep::xml::operator+(const string& a, const XmlChar* b) {
  string res = a + _toString(b);
  return Tag_t(res);
}

Tag_t dd4hep::xml::operator+(const Tag_t& a, const XmlChar* b) {
  string res = a.str() + _toString(b);
  return Tag_t(res);
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
  : m_tag(copy.m_tag), m_node(copy.m_node), m_ptr(0)
{
  reset();
}

/// Initializing constructor
NodeList::NodeList(XmlElement* node, const XmlChar* tag_value)
  : m_tag(tag_value), m_node(node), m_ptr(0)
{
  reset();
}

/// Default destructor
NodeList::~NodeList() {
}

/// Reset the nodelist
XmlElement* NodeList::reset() {
  return m_ptr=node_first(m_node,m_tag);
}

/// Advance to next element
XmlElement* NodeList::next() const {
#ifdef DD4HEP_USE_TINYXML
  if ( m_tag.str()=="*" )
    return m_ptr =_XE(m_ptr ? _E(m_ptr)->NextSiblingElement() : 0);
  return m_ptr = _XE(m_ptr ? _E(m_ptr)->NextSiblingElement(m_tag.str()) : 0);
#else
  xercesc::DOMElement *elt = Xml(m_ptr).e;
  for(elt=elt->getNextElementSibling(); elt; elt=elt->getNextElementSibling()) {
    if ( m_tag.str() == "*" ) return m_ptr=Xml(elt).xe;
    string child_tag = _toString(elt->getTagName());
    if ( child_tag == m_tag ) return m_ptr=Xml(elt).xe;
  }
  return m_ptr=0;
#endif
}

/// Go back to previous element
XmlElement* NodeList::previous() const {
#ifdef DD4HEP_USE_TINYXML
  if ( m_tag.str()=="*" )
    return m_ptr = _XE(m_ptr ? _E(m_ptr)->PreviousSiblingElement() : 0);
  return m_ptr = _XE(m_ptr ? _E(m_ptr)->PreviousSiblingElement(m_tag) : 0);
#else
  xercesc::DOMElement *elt = Xml(m_ptr).e;
  for(elt=elt->getPreviousElementSibling(); elt; elt=elt->getPreviousElementSibling()) {
    if ( m_tag.str()=="*" ) return m_ptr=Xml(elt).xe;
    string child_tag = _toString(elt->getTagName());
    if ( child_tag == m_tag ) return m_ptr=Xml(elt).xe;
  }
  return m_ptr=0;
#endif
}

/// Assignment operator
NodeList& NodeList::operator=(const NodeList& l) {
  if (this != &l) {
    m_tag = l.m_tag;
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
Attribute Handle_t::attr_nothrow(const XmlChar* tag_value) const {
  return attribute_node(m_node, tag_value);
}

/// Check for the existence of a named attribute
bool Handle_t::hasAttr(const XmlChar* tag_value) const {
  return m_node && 0 != _E(m_node)->getAttributeNode(tag_value);
}

/// Retrieve a collection of all attributes of this DOM element
vector<Attribute> Handle_t::attributes() const {
  vector < Attribute > attrs;
  if (m_node) {
#ifdef DD4HEP_USE_TINYXML
    for(TiXmlAttribute* a=_E(m_node)->FirstAttribute(); a; a=a->Next())
      attrs.emplace_back(Attribute(a));
#else
    xercesc::DOMNamedNodeMap* l = _E(m_node)->getAttributes();
    for (XmlSize_t i = 0, n = l->getLength(); i < n; ++i) {
      xercesc::DOMNode* attr_node = l->item(i);
      attrs.emplace_back(Attribute(attr_node));
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

NodeList Handle_t::children(const XmlChar* tag_value) const {
  return NodeList(m_node, tag_value);
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
  else if (!node)
    msg += "Element [INVALID]. Cannot remove [INVALID] child. Big Shit!!!!";

  throw runtime_error(msg);
}

/// Remove children with a given tag name from the DOM node
void Handle_t::removeChildren(const XmlChar* tag_value) const {
#ifdef DD4HEP_USE_TINYXML
  for(TiXmlNode* n=_E(m_node)->FirstChildElement(tag_value);n;n=_E(m_node)->FirstChildElement(tag_value))
    n->RemoveChild(n);
#else
  xercesc::DOMElement* e = _E(m_node);
  xercesc::DOMNodeList* l = e->getElementsByTagName(tag_value);
  for (XmlSize_t i = 0, n = l->getLength(); i < n; ++i)
    e->removeChild(l->item(i));
#endif
}

bool Handle_t::hasChild(const XmlChar* tag_value) const {
  return node_first(m_node, tag_value) != 0;
}

/// Set the element's value
void Handle_t::setValue(const XmlChar* text_value) const {
  _N(m_node)->setNodeValue(text_value);
}

/// Set the element's value
void Handle_t::setValue(const string& text_value) const {
#ifdef DD4HEP_USE_TINYXML
  _N(m_node)->setNodeValue(text_value.c_str());
#else
  _N(m_node)->setNodeValue(Strng_t(text_value));
#endif
}

/// Set the element's text
void Handle_t::setText(const XmlChar* text_value) const {
#ifdef DD4HEP_USE_TINYXML
  _N(m_node)->LinkEndChild(new TiXmlText(text_value));
#else
  _N(m_node)->setTextContent(text_value);
#endif
}

/// Set the element's text
void Handle_t::setText(const string& text_value) const {
#ifdef DD4HEP_USE_TINYXML
  _N(m_node)->LinkEndChild(new TiXmlText(text_value.c_str()));
#else
  _N(m_node)->setTextContent(Strng_t(text_value));
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
#ifdef DD4HEP_USE_TINYXML
void Handle_t::setAttrs(Handle_t elt) const {
  removeAttrs();
  TiXmlElement* e = Xml(elt).e;
  for(TiXmlAttribute* a=e->FirstAttribute(); a; a=a->Next())
    e->SetAttribute(a->Name(),a->Value());
}
#else
void Handle_t::setAttrs(Handle_t /* elt */) const {
  removeAttrs();
  xercesc::DOMElement* e = _E(m_node);
  xercesc::DOMNamedNodeMap* l = e->getAttributes();
  for (XmlSize_t i = 0, len = l->getLength(); i < len; ++i) {
    xercesc::DOMNode* n = l->item(i);
    if (n->getNodeType() == xercesc::DOMNode::ATTRIBUTE_NODE) {
      xercesc::DOMAttr* a = (xercesc::DOMAttr*) n;
      e->setAttribute(a->getName(), a->getValue());
    }
  }
}
#endif

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
const XmlChar* Handle_t::attr_value(const XmlChar* attr_tag) const {
  return attribute_value(attr_ptr(attr_tag));
}

/// Access attribute value by the attribute  (throws exception if not present)
const XmlChar* Handle_t::attr_value(const Attribute attr_val) const {
  return attribute_value(attr_val);
}

/// Access attribute value by the attribute's unicode name (no exception thrown if not present)
const XmlChar* Handle_t::attr_value_nothrow(const XmlChar* attr_tag) const {
  Attribute a = attr_nothrow(attr_tag);
  return a ? attribute_value(a) : 0;
}

/// Generic attribute setter with integer value
Attribute Handle_t::setAttr(const XmlChar* nam, int val) const {
  char txt[32];
  ::snprintf(txt, sizeof(txt), "%d", val);
  return setAttr(nam, Strng_t(txt));
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
  ::snprintf(txt, sizeof(txt), "%.8e", val);
  return setAttr(name, Strng_t(txt));
}

/// Generic attribute setter with double precision floating point value
Attribute Handle_t::setAttr(const XmlChar* name, double val) const {
  char txt[32];
  ::snprintf(txt, sizeof(txt), "%.8e", val);
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
Attribute Handle_t::setAttr(const XmlChar* nam, const Attribute v) const {
  return v ? setAttr(nam, attribute_value(v)) : 0;
}

/// Generic attribute setter with unicode value
Attribute Handle_t::setAttr(const XmlChar* nam, const XmlChar* val) const {
#ifdef DD4HEP_USE_TINYXML
  TiXmlElement* e = Xml(m_node).e;
  e->SetAttribute(nam,val);
  return Attribute(e->AttributeNode(nam));
#else
  xercesc::DOMElement* e = _E(m_node);
  xercesc::DOMAttr* a = e->getAttributeNode(nam);
  if (!a) {
    a = e->getOwnerDocument()->createAttribute(nam);
    e->setAttributeNode(a);
  }
  a->setValue(val);
  return Attribute(a);
#endif
}

/// Add reference child as a new child node. The obj must have the "name" attribute!
Handle_t Handle_t::setRef(const XmlChar* tag_value, const XmlChar* ref_name) {
  Element me(*this);
  Element ref(me.document(), tag_value);
  ref.setAttr(Unicode_ref, ref_name);
  me.append(ref);
  return ref;
}

/// Add reference child as a new child node. The obj must have the "name" attribute!
Handle_t Handle_t::setRef(const XmlChar* tag_value, const string& ref_name) {
  return setRef(tag_value, Strng_t(ref_name).ptr());
}

/// Checksum (sub-)tree of a xml document/tree
static unsigned int adler32(unsigned int adler, const XmlChar* xml_buff, size_t len) {
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
  const char* buf = (const char*)xml_buff;

  if (buf == NULL)
    return 1;

  while (len > 0) {
    int k = len < NMAX ? (int) len : NMAX;
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
typedef unsigned int (fcn_t)(unsigned int, const XmlChar*, size_t);
unsigned int Handle_t::checksum(unsigned int param, fcn_t fcn) const {
#ifdef DD4HEP_USE_TINYXML
  typedef map<string, string> StringMap;
  TiXmlNode* n = Xml(m_node).n;
  if ( n ) {
    if ( 0 == fcn ) fcn = adler32;
    switch (n->Type()) {
    case TiXmlNode::ELEMENT: {
      map<string,string> m;
      TiXmlElement* e = n->ToElement();
      TiXmlAttribute* p=e->FirstAttribute();
      for(; p; p=p->Next()) m.emplace(p->Name(),p->Value());
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
  if ( 0 == fcn ) fcn = adler32;
  if ( 0 == fcn )  {
  }
#endif
  return param;
}

/// Create DOM element
Handle_t Document::createElt(const XmlChar* tag_value) const {
#ifdef DD4HEP_USE_TINYXML
  return _XE(new TiXmlElement(tag_value));
#else
  return _XE(_D(m_doc)->createElement(tag_value));
#endif
}

/// Access the ROOT eleemnt of the DOM document
Handle_t Document::root() const {
  if (m_doc)
    return _XE(_D(m_doc)->getDocumentElement());
  throw runtime_error("Document::root: Invalid handle!");
}

/// Acces the document URI
std::string Document::uri() const   {
  if (m_doc)   {
    Tag_t val(_D(m_doc)->getDocumentURI());
    return val;
  }
  throw runtime_error("Document::uri: Invalid handle!");
}

/// Assign new document. Old document is dropped.
DocumentHolder& DocumentHolder::assign(DOC d)   {
  if (m_doc)   {
    printout(DEBUG,"DocumentHolder","+++ Release DOM document....");
#ifdef DD4HEP_USE_TINYXML
    delete _D(m_doc);
#else
    _D(m_doc)->release();
#endif
  }
  m_doc = d;
  return *this;
}

/// Standard destructor - releases the document
DocumentHolder::~DocumentHolder() {
  assign(0);
}

/// Constructor from DOM document entity
Element::Element(const Document& doc, const XmlChar* type)
  : m_element(Xml(doc.createElt(type)).xe) {
}

/// Access the XmlElements parent
Element::Elt_t Element::parentElement()  const   {
  Handle_t p = m_element.parent();
  if ( p && _N(p.ptr())->getNodeType() == ELEMENT_NODE_TYPE )  {
    return Elt_t(p);
  }
  return Elt_t(0);
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
Attribute Element::setRef(const XmlChar* tag_value, const XmlChar* ref_name) const {
  return setChild(tag_value).setAttr(Unicode_ref, ref_name);
}

/// Set the reference attribute to the node (adds attribute ref="ref-name")
Attribute Element::setRef(const XmlChar* tag_value, const string& ref_name) const {
  return setRef(tag_value, Strng_t(ref_name).ptr());
}

/// Access the value of the reference attribute of the node (attribute ref="ref-name")
const XmlChar* Element::getRef(const XmlChar* tag_value) const {
  return child(tag_value).attr < cpXmlChar > (Unicode_ref);
}

/// Add a new child to the DOM node
Handle_t Element::addChild(const XmlChar* tag_value) const {
  Handle_t e = document().createElt(tag_value);
  m_element.append(e);
  return e;
}

/// Check if a child with the required tag exists - if not create it and add it to the current node
Handle_t Element::setChild(const XmlChar* t) const {
  Elt_t e = m_element.child(t, false);
  return e ? Handle_t(e) : addChild(t);
}

#ifndef DD4HEP_USE_TINYXML
/// Add comment node to the element
void Element::addComment(const XmlChar* text_value) const {
  _N(m_element)->appendChild(_D(document().m_doc)->createComment(text_value));
}
#endif

/// Add comment node to the element
void Element::addComment(const char* text_value) const {
#ifdef DD4HEP_USE_TINYXML
  _N(m_element)->appendChild(new TiXmlComment(text_value));
#else
  _N(m_element)->appendChild(_D(document().m_doc)->createComment(Strng_t(text_value)));
#endif
}

/// Add comment node to the element
void Element::addComment(const string& text_value) const {
  addComment(text_value.c_str());
}

/// Initializing constructor to create a new XMLElement and add it to the document.
RefElement::RefElement(const Document& doc, const XmlChar* typ, const XmlChar* nam)
  : Element(doc, typ) {
  m_name = nam ? setAttr(_U(name), nam) : 0;
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
Collection_t::Collection_t(Handle_t element, const XmlChar* tag_value)
  : m_children(element, tag_value) {
  m_node = m_children.reset();
}
#endif

Collection_t::Collection_t(Handle_t element, const char* tag_value)
  : m_children(element, Strng_t(tag_value)) {
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
void Collection_t::throw_loop_exception(const exception& e) const {
  if (m_node) {
    throw runtime_error(string(e.what()) + "\n" + "dd4hep: Error interpreting XML nodes of type <" + tag() + "/>");
  }
  throw runtime_error(string(e.what()) + "\n" + "dd4hep: Error interpreting collections XML nodes.");
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
