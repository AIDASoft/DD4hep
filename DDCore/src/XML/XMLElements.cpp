#include "XML/XMLElements.h"
#include "XML/Evaluator.h"
#include "xercesc/util/XMLString.hpp"
#include "xercesc/dom/DOM.hpp"

#include <iostream>
#include <stdexcept>

using namespace std;
using namespace DD4hep::XML;
using xercesc::DOMNode;
using xercesc::DOMNodeList;
using xercesc::DOMElement;
using xercesc::DOMDocument;
using xercesc::XMLString;
using xercesc::DOMNamedNodeMap;

namespace DD4hep  {
  XmlTools::Evaluator& evaluator();
}

namespace {
  XmlTools::Evaluator& eval(DD4hep::evaluator());
}

string DD4hep::XML::_toString(const XMLCh *toTranscode)  {
  char *buff = XMLString::transcode(toTranscode);
  string tmp(buff==0 ? "" : buff);
  XMLString::release(&buff);
  return tmp;
}

string DD4hep::XML::_toString(Attribute attr)  {
  if ( attr ) return _toString(attr->getValue());
  return "";
}

template <typename T> static inline string __to_string(T value, const char* fmt)  {
  char text[128];
  ::sprintf(text,fmt,value);
  return text;
}

string DD4hep::XML::_toString(int v,          const char* fmt)  { return __to_string(v,fmt); }
string DD4hep::XML::_toString(unsigned long v,const char* fmt)  { return __to_string(v,fmt); }
string DD4hep::XML::_toString(unsigned int v, const char* fmt)  { return __to_string(v,fmt); }
string DD4hep::XML::_toString(float  v,       const char* fmt)  { return __to_string(v,fmt); }
string DD4hep::XML::_toString(double v,       const char* fmt)  { return __to_string(v,fmt); }

int DD4hep::XML::_toInt(const XMLCh* value)  {
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

bool   DD4hep::XML::_toBool(const XMLCh* value)   {
  if ( value )  {
    string s = _toString(value);
    return s == "true";
  }
  return false;
}

float DD4hep::XML::_toFloat(const XMLCh* value)   {
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

double DD4hep::XML::_toDouble(const XMLCh* value)   {
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

namespace DD4hep { namespace XML {
  Strng_t operator+(const XMLCh* a,       const Strng_t& b);
  Strng_t operator+(const Strng_t& a,     const XMLCh* b);
  Strng_t operator+(const Strng_t& a,     const string& b);
  Strng_t operator+(const Strng_t& a,     const Strng_t& b);
  Strng_t operator+(const Strng_t& a,     const string& b);
  Strng_t operator+(const string& a, const Strng_t& b);
}}

void DD4hep::XML::_toDictionary(const XMLCh* name, const XMLCh* value)  {
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

Strng_t DD4hep::XML::operator+(const Strng_t& a,     const XMLCh* b)
{  return _toString(a.ptr()) + _toString(b);                      }

Strng_t DD4hep::XML::operator+(const XMLCh* a,       const Strng_t& b)
{  return _toString(a) + _toString(b.ptr());                      }

Strng_t DD4hep::XML::operator+(const XMLCh* a,       const string& b)
{  return _toString(a) + b;                                       }

Strng_t DD4hep::XML::operator+(const string& a, const XMLCh* b)
{  return a + _toString(b);                                       }

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

Tag_t   DD4hep::XML::operator+(const Tag_t& a,       const XMLCh* b) 
{  return a.str() + _toString(b);                                   }

Tag_t   DD4hep::XML::operator+(const Tag_t& a,       const Strng_t& b)
{  return a.str() + _toString(b);                                   }

Tag_t   DD4hep::XML::operator+(const Tag_t& a,       const string& b)  
{  return a.str() + b;                                              }


Strng_t& Strng_t::operator=(const char* s)   {
  if (m_xml) xercesc::XMLString::release(&m_xml);
  m_xml = s ? xercesc::XMLString::transcode(s) : 0;
  return *this;
}

Strng_t& Strng_t::operator=(const Strng_t& s)   {
  if (m_xml) xercesc::XMLString::release(&m_xml);
  m_xml = xercesc::XMLString::replicate(s.m_xml);
  return *this;
}

Strng_t& Strng_t::operator=(const string& s)   {
  if (m_xml) xercesc::XMLString::release(&m_xml);
  m_xml = xercesc::XMLString::transcode(s.c_str());
  return *this;
}

Tag_t& Tag_t::operator=(const Tag_t& s)  {
  m_str = s.m_str;
  if (m_xml) xercesc::XMLString::release(&m_xml);
  m_xml = xercesc::XMLString::transcode(m_str.c_str());
  return *this;
}

Tag_t& Tag_t::operator=(const char* s) {
  if (m_xml) xercesc::XMLString::release(&m_xml);
  if ( s )  {
    m_xml=xercesc::XMLString::transcode(s);
    m_str = s;
  }
  else {
    m_xml = 0;
    m_str = "";
  }
  return *this;
}

Tag_t& Tag_t::operator=(const Strng_t& s)  {
  if (m_xml) xercesc::XMLString::release(&m_xml);
  m_str = s.m_xml ? xercesc::XMLString::transcode(s.m_xml) : "";
  m_xml = xercesc::XMLString::transcode(m_str.c_str());
  return *this;
}

Tag_t& Tag_t::operator=(const string& s) {
  if (m_xml) xercesc::XMLString::release(&m_xml);
  m_xml = xercesc::XMLString::transcode(s.c_str());
  m_str = s;
  return *this;
}

/// Clone the DOMelement - with the option of a deep copy
Handle_t Handle_t::clone(DOMDocument* new_doc, bool deep) const  {
  return (DOMElement*)new_doc->importNode(m_node, deep);
}

/// Retrieve a collection of all attributes of this DOM element
vector<Attribute> Handle_t::attributes() const {
  vector<Attribute> attrs;
  if ( m_node ) {
    xercesc::DOMNamedNodeMap* l = m_node->getAttributes();
    for(XMLSize_t i=0, n=l->getLength(); i<n; ++i)  {
      DOMNode* n = l->item(i);
      attrs.push_back(Attribute(n));
    }
  }
  return attrs;
}

size_t Handle_t::numChildren(const XMLCh* t, bool throw_exception) const  {
  NodeList e = (NodeList)(m_node ? m_node->getElementsByTagName(t) : 0); 
  if ( e || !throw_exception ) return e->getLength();
  string msg = "Handle_t::numChildren: ";
  if ( m_node )
    msg += "Element ["+tag()+"] has no children of type '"+_toString(t)+"'";
  else
    msg += "Element [INVALID] has no children of type '"+_toString(t)+"'";
  throw runtime_error(msg);
}

Handle_t Handle_t::child(const XMLCh* t, bool throw_exception) const   { 
  NodeList    l = (NodeList)(m_node ? m_node->getElementsByTagName(t) : 0); 
  DOMElement* e = (DOMElement*)(l ? l->item(0) : 0); 
  if ( e || !throw_exception ) return e;
  string msg = "Handle_t::child: ";
  if ( m_node )
    msg += "Element ["+tag()+"] has no child of type '"+_toString(t)+"'";
  else
    msg += "Element [INVALID] has no child of type '"+_toString(t)+"'";
  throw runtime_error(msg);
}

Handle_t Handle_t::remove(Handle_t node)  const  {
  DOMElement* e = (DOMElement*)(m_node && node.ptr() ? m_node->removeChild(node.ptr()) : 0); 
  if ( e ) return e;
  string msg = "Handle_t::remove: ";
  if ( m_node && node.ptr() )
    msg += "Element ["+tag()+"] has no child of type '"+node.tag()+"'";
  else if ( node )
    msg += "Element [INVALID]. Cannot remove child of type: '"+node.tag()+"'";
  else if ( node )
    msg += "Element [INVALID]. Cannot remove [INVALID] child. Big Shit!!!!";

  throw runtime_error(msg);
}

void Handle_t::removeChildren(const XMLCh* tag)  const  {
  NodeList l=m_node->getElementsByTagName(tag);
  for(XMLSize_t i=0, n=l->getLength(); i<n; ++i)
    m_node->removeChild(l->item(i));
}

bool Handle_t::hasChild(const XMLCh* tag) const   { 
  NodeList n = m_node->getElementsByTagName(tag);
  return n ? n->getLength()>0 : false;
}

void Handle_t::setAttrs(Handle_t e) const   {
  DOMNamedNodeMap* l = e->getAttributes();
  XMLSize_t i, n=l->getLength();
  for(i=0; i<n; ++i)  {
    DOMNode* node = l->item(i);
    if ( node->getNodeType() == DOMNode::ATTRIBUTE_NODE ) {
      Attribute attr = (Attribute)node;
      m_node->setAttribute(attr->getName(),attr->getValue());
    }
  }
}

Attribute Handle_t::attr_ptr(const XMLCh* t)  const    {
  Attribute a = m_node->getAttributeNode(t);
  if ( 0 != a ) return a;
  string msg = "Handle_t::attr_ptr: ";
  if ( m_node )
    msg += "Element ["+tag()+"] has no attribute of type '"+_toString(t)+"'";
  else
    msg += "Element [INVALID] has no attribute of type '"+_toString(t)+"'";
  throw runtime_error(msg);
}

const XMLCh* Handle_t::attr_name(const Attribute attr)  const   {
  return attr->getName();
}

const XMLCh* Handle_t::attr_value(const XMLCh* attr)  const   {
  return attr_ptr(attr)->getValue();
}

const XMLCh* Handle_t::attr_value(const Attribute attr)  const   {
  return attr->getValue();
}

const XMLCh* Handle_t::attr_value_nothrow(const XMLCh* attr)  const   {
  Attribute a = attr_nothrow(attr);
  //Attribute a = attr_ptr(attr);
  return a ? a->getValue() : 0;
}

Attribute Handle_t::setAttr(const XMLCh* name, int val)  const    {
  char txt[32];
  ::sprintf(txt,"%d",val);
  return setAttr(name, Strng_t(txt));
}

Attribute Handle_t::setAttr(const XMLCh* name, bool val)  const   {
  char txt[32];
  ::sprintf(txt,"%s",val ? "true" : "false");
  return setAttr(name, Strng_t(txt));
}

Attribute Handle_t::setAttr(const XMLCh* name, float val)  const   {
  char txt[32];
  ::sprintf(txt,"%f",val);
  return setAttr(name, Strng_t(txt));
}

Attribute Handle_t::setAttr(const XMLCh* name, double val)  const   {
  char txt[32];
  ::sprintf(txt,"%f",val);
  return setAttr(name, Strng_t(txt));
}

Attribute Handle_t::setAttr(const XMLCh* name, const char* v) const    {
  return setAttr(name,Strng_t(v));
}

Attribute Handle_t::setAttr(const XMLCh* name, const Attribute v) const    {
  return v ? setAttr(name,v->getValue()) : 0;
}

Attribute Handle_t::setAttr(const XMLCh* name, const XMLCh* value) const  { 
  Attribute attr = m_node->getAttributeNode(name);
  if ( !attr ) {
    attr = m_node->getOwnerDocument()->createAttribute(name);
    m_node->setAttributeNode(attr);
  }
  attr->setValue(value);
  return attr;
}

Handle_t Document::createElt(const XMLCh* tag)  const {
  return m_doc->createElement(tag);
}

Attribute Document::createAttr(const XMLCh* name, const XMLCh* value) const  {
  Attribute attr = m_doc->createAttribute(name);
  attr->setValue(value);
  return attr;
}

Handle_t Document::root() const  {
  if ( m_doc )  return m_doc->getDocumentElement();
  throw runtime_error("Document::root: Invalid handle!");
}

DocumentHolder::~DocumentHolder()  {
  if (m_doc) m_doc->release();
  m_doc = 0;
}

Element::Element(const Document& document, const XMLCh* type) 
: m_element(document.createElt(type))  
{ }

Document Element::document() const   {
  return Document(m_element ? m_element->getOwnerDocument() : 0);
}

Handle_t Element::clone(Handle_t h, bool /* deep */) const  {
  if ( m_element && h )  {    
    return h.clone(m_element->getOwnerDocument(),true);
  }
  throw runtime_error("Element::clone: Invalid element pointer -- unable to clone node!");
}

Attribute Element::getAttr(const XMLCh* name)   const  {
  return m_element ? m_element->getAttributeNode(name) : 0;
}

Attribute Element::setRef(const XMLCh* tag, const XMLCh* refname)  const  {
  return setChild(tag).setAttr(Attr_ref,refname);
}

const XMLCh* Element::getRef(const XMLCh* tag)  const   {
  return child(tag).attr<cpXMLCh>(Attr_ref);
}

Handle_t Element::addChild(const XMLCh* tag)  const  {
  Handle_t e = m_element->getOwnerDocument()->createElement(tag);
  m_element.append(e);
  return e;
}

Handle_t Element::child(const Strng_t& tag, bool throw_exception) const  {
  NodeList l=m_element->getElementsByTagName(tag);
  if ( l && l->getLength() > 0 ) return Handle_t((DOMElement*)l->item(0));
  if ( throw_exception ) throw runtime_error("Cannot find the required child node!");
  return Handle_t(0);
}

Handle_t Element::setChild(const XMLCh* tag)  const  {
  NodeList l=m_element->getElementsByTagName(tag);
  if ( l && l->getLength() > 0 ) return Handle_t((DOMElement*)l->item(0));
  return addChild(tag);
}

RefElement::RefElement(const Document& document, const XMLCh* type, const XMLCh* name)  
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

const XMLCh* RefElement::name() const  {
  if ( 0 == m_name ) cout << "Error:tag=" << m_element.tag() << endl;
  return m_name->getValue();
}

const XMLCh* RefElement::refName() const  {
  if ( 0 == m_name ) cout << "Error:tag=" << m_element.tag() << endl;
  return m_name->getValue();
}

void RefElement::setName(const XMLCh* new_name)  {
  setAttr(Attr_name,new_name);
}

Collection_t::Collection_t(Elt_t element, DOMNode::NodeType typ) 
  : m_type(typ), m_index(-1)
{
  m_children = element->getChildNodes();
  m_node = (DOMElement*)m_children->item(0);
  ++(*this);
}

Collection_t::Collection_t(Elt_t element, const XMLCh* tag) 
  : m_type(DOMNode::ELEMENT_NODE), m_index(-1)
{
  m_children = element->getElementsByTagName(tag);
  m_node = (DOMElement*)m_children->item(0);
  ++(*this);
}

Collection_t::Collection_t(Elt_t element, const char* tag)
  : m_type(DOMNode::ELEMENT_NODE), m_index(-1)
{
  m_children = element->getElementsByTagName(Strng_t(tag));
  m_node = (DOMElement*)m_children->item(0);
  ++(*this);
}

Collection_t::Collection_t(NodeList node_list) 
  : m_type(DOMNode::ELEMENT_NODE), m_index(-1), m_children(node_list)
{
  m_node = (DOMElement*)m_children->item(0);
  ++(*this);
}

Collection_t::Collection_t(NodeList node_list, DOMNode::NodeType typ)
  : m_type(typ), m_index(-1), m_children(node_list)
{
  m_node = (DOMElement*)m_children->item(0);
  ++(*this);
}

Collection_t& Collection_t::reset()  {
  m_index = -1;
  m_node = (DOMElement*)(m_children->getLength() == 0 ? 0 : m_children->item(0));
  ++(*this);
  return *this;
}

size_t Collection_t::size()  const  {
  return m_children->getLength();
}

void Collection_t::operator++()  const  {
  Elt_t e = Elt_t(m_node ? m_node->getParentNode() : 0);
  while(m_node)  {
    m_node = (DOMElement*)m_children->item(++m_index);
    if ( m_node && m_node->getNodeType() == m_type ) {
      if ( m_node->getParentNode() == e )
	return;
    }
  }
}

void Collection_t::operator--()  const  {
  Elt_t e = Elt_t(m_node ? m_node->getParentNode() : 0);
  while(m_node)  {
    m_node = (DOMElement*)m_children->item(--m_index);
    if ( m_node && m_node->getNodeType() == m_type ) {
      if ( m_node->getParentNode() == e )
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

Handle_t Document::clone(Handle_t source, bool deep) const  {
  return (DOMElement*)(m_doc->importNode(source,deep));
}
