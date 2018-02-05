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
#include "JSON/Printout.h"
#include "JSON/Evaluator.h"
#include "JSON/Elements.h"

// C/C++ include files
#include <iostream>
#include <stdexcept>
#include <cstdio>
#include <map>

using namespace std;
using namespace dd4hep::json;
static const size_t INVALID_NODE = ~0U;

// Forward declarations
namespace dd4hep {
  XmlTools::Evaluator& evaluator();
}
// Static storage
namespace {
  XmlTools::Evaluator& eval(dd4hep::evaluator());
  string _checkEnviron(const string& env)  {
    string r = getEnviron(env);
    return r.empty() ? env : r;
  }
}

namespace {

  // This should ensure we are not passing temporaries of std::string and then
  // returning the "const char*" content calling .c_str()
  const ptree::data_type& value_data(const ptree& entry)  {
    return entry.data();
  }

  JsonElement* node_first(JsonElement* e, const char* tag) {
    if ( e )  {
      string t(tag);
      if ( t == "*" )  {
        ptree::iterator i = e->second.begin();
        return i != e->second.end() ? &(*i) : 0;
      }
      ptree::assoc_iterator i = e->second.find(t);
      return i != e->second.not_found() ? &(*i) : 0;
    }
    return 0;
  }

  size_t node_count(JsonElement* e, const string& t) {
    return e ? (t=="*" ? e->second.size() : e->second.count(t)) : 0;
  }

  Attribute attribute_node(JsonElement* n, const char* t)  {
    if ( n )  {
      auto i = n->second.find(t);
      return i != n->second.not_found() ? &(*i) : 0;
    }
    return 0;
  }

  const char* attribute_value(Attribute a) {
    return value_data(a->second).c_str();
  }
}

string dd4hep::json::_toString(Attribute attr) {
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
string dd4hep::json::_toString(const char* s) {
  if ( !s || *s == 0 ) return "";
  else if ( !(*s == '$' && *(s+1) == '{') ) return s;
  return _checkEnviron(s);
}

/// Do-nothing version. Present for completeness and argument interchangeability
string dd4hep::json::_toString(const string& s) {
  if ( s.length() < 3 || s[0] != '$' ) return s;
  else if ( !(s[0] == '$' && s[1] == '{') ) return s;
  return _checkEnviron(s);
}

/// Format unsigned long integer to string with arbitrary format
string dd4hep::json::_toString(unsigned long v, const char* fmt) {
  return __to_string(v, fmt);
}

/// Format unsigned integer (32 bits) to string with arbitrary format
string dd4hep::json::_toString(unsigned int v, const char* fmt) {
  return __to_string(v, fmt);
}

/// Format signed integer (32 bits) to string with arbitrary format
string dd4hep::json::_toString(int v, const char* fmt) {
  return __to_string(v, fmt);
}

/// Format signed long integer to string with arbitrary format
string dd4hep::json::_toString(long v, const char* fmt)   {
  return __to_string(v, fmt);
}

/// Format single procision float number (32 bits) to string with arbitrary format
string dd4hep::json::_toString(float v, const char* fmt) {
  return __to_string(v, fmt);
}

/// Format double procision float number (64 bits) to string with arbitrary format
string dd4hep::json::_toString(double v, const char* fmt) {
  return __to_string(v, fmt);
}

/// Format pointer to string with arbitrary format
string dd4hep::json::_ptrToString(const void* v, const char* fmt) {
  return __to_string(v, fmt);
}

long dd4hep::json::_toLong(const char* value) {
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
    if (eval.status() != XmlTools::Evaluator::OK) {
      cerr << s << ": ";
      eval.print_error();
      throw runtime_error("dd4hep: Severe error during expression evaluation of " + s);
    }
    return (long) result;
  }
  return -1;
}

int dd4hep::json::_toInt(const char* value) {
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
      throw runtime_error("dd4hep: Severe error during expression evaluation of " + s);
    }
    return (int) result;
  }
  return -1;
}

bool dd4hep::json::_toBool(const char* value) {
  if (value) {
    string s = _toString(value);
    return s == "true";
  }
  return false;
}

float dd4hep::json::_toFloat(const char* value) {
  if (value) {
    string s = _toString(value);
    double result = eval.evaluate(s.c_str());

    if (eval.status() != XmlTools::Evaluator::OK) {
      cerr << s << ": ";
      eval.print_error();
      throw runtime_error("dd4hep: Severe error during expression evaluation of " + s);
    }
    return (float) result;
  }
  return 0.0;
}

double dd4hep::json::_toDouble(const char* value) {
  if (value) {
    string s = _toString(value);
    double result = eval.evaluate(s.c_str());
    if (eval.status() != XmlTools::Evaluator::OK) {
      cerr << s << ": ";
      eval.print_error();
      throw runtime_error("dd4hep: Severe error during expression evaluation of " + s);
    }
    return result;
  }
  return 0.0;
}

void dd4hep::json::_toDictionary(const char* name, const char* value) {
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
    throw runtime_error("dd4hep: Severe error during expression evaluation of " + v);
  }
  eval.setVariable(n.c_str(), result);
}

template <typename T>
void dd4hep::json::_toDictionary(const char* name, T value)   {
  string item = _toString(value);
  _toDictionary(name, item.c_str());
}

template void dd4hep::json::_toDictionary(const char* name, const string& value);
template void dd4hep::json::_toDictionary(const char* name, unsigned long value);
template void dd4hep::json::_toDictionary(const char* name, unsigned int value);
template void dd4hep::json::_toDictionary(const char* name, unsigned short value);
template void dd4hep::json::_toDictionary(const char* name, int value);
template void dd4hep::json::_toDictionary(const char* name, long value);
template void dd4hep::json::_toDictionary(const char* name, short value);
template void dd4hep::json::_toDictionary(const char* name, float value);
template void dd4hep::json::_toDictionary(const char* name, double value);

/// Evaluate string constant using environment stored in the evaluator
string dd4hep::json::getEnviron(const string& env)   {
  size_t id1 = env.find("${");
  size_t id2 = env.rfind("}");
  if ( id1 == string::npos || id2 == string::npos )   {
    return "";
  }
  else  {
    string v = env.substr(0,id2+1);
    const char* ret = eval.getEnviron(v.c_str());
    if (eval.status() != XmlTools::Evaluator::OK) {
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

/// Copy constructor
NodeList::NodeList(const NodeList& copy)
  : m_tag(copy.m_tag), m_node(copy.m_node)
{
  reset();
}

/// Initializing constructor
NodeList::NodeList(JsonElement* node, const string& tag_value)
  : m_tag(tag_value), m_node(node)
{
  reset();
}

/// Default destructor
NodeList::~NodeList() {
}

/// Reset the nodelist
JsonElement* NodeList::reset() {
  if ( m_tag == "*" )
    m_ptr = make_pair(m_node->second.ordered_begin(), m_node->second.not_found());
  else
    m_ptr = m_node->second.equal_range(m_tag);
  if ( m_ptr.first != m_ptr.second )
    return &(*m_ptr.first);
  return 0;
}

/// Advance to next element
JsonElement* NodeList::next() const {
  if ( m_ptr.first != m_ptr.second )  {
    m_ptr.first = ++m_ptr.first;
    if ( m_ptr.first != m_ptr.second ) return &(*m_ptr.first);
  }
  return 0;
}

/// Go back to previous element
JsonElement* NodeList::previous() const {
  if ( m_ptr.first != m_ptr.second )  {
    m_ptr.first = --m_ptr.first;
    if ( m_ptr.first != m_ptr.second ) return &(*m_ptr.first);
  }
  return 0;
}

/// Assignment operator
NodeList& NodeList::operator=(const NodeList& l) {
  if ( this != &l ) {
    m_tag  = l.m_tag;
    m_node = l.m_node;
    reset();
  }
  return *this;
}

/// Unicode text access to the element's tag. This must be wrong ....
const char* Handle_t::rawTag() const {
  return m_node->first.c_str();
}

/// Unicode text access to the element's text
const char* Handle_t::rawText() const {
  return value_data(m_node->second).c_str();
}

/// Unicode text access to the element's value
const char* Handle_t::rawValue() const {
  return value_data(m_node->second).c_str();
}

/// Access attribute pointer by the attribute's unicode name (no exception thrown if not present)
Attribute Handle_t::attr_nothrow(const char* tag_value) const {
  return attribute_node(m_node, tag_value);
}

/// Check for the existence of a named attribute
bool Handle_t::hasAttr(const char* tag_value) const {
  return m_node && 0 != node_first(m_node, tag_value);
}

/// Retrieve a collection of all attributes of this DOM element
vector<Attribute> Handle_t::attributes() const {
  vector < Attribute > attrs;
  if (m_node) {
    for(ptree::iterator i=m_node->second.begin(); i!=m_node->second.end(); ++i)  {
      Attribute a = &(*i);
      attrs.push_back(a);
    }
  }
  return attrs;
}

size_t Handle_t::numChildren(const char* t, bool throw_exception) const {
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
Handle_t Handle_t::child(const char* t, bool throw_exception) const {
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

NodeList Handle_t::children(const char* tag_value) const {
  return NodeList(m_node, tag_value);
}

bool Handle_t::hasChild(const char* tag_value) const {
  return node_first(m_node, tag_value) != 0;
}

/// Access attribute pointer by the attribute's unicode name (throws exception if not present)
Attribute Handle_t::attr_ptr(const char* t) const {
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
const char* Handle_t::attr_name(const Attribute a) const {
  if (a) {
    return a->first.c_str();
  }
  throw runtime_error("Attempt to access invalid XML attribute object!");
}

/// Access attribute value by the attribute's unicode name (throws exception if not present)
const char* Handle_t::attr_value(const char* attr_tag) const {
  return attribute_value(attr_ptr(attr_tag));
}

/// Access attribute value by the attribute  (throws exception if not present)
const char* Handle_t::attr_value(const Attribute attr_val) const {
  return attribute_value(attr_val);
}

/// Access attribute value by the attribute's unicode name (no exception thrown if not present)
const char* Handle_t::attr_value_nothrow(const char* attr_tag) const {
  Attribute a = attr_nothrow(attr_tag);
  return a ? attribute_value(a) : 0;
}


/// Access the ROOT eleemnt of the DOM document
Handle_t Document::root() const   {
  if ( m_doc )   {
    return m_doc;
  }
  throw runtime_error("Document::root: Invalid handle!");
}

/// Assign new document. Old document is dropped.
DocumentHolder& DocumentHolder::assign(DOC d)   {
  if ( m_doc )   {
    printout(DEBUG,"DocumentHolder","+++ Release JSON document....");
    delete m_doc;
  }
  m_doc = d;
  return *this;
}

/// Standard destructor - releases the document
DocumentHolder::~DocumentHolder()   {
  assign(0);
}

Attribute Element::getAttr(const char* name) const {
  return m_element ? attribute_node(m_element, name) : 0;
}

Collection_t::Collection_t(Handle_t element, const char* tag_value)
  : m_children(element, tag_value) {
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
  return Handle_t(m_children.m_node).numChildren(m_children.m_tag.c_str(), false);
}

/// Helper function to throw an exception
void Collection_t::throw_loop_exception(const exception& e) const {
  if (m_node) {
    throw runtime_error(string(e.what()) + "\n" + "dd4hep: Error interpreting XML nodes of type <" + tag() + "/>");
  }
  throw runtime_error(string(e.what()) + "\n" + "dd4hep: Error interpreting collections XML nodes.");
}

void Collection_t::operator++() const {
  while (m_node) {
    m_node = m_children.next();
    if (m_node && m_node->second.size() > 0 )
      return;
  }
}

void Collection_t::operator--() const {
  while (m_node) {
    m_node = m_children.previous();
    if (m_node && m_node->second.size() > 0 )
      return;
  }
}

void Collection_t::operator++(int) const {
  ++(*this);
}

void Collection_t::operator--(int) const {
  --(*this);
}

void dd4hep::json::dumpTree(Handle_t elt)   {
  dumpTree(elt.ptr());
}

void dd4hep::json::dumpTree(Element elt)   {
  dumpTree(elt.ptr());
}

void dd4hep::json::dumpTree(const JsonElement* elt)   {
  struct Dump {
    void operator()(const JsonElement* e, const string& tag)   const  {
      string t = tag+"   ";
      printout(INFO,"DumpTree","+++ %s %s: %s",tag.c_str(), e->first.c_str(), e->second.data().c_str());
      for(auto i=e->second.begin(); i!=e->second.end(); ++i)
        (*this)(&(*i), t);
    }
  } _dmp;
  _dmp(elt," ");
}

