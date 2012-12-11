// $Id:$
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "XML/XMLElements.h"
#include "XML/Evaluator.h"

// C/C++ include files
#include <iostream>
#include <stdexcept>
#include <map>

using namespace std;
using namespace DD4hep::XML;

// Forward declarations
namespace DD4hep  {
  XmlTools::Evaluator& evaluator();
}

// Static storage
namespace {
  XmlTools::Evaluator& eval(DD4hep::evaluator());
}
#define INVALID_NODE ~0x0

#ifdef __TIXML__
#define ELEMENT_NODE_TYPE XmlNode::ELEMENT
namespace {
  Attribute    attribute_node(XmlElement* n, const XmlChar* t)       {  return n->AttributeNode(t);               }
  const XmlChar* attribute_value(Attribute a)                        {  return a->Value();                        }
  int          node_type(XmlNode* n)                                 {  return n->Type();                         }
  XmlElement*  node_first(XmlElement* e, const XmlChar* t)           {  return e ? e->FirstChildElement(t) : 0;   }
  size_t       node_count(XmlElement* e, const XmlChar* t) {
    size_t cnt = 0;
    for(e=e->FirstChildElement(t);e; e=e->NextSiblingElement(t)) ++cnt;
    return cnt;
  }
}
XmlChar* DD4hep::XML::XmlString::replicate(const XmlChar* c)         {  return c ? ::strdup(c) : 0;               }
XmlChar* DD4hep::XML::XmlString::transcode(const char* c)            {  return c ? ::strdup(c) : 0;               }
void     DD4hep::XML::XmlString::release(XmlChar** p)                {  if(p && *p) { ::free(*p); *p=0;}          }

#else

#define ELEMENT_NODE_TYPE XmlNode::ELEMENT_NODE
#include "xercesc/dom/DOM.hpp"
namespace {
  Attribute    attribute_node(XmlElement* n, const XmlChar* t)      {  return n->getAttributeNode(t);             }
  const XmlChar* attribute_value(Attribute a)                       {  return a->getValue();                      }
  int          node_type(XmlNode* n)                                {  return n->getNodeType();                   }
  size_t       node_count(XmlElement* e, const XmlChar* t) {
    XmlNodeList* l = e ? e->getElementsByTagName(t) : 0;
    return l ? l->getLength() : INVALID_NODE; 
  }
  XmlElement*  node_first(XmlElement* e, const XmlChar* t) {
    XmlNodeList* l = e ? e->getElementsByTagName(t) : 0;
    return (XmlElement*)(l ? l->item(0) : 0);
  }
}
#endif

#ifndef __TIXML__
string DD4hep::XML::_toString(const XmlChar *toTranscode)  {
  char *buff = XmlString::transcode(toTranscode);
  string tmp(buff==0 ? "" : buff);
  XmlString::release(&buff);
  return tmp;
}
#endif

string DD4hep::XML::_toString(Attribute attr)  {
  if ( attr ) return _toString(attribute_value(attr));
  return "";
}

template <typename T> static inline string __to_string(T value, const char* fmt)  {
  char text[128];
  ::sprintf(text,fmt,value);
  return text;
}

/// Do-nothing version. Present for completeness and argument interchangeability
std::string DD4hep::XML::_toString(const char* s)               { return string(s ? s : ""); }

/// Do-nothing version. Present for completeness and argument interchangeability
std::string DD4hep::XML::_toString(const std::string& s)        { return s;                  }

/// Format unsigned long integer to string with atrbitrary format
string DD4hep::XML::_toString(unsigned long v,const char* fmt)  { return __to_string(v,fmt); }

/// Format unsigned integer (32 bits) to string with atrbitrary format
string DD4hep::XML::_toString(unsigned int v, const char* fmt)  { return __to_string(v,fmt); }

/// Format signed integer (32 bits) to string with atrbitrary format
string DD4hep::XML::_toString(int v,          const char* fmt)  { return __to_string(v,fmt); }

/// Format single procision float number (32 bits) to string with atrbitrary format
string DD4hep::XML::_toString(float  v,       const char* fmt)  { return __to_string(v,fmt); }

/// Format double procision float number (64 bits) to string with atrbitrary format
string DD4hep::XML::_toString(double v,       const char* fmt)  { return __to_string(v,fmt); }

int DD4hep::XML::_toInt(const XmlChar* value)  {
  if ( value )  {
    string s = _toString(value);
    /*
    int e;
    errno = 0;
    long val = ::strtol(s.c_str(),0,10);
    e = errno;
    if((errno == ERANGE && (val==LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0) ) {
    */
      size_t idx = s.find("(int)");
      if ( idx != string::npos ) 
        s.erase(idx,5);
      while(s[0]==' ')s.erase(0,1);
      double result = eval.evaluate(s.c_str());
      if (eval.status() != XmlTools::Evaluator::OK) {
	cerr << s << ": ";
	eval.print_error();
      }
      return (int)result;
    //}
    //return val;
  }
  return -1;
}

bool   DD4hep::XML::_toBool(const XmlChar* value)   {
  if ( value )  {
    string s = _toString(value);
    return s == "true";
  }
  return false;
}

float DD4hep::XML::_toFloat(const XmlChar* value)   {
  if ( value )  {
    string s = _toString(value);
    double result = eval.evaluate(s.c_str());

    if (eval.status() != XmlTools::Evaluator::OK) {
      cerr << s << ": ";
      eval.print_error();
    }
    return (float)result;
  }
  return 0.0;
}

double DD4hep::XML::_toDouble(const XmlChar* value)   {
  if ( value )  {
    string s = _toString(value);
    double result = eval.evaluate(s.c_str());
    if (eval.status() != XmlTools::Evaluator::OK) {
      cerr << s << ": ";
      eval.print_error();
    }
    return result;
  }
  return 0.0;
}

void DD4hep::XML::_toDictionary(const XmlChar* name, const XmlChar* value)  {
  string n=_toString(name).c_str(), v=_toString(value);
  size_t idx = v.find("(int)");
  if ( idx != string::npos ) 
    v.erase(idx,5);
  while(v[0]==' ')v.erase(0,1);
  double result = eval.evaluate(v.c_str());
  if (eval.status() != XmlTools::Evaluator::OK) {
    cerr << v << ": ";
    eval.print_error();
  }
  eval.setVariable(n.c_str(),result);
}

template <typename B> 
static inline string i_add(const string& a, B b)  {
  string r=a;
  r += b;
  return r;
}

Strng_t DD4hep::XML::operator+(const Strng_t& a,     const string& b)  
{  return _toString(a.ptr()) + b;                                  }

Strng_t DD4hep::XML::operator+(const string& a,      const Strng_t& b)  
{  return a + _toString(b.ptr());                                  }

Strng_t DD4hep::XML::operator+(const Strng_t& a,     const char* b)   
{  return _toString(a.ptr()) + b;                                  }

Strng_t DD4hep::XML::operator+(const char* a,        const Strng_t& b)   
{  return string(a) + _toString(b.ptr());                          }

Strng_t DD4hep::XML::operator+(const Strng_t& a,     const Strng_t& b)  
{  return _toString(a.ptr()) + _toString(b.ptr());                 }

Tag_t   DD4hep::XML::operator+(const Tag_t& a,       const char* b)
{  return a.str() + b;                                              }

Tag_t   DD4hep::XML::operator+(const char* a,        const Tag_t& b)
{  return a + b.str();                                              }

Tag_t   DD4hep::XML::operator+(const Tag_t& a,       const Strng_t& b)
{  return a.str() + _toString(b);                                   }

Tag_t   DD4hep::XML::operator+(const Tag_t& a,       const string& b)  
{  return a.str() + b;                                              }

#ifndef __TIXML__
Strng_t DD4hep::XML::operator+(const Strng_t& a,     const XmlChar* b)
{  return _toString(a.ptr()) + _toString(b);                      }

Strng_t DD4hep::XML::operator+(const XmlChar* a,       const Strng_t& b)
{  return _toString(a) + _toString(b.ptr());                      }

Strng_t DD4hep::XML::operator+(const XmlChar* a,       const string& b)
{  return _toString(a) + b;                                       }

Strng_t DD4hep::XML::operator+(const string& a, const XmlChar* b)
{  return a + _toString(b);                                       }

Tag_t   DD4hep::XML::operator+(const Tag_t& a,       const XmlChar* b) 
{  return a.str() + _toString(b);                                   }

Strng_t& Strng_t::operator=(const XmlChar* s)   {
  if (m_xml) XmlString::release(&m_xml);
  m_xml = s ? XmlString::replicate(s) : 0;
  return *this;
}
#endif

Strng_t& Strng_t::operator=(const char* s)   {
  if (m_xml) XmlString::release(&m_xml);
  m_xml = s ? XmlString::transcode(s) : 0;
  return *this;
}

Strng_t& Strng_t::operator=(const Strng_t& s)   {
  if (m_xml) XmlString::release(&m_xml);
  m_xml = XmlString::replicate(s.m_xml);
  return *this;
}

Strng_t& Strng_t::operator=(const string& s)   {
  if (m_xml) XmlString::release(&m_xml);
  m_xml = XmlString::transcode(s.c_str());
  return *this;
}

Tag_t& Tag_t::operator=(const Tag_t& s)  {
  m_str = s.m_str;
  if (m_xml) XmlString::release(&m_xml);
  m_xml = XmlString::transcode(m_str.c_str());
  return *this;
}

Tag_t& Tag_t::operator=(const char* s) {
  if (m_xml) XmlString::release(&m_xml);
  if ( s )  {
    m_xml=XmlString::transcode(s);
    m_str = s;
  }
  else {
    m_xml = 0;
    m_str = "";
  }
  return *this;
}

Tag_t& Tag_t::operator=(const Strng_t& s)  {
  if (m_xml) XmlString::release(&m_xml);
  m_str = s.m_xml ? XmlString::transcode(s.m_xml) : "";
  m_xml = XmlString::transcode(m_str.c_str());
  return *this;
}

Tag_t& Tag_t::operator=(const string& s) {
  if (m_xml) XmlString::release(&m_xml);
  m_xml = XmlString::transcode(s.c_str());
  m_str = s;
  return *this;
}

/// Copy constructor
NodeList::NodeList(const NodeList& l)
  : m_node(l.m_node), m_ptr(0)
#ifndef __TIXML__
, m_index(0)
#endif
{
  m_tag  = XmlString::replicate(l.m_tag);
  reset();
}

/// Initializing constructor
NodeList::NodeList(XmlElement* node, const XmlChar* tag)
  : m_node(node), m_ptr(0)
#ifndef __TIXML__
, m_index(0)
#endif
{
  m_tag = XmlString::replicate(tag);
  reset();
}

/// Default destructor
NodeList::~NodeList()    {
  if ( m_tag ) XmlString::release(&m_tag);
}

/// Reset the nodelist
XmlElement* NodeList::reset() {
#ifdef __TIXML__
  return m_ptr=node_first(m_node,m_tag);
#else
  m_ptr = m_node->getElementsByTagName(m_tag);
  return (XmlElement*)m_ptr->item(m_index=0);
#endif
}

/// Advance to next element
XmlElement* NodeList::next()  const {
#ifdef __TIXML__
  return m_ptr=(m_ptr ? m_ptr->NextSiblingElement(m_tag) : 0);
#else
  return (XmlElement*)m_ptr->item(++m_index);
#endif
}

/// Go back to previous element
XmlElement* NodeList::previous()  const {
#ifdef __TIXML__
  return m_ptr=(m_ptr ? m_ptr->PreviousSiblingElement(m_tag) : 0);
#else
  return (XmlElement*)(m_index>0 ? m_ptr->item(--m_index) : 0);
#endif
}

/// Assignment operator
NodeList& NodeList::operator=(const NodeList& l) {
  if ( m_tag ) XmlString::release(&m_tag);
  m_tag = XmlString::replicate(l.m_tag);
  m_node = l.m_node;
  reset();
  return *this;
}

/// Clone the DOMelement - with the option of a deep copy
Handle_t Handle_t::clone(XmlDocument* new_doc) const  {
  if ( m_node ) {
#ifdef __TIXML__
    if ( m_node->Type() == ELEMENT_NODE_TYPE ) {
      XmlElement* e = m_node->Clone()->ToElement();
      if ( e ) return e;
    }
    throw runtime_error("TiXml: Handle_t::clone: Invalid source handle type [No element type].");
#else
    return (Elt_t)new_doc->importNode(m_node,true);
#endif
  }
  throw runtime_error("Xml: Handle_t::clone: Invalid source handle.");
}

/// Access the element's parent element
Handle_t Handle_t::parent() const     {
#ifdef __TIXML__
  return m_node ? m_node->Parent()->ToElement() : 0;
#else
  return Elt_t(m_node ? m_node->getParentNode() : 0);
#endif
}

/// Access attribute pointer by the attribute's unicode name (no exception thrown if not present)
Attribute Handle_t::attr_nothrow(const XmlChar* tag)  const  { 
  return attribute_node(m_node,tag);
}

/// Check for the existence of a named attribute
bool Handle_t::hasAttr(const XmlChar* tag) const    { 
#ifdef __TIXML__
  return m_node && 0 != m_node->Attribute(tag);
#else
  return m_node && 0 != m_node->getAttributeNode(tag);
#endif
}

/// Retrieve a collection of all attributes of this DOM element
vector<Attribute> Handle_t::attributes() const {
  vector<Attribute> attrs;
  if ( m_node ) {
#ifdef __TIXML__
    for(TiXmlAttribute* a=m_node->FirstAttribute(); a; a=a->Next())
      attrs.push_back(a);
#else
    xercesc::DOMNamedNodeMap* l = m_node->getAttributes();
    for(XmlSize_t i=0, n=l->getLength(); i<n; ++i)  {
      XmlNode* n = l->item(i);
      attrs.push_back(Attribute(n));
    }
#endif
  }
  return attrs;
}

size_t Handle_t::numChildren(const XmlChar* t, bool throw_exception) const  {
  size_t n = node_count(m_node,t);
  if      ( n  == INVALID_NODE && !throw_exception ) return 0;
  else if ( n  != INVALID_NODE ) return n;
  string msg = "Handle_t::numChildren: ";
  if ( m_node )
    msg += "Element ["+tag()+"] has no children of type '"+_toString(t)+"'";
  else
    msg += "Element [INVALID] has no children of type '"+_toString(t)+"'";
  throw runtime_error(msg);
}

/// Remove a single child node identified by it's handle from the tree of the element
Handle_t Handle_t::child(const XmlChar* t, bool throw_exception)  const  {
  Elt_t e = node_first(m_node,t);
  if ( e || !throw_exception ) return e;
  string msg = "Handle_t::child: ";
  if ( m_node )
    msg += "Element ["+tag()+"] has no child of type '"+_toString(t)+"'";
  else
    msg += "Element [INVALID]. Cannot remove child of type: '"+_toString(t)+"'";
  throw runtime_error(msg);
}

NodeList Handle_t::children(const XmlChar* tag) const {
  return NodeList(m_node,tag);
}

/// Append a DOM element to the current node
void Handle_t::append(Handle_t e) const { 
#ifdef __TIXML__
  m_node->LinkEndChild(e.ptr());
#else
  m_node->appendChild(e.ptr());
#endif
}

/// Remove a single child node identified by it's handle from the tree of the element
Handle_t Handle_t::remove(Handle_t node)  const  {
#ifdef __TIXML__
  bool e = (m_node && node.ptr() ? m_node->RemoveChild(node.ptr()) : false); 
#else
  Elt_t e = (Elt_t)(m_node && node.ptr() ? m_node->removeChild(node.ptr()) : 0); 
#endif
  if ( e ) return node.ptr();
  string msg = "Handle_t::remove: ";
  if ( m_node && node.ptr() )
    msg += "Element ["+tag()+"] has no child of type '"+node.tag()+"'";
  else if ( node )
    msg += "Element [INVALID]. Cannot remove child of type: '"+node.tag()+"'";
  else if ( node )
    msg += "Element [INVALID]. Cannot remove [INVALID] child. Big Shit!!!!";

  throw runtime_error(msg);
}

/// Remove children with a given tag name from the DOM node
void Handle_t::removeChildren(const XmlChar* tag)  const  {
#ifdef __TIXML__
  for(TiXmlNode* node = m_node->FirstChildElement(tag);node;m_node->FirstChildElement(tag))
    m_node->RemoveChild(node);
#else
  XmlNodeList* l=m_node->getElementsByTagName(tag);
  for(XmlSize_t i=0, n=l->getLength(); i<n; ++i)
    m_node->removeChild(l->item(i));
#endif
}

bool Handle_t::hasChild(const XmlChar* tag) const   { 
  return node_first(m_node,tag) != 0;
}

/// Set the element's value
void Handle_t::setValue(const XmlChar* text) const   {
#ifdef __TIXML__
  m_node->SetValue(text);
#else
  m_node->setNodeValue(text);
#endif
}

/// Set the element's value
void Handle_t::setValue(const string& text) const   {
#ifdef __TIXML__
  m_node->SetValue(text.c_str());
#else
  m_node->setNodeValue(Strng(text));
#endif
}

/// Set the element's text
void Handle_t::setText(const XmlChar* text) const   {
#ifdef __TIXML__
  m_node->LinkEndChild(new TiXmlText(text));
#else
  m_node->setNodeText(text);
#endif
}

/// Set the element's text
void Handle_t::setText(const string& text) const   {
#ifdef __TIXML__
  m_node->LinkEndChild(new TiXmlText(text.c_str()));
#else
  m_node->setNodeText(Strng(text));
#endif
}

/// Remove all attributes of this element
void Handle_t::removeAttrs() const   {
#ifdef __TIXML__
  m_node->ClearAttributes();
#else
  xercesc::DOMNamedNodeMap* l = m_node->getAttributes();
  XmlSize_t i, n=l->getLength();
  for(i=0; i<n; ++i)  {
    XmlAttr* a = (XmlAttr*)l->item(i);
    m_node->removeAttributeNode(a);
  }
#endif
}

/// Set attributes as in argument handle
void Handle_t::setAttrs(Handle_t e) const   {
  removeAttrs();
#ifdef __TIXML__
  for(TiXmlAttribute* a=e->FirstAttribute(); a; a=a->Next())
    m_node->SetAttribute(a->Name(),a->Value());
#else
  xercesc::DOMNamedNodeMap* l = e->getAttributes();
  XmlSize_t i, n=l->getLength();
  for(i=0; i<n; ++i)  {
    XmlNode* node = l->item(i);
    if ( node->getNodeType() == XmlNode::ATTRIBUTE_NODE ) {
      Attribute attr = (Attribute)node;
      m_node->setAttribute(attr->getName(),attr->getValue());
    }
  }
#endif
}

/// Access attribute pointer by the attribute's unicode name (throws exception if not present)
Attribute Handle_t::attr_ptr(const XmlChar* t)  const    {
  Attribute a = attribute_node(m_node,t);
  if ( 0 != a ) return a;
  string msg = "Handle_t::attr_ptr: ";
  if ( m_node )
    msg += "Element ["+tag()+"] has no attribute of type '"+_toString(t)+"'";
  else
    msg += "Element [INVALID] has no attribute of type '"+_toString(t)+"'";
  throw runtime_error(msg);
}

/// Access attribute name (throws exception if not present)
const XmlChar* Handle_t::attr_name(const Attribute attr)  const   {
  if ( attr ) {
#ifdef __TIXML__
    return attr->Name();
#else
    return attr->getName();
#endif
  }
  throw runtime_error("Attempt to access invalid XML attribute object!");
}

/// Access attribute value by the attribute's unicode name (throws exception if not present)
const XmlChar* Handle_t::attr_value(const XmlChar* attr)  const   {
  return attribute_value(attr_ptr(attr));
}

/// Access attribute value by the attribute  (throws exception if not present)
const XmlChar* Handle_t::attr_value(const Attribute attr)  const   {
  return attribute_value(attr);
}

/// Access attribute value by the attribute's unicode name (no exception thrown if not present)
const XmlChar* Handle_t::attr_value_nothrow(const XmlChar* attr)  const   {
  Attribute a = attr_nothrow(attr);
  return a ? attribute_value(a) : 0;
}

/// Generic attribute setter with integer value
Attribute Handle_t::setAttr(const XmlChar* name, int val)  const    {
  char txt[32];
  ::sprintf(txt,"%d",val);
  return setAttr(name, Strng_t(txt));
}

/// Generic attribute setter with boolen value
Attribute Handle_t::setAttr(const XmlChar* name, bool val)  const   {
  char txt[32];
  ::sprintf(txt,"%s",val ? "true" : "false");
  return setAttr(name, Strng_t(txt));
}

/// Generic attribute setter with floating point value
Attribute Handle_t::setAttr(const XmlChar* name, float val)  const   {
  char txt[32];
  ::sprintf(txt,"%f",val);
  return setAttr(name, Strng_t(txt));
}

/// Generic attribute setter with double precision floating point value
Attribute Handle_t::setAttr(const XmlChar* name, double val)  const   {
  char txt[32];
  ::sprintf(txt,"%f",val);
  return setAttr(name, Strng_t(txt));
}

/// Generic attribute setter with string value
Attribute Handle_t::setAttr(const XmlChar* name, const string& val)  const {
  return setAttr(name, Strng_t(val.c_str()));
}

#ifndef __TIXML__
Attribute Handle_t::setAttr(const XmlChar* name, const char* v) const    {
  return setAttr(name, Strng_t(v));
}
#endif

/// Generic attribute setter with XmlAttr value
Attribute Handle_t::setAttr(const XmlChar* name, const Attribute v) const    {
  return v ? setAttr(name,attribute_value(v)) : 0;
}

/// Generic attribute setter with unicode value
Attribute Handle_t::setAttr(const XmlChar* name, const XmlChar* value) const  { 
#ifdef __TIXML__
  m_node->SetAttribute(name,value);
  return m_node->AttributeNode(name);
#else
  XmlAttr* attr = m_node->getAttributeNode(name);
  if ( !attr ) {
    attr = m_node->getOwnerDocument()->createAttribute(name);
    m_node->setAttributeNode(attr);
  }
  attr->setValue(value);
  return attr;
#endif
}

/// Add reference child as a new child node. The obj must have the "name" attribute!
Handle_t Handle_t::setRef(const XmlChar* tag, const XmlChar* ref_name) {
  Element me(*this);
  Element ref(me.document(),tag);
  ref.setAttr(Attr_ref,ref_name);
  me.append(ref);
  return ref;
}

/// Checksum (sub-)tree of a xml document/tree
static unsigned int adler32(unsigned int adler,const char* buf,size_t len)    {
#define DO1(buf,i)  {s1 +=(unsigned char)buf[i]; s2 += s1;}
#define DO2(buf,i)  DO1(buf,i); DO1(buf,i+1);
#define DO4(buf,i)  DO2(buf,i); DO2(buf,i+2);
#define DO8(buf,i)  DO4(buf,i); DO4(buf,i+4);
#define DO16(buf)   DO8(buf,0); DO8(buf,8);

  static const unsigned int BASE = 65521;    /* largest prime smaller than 65536 */
  /* NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1 */
  static const unsigned int NMAX = 5550;
  unsigned int s1 = adler & 0xffff;
  unsigned int s2 = (adler >> 16) & 0xffff;
  int k;

  if (buf == NULL) return 1;

  while (len > 0) {
    k = len < NMAX ? (int)len : NMAX;
    len -= k;
    while (k >= 16) {
      DO16(buf);
      buf += 16;
      k -= 16;
    }
    if (k != 0) do {
      s1 += (unsigned char)*buf++;
      s2 += s1;
    } while (--k);
    s1 %= BASE;
    s2 %= BASE;
  }
  unsigned int result = (s2 << 16) | s1;
  return result;
}

/// Checksum (sub-)tree of a xml document/tree
unsigned int Handle_t::checksum(unsigned int param,unsigned int (fcn)(unsigned int,const XmlChar*,size_t)) const  {
  typedef std::map<std::string,std::string> StringMap;
#ifdef __TIXML__
  TiXmlNode* n = m_node;
  if ( n )   {
    if ( 0 == fcn ) fcn = adler32;
    switch (n->Type())  {
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
      param = Handle_t(c->ToElement()).checksum(param,fcn);
  }
#else

#endif
  return param;
}

/// Create DOM element
Handle_t Document::createElt(const XmlChar* tag)  const {
#ifdef __TIXML__
  return new TiXmlElement(tag);
#else
  return m_doc->createElement(tag);
#endif
}

/// Access the ROOT eleemnt of the DOM document
Handle_t Document::root() const  {
#ifdef __TIXML__
  if ( m_doc )  return m_doc->RootElement();
#else
  if ( m_doc )  return m_doc->getDocumentElement();
#endif
  throw runtime_error("Document::root: Invalid handle!");
}

/// Standard destructor - releases the document
DocumentHolder::~DocumentHolder()  {
#ifdef __TIXML__
  if (m_doc) delete m_doc;
#else
  if (m_doc) m_doc->release();
#endif
  m_doc = 0;
}

/// Constructor from DOM document entity
Element::Element(const Document& document, const XmlChar* type) 
: m_element(document.createElt(type))  
{ }

/// Access the hosting document handle of this DOM element
Document Element::document() const   {
#ifdef __TIXML__
  return Document(m_element ? m_element->GetDocument() : 0);
#else
  return Document(m_element ? m_element->getOwnerDocument() : 0);
#endif
}

/// Clone the DOM element tree
Handle_t Element::clone(Handle_t h) const  {
  if ( m_element && h )  {    
    Document d = document();
    return h.clone(Document::DOC(document()));
  }
  throw runtime_error("Element::clone: Invalid element pointer -- unable to clone node!");
}

Attribute Element::getAttr(const XmlChar* name)   const  {
  return m_element ? attribute_node(m_element,name) : 0;
}

/// Set the reference attribute to the node (adds attribute ref="ref-name")
Attribute Element::setRef(const XmlChar* tag, const XmlChar* refname)  const  {
  return setChild(tag).setAttr(Attr_ref,refname);
}

/// Access the value of the reference attribute of the node (attribute ref="ref-name")
const XmlChar* Element::getRef(const XmlChar* tag)  const   {
  return child(tag).attr<cpXmlChar>(Attr_ref);
}

/// Add a new child to the DOM node
Handle_t Element::addChild(const XmlChar* tag)  const  {
  Handle_t e = document().createElt(tag);
  m_element.append(e);
  return e;
}

/// Check if a child with the required tag exists - if not create it and add it to the current node
Handle_t Element::setChild(const XmlChar* t)  const  {
  Elt_t e = m_element.child(t);
  return e ? Handle_t(e) : addChild(t);
}

RefElement::RefElement(const Document& document, const XmlChar* type, const XmlChar* name)  
: Element(document, type) 
{
  m_name = name ? setAttr(Attr_name,name) : 0;
}

RefElement::RefElement(const Handle_t& e)  
: Element(e) 
{
  m_name = m_element ? getAttr(Attr_name) : 0;
}

RefElement::RefElement(const RefElement& e)  
: Element(e), m_name(e.m_name)
{
}

const XmlChar* RefElement::name() const  {
  if ( 0 == m_name ) cout << "Error:tag=" << m_element.tag() << endl;
  return attribute_value(m_name);
}

const XmlChar* RefElement::refName() const  {
  if ( 0 == m_name ) cout << "Error:tag=" << m_element.tag() << endl;
  return attribute_value(m_name);
}

void RefElement::setName(const XmlChar* new_name)  {
  setAttr(Attr_name,new_name);
}

#ifndef __TIXML__
Collection_t::Collection_t(Elt_t element, const XmlChar* tag) 
  : m_children(element,tag)
{
  m_node = m_children.reset();
}
#endif

Collection_t::Collection_t(Elt_t element, const char* tag)
  : m_children(element,Strng_t(tag))
{
  m_node = m_children.reset();
}

Collection_t::Collection_t(NodeList node_list) 
  : m_children(node_list)
{
  m_node = m_children.reset();
}

Collection_t& Collection_t::reset()  {
  m_node = m_children.reset();
  return *this;
}

size_t Collection_t::size()  const  {
  return Handle_t(m_children.m_node).numChildren(m_children.m_tag,false);
}

void Collection_t::operator++()  const  {
  Elt_t e = this->parent();
  while(m_node)  {
    m_node = m_children.next();
    if ( m_node && node_type(m_node) == ELEMENT_NODE_TYPE ) {
      if ( this->parent() == e )
	return;
    }
  }
}

void Collection_t::operator--()  const  {
  Elt_t e = this->parent();
  while(m_node)  {
    m_node = m_children.previous();
    if ( m_node && node_type(m_node) == ELEMENT_NODE_TYPE ) {
      if ( this->parent() == e )
	return;
    }
  }
}

void Collection_t::operator++(int)  const  {
  ++(*this);
}

void Collection_t::operator--(int)  const  {
  --(*this);
}

Handle_t Document::clone(Handle_t source) const  {
#ifdef __TIXML__
  return (XmlElement*)source.clone(0);
#else
  return (XmlElement*)(m_doc->importNode(source));
#endif
}

#ifdef __TIXML__
#include "tinyxml_inl.h"
#include "tinyxmlerror_inl.h"
#include "tinyxmlparser_inl.h"
#endif
